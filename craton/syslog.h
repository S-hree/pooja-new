/* Copyright (C) 2014-2015 Autotalks Ltd. */
#ifndef _CRATON_SYSLOG_H
#define _CRATON_SYSLOG_H

#include <atlk/sdk.h>

#include <craton/check.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   CRATON system logger API
*/

/** Log trace level */
typedef enum {
  /** Error conditions */
  LOG_ERR = 3,

  /** Warning conditions */
  LOG_WARNING = 4,

  /** Normal but significant condition */
  LOG_NOTICE = 5,

  /** Informational messages */
  LOG_INFO = 6,

  /** Debug-level messages */
  LOG_DEBUG = 7

} syslog_level_t;

/**
   Generate a log message.

   @param[in] level Message level
   @param[in] format Message format
*/
void
syslog(syslog_level_t level,
       const char *format, ...)
  atlk_format_printf(2, 3);

#ifdef __cplusplus
}
#endif

#endif /* _CRATON_SYSLOG_H */
