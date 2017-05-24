/* Copyright (C) 2014-2015 Autotalks Ltd. */
#ifndef _ATLK_V2X_H
#define _ATLK_V2X_H

#include <atlk/sdk.h>
#include <atlk/eui48.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   V2X API declarations
*/

/** Data rates in units of 500 kbit/s */
typedef enum {
  /** Data rate is N/A */
  V2X_DATARATE_NA = 0,

  /** 3 Mbit/s */
  V2X_DATARATE_3MBPS = 6,

  /** 4.5 Mbit/s */
  V2X_DATARATE_4_5MBPS = 9,

  /** 6 Mbit/s */
  V2X_DATARATE_6MBPS = 12,

  /** 9 Mbit/s */
  V2X_DATARATE_9MBPS = 18,

  /** 12 Mbit/s */
  V2X_DATARATE_12MBPS = 24,

  /** 18 Mbit/s */
  V2X_DATARATE_18MBPS = 36,

  /** 24 Mbit/s */
  V2X_DATARATE_24MBPS = 48,

  /** 27 Mbit/s */
  V2X_DATARATE_27MBPS = 54,

  /** 36 Mbit/s */
  V2X_DATARATE_36MBPS = 72,

  /** 48 Mbit/s */
  V2X_DATARATE_48MBPS = 96,

  /** 54 Mbit/s */
  V2X_DATARATE_54MBPS = 108

} v2x_datarate_t;

/** Power in units of dBm */
typedef int8_t v2x_power_dbm_t;

/** Value indicating that power in units of dBm is N/A */
#define V2X_POWER_DBM_NA INT8_MIN

/** Power in units of 1/8 dBm */
typedef int16_t v2x_power_dbm8_t;

/** Value indicating that power in units of 1/8 dBm is N/A */
#define V2X_POWER_DBM8_NA INT16_MIN

/** 1/8 dBm to dBm conversion factor */
#define V2X_POWER_DBM8_PER_DBM 8

/** Physical channel number */
typedef uint16_t v2x_channel_num_t;

/** Value indicating that physical channel number is N/A */
#define V2X_CHANNEL_NUM_NA 0

/** MAC interface index */
typedef uint8_t v2x_if_index_t;

/** Value indicating that MAC interface index is N/A */
#define V2X_IF_INDEX_NA 0

/** Minimum valid MAC interface index */
#define V2X_IF_INDEX_MIN 1

/** Maximum valid MAC interface index */
#define V2X_IF_INDEX_MAX 4

/** V2X MAC User Priority */
typedef int8_t v2x_user_priority_t;

/** V2X MAC User Priority minimum value */
#define V2X_USER_PRIORITY_MIN 0

/** V2X MAC User Priority maximum value */
#define V2X_USER_PRIORITY_MAX 7

/** Value indicating that V2X MAC User Priority is N/A */
#define V2X_USER_PRIORITY_NA INT8_MIN

/** V2X expiration time in millisecond */
typedef uint16_t v2x_expiry_time_ms_t;

/** Value indicating maximum allowed expiration time in milliseconds */
#define V2X_EXPIRY_TIME_MS_MAX 0x7FFF

/** Value indicating that V2X expiration time is N/A */ 
#define V2X_EXPIRY_TIME_MS_NA 0

/** V2X MAC frame type */
typedef enum  {
  /**
     Data frame.

     Implementation assumes that all data frames carry an LLC PDU of type UI
     (Unnumbered Information) that carries a SNAP PDU (i.e. LLC header octets
     are AA-AA-03).
  */
  V2X_FRAME_TYPE_DATA = 0,

  /** IEEE 802.11 vendor-specific action frame */
  V2X_FRAME_TYPE_VSA = 1

} v2x_frame_type_t;

/**
   V2X protocol descriptor.

   For example:
   @code{.c}
   static const v2x_protocol_t wsmp_protocol = {
     .frame_type = V2X_FRAME_TYPE_DATA,
     .protocol_id = 0x88dc
   };
   static const v2x_protocol_t wsa_protocol = {
     .frame_type = V2X_FRAME_TYPE_VSA,
     .protocol_id = 0x50c24a43
   };
   @endcode
*/
typedef struct {
  /** V2X MAC frame type */
  v2x_frame_type_t frame_type;

  /**
     Protocol identifier.

     If @p frame_type is v2x_frame_type_t::V2X_FRAME_TYPE_DATA then this is
     the 5-octet SNAP protocol identifer.

     If @p frame_type is v2x_frame_type_t::V2X_FRAME_TYPE_VSA then this is the
     organizational identifier (OUI-36) and 4 least-significant bits specified
     by the identified organization.
  */
  uint64_t protocol_id;

} v2x_protocol_t;

/** Default protocol descriptor initializer */
#define V2X_PROTOCOL_INIT {             \
  .frame_type = V2X_FRAME_TYPE_DATA,    \
  .protocol_id = UINT64_MAX             \
}

/**
   Value indicating the protocol ID used to create a V2X socket which can
   send/receive frames with data which includes layer 2 (i.e. a "raw" socket
   above layer 1).

   @note Only one socket of this type can be created per interface index
   and frame type. Standard V2X sockets cannot be opened when this type
   of socket is open (and vice-versa).
*/
#define V2X_PROTOCOL_ID_ANY 0ULL

/** V2X sample type */
typedef enum {
  /** Sample type is N/A */
  V2X_SAMPLE_TYPE_NA = 0,

  /**
    CBR (channel busy ratio) sample.

    Samples should be received via ::v2x_sample_int32_receive. Sample range
    and precision are the same as for ::mib_get_wlanChannelBusyRatio.
    */  
  V2X_SAMPLE_TYPE_CBR = 1,

} v2x_sample_type_t;

/** V2X service instance */
typedef struct v2x_service v2x_service_t;

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_V2X_H */
