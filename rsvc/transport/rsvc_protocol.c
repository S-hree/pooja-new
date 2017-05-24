/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdlib.h>
#include <string.h>

#include <atlk/bpool.h>
#include <atlk/compiler.h>
#include <atlk/sdk.h>
#include <atlk/remote.h>

#include <atlk/mib_service.h>
#include <atlk/mib_remote.h>
#include <atlk/mibs/rsvc-mib.h>

#include <atlk/v2x_service.h>
#include <atlk/v2x_remote.h>
#include <atlk/dx.h>
#include <atlk/rsvc_protocol.h>
#include <atlk/rsvc_v2x_protocol.h>

/**
   Verifies that all pointers in rsvc_protocol_api_send_param_t are not NULL

   @param[in] param A parameters struct for SEND API

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_inline atlk_rc_t atlk_must_check
rsvc_protocol_send_param_valid(const rsvc_protocol_api_send_param_t *param)
{
  if (atlk_unlikely(!param)) {
    TR_ERROR("param is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!param->message_id)) {
    TR_ERROR("message_id is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!param->dest_v2x_socket_index)) {
    TR_ERROR("dest_v2x_socket_index is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!param->payload_size)) {
    TR_ERROR("payload_size is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!param->params)) {
    TR_ERROR("param->params is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  return ATLK_OK;
}

/**
   Verifies that all pointers in rsvc_protocol_api_receive_param_t are not NULL

   @param[in] param A parameters struct for RECEIVE API

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_inline atlk_rc_t atlk_must_check
rsvc_protocol_recv_param_valid(const rsvc_protocol_api_receive_param_t *param)
{
  if (atlk_unlikely(!param)) {
    TR_ERROR("param is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!param->message_id)) {
    TR_ERROR("message_id is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!param->payload_size)) {
    TR_ERROR("payload_size is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!param->params)) {
    TR_ERROR("param->params is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
rsvc_protocol_send_request_encode(rsvc_service_id_t service_id,
                               dx_rsvc_v2x_send_req_header_t *send_req_hdr,
                               const rsvc_protocol_api_send_param_t *param_in)
{
  atlk_rc_t rc;
  uint16_t message_id;
  uint8_t dest_v2x_socket_index;
  size_t payload_size;
  const v2x_send_params_t *params = NULL;

  /* is valid params struct? */
  rc = rsvc_protocol_send_param_valid(param_in);
  if (atlk_error(rc)) {
    TR_ERROR("Invalid param_in");
    return rc;
  }

  message_id = *param_in->message_id;
  dest_v2x_socket_index = *param_in->dest_v2x_socket_index;
  payload_size = *param_in->payload_size;
  params = param_in->params;

  if (atlk_unlikely(!send_req_hdr)) {
    TR_ERROR("send_req_hdr is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  /* Initialize msg_header */
  rsvc_protocol_magic_set(&send_req_hdr->header.magic);
  send_req_hdr->header.version = RSVC_REMOTE_PROTOCOL_VERSION;
  send_req_hdr->header.reserved = 0;
  send_req_hdr->header.service_id = service_id;
  dx_uint16le_set(&send_req_hdr->header.message_id, message_id);
  dx_uint16le_set(&send_req_hdr->header.msg_version, 0);

  /* Initialize send_req header */
  dx_uint32le_set(&send_req_hdr->v2x_data_length, (uint32_t)payload_size);

  /* Initialize send params */
  memcpy(&send_req_hdr->v2x_send_params.source_address.octets,
         &params->source_address.octets,
         EUI48_LEN);
  memcpy(&send_req_hdr->v2x_send_params.dest_address.octets,
         &params->dest_address.octets,
         EUI48_LEN);
  dx_uint32le_set(&send_req_hdr->v2x_send_params.datarate,
                  params->datarate);
  send_req_hdr->v2x_send_params.user_priority = params->user_priority;
  dx_uint16le_set(&send_req_hdr->v2x_send_params.power_dbm8,
                  (uint16_t)params->power_dbm8);

  dx_uint16le_set(&send_req_hdr->v2x_send_params.expiry_time_ms,
                  (uint16_t)params->expiry_time_ms);

  send_req_hdr->dest_v2x_socket_index = dest_v2x_socket_index;

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
rsvc_protocol_send_request_decode(rsvc_service_id_t service_id,
                             rsvc_protocol_api_send_param_t *params_out,
                             const dx_rsvc_v2x_send_req_header_t *send_req_hdr)
{
  atlk_rc_t rc;
  uint32_t payload_size_u32;
  uint32_t datarate_u32;
  uint8_t *dest_v2x_socket_index = NULL;
  size_t *payload_size = NULL;
  uint16_t *message_id = NULL;
  uint16_t msg_version = 0;
  v2x_send_params_t *params = NULL;

  if (!params_out) {
    TR_ERROR("params_out is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  /* is valid params_out struct? */
  rc = rsvc_protocol_send_param_valid(params_out);
  if (atlk_error(rc)) {
    TR_ERROR("Invalid params_out");
    return rc;
  }

  dest_v2x_socket_index = params_out->dest_v2x_socket_index;
  payload_size = params_out->payload_size;
  message_id = params_out->message_id;
  params = params_out->params;

  /* Check msg_header */
  if (!rsvc_protocol_magic_is_valid(&send_req_hdr->header.magic)) {
    TR_ERROR("Wrong protocol magic");
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  if (send_req_hdr->header.version != RSVC_REMOTE_PROTOCOL_VERSION) {
    TR_ERROR("Incompatible protocol version: %d",
             send_req_hdr->header.version);
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  if (send_req_hdr->header.service_id != service_id) {
    TR_ERROR("Wrong service ID: %d", send_req_hdr->header.service_id);
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  *dest_v2x_socket_index = send_req_hdr->dest_v2x_socket_index;

  dx_uint16le_get(&send_req_hdr->header.message_id, message_id);

  dx_uint16le_get(&send_req_hdr->header.msg_version, &msg_version);
  if (msg_version != 0) {
    TR_ERROR("Incompatible message version: %d",
             msg_version);
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  /* Get payload size */
  dx_uint32le_get(&send_req_hdr->v2x_data_length, &payload_size_u32);
  *payload_size = (size_t)payload_size_u32;

  /* Initialize send params */
  memcpy(&params->source_address.octets,
         &send_req_hdr->v2x_send_params.source_address.octets,
         EUI48_LEN);

  memcpy(&params->dest_address.octets,
         &send_req_hdr->v2x_send_params.dest_address.octets,
         EUI48_LEN);
  dx_uint32le_get(&send_req_hdr->v2x_send_params.datarate,
                  &datarate_u32);
  params->datarate = (v2x_datarate_t)datarate_u32;

  params->user_priority = send_req_hdr->v2x_send_params.user_priority;

  dx_uint16le_get(&send_req_hdr->v2x_send_params.power_dbm8,
                  (uint16_t *)&params->power_dbm8);

  dx_uint16le_get(&send_req_hdr->v2x_send_params.expiry_time_ms,
                  (uint16_t *)&params->expiry_time_ms);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
rsvc_protocol_receive_ind_encode(rsvc_service_id_t service_id,
                         dx_rsvc_v2x_receive_indication_header_t *recv_ind_hdr,
                         const rsvc_protocol_api_receive_param_t *param_in)
{
  atlk_rc_t rc;
  uint32_t receive_time_us_msb;
  uint32_t receive_time_us_lsb;
  uint64_t receive_time_us;
  uint16_t message_id;
  size_t payload_size;
  v2x_receive_params_t *params = NULL;

  /* is valid params_out struct? */
  rc = rsvc_protocol_recv_param_valid(param_in);
  if (atlk_error(rc)) {
    TR_ERROR("Invalid param_in");
    return rc;
  }

  message_id = *param_in->message_id;
  payload_size = *param_in->payload_size;
  params = param_in->params;

  if (!recv_ind_hdr) {
    TR_ERROR("recv_ind_hdr is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  /* Initialize msg_header */
  rsvc_protocol_magic_set(&recv_ind_hdr->header.magic);
  recv_ind_hdr->header.version = RSVC_REMOTE_PROTOCOL_VERSION;
  recv_ind_hdr->header.reserved = 0;
  recv_ind_hdr->header.service_id = service_id;
  dx_uint16le_set(&recv_ind_hdr->header.message_id, (uint32_t)message_id);
  dx_uint16le_set(&recv_ind_hdr->header.msg_version, 0);

  /* Initialize receive_ind header */
  dx_uint32le_set(&recv_ind_hdr->v2x_data_length, (uint32_t)payload_size);

  /* Initialize receive params */
  memcpy(&recv_ind_hdr->v2x_recv_params.source_address.octets,
         &params->source_address.octets,
         EUI48_LEN);
  memcpy(&recv_ind_hdr->v2x_recv_params.dest_address.octets,
         &params->dest_address.octets,
         EUI48_LEN);

  receive_time_us = params->receive_time_us;
  receive_time_us_msb = (uint32_t)(receive_time_us >> 32);
  receive_time_us_lsb = (uint32_t)(receive_time_us & 0xFFFFFFFF);
  dx_uint32le_set(&recv_ind_hdr->v2x_recv_params.receive_time_us_msb,
                  receive_time_us_msb);
  dx_uint32le_set(&recv_ind_hdr->v2x_recv_params.receive_time_us_lsb,
                  receive_time_us_lsb);

  dx_uint32le_set(&recv_ind_hdr->v2x_recv_params.datarate,
                  params->datarate);
  recv_ind_hdr->v2x_recv_params.user_priority = params->user_priority;
  dx_uint16le_set(&recv_ind_hdr->v2x_recv_params.power_dbm8,
                  (uint16_t)params->power_dbm8);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
rsvc_protocol_receive_ind_decode(rsvc_service_id_t service_id,
                  rsvc_protocol_api_receive_param_t *params_out,
                  const dx_rsvc_v2x_receive_indication_header_t *recv_ind_hdr)
{
  atlk_rc_t rc;
  uint32_t payload_size_u32;
  uint32_t receive_time_us_msb;
  uint32_t receive_time_us_lsb;
  uint32_t datarate_u32;
  v2x_receive_params_t *params = NULL;
  size_t *payload_size = NULL;
  uint16_t *message_id = NULL;
  uint16_t msg_version = 0;

  /* is valid params_out struct? */
  rc = rsvc_protocol_recv_param_valid(params_out);
  if (atlk_error(rc)) {
    TR_ERROR("Invalid params_out");
    return rc;
  }

  if (!recv_ind_hdr) {
    TR_ERROR("recv_ind_hdr is a NULL pointer");
    return ATLK_E_INVALID_ARG;
  }

  params = params_out->params;
  payload_size = params_out->payload_size;
  message_id = params_out->message_id;

  /* Check msg_header */
  if (!rsvc_protocol_magic_is_valid(&recv_ind_hdr->header.magic)) {
    TR_ERROR("Wrong protocol magic");
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  if (recv_ind_hdr->header.version != RSVC_REMOTE_PROTOCOL_VERSION) {
    TR_ERROR("Incompatible protocol version: %d",
             recv_ind_hdr->header.version);
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  if (recv_ind_hdr->header.service_id != service_id) {
    TR_ERROR("Wrong service ID: %d", recv_ind_hdr->header.service_id);
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  dx_uint16le_get(&recv_ind_hdr->header.message_id, message_id);

  dx_uint16le_get(&recv_ind_hdr->header.msg_version, &msg_version);
  if (msg_version != 0) {
    TR_ERROR("Incompatible message version: %d",
             msg_version);
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  /* Get payload size */
  dx_uint32le_get(&recv_ind_hdr->v2x_data_length, &payload_size_u32);
  *payload_size = (size_t)payload_size_u32;

  /* Initialize receive params */
  memcpy(&params->source_address.octets,
         &recv_ind_hdr->v2x_recv_params.source_address.octets,
         EUI48_LEN);

  memcpy(&params->dest_address.octets,
         &recv_ind_hdr->v2x_recv_params.dest_address.octets,
         EUI48_LEN);

  dx_uint32le_get(&recv_ind_hdr->v2x_recv_params.receive_time_us_msb,
                  &receive_time_us_msb);
  dx_uint32le_get(&recv_ind_hdr->v2x_recv_params.receive_time_us_lsb,
                  &receive_time_us_lsb);
  params->receive_time_us = (uint64_t)receive_time_us_lsb |
                            ((uint64_t)receive_time_us_msb << 32);

  dx_uint32le_get(&recv_ind_hdr->v2x_recv_params.datarate,
                  &datarate_u32);
  params->datarate = (v2x_datarate_t)datarate_u32;

  params->user_priority = recv_ind_hdr->v2x_recv_params.user_priority;
  dx_uint16le_get(&recv_ind_hdr->v2x_recv_params.power_dbm8,
                  (uint16_t *)&params->power_dbm8);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
rsvc_protocol_msg_header_set(dx_rsvc_msg_header_t *msg_header,
                             uint8_t service_id,
                             uint16_t message_id)
{
  if (atlk_unlikely(!msg_header)) {
    TR_ERROR("Error, invalid argument");
    return ATLK_E_INVALID_ARG;
  }

  /* Initialize msg_header */
  rsvc_protocol_magic_set(&msg_header->magic);
  msg_header->version = RSVC_REMOTE_PROTOCOL_VERSION;
  msg_header->reserved = 0;
  msg_header->service_id = service_id;
  dx_uint16le_set(&msg_header->message_id, message_id);
  dx_uint16le_set(&msg_header->msg_version, 0);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
rsvc_protocol_msg_header_parse(dx_rsvc_msg_header_t *msg_header,
                               uint8_t *service_id,
                               uint16_t *message_id)
{
  uint16_t msg_version = 0;

  if (atlk_unlikely(!msg_header)) {
    TR_ERROR("Error, invalid argument");
    return ATLK_E_INVALID_ARG;
  }

  /* Check msg_header */
  if (!rsvc_protocol_magic_is_valid(&msg_header->magic)) {
    TR_ERROR("Wrong protocol magic");
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  if (msg_header->version != RSVC_REMOTE_PROTOCOL_VERSION) {
    TR_ERROR("Incompatible protocol version: %d", msg_header->version);
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  *service_id = msg_header->service_id;
  dx_uint16le_get(&msg_header->message_id, message_id);

  dx_uint16le_get(&msg_header->msg_version, &msg_version);
  if (msg_version != 0) {
    TR_ERROR("Incompatible message version: %d",
             msg_version);
    return ATLK_E_PROTOCOL_MISMATCH;
  }

  return ATLK_OK;
}
