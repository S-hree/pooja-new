/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdlib.h>
#include <string.h>

#include <atlk/bpool.h>
#include <atlk/compiler.h>
#include <atlk/sdk.h>
#include <atlk/atomic.h>
#include <atlk/remote.h>

#include <atlk/mib_service.h>
#include <atlk/mib_remote.h>
#include <atlk/mibs/rsvc-mib.h>

#include <atlk/v2x_service.h>
#include <atlk/v2x_remote.h>
#include <atlk/dx.h>
#include <atlk/rsvc_protocol.h>
#include <atlk/rsvc_v2x_protocol.h>
#include <atlk/remote_internal.h>
#include <atlk/remote.h>

#define RSVC_V2X_SERVICE_SOCKET_HANDSHAKE_TIMEOUT_MS 1000
#define RSVC_V2X_SERVICE_SOCKET_HANDSHAKE_NUM_RETRIES 3

/** v2x_service default initializer */
#define V2X_SERVICE_INIT { \
    .transport = NULL, \
};

#define V2X_SOCKET_INIT { \
    .transport_handle = REMOTE_TRANSPORT_HANDLE_UNASSIGNED \
};

/** V2X socket */
struct v2x_socket {
  /* V2X service the is associated with */
  v2x_service_t *v2x_service;

  /* transport_handle - Handle to the remote_transport stream */
  int transport_handle;

  /* MIB row index used by this socket */
  int32_t mib_row_index;

  /* Is Active? boolean */
  int active;

};

struct v2x_service {
  /* remote_transport struct */
  remote_transport_t *transport;

  v2x_socket_t v2x_sockets[V2X_REMOTE_NUM_SOCKETS_MAX];

  atlk_atomic_t num_v2x_sockets_allocated;

  mib_service_t* mib_service;

};


/* Used for allocating a remote socket, ****not protected**** */

/**
 Allocates a new v2x_socket and creates its transport_handle

 @param[in] v2x_service Instance of V2X service
 @param[out] socket A new allocated V2X socket

 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
static atlk_rc_t atlk_must_check
v2x_remote_socket_allocate(v2x_service_t *v2x_service, v2x_socket_t **socket);

/**
 De-allocates a v2x_socket (along with its transport_handle)

 @param[in] v2x_service Instance of V2X service
 @param[in] socket A new allocated V2X socket

 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
static atlk_rc_t atlk_must_check
v2x_remote_socket_deallocate(v2x_service_t *v2x_service, v2x_socket_t *socket);

/**
 Finds the first non-active socket row in the MIB database

 @param[in] mib_service Instance of MIB service
 @param[out] row_index row index

 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
static atlk_rc_t atlk_must_check
v2x_remote_socket_mib_nonactive_row_find(mib_service_t *mib_service,
                                         int32_t *row_index);

/**
 Deletes all active sockets on the target

 @param[in] mib_service Instance of MIB service

 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
static atlk_rc_t atlk_must_check
v2x_remote_all_sockets_delete(mib_service_t *mib_service);

atlk_rc_t atlk_must_check
v2x_remote_service_create(remote_transport_t *transport,
                          const v2x_remote_service_config_t *config,
                          v2x_service_t **service_ptr)
{
  atlk_rc_t rc;
  v2x_service_t *service = NULL;
  mib_service_t *mib_service = NULL;

  /* Checking config struct ptr is not done for now, (can be NULL) */

  /* check that transport is valid */
  if (atlk_unlikely(!transport)) {
    TR_ERROR("v2x transport pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!service_ptr)) {
    TR_ERROR("service_ptr pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  service = malloc(sizeof(*service));
  if (atlk_unlikely(!service)) {
    TR_ERROR("Failed to create v2x_service_t obj");
    return ATLK_E_OUT_OF_MEMORY;
  }

  /* Clear V2X socket */
  memset(service, 0x00, sizeof(*service));

  /* allocate mib_service */
  rc = mib_remote_service_create(transport, NULL, &mib_service);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to allocate mib_service: %s\n", atlk_rc_to_str(rc));
    free(service);
    return rc;
  }

  /* Attach to transport layer */
  service->transport = transport;

  service->mib_service = mib_service;

  *service_ptr = (v2x_service_t *)service;

  /* Delete all sockets */
  rc = v2x_remote_all_sockets_delete(service->mib_service);
  if (atlk_error(rc)) {
    TR_ERROR("Deleting all active V2X sockets: %s\n",
              atlk_rc_to_str(rc));
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
v2x_remote_service_transport_get(v2x_service_t *service_ptr,
                                 remote_transport_t **transport)
{
  v2x_service_t *svc = (v2x_service_t *)service_ptr;

  /* check that service_ptr is valid */
  if (atlk_unlikely(!service_ptr)) {
    TR_ERROR("v2x service_ptr pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* check that transport ptr is valid */
  if (atlk_unlikely(!transport)) {
    TR_ERROR("transport is NULL");
    return ATLK_E_INVALID_ARG;
  }

  *transport = svc->transport;

  return ATLK_OK;
}

atlk_rc_t v2x_service_delete(v2x_service_t *service)
{
  v2x_service_t *svc = (v2x_service_t *)service;
  int num_sockets_allocated;

  /* check that service is valid */
  if (atlk_unlikely(!service)) {
    TR_ERROR("v2x_service_t pointer is invalid");
    return ATLK_E_INVALID_ARG;
  }

  num_sockets_allocated = atlk_atomic_read(&svc->num_v2x_sockets_allocated);

  /* check that service_ptr is valid */
  if (atlk_unlikely(num_sockets_allocated > 0)) {
    TR_ERROR("Cannot delete service, v2x_sockets are still allocated (%d)",
             num_sockets_allocated);
    return ATLK_E_UNSPECIFIED;
  }

  /* On Host, the v2x service is a global ==>
   * Must set the transport pointer to NULL
   * Also, the user himself must delete the transport */
  svc->transport = NULL;

  free(service);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
v2x_socket_create(v2x_service_t *service,
                  v2x_socket_t **socket_ptr,
                  const v2x_socket_config_t *config)
{
  atlk_rc_t rc;
  atlk_rc_t temp_rc;
  mib_service_t *mib_service = NULL;
  v2x_service_t *v2x_service = service;
  v2x_socket_t *v2x_socket = NULL;
  uint16_t local_port_number;
  int32_t non_active_mib_row_index = 0;
  struct in_addr local_ipv4_address;

  if (atlk_unlikely(!service)) {
    TR_ERROR("service pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!socket_ptr)) {
    TR_ERROR("socket_ptr pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!config)) {
    TR_ERROR("config pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  mib_service = v2x_service->mib_service;
  if (!mib_service) {
    TR_ERROR("mib_service is not initialized");
    return ATLK_E_INVALID_ARG;
  }

  /* Allocate a V2X socket */
  rc = v2x_remote_socket_allocate(service, &v2x_socket);
  if (atlk_error(rc)) {
    TR_ERROR("Error allocating socket");
    return rc;
  }

  /* Find a non-active MIB line index */
  rc = v2x_remote_socket_mib_nonactive_row_find(mib_service,
                                                &non_active_mib_row_index);
  if (atlk_error(rc)) {
    TR_ERROR("Error finding non-active row in MIB, rc = %d", rc);
    goto v2x_socket_create_exit_with_error;
  }

  /* Set Host IP address on target */
  rc = remote_transport_local_ip_port_get(v2x_service->transport,
                                          v2x_socket->transport_handle,
                                          &local_ipv4_address,
                                          &local_port_number);
  if (atlk_error(rc)) {
    TR_ERROR("Error getting local ip/port for socket rc=%d", rc);
    goto v2x_socket_create_exit_with_error;
  }

  rc = mib_set_rsvcDefaultDestIpAddressIPv4(mib_service,
                                   (uint32_t)htonl(local_ipv4_address.s_addr));
  if (atlk_error(rc)) {
    TR_ERROR("Error assigning Host IP address to target, rc = %d", rc);
    goto v2x_socket_create_exit_with_error;
  }


  /* Assign row index to v2x_socket */
  v2x_socket->mib_row_index = non_active_mib_row_index;

  /* Set pointer to v2x service */
  v2x_socket->v2x_service = v2x_service;

  /* Set all columns of row... */
  rc = mib_set_rsvcWlanFwdDestPortNumber(mib_service,
                                         non_active_mib_row_index,
                                         local_port_number);
  if (atlk_error(rc)) {
    TR_ERROR("Error setting Host port number in MIB, rc = %d", rc);
    goto v2x_socket_create_exit_with_error;
  }

  rc = mib_set_rsvcWlanFwdIfIndex(mib_service,
                                  non_active_mib_row_index,
                                  config->if_index);
  if (atlk_error(rc)) {
    TR_ERROR("Error setting if_index, rc = %d", rc);
    goto v2x_socket_create_exit_with_error;
  }

  rc = mib_set_rsvcWlanFwdFrameType(mib_service,
                                    non_active_mib_row_index,
                                    config->protocol.frame_type);
  if (atlk_error(rc)) {
    TR_ERROR("Error setting rsvcWlanFwdFrameType, rc = %d", rc);
    goto v2x_socket_create_exit_with_error;
  }

  rc = mib_set_rsvcWlanFwdProtocolId(mib_service,
                                     non_active_mib_row_index,
                                     config->protocol.protocol_id);
  if (atlk_error(rc)) {
    TR_ERROR("Error setting rsvcWlanFwdProtocolId, rc = %d", rc);
    goto v2x_socket_create_exit_with_error;
  }

  /* And finally: Set the row status to ACTIVE state */
  rc = mib_set_rsvcWlanFwdRowStatus(mib_service,
                                    non_active_mib_row_index,
                                    MIB_RowStatus_active);
  if (atlk_error(rc)) {
    TR_ERROR("Error setting rsvcWlanFwdRowStatus to active state, rc = %d", rc);
    goto v2x_socket_create_exit_with_error;
  }

v2x_socket_create_exit_with_error:
  if (atlk_error(rc)) {
    /* Clean-up */
    TR_ERROR("v2x_socket_create_exit_with_error, rc = %d", rc);

    /* Clean-up socket resources on the target */
    temp_rc = mib_set_rsvcWlanFwdRowStatus(mib_service,
                                           non_active_mib_row_index,
                                           MIB_RowStatus_destroy);
    if (atlk_error(temp_rc)) {
      TR_ERROR("Error setting rsvcWlanFwdRowStatus to inactive state, rc = %d",
          temp_rc);
    }

    /* Deallocate the V2X socket */
    temp_rc = v2x_remote_socket_deallocate(service, v2x_socket);
    if (atlk_error(temp_rc)) {
      TR_ERROR("Error deallocating socket");
    }
    return rc;
  }

  *socket_ptr = v2x_socket;

  return ATLK_OK;
}

atlk_rc_t v2x_socket_delete(v2x_socket_t *socket)
{
  atlk_rc_t rc;
  v2x_service_t *v2x_service;
  mib_service_t *mib_service = NULL;

  if (atlk_unlikely(!socket)) {
    TR_ERROR("socket pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Get the default MIB service pointer */
  rc = mib_default_service_get(&mib_service);
  if (atlk_error(rc)) {
    TR_ERROR("Error acquiring the default MIB service ptr, rc = %d", rc);
    return rc;
  }

  v2x_service = socket->v2x_service;

  /* Set the row status to non-active / destroyed state */
  rc = mib_set_rsvcWlanFwdRowStatus(mib_service, socket->mib_row_index,
      MIB_RowStatus_destroy);
  if (atlk_error(rc)) {
    TR_ERROR("Error setting rsvcWlanFwdRowStatus to non-active state, rc = %d",
        rc);
    return rc;
  }

  /* Deallocate the socket */
  rc = v2x_remote_socket_deallocate(v2x_service, socket);
  if (atlk_error(rc)) {
    TR_ERROR("Error deallocating socket, rc = %d", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
v2x_send(v2x_socket_t *socket,
         const void *data_ptr,
         size_t data_size,
         const v2x_send_params_t *params,
         const atlk_wait_t *wait)
{
  atlk_rc_t rc;
  dx_rsvc_v2x_send_req_header_t *send_req_hdr;
  uint8_t pdu[RSVC_V2X_MAX_PDU_SIZE];
  size_t pdu_size;
  size_t payload_offset;
  uint16_t message_id;
  uint8_t dest_v2x_socket_index;
  const rsvc_protocol_api_send_param_t param_in = {
      &message_id,
      &dest_v2x_socket_index,
      &data_size,
      (v2x_send_params_t *)params
  };

  if (atlk_unlikely(!socket)) {
    TR_ERROR("socket pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!data_ptr)) {
    TR_ERROR("data_ptr pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!params)) {
    TR_ERROR("params pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Calculate expected PDU size */
  pdu_size = data_size + sizeof(dx_rsvc_v2x_send_req_header_t);

  /* Check if its not too large */
  if (atlk_unlikely(pdu_size > RSVC_V2X_MAX_PDU_SIZE)) {
    TR_ERROR("Packet size is too large, max supported: %u",
        (unsigned int)sizeof(pdu));
    return ATLK_E_INVALID_ARG;
  }

  /* Encode request - Prepare header */
  send_req_hdr = (dx_rsvc_v2x_send_req_header_t *)pdu;
  message_id = RSVC_V2X_SERVICE_SOCKET_SEND_REQ;
  dest_v2x_socket_index = (uint8_t)socket->mib_row_index;
  rc = rsvc_protocol_send_request_encode(RSVC_ID_V2X, send_req_hdr, &param_in);
  if (atlk_error(rc)) {
    TR_ERROR("Error encoding V2X send request msg, rc = %d", rc);
    return rc;
  }

  /*
   * TODO: Implement scatter / gather at this point
   */

  /* Prepare packet */
  payload_offset = sizeof(dx_rsvc_v2x_send_req_header_t);
  memcpy(&pdu[payload_offset], data_ptr, data_size);

  /* Send the request */
  rc = remote_transport_send(socket->v2x_service->transport,
                             socket->transport_handle,
                             V2X_RSVCD_SERVICE_PORT_NUM,
                             pdu,
                             &pdu_size);
  if (atlk_error(rc)) {
    TR_ERROR("Could not send PDU, rc = %d, pdu_size = %d", rc, (int)pdu_size);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
v2x_receive(v2x_socket_t *socket,
            void *data_ptr,
            size_t *data_size_ptr,
            v2x_receive_params_t *params,
            const atlk_wait_t *wait)
{
  atlk_rc_t rc;
  size_t total_size_rx;
  size_t payload_size;
  dx_rsvc_v2x_receive_indication_header_t *receive_ind_hdr;
  uint32_t timeout_ms = REMOTE_TRANSPORT_DEFAULT_RTT;
  uint16_t remote_port;
  uint8_t *msg_ptr;
  uint16_t message_id;
  uint8_t pdu[RSVC_MAX_PDU_SIZE];
  size_t size_rx = (size_t)sizeof(pdu);
  rsvc_protocol_api_receive_param_t params_out;

  if (atlk_unlikely(!socket)) {
    TR_ERROR("socket pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!data_ptr)) {
    TR_ERROR("data_ptr pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!data_size_ptr)) {
    TR_ERROR("data_size_ptr pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!params)) {
    TR_ERROR("params pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (!wait) {
    /* NULL wait param means non-blocking */
    timeout_ms = 0;
  }
  else if (wait->wait_type == ATLK_WAIT_FOREVER) {
    timeout_ms = REMOTE_TRANSPORT_NO_TIMEOUT;
  } else {
    timeout_ms = wait->wait_usec / 1000;
  }

  /* Receive from remote socket */
  rc = remote_transport_receive(socket->v2x_service->transport,
      socket->transport_handle, pdu, &size_rx, timeout_ms, &remote_port);
  if (atlk_error(rc)) {
    if (rc != ATLK_E_TIMEOUT) {
      TR_ERROR("Could not receive PDU, rc = %d, pdu_size = %d", rc,
               (int)size_rx);
    }
    return rc;
  }

  total_size_rx = (size_t)size_rx;

  /* Decode request */
  receive_ind_hdr = (dx_rsvc_v2x_receive_indication_header_t *)pdu;

  params_out.message_id = &message_id;
  params_out.params = params;
  params_out.payload_size = &payload_size;
  rc = rsvc_protocol_receive_ind_decode(RSVC_ID_V2X,
                                        &params_out,
                                        receive_ind_hdr);
  if (atlk_error(rc)) {
    TR_ERROR("Error encoding V2X receive indication msg, rc = %d", rc);
    return rc;
  }

  if (payload_size > total_size_rx) {
    TR_ERROR("payload_size > *data_size_ptr, (%u > %u) rc = %d", rc,
        (unsigned int)payload_size, (unsigned int)total_size_rx);
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  TR_DEBUG("size_rx = %u, payload_size = %u, ", (unsigned int)size_rx,
      (unsigned int)payload_size);

  if (payload_size > *data_size_ptr) {
    TR_ERROR("Buffer too small, payload_size = %u",
             (unsigned int)payload_size);
  }

  /* Copy message data into the correct place in buffer */
  msg_ptr = (uint8_t *)pdu;
  msg_ptr += sizeof(dx_rsvc_v2x_receive_indication_header_t);
  memcpy(data_ptr, msg_ptr, payload_size);

  /* Set data size */
  *data_size_ptr = (size_t) payload_size;

  return ATLK_OK;
}

static atlk_rc_t atlk_must_check
v2x_remote_socket_allocate(v2x_service_t *v2x_service,
                           v2x_socket_t **socket)
{
  uint32_t i;
  atlk_rc_t rc;
  int transport_handle;

  if (atlk_unlikely(!v2x_service)) {
    TR_ERROR("v2x_service pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!socket)) {
    TR_ERROR("socket pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  for (i = 0; i < V2X_REMOTE_NUM_SOCKETS_MAX; i++) {
    if (!v2x_service->v2x_sockets[i].active) {
      /* Allocate remote transport_handle to associate with v2x_socket */
      rc = remote_transport_handle_allocate(v2x_service->transport,
                                            &transport_handle);
      if (atlk_error(rc)) {
        TR_ERROR("Error creating the transport, rc = %d", rc);
        return rc;
      }

      /* Set socket to active state */
      v2x_service->v2x_sockets[i].active = 1;

      /* Assign transport handle to socket */
      v2x_service->v2x_sockets[i].transport_handle = transport_handle;
      v2x_service->v2x_sockets[i].v2x_service = v2x_service;
      *socket = &v2x_service->v2x_sockets[i];

      atlk_atomic_inc(&v2x_service->num_v2x_sockets_allocated);

      return ATLK_OK;
    }
  }

  return ATLK_E_OUT_OF_MEMORY;
}

static atlk_rc_t atlk_must_check
v2x_remote_socket_deallocate(v2x_service_t *v2x_service, v2x_socket_t *socket)
{
  atlk_rc_t rc;
  int32_t idx = socket->mib_row_index;

  if (atlk_unlikely(!v2x_service)) {
    TR_ERROR("v2x_service pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!socket)) {
    TR_ERROR("socket pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(idx >= V2X_REMOTE_NUM_SOCKETS_MAX)) {
    TR_ERROR("Index of socket out of bounds: %u", (int)idx);
    return ATLK_E_INVALID_ARG;
  }

  /* Deallocate remote transport_handle */
  rc = remote_transport_handle_deallocate(v2x_service->transport,
                                          socket->transport_handle);
  if (atlk_error(rc)) {
    TR_ERROR("Error deallocating the transport, rc = %d", rc);
    return rc;
  }

  v2x_service->v2x_sockets[idx].mib_row_index = -1;
  v2x_service->v2x_sockets[idx].v2x_service = NULL;
  v2x_service->v2x_sockets[idx].active = 0;
  v2x_service->v2x_sockets[idx].transport_handle = REMOTE_TRANSPORT_HANDLE_UNASSIGNED;

  BUG_ON(atlk_unlikely(atlk_atomic_read(&v2x_service->num_v2x_sockets_allocated) <= 0));

  atlk_atomic_dec(&v2x_service->num_v2x_sockets_allocated);

  return ATLK_OK;
}

static atlk_rc_t atlk_must_check
v2x_remote_socket_mib_nonactive_row_find(mib_service_t *mib_service,
                                         int32_t *row_index)
{
  int32_t i;
  atlk_rc_t rc;
  int32_t num_entries = (int32_t)V2X_REMOTE_NUM_SOCKETS_MAX;
  mib_RowStatus_t row_status;

  if (atlk_unlikely(!mib_service)) {
    TR_ERROR("mib_service pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!row_index)) {
    TR_ERROR("row_index pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  for (i = 0; i < num_entries; i++) {
    rc = mib_get_rsvcWlanFwdRowStatus(mib_service, i, &row_status);
    if (atlk_error(rc)) {
      TR_ERROR("Error getting FWD row status, rc = %d", rc);
      return rc;
    }

    if (row_status != MIB_RowStatus_active) {
      *row_index = i;
      return ATLK_OK;
    }
  }

  return ATLK_E_NOT_FOUND;
}

static atlk_rc_t atlk_must_check
v2x_remote_all_sockets_delete(mib_service_t *mib_service)
{
  int32_t i;
  atlk_rc_t rc;
  int32_t num_entries = (int32_t)V2X_REMOTE_NUM_SOCKETS_MAX;
  mib_RowStatus_t row_status;

  if (atlk_unlikely(!mib_service)) {
    TR_ERROR("mib_service pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  for (i = 0; i < num_entries; i++) {
    rc = mib_get_rsvcWlanFwdRowStatus(mib_service, i, &row_status);
    if (atlk_error(rc)) {
      TR_ERROR("Error getting FWD row status, rc = %d", rc);
      return rc;
    }

    /* ACTIVE? ==> Destroy the socket */
    if (row_status == MIB_RowStatus_active) {
      rc = mib_set_rsvcWlanFwdRowStatus(mib_service, i, MIB_RowStatus_destroy);
      if (atlk_error(rc)) {
        TR_ERROR("Error setting FWD row status, rc = %d", rc);
        return rc;
      }
    }
  }

  return ATLK_OK;
}
