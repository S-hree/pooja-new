/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_MEMIO_H
#define _ATLK_MEMIO_H

/**
   @file
   Binary variable-length structure I/O.

   Allows to read/write fixed/variable length fields in big-endian order
   without being dependent on architecture endianness. Includes partial
   support for little-endian fields.
*/

#ifdef __KERNEL__
#include <linux/uio.h>             /* Declares struct iovec */
#include <linux/string.h>          /* Declares memcpy */
#else
#include <string.h>                /* Declares memcpy */
#endif

#include <atlk/sdk.h>
#include <platform/common.h>
#include <atlk/compiler.h>

/** Buffer read/write tracker structure */
struct memio {
  void *next;    /**< Pointer to next available byte */
  void *end;     /**< Pointer to one-beyond-last byte */
};

/** Initialize ::memio structure given buffer and length */
atlk_inline void
memio_init(struct memio *memio, void* buf, size_t len)
{
  BUG_ON(!buf && (len > 0));
  memio->next = buf;
  memio->end = (uint8_t*)buf + len;
}

/** Return number of bytes remaining till end of buffer */
atlk_inline size_t
memio_remaining(const struct memio *memio)
{
  BUG_ON(memio->end < memio->next);
  return (uint8_t*)memio->end - (uint8_t*)memio->next;
}

/** Return number of bytes consumed from start of buffer */
atlk_inline size_t
memio_consumed(const struct memio *memio,
	       const void *buf)
{
  BUG_ON(memio->next < buf);
  return (uint8_t*)memio->next - (uint8_t*)buf;
}

/** Reserve @a len bytes in buffer */
atlk_inline atlk_rc_t atlk_must_check
memio_reserve(struct memio *memio, size_t len)
{
  void *next = (uint8_t*)memio->next + len;
  if (atlk_unlikely(next > memio->end)) {
    return ATLK_E_OUT_OF_BOUNDS;
  }
  memio->next = next;
  return ATLK_OK;
}

/** Read octet string from buffer */
atlk_inline atlk_rc_t atlk_must_check
memio_read_opaque(struct memio *memio,
		  void *buf,
		  size_t len)
{
  void *src = memio->next;
  atlk_rc_t rc = memio_reserve(memio, len);
  if (atlk_error(rc)) {
    return rc;
  }
  memcpy(buf, src, len);
  return 0;
}

/** Write an octet string into buffer */
atlk_inline atlk_rc_t atlk_must_check
memio_write_opaque(struct memio *memio,
                   const void *buf,
                   size_t len)
{
  void *dest = memio->next;
  atlk_rc_t rc = memio_reserve(memio, len);
  if (atlk_error(rc)) {
    return rc;
  }
  memcpy(dest, buf, len);
  return 0;
}

/** Load @c uint8_t from a location assumed valid */
atlk_inline void memio_load_uint8(uint8_t *dest, const void *src)
{
  *dest = *(uint8_t*)src;
}

/** Load big-endian @c uint16_t from a location assumed valid */
atlk_inline void memio_load_uint16(uint16_t *dest, const void *src)
{
  uint8_t hi, lo;
  memio_load_uint8(&hi, (uint8_t*)src);
  memio_load_uint8(&lo, (uint8_t*)src + 1);
  *dest = ((uint16_t)hi << 8) | lo;
}

/** Load big-endian @c uint32_t from a location assumed valid */
atlk_inline void memio_load_uint32(uint32_t *dest, const void *src)
{
  uint16_t hi, lo;
  memio_load_uint16(&hi, (uint8_t *)src);
  memio_load_uint16(&lo, (uint8_t *)src + 2);
  *dest = ((uint32_t)hi << 16) | lo;
}

/** Load big-endian @c uint64_t from a location assumed valid */
atlk_inline void memio_load_uint64(uint64_t *dest, const void *src)
{
  uint32_t hi, lo;
  memio_load_uint32(&hi, (uint8_t *)src);
  memio_load_uint32(&lo, (uint8_t *)src + 4);
  *dest = ((uint64_t)hi << 32) | lo;
}

/** Load little-endian @c uint16_t from a location assumed valid */
atlk_inline void memio_load_uint16le(uint16_t *dest, const void *src)
{
  uint8_t hi, lo;
  memio_load_uint8(&lo, (uint8_t *)src);
  memio_load_uint8(&hi, (uint8_t *)src + 1);
  *dest = ((uint16_t)hi << 8) | lo;
}

/** Load big-endian @c uint32_t from a location assumed valid */
atlk_inline void memio_load_uint32le(uint32_t *dest, const void *src)
{
  uint16_t hi, lo;
  memio_load_uint16le(&lo, (uint8_t *)src);
  memio_load_uint16le(&hi, (uint8_t *)src + 2);
  *dest = ((uint32_t)hi << 16) | lo;
}

/** Store @c uint8_t to a location assumed valid */
atlk_inline void memio_store_uint8(void *dest, uint8_t value)
{
  *(uint8_t*)dest = value;
}

/** Store @c uint16_t in big-endian to a location assumed valid */
atlk_inline void memio_store_uint16(void *dest, uint16_t value)
{
  memio_store_uint8(dest, (uint8_t)(value >> 8));
  memio_store_uint8((uint8_t *)dest + 1, (uint8_t)(value & UINT8_MAX));
}

/** Store @c uint32_t in big-endian to a location assumed valid */
atlk_inline void memio_store_uint32(void *dest, uint32_t value)
{
  memio_store_uint16(dest, (uint16_t)(value >> 16));
  memio_store_uint16((uint8_t *)dest + 2, (uint16_t)(value & UINT16_MAX));
}

/** Store @c uint64_t in big-endian to a location assumed valid */
atlk_inline void memio_store_uint64(void *dest, uint64_t value)
{
  memio_store_uint32(dest, (uint32_t)(value >> 32));
  memio_store_uint32((uint8_t *)dest + 4, (uint32_t)(value & UINT32_MAX));
}

/** Store @c uint16_t in little-endian to a location assumed valid */
atlk_inline void memio_store_uint16le(void *dest, uint16_t value)
{
  memio_store_uint8((uint8_t *)dest + 1, (uint8_t)(value >> 8));
  memio_store_uint8(dest, (uint8_t)(value & UINT8_MAX));
}

/** Store @c uint32_t in little-endian to a location assumed valid */
atlk_inline void memio_store_uint32le(void *dest, uint32_t value)
{
  memio_store_uint16le((uint8_t *)dest + 2, (uint16_t)(value >> 16));
  memio_store_uint16le(dest, (uint16_t)(value & UINT16_MAX));
}

#define _MEMIO_READ_FIXED(memio, type, dest) do {	\
    void *src = memio->next;				\
    atlk_rc_t rc = memio_reserve(memio, sizeof(type ## _t));  \
    if (atlk_error(rc)) return rc;				\
    memio_load_ ## type(dest, src);			\
    return ATLK_OK; } while(0)

/** Read @c uint8_t from stream */
atlk_inline atlk_rc_t atlk_must_check
memio_read_uint8(struct memio *memio, uint8_t *dest)
{
  _MEMIO_READ_FIXED(memio, uint8, dest);
}

/** Read big-endian @c uint16_t from stream */
atlk_inline atlk_rc_t atlk_must_check
memio_read_uint16(struct memio *memio, uint16_t *dest)
{
  _MEMIO_READ_FIXED(memio, uint16, dest);
}

/** Read big-endian @c uint32_t from stream */
atlk_inline atlk_rc_t atlk_must_check
memio_read_uint32(struct memio *memio, uint32_t *dest)
{
  _MEMIO_READ_FIXED(memio, uint32, dest);
}

/** Read big-endian @c uint32_t from stream */
atlk_inline atlk_rc_t atlk_must_check
memio_read_int32(struct memio *memio, int32_t *dest)
{
  _MEMIO_READ_FIXED(memio, uint32, (uint32_t *)dest);
}

/** Read big-endian @c uint64_t from stream */
atlk_inline atlk_rc_t atlk_must_check
memio_read_uint64(struct memio *memio, uint64_t *dest)
{
  _MEMIO_READ_FIXED(memio, uint64, dest);
}

#define _MEMIO_WRITE_FIXED(memio, type, value) do {     \
    void *dest = memio->next;                           \
    atlk_rc_t rc = memio_reserve(memio, sizeof(type ## _t));  \
    if (atlk_error(rc)) return rc;				\
    memio_store_ ## type(dest, value);                  \
    return ATLK_OK; } while (0)

/** Write @c uint8_t to stream */
atlk_inline atlk_rc_t atlk_must_check
memio_write_uint8(struct memio *memio, uint8_t value)
{
  _MEMIO_WRITE_FIXED(memio, uint8, value);
}

/** Write big-endian @c uint16_t to stream */
atlk_inline atlk_rc_t atlk_must_check
memio_write_uint16(struct memio *memio, uint16_t value)
{
  _MEMIO_WRITE_FIXED(memio, uint16, value);
}

/** Write big-endian @c uint32_t to stream */
atlk_inline atlk_rc_t atlk_must_check
memio_write_uint32(struct memio *memio, uint32_t value)
{
  _MEMIO_WRITE_FIXED(memio, uint32, value);
}

/** Write big-endian @c uint64_t to stream */
atlk_inline atlk_rc_t atlk_must_check
memio_write_uint64(struct memio *memio, uint64_t value)
{
  _MEMIO_WRITE_FIXED(memio, uint64, value);
}

#endif /* _ATLK_MEMIO_H */
