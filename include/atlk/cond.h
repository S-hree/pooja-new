/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_COND_H
#define _ATLK_COND_H

#include <sys/time.h>
#include <time.h>

#include <platform/common.h>
#include <atlk/mutex.h>

#if defined ATLK_OS_POSIX
#include <pthread.h>
#elif defined __THREADX__
#include <tx_api.h>
#endif

/**
   @file
   Condition variable API

   Provides condition variable API on POSIX and ThreadX runtime environments.
*/

/** Condition variable descriptor */
typedef struct {
  int created;
#if defined ATLK_OS_POSIX
  pthread_cond_t cond;
#elif defined __THREADX__
  TX_SEMAPHORE semaphore;
#endif
} atlk_cond_t;

/** Condition variable static initializer */
#define ATLK_COND_INIT { 0 }

/** Condition variable dynamic initializer */
atlk_inline void
atlk_cond_init(atlk_cond_t *cond)
{
  cond->created = 0;
}

/**
   Create condition variable.

   @param[in,out] cond Initialized condition variable descriptor
   @param[in] name Condition variable name

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
atlk_cond_create(atlk_cond_t *cond, const char *name);

/**
   Destroy condition variable.

   @param[in,out] cond Condition variable to destroy
*/
atlk_inline void
atlk_cond_destroy(atlk_cond_t *cond)
{
  if (cond->created) {
#if defined ATLK_OS_POSIX
    int rc = pthread_cond_destroy(&cond->cond);
    BUG_ON(rc);
#elif defined __THREADX__
    UINT rc = tx_semaphore_delete(&cond->semaphore);
    BUG_ON(rc != TX_SUCCESS);
#endif
    cond->created = 0;
  }
}

/**
   Signal condition variable.

   Unblock at least one of the threads that are blocked on the specified
   condition variable @p cond (if any threads are blocked on @p cond).

   @param[in] cond Condition variable to signal
*/ 
atlk_inline void
atlk_cond_signal(atlk_cond_t *cond)
{
#if defined ATLK_OS_POSIX
  int rc = pthread_cond_signal(&cond->cond);
  BUG_ON(rc);
#elif defined __THREADX__
  TX_SEMAPHORE *semaphore;

  semaphore = &cond->semaphore;
  if (semaphore->tx_semaphore_suspended_count) {
    UINT rc;

    rc = tx_semaphore_prioritize(semaphore);
    BUG_ON(rc != TX_SUCCESS);

    rc = tx_semaphore_put(semaphore);
    BUG_ON(rc != TX_SUCCESS);
  }
#endif
}

/**
   Broadcast condition variable.

   Unblock all threads currently blocked on the specified
   condition variable @p cond.

   @param[in] cond Condition variable to broadcast
*/ 
void
atlk_cond_broadcast(atlk_cond_t *cond);

/**
   Wait on a condition variable.

   Release @p mutex and cause the calling thread to block on
   the condition variable @p cond. Upon successful return, the mutex
   has been locked and is owned by the calling thread.

   @param[in] cond Condition variable to wait on
   @param[in] mutex Mutex associated with this condition
*/ 
atlk_inline void
atlk_cond_wait(atlk_cond_t *cond, atlk_mutex_t *mutex)
{
#if defined ATLK_OS_POSIX
  int rc = pthread_cond_wait(&cond->cond, &mutex->mutex);
  BUG_ON(rc);
#elif defined __THREADX__
  TX_SEMAPHORE *semaphore;
  UINT rc, old_threshold, dummy;
  TX_THREAD *thread;

  thread = tx_thread_identify();
  rc = tx_thread_preemption_change(thread, 0, &old_threshold); 
  BUG_ON(rc != TX_SUCCESS);

  atlk_mutex_unlock(mutex);

  semaphore = &cond->semaphore;

  rc = tx_semaphore_get(semaphore, TX_WAIT_FOREVER);
  if ((rc != TX_SUCCESS) && (rc != TX_WAIT_ABORTED)) {
    BUG();
  }

  rc = tx_thread_preemption_change(thread, old_threshold, &dummy);
  BUG_ON(rc != TX_SUCCESS);

  rc = tx_semaphore_prioritize(semaphore);
  BUG_ON(rc != TX_SUCCESS);

  atlk_mutex_lock(mutex);
#endif
}

/**
   Wait on a condition variable with relative timeout.

   Same as atlk_cond_wait() except that an error is returned
   if the relative time specified by @p tv passes (that is, at least @p tv
   time has passed since system time at function call) before the condition
   cond is signaled or broadcasted.
   
   When such time-outs occur, atlk_cond_rel_timedwait() will nonetheless
   release and reacquire the mutex referenced by @p mutex.

   @param[in] cond Condition variable to wait on
   @param[in] mutex Mutex associated with this condition
   @param[in] tv Maximum time to wait

   @retval ::ATLK_OK if succeeded
   @retval ::ATLK_E_TIMEOUT if timeout occured
   @retval Error code if failed
*/
atlk_rc_t atlk_must_check
atlk_cond_rel_timedwait(atlk_cond_t *cond,
            atlk_mutex_t *mutex,
            const struct timeval *tv);

#endif /* _ATLK_COND_H */
