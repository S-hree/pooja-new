/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _ATLK_GNSS_INTERNAL_H
#define _ATLK_GNSS_INTERNAL_H

#include <atlk/sdk.h>
#include <atlk/nmea_io.h>
#include <atlk/nmea.h>
#include <atlk/nmea_nav.h>
#include <atlk/coord.h>

#ifdef __cplusplus
extern "C" {
#endif

/** GNSS NMEA objects */
typedef struct {
  /** NMEA I/O object */
  nmea_io_t nmea_io;

  /** NMEA object */
  nmea_t nmea;

  /** NMEA navigation object */
  nmea_nav_t nmea_nav;

} gnss_nmea_t;

/** GNSS NMEA objects default initializer */
#define GNSS_NMEA_INIT {    \
  .nmea_io = NMEA_IO_INIT,  \
  .nmea = NMEA_INIT,        \
  .nmea_nav = NMEA_NAV_INIT \
}

/**
   Start polling GNSS input.

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
gnss_input_poll_start(void);

/**
   Stop polling GNSS input.

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
gnss_input_poll_stop(void);

/**
   Whether polling GNSS input is enabled.

   @return 1 if polling is enabled, 0 otherwise.
*/
int
gnss_input_poll_enabled(void);

/**
   Get GNSS status.

   @param[out] value NMEA I/O status value

   @retval ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
gnss_stats_get(nmea_io_stats_t *value);

/**
   Reset GNSS status.

   @retval ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
gnss_stats_reset(void);

/**
   Set GNSS antenna offset.

   @note To disable antenna offset set it to ::COORD_EGO_ZERO_INIT.

   @param[in] ant_offset GNSS antenna offset

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
gnss_antenna_offset_set(const coord_ego_t *ant_offset);

/**
   Get GNSS antenna offset.

   @param[out] ant_offset GNSS antenna offset

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
gnss_antenna_offset_get(coord_ego_t *ant_offset);

/**
   Get pointer to NMEA I/O object.

   @return Pointer to NMEA I/O object or NULL.
*/
nmea_io_t *
gnss_nmea_io_get(void);

#ifdef __CRATON__
#include <craton/cli.h>

/**
   Init GNSS CLI.

   @param[in,out] cli CLI instance

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t
gnss_cli_init(cli_instance_t *cli);
#endif /* __CRATON__ */

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_GNSS_INTERNAL_H */
