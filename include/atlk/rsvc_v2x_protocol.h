/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _REMOTE_V2X_PROTOCOL_H
#define _REMOTE_V2X_PROTOCOL_H

#include <atlk/eui48.h>
#include <atlk/v2x_service.h>
#include <atlk/ecc_service.h>
#include <atlk/rsvc_protocol.h>
#include <atlk/rsvc_v2x_protocol.h>

/* Constant local port number at the target side */
#define V2X_RSVCD_SERVICE_PORT_NUM 63000

/* Max number of remote sockets */
#define V2X_REMOTE_NUM_SOCKETS_MAX      8

typedef enum {
  RSVC_V2X_SERVICE_RESERVED_0               = 0x00,
  RSVC_V2X_SERVICE_RESERVED_1               = 0x01,

  RSVC_V2X_SERVICE_SOCKET_SEND_REQ          = 0x02,
  RSVC_V2X_SERVICE_SOCKET_RECV_IND          = 0x03,
} rsvc_v2x_msg_id_t;

/** 48-bit Extended Unique Identifier */
typedef dx_struct {
  uint8_t octets[EUI48_LEN];
} dx_rsvc_eui48_t;

/** V2X send parameters */
typedef dx_struct {
  /**
     Source MAC address

     If not set to all zeros (::EUI48_ZERO_INIT) and wlanTxSaOverrideEnabled is
     set to "true", the value of this field will determine the MAC frame's
     source address. Otherwise it has no effect.
  */
  dx_rsvc_eui48_t source_address;

  /** Destination MAC address */
  dx_rsvc_eui48_t dest_address;

  /** Transmission data rate */
  dx_uint32le_t datarate; /* v2x_datarate_t */

  /**
     MAC User Priority

     @todo Currently unsupported.
  */
  uint8_t user_priority; /* v2x_user_priority_t */

  /** Transmission power level in units of 1/8 dBm */
  dx_uint16le_t power_dbm8; /* v2x_power_dbm8_t */

  /** expiry_time_ms */
  dx_uint16le_t expiry_time_ms;

} dx_rsvc_v2x_rsvc_send_params_t;


/** V2X receive parameters */
typedef dx_struct {
  /** Source MAC address */
  dx_rsvc_eui48_t source_address;

  /** Destination MAC address */
  dx_rsvc_eui48_t dest_address;

  /**
     Receive time in microseconds

     Format: number of TAI microseconds since 2004-01-01T00:00:00Z (UTC).
  */
  dx_uint32le_t receive_time_us_msb;
  dx_uint32le_t receive_time_us_lsb;

  /**
     Data rate

     @todo Currently unsupported.
  */
  dx_uint32le_t datarate; /* v2x_datarate_t */

  /**
     MAC User Priority

     @todo Currently unsupported.
  */
  uint8_t user_priority; /* v2x_user_priority_t */

  /**
     Physical channel number

     @todo Currently unsupported.
  */
  dx_uint16le_t channel_num; /* v2x_channel_num_t */

  /** Average input power of frame in units of 1/8 dBm */
  dx_uint16le_t power_dbm8; /* v2x_power_dbm8_t */

} dx_rsvc_v2x_receive_params_t;

typedef dx_struct {
  /* Message header */
  dx_rsvc_msg_header_t           header;

  /* Destination V2X Socket index on the target */
  uint8_t dest_v2x_socket_index;

  /* Data length */
  dx_uint32le_t                  v2x_data_length;

  /* V2X send params */
  dx_rsvc_v2x_rsvc_send_params_t v2x_send_params;

} dx_rsvc_v2x_send_req_header_t;

/*
 * Note: Actual SEND message will look like:
 * {
 *    dx_v2x_send_req_header_t header;
 *    uint8_t                  v2x_send_raw_data[];
 *
 * */

typedef dx_struct {
  /* Message header */
  dx_rsvc_msg_header_t         header;

  /* Data length */
  dx_uint32le_t                v2x_data_length;

  /* V2X receive params */
  dx_rsvc_v2x_receive_params_t v2x_recv_params;

} dx_rsvc_v2x_receive_indication_header_t;

/*
 * Note: Actual RECV message will look like:
 * {
 *    dx_v2x_recv_ind_header_t header;
 *    uint8_t                  v2x_recv_raw_data[];
 *
 * */

/**
   Encode a V2X socket Send-request.

   @param[in] service_id remote service ID.
   @param[out] send_req_hdr Send request header (encoded).
   @param[in] param_in params in.

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
rsvc_protocol_send_request_encode(rsvc_service_id_t service_id,
                               dx_rsvc_v2x_send_req_header_t *send_req_hdr,
                               const rsvc_protocol_api_send_param_t *param_in);

/**
   Decode a V2X socket Send-request.

   @param[in] service_id remote service ID.
   @param[out] params V2X send params (Decoded).
   @param[in] send_req_hdr Send request header (encoded).

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
rsvc_protocol_send_request_decode(rsvc_service_id_t service_id,
                            rsvc_protocol_api_send_param_t *params_out,
                            const dx_rsvc_v2x_send_req_header_t *send_req_hdr);

/**
   Encode a V2X socket receive indication message.

   @param[in] service_id remote service ID
   @param[out] recv_ind_hdr receive indication header (encoded)
   @param[in] param_in params in (dest_v2x_socket_index field == NULL).

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
rsvc_protocol_receive_ind_encode(rsvc_service_id_t service_id,
                         dx_rsvc_v2x_receive_indication_header_t *recv_ind_hdr,
                         const rsvc_protocol_api_receive_param_t *param_in);

/**
   Decode a V2X socket receive indication message.

   @param[in] service_id remote service ID
   @param[out] params_out params out
   @param[in] recv_ind_hdr receive indication header (encoded)

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
rsvc_protocol_receive_ind_decode(rsvc_service_id_t service_id,
                  rsvc_protocol_api_receive_param_t *params_out,
                  const dx_rsvc_v2x_receive_indication_header_t *recv_ind_hdr);

#endif /* _REMOTE_V2X_PROTOCOL_H */
