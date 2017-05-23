/* Copyright (C) 2015 Autotalks Ltd. */
#ifndef _ATLK_CRC_H
#define _ATLK_CRC_H

#include <atlk/sdk.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   CRC API
*/

#define CRC_WIDTH_MAX 32
#define CRC_LUT_NUM_ENTRIES 256

/** CRC LUT type*/
typedef struct {
  uint32_t lut[CRC_LUT_NUM_ENTRIES];
} crc_lut_t;

/**
   Calculate CRC according to pre-computed LUT

   @param[in] lut Lookup table
   @param[in] buf Buffer contains data to compute CRC upon
   @param[in] buf_size Buffer size
   @param[in,out] crc_ptr CRC (note: initial value must be set first)
*/
void
crc_compute(const crc_lut_t *lut,
            const void *buf,
            size_t buf_size,
            uint32_t *crc_ptr);

/* CRC LUTs*/
extern const crc_lut_t crc_lut_koopman_0x571;
extern const crc_lut_t crc_lut_koopman_0x90022004;
extern const crc_lut_t crc_lut_regular_0xedb88320;

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_CRC_H */
