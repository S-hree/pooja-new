/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_DX_H
#define _ATLK_DX_H

#include <stdint.h>

#include <atlk/sdk.h>
#include <atlk/compiler.h>

/**
   @file
   Data exchange API

   This API allows the user to define an over-the-wire or on-disk binary data
   format using C structure definition that leaves byte order and field padding
   unambiguous and compiler-independent.

   The following defines a 6-byte data frame composed of 2 little-endian fields:
   @code{.c}
   typedef dx_struct {
     dx_uint16le_t foo;
     dx_uint32le_t bar;
   } my_data_t;
   @endcode

   Writing and reading fields of dx_ types should be done only via the appropriate
   inline functions defined here (e.g. ::dx_uint16le_get).
*/

/** Structure packing support */
#if defined __CC_ARM
#define dx_struct __packed struct
#define dx_union __packed union
#elif defined __GNUC__
#define dx_struct struct __attribute__((packed))
#define dx_union union __attribute__((packed))
#else
#error "Unknown compiler; structure packing unsupported"
#endif

/** Data exchange primitive types */
#define ATLK_DX_PTR_VALID_CHECK(ptr_to_check) { \
    if (!ptr_to_check) { \
      /* All setters / getters are void */ \
      return; \
    } \
}


typedef dx_struct {
  uint8_t value;
} dx_uint8_t;

typedef dx_struct {
  uint16_t value;
} dx_uint16le_t;

typedef dx_struct {
  uint16_t value;
} dx_uint16be_t;

typedef dx_struct {
  uint32_t value;
} dx_uint32le_t;

typedef dx_struct {
  uint32_t value;
} dx_uint32be_t;

/** Data exchange functions */

atlk_inline uint16_t
dx_bswap16(uint16_t x)
{
  return (x << 8) | (x >> 8);
}

atlk_inline uint32_t
dx_bswap32(uint32_t x)
{
#if defined __CC_ARM
  return __rev(x);
#elif defined __GNUC__
  return __builtin_bswap32(x);
#else
return ((x << 24) & 0xff000000) |
       ((x <<  8) & 0x00ff0000) |
       ((x >>  8) & 0x0000ff00) |
       ((x >> 24) & 0x000000ff);
#endif
}

atlk_inline void
dx_uint8_set(dx_uint8_t *dst, uint8_t value)
{
  ATLK_DX_PTR_VALID_CHECK(dst);
  dst->value = value;
}

atlk_inline void
dx_uint8_get(const dx_uint8_t *src, uint8_t *value)
{
  ATLK_DX_PTR_VALID_CHECK(src);
  ATLK_DX_PTR_VALID_CHECK(value);
  *value = src->value;
}

#if ((defined(__CC_ARM) && !defined(__BIG_ENDIAN)) || \
     (defined(__GNUC__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)))

atlk_inline void
dx_uint16le_set(dx_uint16le_t *dst, uint16_t value)
{
  ATLK_DX_PTR_VALID_CHECK(dst);
  dst->value = value;
}

atlk_inline void
dx_uint16le_get(const dx_uint16le_t *src, uint16_t *value)
{
  ATLK_DX_PTR_VALID_CHECK(src);
  ATLK_DX_PTR_VALID_CHECK(value);
  *value = src->value;
}

atlk_inline void
dx_uint32le_set(dx_uint32le_t *dst, uint32_t value)
{
  ATLK_DX_PTR_VALID_CHECK(dst);
  dst->value = value;
}

atlk_inline void
dx_uint32le_get(const dx_uint32le_t *src, uint32_t *value)
{
  ATLK_DX_PTR_VALID_CHECK(src);
  *value = src->value;
}

atlk_inline void
dx_uint16be_set(dx_uint16be_t *dst, uint16_t value)
{
  ATLK_DX_PTR_VALID_CHECK(dst);
  dst->value = dx_bswap16(value);
}

atlk_inline void
dx_uint16be_get(const dx_uint16be_t *src, uint16_t *value)
{
  ATLK_DX_PTR_VALID_CHECK(src);
  ATLK_DX_PTR_VALID_CHECK(value);
  *value = dx_bswap16(src->value);
}

atlk_inline void
dx_uint32be_set(dx_uint32be_t *dst, uint32_t value)
{
  ATLK_DX_PTR_VALID_CHECK(dst);
  dst->value = dx_bswap32(value);
}

atlk_inline void
dx_uint32be_get(const dx_uint32be_t *src, uint32_t *value)
{
  ATLK_DX_PTR_VALID_CHECK(src);
  ATLK_DX_PTR_VALID_CHECK(value);
  *value = dx_bswap32(src->value);
}

#elif ((defined(__CC_ARM) && defined(__BIG_ENDIAN)) || \
       (defined(__GNUC__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)))

atlk_inline void
dx_uint16le_set(dx_uint16le_t *dst, uint16_t value)
{
  ATLK_DX_PTR_VALID_CHECK(dst);
  dst->value = dx_bswap16(value);
}

atlk_inline void
dx_uint16le_get(const dx_uint16le_t *src, uint16_t *value)
{
  ATLK_DX_PTR_VALID_CHECK(src);
  ATLK_DX_PTR_VALID_CHECK(value);
  *value = dx_bswap16(src->value);
}

atlk_inline void
dx_uint32le_set(dx_uint32le_t *dst, uint32_t value)
{
  ATLK_DX_PTR_VALID_CHECK(dst);
  dst->value = dx_bswap32(value);
}

atlk_inline void
dx_uint32le_get(const dx_uint32le_t *src, uint32_t *value)
{
  ATLK_DX_PTR_VALID_CHECK(src);
  ATLK_DX_PTR_VALID_CHECK(value);
  *value = dx_bswap32(src->value);
}

atlk_inline void
dx_uint16be_set(dx_uint16be_t *dst, uint16_t value)
{
  ATLK_DX_PTR_VALID_CHECK(dst);
  dst->value = value;
}

atlk_inline void
dx_uint16be_get(const dx_uint16be_t *src, uint16_t *value)
{
  ATLK_DX_PTR_VALID_CHECK(src);
  ATLK_DX_PTR_VALID_CHECK(value);
  *value = src->value;
}

atlk_inline void
dx_uint32be_set(dx_uint32be_t *dst, uint32_t value)
{
  ATLK_DX_PTR_VALID_CHECK(dst);
  dst->value = value;
}

atlk_inline void
dx_uint32be_get(const dx_uint32be_t *src, uint32_t *value)
{
  ATLK_DX_PTR_VALID_CHECK(src);
  ATLK_DX_PTR_VALID_CHECK(value);
  *value = src->value;
}

#else 
#error "Unsupported compiler for data exchange API"
#endif

#endif /* _ATLK_DX_H */
