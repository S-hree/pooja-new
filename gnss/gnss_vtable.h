/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _GNSS_VTABLE_H
#define _GNSS_VTABLE_H

#include <atlk/sdk.h>
#include <atlk/gnss.h>
#include <atlk/gnss_internal.h>

#ifdef __cplusplus
extern "C" {
#endif

/** GNSS virtual method table */
typedef struct {
  /** GNSS init method */
  atlk_rc_t
  (*init)(const gnss_config_t *config,
            gnss_nmea_t *gnss_nmea);

  /** GNSS FW version get method */
  atlk_rc_t
  (*fw_version_get)(char *fw_version,
            size_t *fw_version_size,
            const atlk_wait_t *wait,
            gnss_nmea_t *gnss_nmea);

  /** GNSS reset method */
  atlk_rc_t
  (*reset)(const gnss_reset_params_t *params,
            const atlk_wait_t *wait,
            gnss_nmea_t *gnss_nmea);

} gnss_vtable_t;

/** GNSS virtual method table default initializer */
#define GNSS_VTABLE_INIT {      \
  .init = NULL,                 \
  .fw_version_get = NULL,       \
  .reset = NULL                 \
}

#ifdef __cplusplus
}
#endif

#endif /* _GNSS_GNSS_VTABLE_H */
