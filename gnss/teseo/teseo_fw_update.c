/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <atlk/sdk.h>
#include <atlk/gnss_teseo.h>

#include "teseo_fw_update.h"

atlk_rc_t atlk_must_check
gnss_teseo_fw_update(const gnss_teseo_fw_update_params_t *params)
{
  (void)params;

  /* TODO: Awaiting ST approval to release to customers */

  return ATLK_E_UNSUPPORTED;
}

atlk_rc_t atlk_must_check
teseo_fw_update_init(gnss_nmea_t *gnss_nmea,
            gnss_hw_reset_t hw_reset)
{
  (void)gnss_nmea;
  (void)hw_reset;

  /* TODO: Awaiting ST approval to release to customers */

  return ATLK_OK;
}
