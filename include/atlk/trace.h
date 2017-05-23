/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_TRACE_H
#define _ATLK_TRACE_H

/**
   @file
   Tracing helper macros
*/

#include <atlk/compiler.h>

/* Default value of _TR_MODULE */
#if !defined _TR_MODULE
#define _TR_MODULE "?"
#endif

/* Trace output formatter */
#define TR_FMT(fmt) "(%s) %s: " fmt, _TR_MODULE, __func__

/* Helper for TR_DEBUG definition: if debug traces are disabled then wrap
   each debug trace statement so that it's compiled but then optimized away.
   To enable DEBUG level for a specific source file add:
   "#define TR_DEBUG_ON 1" BEFORE all includes in the relevant file.
*/
#ifdef TR_DEBUG_ON
#define TR_IF_DEBUG(...) __VA_ARGS__
#else
#define TR_IF_DEBUG(...) if (0) { __VA_ARGS__; }
#endif

#if defined __CRATON__

/* On CRATON, trace via syslog */
#include <craton/syslog.h>

#define TR_FUNC syslog

/* Log level mapping for syslog */
#define TR_LEVEL_ERROR LOG_ERR
#define TR_LEVEL_WARNING LOG_WARNING
#define TR_LEVEL_NOTICE LOG_NOTICE
#define TR_LEVEL_INFO LOG_INFO
#define TR_LEVEL_DEBUG LOG_DEBUG

#else /* __CRATON__ */

/* Outside of CRATON, trace via tr_log */
void tr_log(int level, const char *format, ...)
  atlk_format_printf(2, 3);

#define TR_FUNC tr_log

/* Log level mapping for tr_log */
#define TR_LEVEL_ERROR 0
#define TR_LEVEL_WARNING 1
#define TR_LEVEL_NOTICE 2
#define TR_LEVEL_INFO 3
#define TR_LEVEL_DEBUG 4

#endif /* __CRATON__ */

#define TR_ERROR_NO_ARGS(fmt) TR_FUNC(TR_LEVEL_ERROR, TR_FMT(fmt))
#define TR_WARNING_NO_ARGS(fmt) TR_FUNC(TR_LEVEL_WARNING, TR_FMT(fmt))
#define TR_NOTICE_NO_ARGS(fmt) TR_FUNC(TR_LEVEL_NOTICE, TR_FMT(fmt))
#define TR_INFO_NO_ARGS(fmt) TR_FUNC(TR_LEVEL_INFO, TR_FMT(fmt))
#define TR_DEBUG_NO_ARGS(fmt) TR_IF_DEBUG(TR_FUNC(TR_LEVEL_DEBUG, TR_FMT(fmt)))

#define TR_ERROR(fmt, ...) \
  TR_FUNC(TR_LEVEL_ERROR, TR_FMT(fmt), ## __VA_ARGS__)
#define TR_WARNING(fmt, ...) \
  TR_FUNC(TR_LEVEL_WARNING, TR_FMT(fmt), ## __VA_ARGS__)
#define TR_NOTICE(fmt, ...) \
  TR_FUNC(TR_LEVEL_NOTICE, TR_FMT(fmt), ## __VA_ARGS__)
#define TR_INFO(fmt, ...) \
  TR_FUNC(TR_LEVEL_INFO, TR_FMT(fmt), ## __VA_ARGS__)
#define TR_DEBUG(fmt, ...) \
  TR_IF_DEBUG(TR_FUNC(TR_LEVEL_DEBUG, TR_FMT(fmt), ## __VA_ARGS__))

#endif /* _ATLK_TRACE_H */
