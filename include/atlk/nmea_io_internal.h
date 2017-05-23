/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _ATLK_NMEA_IO_INTERNAL_H
#define _ATLK_NMEA_IO_INTERNAL_H

#include <atlk/sdk.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum length of parametric NMEA sentence, including <CR><LF> */
#define NMEA_LEN_MAX 82

/** Minimum length of parametric NMEA sentence, including <CR><LF> */
#define NMEA_LEN_MIN 12

/** Maximum length of proprietary NMEA sentence, including <CR><LF> */
#define NMEA_P_LEN_MAX 242

/** Minimum length of proprietary NMEA sentence, including <CR><LF> */
#define NMEA_P_LEN_MIN 7

/**
   Calculate NMEA checksum.

   @param[in] sentence Sentence for which to calculate checksum
   @param[out] end_ptr Next char after last checksummed char (optional)

   @return Calculated checksum
*/
uint8_t
nmea_checksum(const char *sentence,
            const char **end_ptr);

/**
   Check NMEA sentence validity.

   @param[in] sentence Sentence to check
   @param[in] sentence_size Sentence size in chars, including null-termination

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_sentence_check(const char *sentence,
            size_t sentence_size);

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_NMEA_IO_INTERNAL_H */
