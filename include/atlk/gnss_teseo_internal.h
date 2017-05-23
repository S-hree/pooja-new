/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _ATLK_GNSS_TESEO_INTERNAL_H
#define _ATLK_GNSS_TESEO_INTERNAL_H

#include <atlk/sdk.h>
#include <atlk/gnss.h>
#include <atlk/gnss_internal.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   Initialize Teseo.

   @param[in] config Teseo configuration parameters
   @param[in] gnss_nmea GNSS NMEA objects

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
teseo_init(const gnss_config_t *config,
            gnss_nmea_t *gnss_nmea);

/**
   Get Teseo firmware version.

   @param[out] fw_version Teseo firmware version buffer
   @param[in,out] fw_version_size Maximum size (in) and actual (out) in chars
   @param[in] wait Wait specification (optional)
   @param[in] gnss_nmea GNSS NMEA objects

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
teseo_fw_version_get(char *fw_version,
            size_t *fw_version_size,
            const atlk_wait_t *wait,
            gnss_nmea_t *gnss_nmea);

/**
   Teseo reset.

   @param[in] params GNSS reset parameters
   @param[in] wait Wait specification (optional)
   @param[in] gnss_nmea GNSS NMEA objects

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
teseo_reset(const gnss_reset_params_t *params,
            const atlk_wait_t *wait,
            gnss_nmea_t *gnss_nmea);

#ifdef __CRATON__
#include <craton/cli.h>

/**
   Init Teseo CLI.

   @param[in,out] cli CLI instance

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t
teseo_cli_init(cli_instance_t *cli);
#endif /* __CRATON__ */

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_GNSS_TESEO_INTERNAL_H */
