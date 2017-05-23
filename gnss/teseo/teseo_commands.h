/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _GNSS_TESEO_COMMANDS_H
#define _GNSS_TESEO_COMMANDS_H

#include <atlk/sdk.h>
#include <atlk/nmea.h>
#include <atlk/gnss.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   Init Teseo commands.

   @param[in,out] nmea NMEA object
   @param[in] hw_reset HW reset procedure (optional)

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
teseo_commands_init(nmea_t *nmea,
            gnss_hw_reset_t hw_reset);

/**
   Enable/disable DR calibration reporting.

   @note Only relevant for DR firmware which emits message $PSTMDRCAL.

   @param[in] enabled Whether calibration reporting is enabled
*/
void
teseo_drcal_report_set(int enabled);

#ifdef __cplusplus
}
#endif

#endif /* _GNSS_TESEO_COMMANDS_H */
