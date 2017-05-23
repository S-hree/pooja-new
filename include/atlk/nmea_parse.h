/* Copyright (C) 2012-2016 Autotalks Ltd. */
/* File generated automatically; do not edit. */
/* Generated: 2016-05-22 19:08:27.244996. */
#ifndef _ATLK_NMEA_PARSE_H
#define _ATLK_NMEA_PARSE_H

#include <atlk/sdk.h>
#include <atlk/nmea.h>

#ifdef __cplusplus
extern "C" {
#endif

#define nmea_is_field(c) \
    (c != '\0' && c != '\r' && c != '\n' && c != ',' && c != '*')

atlk_rc_t atlk_must_check
nmea_parse_rmc(nmea_data_rmc_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_gga(nmea_data_gga_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_gsa(nmea_data_gsa_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_gll(nmea_data_gll_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_gst(nmea_data_gst_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_gsv(nmea_data_gsv_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_vtg(nmea_data_vtg_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_gns(nmea_data_gns_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_stm_cpu(nmea_data_stm_cpu_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_stm_kfcov(nmea_data_stm_kfcov_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_stm_utc(nmea_data_stm_utc_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_stm_tg(nmea_data_stm_tg_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_stm_setpar(nmea_data_stm_setpar_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_stm_drcal(nmea_data_stm_drcal_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_stm_drcov(nmea_data_stm_drcov_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_ubx_time(nmea_data_ubx_time_t *data, 
            const char *sentence);

atlk_rc_t atlk_must_check
nmea_parse_no_data(nmea_data_no_data_t *data, 
            const char *sentence);

#define NMEA_TALKER_ID_SIZE 7
extern const char *nmea_talker_id[];

#define NMEA_FORMATTER_SIZE 9
extern const char *nmea_formatter[];

#define NMEA_FORMATTER_STM_SIZE 11
extern const char *nmea_formatter_stm[];

#define NMEA_FORMATTER_UBX_SIZE 2
extern const char *nmea_formatter_ubx[];

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_NMEA_PARSE_H */
