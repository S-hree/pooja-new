/* Copyright (C) 2014-2015 Autotalks Ltd. */
#ifndef _CRATON_BOOTPARAM_H
#define _CRATON_BOOTPARAM_H

#include <atlk/sdk.h>

#include <craton/check.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   CRATON boot parameter API
*/

/**
   Get boot parameter value.

   @param[in] name Boot parameter name
   @param[out] value Boot parameter value
   @param[in,out] size Size of value

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
bootparam_get(const char *name,
            char *value,
            size_t *size);

#ifdef __cplusplus
}
#endif

#endif /* _CRATON_BOOTPARAM_H */
