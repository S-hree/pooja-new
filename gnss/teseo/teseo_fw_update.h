/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _GNSS_TESEO_FW_UPDATE_H
#define _GNSS_TESEO_FW_UPDATE_H

#include <atlk/sdk.h>
#include <atlk/gnss.h>
#include <atlk/gnss_internal.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   Init Teseo FW update.

   @param[in,out] gnss_nmea GNSS NMEA objects
   @param[in] hw_reset HW reset procedure (optional)

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
teseo_fw_update_init(gnss_nmea_t *gnss_nmea,
            gnss_hw_reset_t hw_reset);

#ifdef __cplusplus
}
#endif

#endif /* _GNSS_TESEO_FW_UPDATE_H */
