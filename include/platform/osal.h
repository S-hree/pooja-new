/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _PLATFORM_OSAL_H
#define _PLATFORM_OSAL_H

#include <atlk/sdk.h>
#include <atlk/once.h>
#include <atlk/mutex.h>

/* Once API */

/* Once control */
typedef atlk_once_t osal_once_t;

/* Once control default static initializer */
#define OSAL_ONCE_INIT ATLK_ONCE_INIT

/* Routine to run once */
typedef atlk_once_routine_t osal_once_routine_t;

/* Run once */
atlk_inline void
osal_once_run(osal_once_t *once, osal_once_routine_t routine)
{
  atlk_once_run(once, routine);
}

/* Mutex API */

/* Mutex descriptor */
typedef atlk_mutex_t osal_mutex_t;

/* Create mutex */
atlk_inline void
osal_mutex_create(osal_mutex_t *mutex, const char *name)
{
  atlk_rc_t rc;

  rc = atlk_mutex_create(mutex, name);
  BUG_ON(atlk_error(rc));
}

/* Lock mutex */
atlk_inline atlk_rc_t atlk_must_check
osal_mutex_lock(osal_mutex_t *mutex)
{
  atlk_mutex_lock(mutex);
  return ATLK_OK;
}

/* Unlock mutex */
atlk_inline void
osal_mutex_unlock(osal_mutex_t *mutex)
{
  atlk_mutex_unlock(mutex);
}

/* Destroy mutex */
atlk_inline void
osal_mutex_delete(osal_mutex_t *mutex)
{
  atlk_mutex_destroy(mutex);
}

#endif /* _PLATFORM_OSAL_H */
