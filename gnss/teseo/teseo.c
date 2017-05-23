/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include <platform/common.h>
#include <atlk/uart.h>
#include <atlk/gnss.h>
#include <atlk/gnss_internal.h>
#include <atlk/gnss_teseo.h>
#include <atlk/timestd.h>

#if defined __CRATON__
#include <atlk/nmea_debug.h>
#include <atlk/mib_service.h>
#include <atlk/mibs/nav-mib.h>
#include <craton/bootparam.h>
#endif /* __CRATON__ */

#include "teseo_fw_update.h"
#include "teseo_commands.h"

/* Expected cycle ender */
static const nmea_address_t cycle_ender = {
  .type = NMEA_TYPE_PARAMETRIC,
  .sentence_id = NMEA_TALKER_ID_NA,
  .formatter = NMEA_FORMATTER_GLL
};

/* Minimum speed for which heading is used for antenna offset calc */
#define ANTENNA_OFFSET_MIN_SPEED_MPS 5.0

atlk_rc_t atlk_must_check
teseo_init(const gnss_config_t *config,
            gnss_nmea_t *gnss_nmea)
{
  nmea_io_config_t io_config = NMEA_IO_CONFIG_INIT;
  nmea_nav_config_t nmea_nav_config = NMEA_NAV_CONFIG_INIT;
  int32_t leap_seconds = LEAP_SECONDS_FROM_2004JAN1_TO_2016DEC31;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!config || !gnss_nmea);

  /* Initilaize NMEA object */
  rc = nmea_init(&gnss_nmea->nmea);
  if (atlk_error(rc)) {
    TR_ERROR("NMEA init failed (rc=%d)", rc);
    return rc;
  }

#ifdef __CRATON__
  mib_service_t *mib_service = NULL;
  int32_t ant_offset_cm = 0;

  /* Get mib service */
  rc = mib_default_service_get(&mib_service);
  if (atlk_error(rc)) {
    TR_ERROR("mib_default_service_get: %s", atlk_rc_to_str(rc));
    return rc;
  }

  rc = mib_get_navSysTimeLeapSeconds(mib_service, &leap_seconds);
  if (atlk_error(rc)) {
    TR_ERROR("mib_get_navSysTimeLeapSeconds: %s", atlk_rc_to_str(rc));
    return rc;
  }

  rc = mib_get_navGnssAntennaOffsetX(mib_service, &ant_offset_cm);
  if (atlk_error(rc)) {
    TR_ERROR("mib_get_navGnssAntennaOffsetX: %s", atlk_rc_to_str(rc));
    return rc;
  }
  nmea_nav_config.ant_offset.x_m = (double)ant_offset_cm / CENTI_PER_UNIT;

  rc = mib_get_navGnssAntennaOffsetY(mib_service, &ant_offset_cm);
  if (atlk_error(rc)) {
    TR_ERROR("mib_get_navGnssAntennaOffsetY: %s", atlk_rc_to_str(rc));
    return rc;
  }
  nmea_nav_config.ant_offset.y_m = (double)ant_offset_cm / CENTI_PER_UNIT;

  rc = mib_get_navGnssAntennaOffsetZ(mib_service, &ant_offset_cm);
  if (atlk_error(rc)) {
    TR_ERROR("mib_get_navGnssAntennaOffsetZ: %s", atlk_rc_to_str(rc));
    return rc;
  }
  nmea_nav_config.ant_offset.z_m = (double)ant_offset_cm / CENTI_PER_UNIT;

  mib_service_delete(mib_service);
#endif /* __CRATON__ */

  /* Set NMEA navigation configuration parameters */
  nmea_nav_config.nmea = &gnss_nmea->nmea;
  nmea_nav_config.utc_offset_sec =
    leap_seconds + LEAP_SECONDS_FROM_GPS_1980JAN6_TO_TAI_2004JAN1;
  nmea_nav_config.cycle_ender = cycle_ender;
  nmea_nav_config.min_speed_mps = ANTENNA_OFFSET_MIN_SPEED_MPS;
  nmea_nav_config.handler = config->handler;
  nmea_nav_config.service = config->service;

  /* Initilaize NMEA navigation object */
  rc = nmea_nav_init(&gnss_nmea->nmea_nav, &nmea_nav_config);
  if (atlk_error(rc)) {
    TR_ERROR("NMEA-Nav init failed (rc=%d)", rc);
    return rc;
  }

  /* Set NMEA I/O configuration parameters */
  io_config.dev_name = config->dev_name;
  io_config.speed_bps = config->nmea_speed_bps;
  io_config.sched_params = config->sched_params;
  io_config.handler = nmea_sentence_process;
  io_config.nmea = &gnss_nmea->nmea;

  /* Open NMEA I/O */
  rc = nmea_io_open(&gnss_nmea->nmea_io, &io_config);
  if (atlk_error(rc)) {
    TR_ERROR("NMEA I/O open failed (rc=%d)", rc);
    return rc;
  }

  /* Initilaize Teseo FW update */
  rc = teseo_fw_update_init(gnss_nmea, config->hw_reset);
  if (atlk_error(rc)) {
    TR_ERROR("Teseo FW update init failed (rc=%d)", rc);
    return rc;
  }

  /* Initialize Teseo commands */
  rc = teseo_commands_init(&gnss_nmea->nmea, config->hw_reset);
  if (atlk_error(rc)) {
    TR_ERROR("Teseo commands init failed (rc=%d)", rc);
    return rc;
  }

#ifdef __CRATON__
  /* Initilaize NMEA UDP forwarding */
  rc = nmea_udp_forward_init(&gnss_nmea->nmea_io, NULL);
  if (atlk_error(rc)) {
    TR_DEBUG("NMEA UDP forward init failed (rc=%d)", rc);
    rc = ATLK_OK;
  }
#endif /* __CRATON__ */

  /* Start NMEA input polling */
  rc = nmea_io_input_poll_start(&gnss_nmea->nmea_io);
  if (atlk_error(rc)) {
    TR_ERROR("NMEA input polling start failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}
