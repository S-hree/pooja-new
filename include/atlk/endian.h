/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_ENDIAN_H
#define _ATLK_ENDIAN_H

#include <stdint.h>

#if defined unix
#include <netinet/in.h> /* ntohs, htons, ntohl, ntohs */
#elif defined __GNUC__ 
static inline uint16_t _bswap16(uint16_t x)
{
  return (x << 8) | (x >> 8);
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define ntohs(x) _bswap16(x)
#define ntohl(x) (uint32_t)__builtin_bswap32(x)
#define htons(x) _bswap16(x)
#define htonl(x) (uint32_t)__builtin_bswap32(x)
static inline uint64_t htonll(uint64_t val)
{
  return (uint64_t)htonl((uint32_t)(val >> 32)) |
    ((uint64_t)htonl((uint32_t)val) << 32);
}

static inline uint64_t ntohll(uint64_t val)
{
  return (uint64_t)ntohl((uint32_t)(val >> 32)) |
    ((uint64_t)ntohl((uint32_t)val) << 32);
}
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define ntohs(x) (x)
#define ntohl(x) (x)
#define htons(x) (x)
#define htonl(x) (x)
static inline uint64_t htonll(uint64_t val)
{
  return val;
}

static inline uint64_t ntohll(uint64_t val)
{
  return val;
}
#else /* __BYTE_ORDER__ */
#error "Unsupported __BYTE_ORDER__"
#endif /* __BYTE_ORDER__ */

#endif /* __GNUC__ */

#endif /* _ATLK_ENDIAN_H */
