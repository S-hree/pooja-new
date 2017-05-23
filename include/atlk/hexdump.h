/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_HEXDUMP_H
#define _ATLK_HEXDUMP_H

#include <atlk/sdk.h>

/**
   Print a text hex dump to stdout for a binary blob of data.

   Given a buffer of octets, print_hex_dump() prints a hex + ASCII dump
   to sdtout.

   Example output:
   00000000: 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f 70  abcdefghijklmnop
   00000010: 71 72 73 74 75 76 77 78 79 7a 00                 qrstuvwxyz.

   @param[in] buf Buffer to dump
   @param[in] len Number of bytes in the buffer
   @param[in] ascii Include ASCII after the hex output
*/
void
print_hexdump(const void *buf, size_t len, int ascii);

/**
   Convert hex encoded string to octet buffer.

   @param[in] hexstr Hex encoded string
   @param[in] hexstr_len String length (not including the terminating '\0' byte!)
   @param[in,out] buffer Buffer to fill
   @param[in,out] buffer_len Used buffer length

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
hexstr_to_buffer(const char *hexstr, size_t hexstr_len,
            uint8_t *buffer, size_t *buffer_len);

#endif /* _ATLK_HEXDUMP_H */
