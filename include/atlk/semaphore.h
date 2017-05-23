/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_SEMAPHORE_H
#define _ATLK_SEMAPHORE_H

#include <platform/common.h>

#if defined ATLK_OS_POSIX
#include <semaphore.h>
#elif defined __THREADX__
#include <tx_api.h>
#endif

/**
   @file
   Semaphore API

   Provides semaphore API on POSIX and ThreadX runtime environments.
*/

/** Semaphore descriptor */
typedef struct {
  int created;
#if defined ATLK_OS_POSIX
  sem_t semaphore;
#elif defined __THREADX__
  TX_SEMAPHORE semaphore;
#endif
} atlk_sem_t;

/** Semaphore static initializer */
#define ATLK_SEMAPHORE_INIT { 0 }

/** Semaphore dynamic initializer */
atlk_inline void
atlk_semaphore_init(atlk_sem_t *sem)
{
  sem->created = 0;
}

/**
   Create semaphore.

   @param[in,out] sem Initialized semaphore descriptor
   @param[in] name Semaphore name
   @param[in] value Semaphore initial value

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
atlk_sem_create(atlk_sem_t *sem, const char *name, unsigned int value);

/**
   Destroy semaphore.

   @param[in,out] sem Semaphore to destroy
*/
atlk_inline void
atlk_sem_destroy(atlk_sem_t *sem)
{
  if (sem->created) {
#if defined ATLK_OS_POSIX
    int rc = sem_destroy(&sem->semaphore);
    BUG_ON(rc);
#elif defined __THREADX__
    UINT rc = tx_semaphore_delete(&sem->semaphore);
    BUG_ON(rc != TX_SUCCESS);
#endif
    sem->created = 0;
  }
}

/**
   Acquire semaphore.

   @param[in] sem Semaphore to acquire
*/
atlk_inline void
atlk_sem_wait(atlk_sem_t *sem)
{
#if defined ATLK_OS_POSIX
  int rc = sem_wait(&sem->semaphore);
  BUG_ON(rc);
#elif defined __THREADX__
  UINT rc = tx_semaphore_get(&sem->semaphore, TX_WAIT_FOREVER);
  BUG_ON(rc != TX_SUCCESS);
#endif
}

/**
   Release semaphore.

   @param[in] sem Semaphore to release
*/
atlk_inline void
atlk_sem_post(atlk_sem_t *sem)
{
#if defined ATLK_OS_POSIX
  int rc = sem_post(&sem->semaphore);
  BUG_ON(rc);
#elif defined __THREADX__
  UINT rc = tx_semaphore_put(&sem->semaphore);
  BUG_ON(rc != TX_SUCCESS);
#endif
}

/**
   Get semaphore current value.

   @param[in] sem Semaphore to get value of
   @param[out] sval Semaphore value
*/
atlk_inline void
atlk_sem_getvalue(atlk_sem_t *sem, int *sval)
{
#if defined ATLK_OS_POSIX
  int rc = sem_getvalue(&sem->semaphore, sval);
  BUG_ON(rc);
#elif defined __THREADX__
  ULONG value;
  UINT rc = tx_semaphore_info_get(&sem->semaphore, TX_NULL, &value, 
    TX_NULL, TX_NULL, TX_NULL);
  BUG_ON(rc != TX_SUCCESS);
  *sval = (int)value;
#endif
}

#endif /* _ATLK_SEMAPHORE_H */
