/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _RSVC_PROTOCOL_H
#define _RSVC_PROTOCOL_H

#include <atlk/dx.h>
#include <atlk/v2x_service.h>
#include <atlk/ecc_service.h>

#define RSVC_REMOTE_PROTOCOL_VERSION  3

#define RSVC_PROTOCOL_MAGIC_BYTE_0 0x41
#define RSVC_PROTOCOL_MAGIC_BYTE_1 0x54
#define RSVC_PROTOCOL_MAGIC_BYTE_2 0x4B

typedef enum {
  RSVC_ID_CONTROL = 0x00,
  RSVC_ID_V2X = 0x01,
  RSVC_ID_HSM = 0x02,
  RSVC_ID_ECC = 0x03
} rsvc_service_id_t;

typedef dx_struct {
  uint8_t value[3];
} dx_rsvc_magic_t;

typedef enum {
  RSVC_MESSAGE_RESERVED1         = 0x00,
  RSVC_MESSAGE_RESERVED2         = 0x01,

} rsvc_message_id_t;

/** Generic message header */
typedef dx_struct {
  /** Magic of the protocol = ATLK_REMOTE_PROTOCOL_MAGIC_SET */
  dx_rsvc_magic_t magic;

  /** version of the protocol = ATLK_REMOTE_PROTOCOL_VERSION */
  uint8_t version;

  /** Reserved field (FFU -- e.g. security type) */
  uint8_t reserved;

  /** Remote Service Id, defined in typedef rsvc_service_id_t*/
  uint8_t service_id;

  /** Remote Function Id, defined for each service type in:
    - export/include/atlk/rsvc_v2x_protocol.h
    - export/include/atlk/rsvc_ecc_protocol.h
    - export/include/atlk/rsvc_hsm_protocol.h
   *   */
  dx_uint16le_t message_id;

  /** Message version */
  dx_uint16le_t msg_version;

} dx_rsvc_msg_header_t;

typedef dx_struct {
  /** Message header */
  dx_rsvc_msg_header_t header;

} dx_rsvc_req_t;

/** Error response */
typedef dx_struct {
  dx_rsvc_msg_header_t header;
  dx_uint32le_t error_code;
} dx_rsvc_error_rsp_t;

/* Parameters for rsvc_protocol_receive_ind_decode() */
typedef struct {
  /* RSVC message ID */
  uint16_t *message_id;

  /* Destination V2X socket index */
  uint8_t *dest_v2x_socket_index;

  /* Payload size */
  size_t *payload_size;

  /* V2X Params */
  v2x_send_params_t *params;

} rsvc_protocol_api_send_param_t;

/* Parameters for rsvc_protocol_receive_ind_decode() */
typedef struct {
  /* Pointer to params */
  v2x_receive_params_t *params;

  /* Pointer to payload_size */
  size_t *payload_size;

  /* Pointer to message_id */
  uint16_t *message_id;

} rsvc_protocol_api_receive_param_t;

/**
   Sets the RSVC protocol magic

   @param[out] A dx_rsvc_magic_t structure

   @retval ::None
   @return Error code if failed
*/
atlk_inline
void rsvc_protocol_magic_set(dx_rsvc_magic_t *magic)
{
  /** 0x41, 0x54, 0x4B ==> "ATK" */
  magic->value[0] = RSVC_PROTOCOL_MAGIC_BYTE_0;
  magic->value[1] = RSVC_PROTOCOL_MAGIC_BYTE_1;
  magic->value[2] = RSVC_PROTOCOL_MAGIC_BYTE_2;
}

/**
   Verifies the RSVC protocol magic

   @param[out] A dx_rsvc_magic_t structure

   @retval :: Valid => 1 / Invalid => 0.
   @return Error code if failed
*/
atlk_inline atlk_must_check
int rsvc_protocol_magic_is_valid(const dx_rsvc_magic_t *magic)
{
  return ((magic->value[0] == RSVC_PROTOCOL_MAGIC_BYTE_0) &&
          (magic->value[1] == RSVC_PROTOCOL_MAGIC_BYTE_1) &&
          (magic->value[2] == RSVC_PROTOCOL_MAGIC_BYTE_2));
}


/**
   Generates a dx_rsvc_msg_header_t msg_header

   @param[out] msg_header message header
   @param[in] service_id message header
   @param[in] message_id message header

   @retval :: ATLK_OK if OK
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
rsvc_protocol_msg_header_set(dx_rsvc_msg_header_t *msg_header,
                             uint8_t service_id,
                             uint16_t message_id);
/**
   Parses a dx_rsvc_msg_header_t msg_header

   @param[in] msg_header message header
   @param[out] service_id message header
   @param[out] message_id message header

   @retval :: ATLK_OK if OK
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
rsvc_protocol_msg_header_parse(dx_rsvc_msg_header_t *msg_header,
                               uint8_t *service_id,
                               uint16_t *message_id);


#endif /* _RSVC_PROTOCOL_H */
