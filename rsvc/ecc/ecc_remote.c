/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdlib.h>
#include <string.h>

#include <atlk/sdk.h>
#include <atlk/atomic.h>
#include <atlk/remote.h>

#include <atlk/ecc_service.h>
#include <atlk/ecc_remote.h>

#include <atlk/dx.h>
#include <atlk/rsvc_protocol.h>
#include <atlk/rsvc_ecc_protocol.h>
#include <atlk/remote_internal.h>

#include <crypto/ecc_curves.h>

struct ecc_service {
  /* Remote transport */
  remote_transport_t *transport;

  /* Number of active sockets */
  atlk_atomic_t num_sockets;

};

struct ecc_socket {
  /* ECC service  */
  ecc_service_t *service;

  /* Remote transport handle */
  int transport_handle;

};

atlk_rc_t
ecc_remote_service_create(remote_transport_t *transport,
                          const ecc_remote_service_config_t *config,
                          ecc_service_t **service_ptr)
{
  ecc_service_t *service = NULL;

  if (atlk_unlikely(!transport)) {
    TR_ERROR("Remote transport is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!service_ptr)) {
    TR_ERROR("ECC service is not specified");
    return ATLK_E_INVALID_ARG;
  }

  service = malloc(sizeof(*service));
  if (atlk_unlikely(!service)) {
    TR_ERROR("Failed to allocate %zu bytes for ECC service", sizeof(*service));
    return ATLK_E_OUT_OF_MEMORY;
  }

  service->transport = transport;
  atlk_atomic_set(&service->num_sockets, 0);

  *service_ptr = service;

  return ATLK_OK;
}

atlk_rc_t
ecc_service_delete(ecc_service_t *service)
{
  int num_sockets;

  if (atlk_unlikely(!service)) {
    TR_ERROR("ECC service is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Are there any sockets still active? */
  num_sockets = atlk_atomic_read(&service->num_sockets);
  if (num_sockets) {
    TR_ERROR("Cannot delete ECC service: there are still %d allocated sockets",
      num_sockets);
    return ATLK_E_INVALID_STATE;
  }

  /* Of course, at this point new sockets can be created nevertheless... */

  service->transport = NULL;
  free(service);

  return ATLK_OK;
}

atlk_rc_t
ecc_socket_create(ecc_service_t *service,
                  ecc_socket_t **socket_ptr)
{
  ecc_socket_t *socket = NULL;
  int transport_handle;
  atlk_rc_t rc = ATLK_OK;

  if (atlk_unlikely(!service)) {
    TR_ERROR("ECC service is unspecified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!socket_ptr)) {
    TR_ERROR("ECC socket is unspecified");
    return ATLK_E_INVALID_ARG;
  }

  /* Allocate socket */
  socket = malloc(sizeof(*socket));
  if (!socket) {
    TR_ERROR("Failed to allocate %zu bytes for ECC socket", sizeof(*socket));
    return ATLK_E_OUT_OF_MEMORY;
  }

  /* Allocate remote transport handle */
  rc = remote_transport_handle_allocate(service->transport, &transport_handle);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to allocate remote transport handle (rc=%d)", rc);
    goto error;
  }

  /* Initialize socket */
  socket->service = service;
  socket->transport_handle = transport_handle;

  /* Update service */
  atlk_atomic_inc(&service->num_sockets);

  /* Return socket */
  *socket_ptr = socket;

  return ATLK_OK;

error:
  free(socket);
  return rc;
}

atlk_rc_t
ecc_socket_delete(ecc_socket_t *socket)
{
  atlk_rc_t rc;

  if (atlk_unlikely(!socket)) {
    TR_ERROR("ECC socket is unspecified");
    return ATLK_E_INVALID_ARG;
  }

  /* Update service */
  atlk_atomic_dec(&socket->service->num_sockets);

  /* Free remote transport handle */
  rc = remote_transport_handle_deallocate(socket->service->transport,
                                          socket->transport_handle);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to free remote transport handle (rc=%d)", rc);
  }

  /* Free socket */
  free(socket);

  return ATLK_OK;
}

atlk_rc_t
ecc_request_send(ecc_socket_t *socket,
                 const ecc_request_t *request,
                 const atlk_wait_t *wait)
{
  const ecc_request_context_t *context;
  rsvc_ecc_request_msg_t msg;
  size_t size;
  atlk_rc_t rc;

  if (atlk_unlikely(!socket)) {
    TR_ERROR("ECC socket is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!request)) {
    TR_ERROR("ECC request is not specified");
    return ATLK_E_INVALID_ARG;
  }

  context = &request->context;
  if (!ecc_curve_valid(context->curve)) {
    TR_ERROR("Invalid ECC curve %d", context->curve);
    return ATLK_E_INVALID_ARG;
  }

  switch (context->request_type) {
    case ECC_REQUEST_TYPE_SIGN:
    case ECC_REQUEST_TYPE_VERIFY:
    case ECC_REQUEST_TYPE_PMA:
      break;
    default:
      TR_ERROR("Invalid request type %d", context->request_type);
      return ATLK_E_INVALID_ARG;
  }

  /* Set message header */
  rc = rsvc_protocol_msg_header_set(&msg.header,
                                    RSVC_ID_ECC,
                                    RSVC_ECC_SERVICE_REQUEST);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to encode message request (rc=%d)", rc);
    return rc;
  }

  /* Encode ECC request */
  rc = rsvc_ecc_request_encode(&msg.request, request);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to encode ECC request (rc=%d)", rc);
    return rc;
  }

  /* Send the request */
  size = sizeof(msg.header) + rsvc_ecc_request_size(context->request_type);
  rc = remote_transport_send(socket->service->transport,
                             socket->transport_handle,
                             ECC_RSVCD_SERVICE_PORT_NUM,
                             &msg,
                             &size);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to send PDU (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t
ecc_response_receive(ecc_socket_t *socket,
                     ecc_response_t *response,
                     const atlk_wait_t *wait)
{
  uint8_t pdu[RSVC_ECC_MAX_PDU_SIZE];
  rsvc_ecc_response_msg_t *msg = (rsvc_ecc_response_msg_t *)pdu;
  ecc_request_context_t *context;
  size_t size, expected_size;
  uint32_t timeout_ms;
  uint16_t remote_port;
  uint8_t service_id;
  uint16_t message_id;
  atlk_rc_t rc;

  if (atlk_unlikely(!socket)) {
    TR_ERROR("ECC socket is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!response)) {
    TR_ERROR("ECC response is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!wait)) {
    TR_ERROR("Wait specification is not specified");
    return ATLK_E_UNSUPPORTED;
  }

  switch (wait->wait_type) {
    case ATLK_WAIT_INTERVAL:
      timeout_ms = wait->wait_usec / 1000;
      break;
    case ATLK_WAIT_FOREVER:
      timeout_ms = REMOTE_TRANSPORT_NO_TIMEOUT;
      break;
    default:
      TR_ERROR("Invalid wait type %d", wait->wait_type);
      return ATLK_E_INVALID_ARG;
  }

  /* Receive from remote socket */
  size = sizeof(pdu);
  rc = remote_transport_receive(socket->service->transport,
                                socket->transport_handle,
                                pdu, &size, timeout_ms, &remote_port);
  if (atlk_error(rc)) {
    /* TODO: Why TR_DEBUG and not TR_ERROR? */
    TR_DEBUG("Failed to receive PDU (rc=%d)", rc);
    return rc;
  }

  /* Parse message header */
  rc = rsvc_protocol_msg_header_parse(&msg->header, &service_id, &message_id);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to parse message header (rc=%d)", rc);
    return rc;
  }

  if (service_id != RSVC_ID_ECC) {
    TR_ERROR("Unexpected service ID %d (!= expected %d)",
      service_id, RSVC_ID_ECC);
    return ATLK_E_PROTOCOL_ERROR;
  }

  if (message_id != RSVC_ECC_SERVICE_RESPONSE) {
    TR_ERROR("Unexpected message ID %d (!= expected %d)",
      message_id, RSVC_ECC_SERVICE_RESPONSE);
    return ATLK_E_PROTOCOL_ERROR;
  }

  /* Decode ECC response */
  rc = rsvc_ecc_response_decode(response, &msg->response);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to decode ECC response");
    return rc;
  }

  context = &response->context;
  expected_size = sizeof(msg->header) + \
                  rsvc_ecc_response_size(context->request_type);
  if (size != expected_size) {
    TR_ERROR("Unexpected message size %zu (!= expected %zu)",
      size, expected_size);
    return ATLK_E_PROTOCOL_ERROR;
  }

  return ATLK_OK;
}

