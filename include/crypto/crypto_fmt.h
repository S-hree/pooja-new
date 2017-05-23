/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _CRYPTO_CRYPTO_FMT_H
#define _CRYPTO_CRYPTO_FMT_H

/**
   Implementation of I2OSP (integer to octet string primitive) as defined
   on page 16 of IEEE 1363-2000.
*/
void
fmt_i2osp(const uint32_t *input,
	        uint8_t *output,
	        size_t u32_len);

/**
   Implementation of OS2IP (octet string to integer primitive) as defined
   on page 16 of IEEE 1363-2000.
*/
void
fmt_os2ip(const uint8_t *input,
	        uint32_t *output,
	        size_t u32_len);

#endif /* _CRYPTO_CRYPTO_FMT_H */
