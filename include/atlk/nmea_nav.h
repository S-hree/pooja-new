/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _ATLK_NMEA_NAV_H
#define _ATLK_NMEA_NAV_H

#include <atlk/sdk.h>
#include <atlk/nav.h>
#include <atlk/nmea.h>
#include <atlk/coord.h>

#ifdef __cplusplus
extern "C" {
#endif

/** UTC to GPS time offset status */
typedef enum {
  /** UTC offset is not valid */
  NMEA_UTC_OFFSET_STATUS_NOT_VALID = 0,

  /** UTC offset is the firmware default value */
  NMEA_UTC_OFFSET_STATUS_FIRMWARE_DEFAULT,

  /** UTC offset was read from NVM */
  NMEA_UTC_OFFSET_STATUS_READ_FROM_NVM,

  /** UTC offset was set by user */
  NMEA_UTC_OFFSET_STATUS_SET_BY_USER,

  /** UTC offset was downloaded from sky */
  NMEA_UTC_OFFSET_STATUS_VALID,

  /** UTC offset is not available */
  NMEA_UTC_OFFSET_STATUS_NA

} nmea_utc_offset_status_t;

/** NMEA timestamp */
typedef struct {
  /** UTC time */
  nmea_time_t utc_time;

  /** UTC date */
  nmea_date_t utc_date;

  /** UTC to GPS time offset in seconds */
  nmea_int_t utc_offset_sec;

  /** UTC to GPS time offset status */
  nmea_utc_offset_status_t utc_offset_status;
  
} nmea_timestamp_t;

/** NMEA timestamp default initializer */
#define NMEA_TIMESTAMP_INIT {                       \
  .utc_time = NMEA_TIME_NA,                         \
  .utc_date = NMEA_DATE_NA,                         \
  .utc_offset_sec = NMEA_INT_NA ,                   \
  .utc_offset_status = NMEA_UTC_OFFSET_STATUS_NA    \
}

/** NMEA navigation object */
typedef struct {
  /** NMEA cycle ender */
  nmea_address_t cycle_ender;

  /** Navigation fix data frame */
  nav_fix_t fix;

  /** Last known NMEA timestamp */
  nmea_timestamp_t timestamp;

  /** GNSS antenna offset */
  coord_ego_t ant_offset;

  /** Last known "good" heading in degrees */
  double last_heading_deg;

  /** Minimum speed for which heading is considered as "good" */
  double min_speed_mps;

  /** Whether TG sentence is used */
  int stm_tg_used;

  /** Navigation fix handler */
  nav_fix_handler_t handler;

  /** Navigation service */
  nav_service_t *service;

} nmea_nav_t;

/** NMEA navigation object default initializer */
#define NMEA_NAV_INIT {                 \
  .cycle_ender = NMEA_ADDRESS_INIT,     \
  .fix = NAV_FIX_INIT,                  \
  .timestamp = NMEA_TIMESTAMP_INIT,     \
  .ant_offset = COORD_EGO_ZERO_INIT,    \
  .last_heading_deg = NAN,              \
  .min_speed_mps = NAN,                 \
  .stm_tg_used = 0,                     \
  .handler = NULL,                      \
  .service = NULL,                      \
}

/** NMEA navigation object configuration parameters */
typedef struct {
  /** Pointer to NMEA object */
  nmea_t *nmea;

  /** UTC offset in seconds */
  nmea_int_t utc_offset_sec;

  /**
    Cycle ender address.

    @note When @p cycle_ender is of type ::NMEA_TYPE_PARAMETRIC and talker ID is
    set to ::NMEA_TALKER_ID_NA, talker ID is ignored (i.e. "don't care").
  */
  nmea_address_t cycle_ender;

  /** GNSS antenna offset */
  coord_ego_t ant_offset;

  /** Minimum speed for which heading is considered as "good" */
  double min_speed_mps;

  /** Navigation fix handler */
  nav_fix_handler_t handler;

  /** Navigation service */
  nav_service_t *service;

} nmea_nav_config_t;

/** NMEA navigation object configuration parameters default initializer */
#define NMEA_NAV_CONFIG_INIT {          \
  .nmea = NULL,                         \
  .utc_offset_sec = NMEA_INT_NA,        \
  .cycle_ender = NMEA_ADDRESS_INIT,     \
  .ant_offset = COORD_EGO_INIT,         \
  .min_speed_mps = NAN,                 \
  .handler = NULL,                      \
  .service = NULL                       \
}

/**
   Initilaize NMEA navigation object.

   @param[out] nmea_nav NMEA navigation object to init
   @param[in] config NMEA navigation object configuration parameters

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_nav_init(nmea_nav_t *nmea_nav,
            const nmea_nav_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_NMEA_NAV_H */
