/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _PLATFORM_COMMON_H
#define _PLATFORM_COMMON_H

/**
   @file
   Autotalks Ltd. common top-level header for internal code.

   Declares common implementation helpers, such as limits,
   simple functions on integers, error codes, macros.
*/

#include <inttypes.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>

#include <atlk/sdk.h>
#include <atlk/os.h>
#include <atlk/trace.h>

/* Define short aliases for macros defined by SDK */
#define likely(x) atlk_likely(x)
#define unlikely(x) atlk_unlikely(x)

/* Suppress `unused variable' warnings on a variable */
#define __maybe_unused __attribute__((unused))

/* Helper macros */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define ROUND_UP(n, d) (DIV_ROUND_UP(n, d) * (d))
#define ROUND_DOWN(n, d) (((n) / (d)) * (d))

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#if !defined __cplusplus
#undef min
#define min(x, y) ({                      \
        typeof(x) _min1 = (x);            \
        typeof(y) _min2 = (y);            \
        (void) (&_min1 == &_min2);        \
        _min1 < _min2 ? _min1 : _min2; })

#undef max
#define max(x, y) ({                      \
        typeof(x) _max1 = (x);            \
        typeof(y) _max2 = (y);            \
        (void) (&_max1 == &_max2);        \
        _max1 > _max2 ? _max1 : _max2; })
#endif /* !__cplusplus */

#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif /* !MAX */
#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif /* !MIN */

/* Clamp a number into the interval [min, max] */
#define clamp(x, min, max) ({             \
        typeof(x) _x = (x);               \
        typeof(min) _min = (min);         \
        typeof(max) _max = (max);         \
        (void) (&_x == &_min);            \
        (void) (&_x == &_max);            \
        _x > _max ? _max : (_x < _min ? _min : _x); })

/* Break the build if condition is true (no runtime or code size penalty) */
#define BUILD_BUG_ON(condition) BUILD_BUG_AUX1(condition, __COUNTER__)
#define BUILD_BUG_AUX1(condition, counter) BUILD_BUG_AUX2(condition, counter)
#define BUILD_BUG_AUX2(condition, counter)               \
struct __build_bug_check_ ## counter {                   \
  char negative_size_on_build_bug[(condition) ? -1 : 0]; \
}

/* Helper function for BUG_ON */
void
bug_handler(const char *file,
            int line,
            const char *condition)
  atlk_no_return;

/* Linux kernel-style assertion macro */
#define BUG_ON(condition) do { \
  if (atlk_unlikely(condition)) { \
    bug_handler(__FILE__, __LINE__, #condition); \
  } \
} while (0)

#define BUG() BUG_ON(1)

#define FAILED(rc) unlikely((rc) < 0)
#define SUCCEEDED(rc) likely((rc) >= 0)

/* Unit scale conversion factors */
#define CENTI_PER_UNIT  100
#define MILLI_PER_UNIT  1000
#define MICRO_PER_MILLI 1000
#define NANO_PER_MICRO  1000
#define MICRO_PER_UNIT  1000000
#define NANO_PER_MILLI  1000000
#define NANO_PER_UNIT   1000000000

/* Macro that evaluates to `nbits' `1' bits */
#define BITMASK(nbits) ((1 << nbits) - 1)

/* Macro that evaluates a single '1' bit with offset */
#define BIT_N(bit) (1 << bit)

/* Single `1' bit constants */
#define BIT31 (1 << 31)
#define BIT30 (1 << 30)
#define BIT29 (1 << 29)
#define BIT28 (1 << 28)
#define BIT27 (1 << 27)
#define BIT26 (1 << 26)
#define BIT25 (1 << 25)
#define BIT24 (1 << 24)
#define BIT23 (1 << 23)
#define BIT22 (1 << 22)
#define BIT21 (1 << 21)
#define BIT20 (1 << 20)
#define BIT19 (1 << 19)
#define BIT18 (1 << 18)
#define BIT17 (1 << 17)
#define BIT16 (1 << 16)
#define BIT15 (1 << 15)
#define BIT14 (1 << 14)
#define BIT13 (1 << 13)
#define BIT12 (1 << 12)
#define BIT11 (1 << 11)
#define BIT10 (1 << 10)
#define BIT9  (1 << 9)
#define BIT8  (1 << 8)
#define BIT7  (1 << 7)
#define BIT6  (1 << 6)
#define BIT5  (1 << 5)
#define BIT4  (1 << 4)
#define BIT3  (1 << 3)
#define BIT2  (1 << 2)
#define BIT1  (1 << 1)
#define BIT0  (1 << 0)

/* Convert standard errno to atlk_rc_t */
atlk_rc_t atlk_must_check
errno_to_atlk_rc(int _errno);

#if defined __THREADX__

/* Convert microsecond count to ThreadX tick count */
atlk_inline ULONG
usec_to_tick_round_up(uint32_t usec)
{
  BUILD_BUG_ON(MICRO_PER_UNIT % TX_TICK_RATE != 0);
  BUILD_BUG_ON(TX_TICK_RATE > MICRO_PER_UNIT);

  return DIV_ROUND_UP(usec, MICRO_PER_UNIT / TX_TICK_RATE);
}

/* Convert wait option to ThreadX wait parameter */
ULONG atlk_wait_to_tx_wait(const atlk_wait_t *wait);

#endif /* __THREADX__ */

#endif /* _PLATFORM_COMMON_H */
