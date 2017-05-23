/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdlib.h>
#include <string.h>

#include <platform/common.h>
#include <atlk/gnss.h>
#include <atlk/gnss_internal.h>
#include <atlk/gnss_teseo.h>
#include <atlk/gnss_teseo_internal.h>

#ifdef __CRATON__
#include <atlk/gnss_max_internal.h>
#include <atlk/gnss_wd.h>
#include <atlk/nmea.h>
#include <atlk/nmea_internal.h>
#include <craton/bootparam.h>
#endif /* __CRATON__ */

#include "gnss_vtable.h"

/* Default GNSS wait time - currently 1 Second */
const atlk_wait_t gnss_default_wait = {
  .wait_type = ATLK_WAIT_INTERVAL,
  .wait_usec = MICRO_PER_UNIT
};

/* GNSS virtual method table */
static gnss_vtable_t gnss_vtable = GNSS_VTABLE_INIT;

/* GNSS NMEA objects */
static gnss_nmea_t gnss_nmea = GNSS_NMEA_INIT;

/* Whether GNSS is initialized */
static int gnss_initialized = 0;

atlk_rc_t atlk_must_check
gnss_init(const gnss_config_t *config)
{
  atlk_rc_t rc = ATLK_OK;

  if (atlk_unlikely(!config)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(gnss_initialized)) {
    TR_WARNING_NO_ARGS("GNSS is already initialized");
    return ATLK_OK;
  }

  switch (config->model) {
  case GNSS_MODEL_STMICRO_TESEO_II:
  case GNSS_MODEL_STMICRO_TESEO_III:
  {
    gnss_vtable.init = teseo_init;
    gnss_vtable.fw_version_get = teseo_fw_version_get;
    gnss_vtable.reset = teseo_reset;
    break;
  }
  case GNSS_MODEL_UBLOX_MAX_7:
  case GNSS_MODEL_UBLOX_MAX_M8:
  {
#ifdef __CRATON__
    if (config->wd_enabled) {
      TR_ERROR_NO_ARGS("GNSS WD is not currently supported with U-Blox");
      return ATLK_E_UNSUPPORTED;
    }

    gnss_vtable.init = max_init;
    gnss_vtable.fw_version_get = max_fw_version_get;
    gnss_vtable.reset = max_reset;
    break;
#endif /* __CRATON__ */
  }
  case GNSS_MODEL_NA:
  default:
    TR_ERROR("GNSS model %d is not supported", config->model);
    return ATLK_E_UNSUPPORTED;
  }

  rc = gnss_vtable.init(config, &gnss_nmea);
  if (atlk_error(rc)) {
    TR_ERROR("GNSS init failed (rc=%d)", rc);
    return rc;
  }

#ifdef __CRATON__
  if (config->wd_enabled) {
    gnss_wd_config_t gnss_wd_config = GNSS_WD_CONFIG_INIT;

    gnss_wd_config.hw_reset = config->hw_reset;
    gnss_wd_config.sched_params = config->wd_sched_params;

    rc = gnss_wd_init(&gnss_wd_config);
    if (atlk_error(rc)) {
      TR_ERROR("GNSS WD init failed (rc=%d)", rc);
      return rc;
    }

    /* TODO: This might need to be board-specific */
    rc = gnss_wd_kick(10 * MILLI_PER_UNIT);
    if (atlk_error(rc)) {
      TR_ERROR("GNSS WD kick failed (rc=%d)", rc);
      return rc;
    }
  }
#endif /* __CRATON__ */

  gnss_initialized = 1;
  return ATLK_OK;
}

#define GNSS_INIT_CHECK \
  if (atlk_unlikely(!gnss_initialized)) { \
    TR_DEBUG_NO_ARGS("GNSS is not initialized"); \
    return ATLK_E_INVALID_STATE; \
  }

atlk_rc_t atlk_must_check
gnss_antenna_offset_set(const coord_ego_t *ant_offset)
{
  if (atlk_unlikely(!ant_offset)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(isnan(ant_offset->x_m) ||
                    isnan(ant_offset->y_m) ||
                    isnan(ant_offset->z_m))) {
    TR_ERROR_NO_ARGS("Antenna offset contains NAN value");
    return ATLK_E_INVALID_ARG;
  }

  GNSS_INIT_CHECK;

  gnss_nmea.nmea_nav.ant_offset = *ant_offset;
  return ATLK_OK;
}

atlk_rc_t atlk_must_check
gnss_antenna_offset_get(coord_ego_t *ant_offset)
{
  if (atlk_unlikely(!ant_offset)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  GNSS_INIT_CHECK;

  *ant_offset = gnss_nmea.nmea_nav.ant_offset;
  return ATLK_OK;
}

atlk_rc_t atlk_must_check
gnss_fw_version_get(char *fw_version,
            size_t *fw_version_size,
            const atlk_wait_t *wait)
{
  if (atlk_unlikely(!fw_version || !fw_version_size)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(*fw_version_size < 2)) {
    TR_ERROR_NO_ARGS("FW version buffer size is too small");
    return ATLK_E_INVALID_ARG;
  }

  GNSS_INIT_CHECK;

  if (gnss_vtable.fw_version_get) {
    return gnss_vtable.fw_version_get(fw_version, fw_version_size,
                                      wait, &gnss_nmea);
  }

  TR_ERROR_NO_ARGS("GNSS FW version getter is not supported");
  return ATLK_E_UNSUPPORTED;
}

static atlk_rc_t atlk_must_check
check_reset_params(const gnss_reset_params_t *params)
{
  switch (params->reset_type) {
  case GNSS_RESET_TYPE_HW:
  case GNSS_RESET_TYPE_SW:
  case GNSS_RESET_TYPE_GNSS_ONLY:
    break;
  case GNSS_RESET_TYPE_NA:
  default:
    TR_ERROR("Invalid GNSS reset type %d", params->reset_type);
    return ATLK_E_INVALID_ARG;
  }

  switch (params->start_type) {
  case GNSS_START_TYPE_HOT:
  case GNSS_START_TYPE_WARM:
  case GNSS_START_TYPE_COLD:
    break;
  case GNSS_START_TYPE_NA:
  default:
    TR_ERROR("Invalid GNSS start type %d", params->start_type);
    return ATLK_E_INVALID_ARG;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
gnss_reset(const gnss_reset_params_t *params,
            const atlk_wait_t *wait)
{
  atlk_rc_t rc = ATLK_OK;

  if (atlk_unlikely(!params)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  rc = check_reset_params(params);
  if (atlk_error(rc)) {
    return rc;
  }

  GNSS_INIT_CHECK;

  if (gnss_vtable.reset) {
    return gnss_vtable.reset(params, wait, &gnss_nmea);
  }

  TR_ERROR_NO_ARGS("GNSS reset is not supported");
  return ATLK_E_UNSUPPORTED;
}

atlk_rc_t atlk_must_check
gnss_input_poll_start(void)
{
  GNSS_INIT_CHECK;

  return nmea_io_input_poll_start(&gnss_nmea.nmea_io);
}

atlk_rc_t atlk_must_check
gnss_input_poll_stop(void)
{
  GNSS_INIT_CHECK;
  
  return nmea_io_input_poll_stop(&gnss_nmea.nmea_io);
}

int
gnss_input_poll_enabled(void)
{
  return nmea_io_input_poll_enabled(&gnss_nmea.nmea_io);
}

atlk_rc_t atlk_must_check
gnss_stats_get(nmea_io_stats_t *value)
{
  if (atlk_unlikely(!value)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }
  
  GNSS_INIT_CHECK;

  return nmea_io_stats_get(&gnss_nmea.nmea_io, value);
}

atlk_rc_t atlk_must_check
gnss_stats_reset(void)
{
  GNSS_INIT_CHECK;

  return nmea_io_stats_reset(&gnss_nmea.nmea_io);
}

nmea_io_t *
gnss_nmea_io_get(void)
{
  return &gnss_nmea.nmea_io;
}
