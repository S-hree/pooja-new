/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_MUTEX_H
#define _ATLK_MUTEX_H

#include <platform/common.h>

#if defined ATLK_OS_POSIX
#include <pthread.h>
#elif defined __THREADX__
#include <tx_api.h>
#endif

/**
   @file
   Mutex API

   Provides mutex API on POSIX and ThreadX runtime environments.
*/

/** Mutex descriptor */
typedef struct {
  int created;
#if defined ATLK_OS_POSIX
  pthread_mutex_t mutex;
#elif defined __THREADX__
  TX_MUTEX mutex;
#endif
} atlk_mutex_t;

/** Mutex static initializer */
#define ATLK_MUTEX_INIT { 0 }

/** Mutex dynamic initializer */
atlk_inline void
atlk_mutex_init(atlk_mutex_t *mutex)
{
  mutex->created = 0;
}

/**
   Create mutex.

   @param[in,out] mutex Initialized mutex descriptor
   @param[in] name Mutex name

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/ 
atlk_rc_t atlk_must_check
atlk_mutex_create(atlk_mutex_t *mutex, const char *name);

/**
   Destroy mutex.

   @param[in,out] mutex Mutex to destroy
*/
void
atlk_mutex_destroy(atlk_mutex_t *mutex);

/**
   Lock mutex.

   @param[in] mutex Mutex to lock
*/
atlk_inline void
atlk_mutex_lock(atlk_mutex_t *mutex)
{
#if defined ATLK_OS_POSIX
  int rc = pthread_mutex_lock(&mutex->mutex);
  BUG_ON(rc);
#elif defined __THREADX__
  UINT rc = tx_mutex_get(&mutex->mutex, TX_WAIT_FOREVER);
  BUG_ON(rc != TX_SUCCESS);
#endif
}

/**
   Unlock mutex.

   @param[in] mutex Mutex to unlock
*/
atlk_inline void
atlk_mutex_unlock(atlk_mutex_t *mutex)
{
#if defined ATLK_OS_POSIX
  int rc = pthread_mutex_unlock(&mutex->mutex);
  BUG_ON(rc);
#elif defined __THREADX__
  UINT rc = tx_mutex_put(&mutex->mutex);
  BUG_ON(rc != TX_SUCCESS);
#endif
}

#endif /* _ATLK_MUTEX_H */
