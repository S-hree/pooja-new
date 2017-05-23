/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_ONCE_H
#define _ATLK_ONCE_H

#if defined ATLK_OS_POSIX
#include <pthread.h>
#elif defined __THREADX__
#include <tx_api.h>
#else
#error Neither of ATLK_OS_POSIX or __THREADX__ is defined
#endif

#include <atlk/sdk.h>

/**
   @file
   Once API
*/

#if defined ATLK_OS_POSIX

typedef pthread_once_t atlk_once_t;

#define ATLK_ONCE_INIT PTHREAD_ONCE_INIT

#elif defined __THREADX__

/** Once control state */
typedef enum {
  ATLK_ONCE_S_NOT_STARTED,
  ATLK_ONCE_S_RUNNING,
  ATLK_ONCE_S_FINISHED
} atlk_once_state_t;

/** Once control descriptor */
typedef struct {
  /** Control state */
  atlk_once_state_t state;

  /** Semaphore for waiting for routine completion */
  TX_SEMAPHORE semaphore;

  /** Number of threads waiting for routine completion */
  UINT num_waiting;

} atlk_once_t;

/** Once control static initializer */
#define ATLK_ONCE_INIT {             \
  .state = ATLK_ONCE_S_NOT_STARTED,  \
  .num_waiting = 0                   \
}
#endif

/** Routine to be run once */
typedef void (*atlk_once_routine_t)(void);

/**
   Run once

   @param[in,out] once Once control descriptor
   @param[in] routine Routine to run once

   @retval ::ATLK_OK if arguments are valid
   @retval ::ATLK_E_INVALID_ARG if @p once or @p routine is NULL
*/
atlk_rc_t
atlk_once_run(atlk_once_t *once, atlk_once_routine_t routine);

/**
   Check once state

   @param[in] once Once control descriptor

   @retval ::ATLK_OK if once already finished
   @retval ::ATLK_E_INVALID_STATE if once is not finished
*/
atlk_inline atlk_rc_t atlk_must_check
atlk_once_check(const atlk_once_t *once)
{
#if defined ATLK_OS_POSIX
  return ATLK_E_UNSUPPORTED;
#elif defined __THREADX__
  UINT interrupt_status;
  atlk_rc_t rc = ATLK_OK;

  /* Disable interrupts */
  interrupt_status = tx_interrupt_control(TX_INT_DISABLE);

  if (once->state != ATLK_ONCE_S_FINISHED) {
    rc = ATLK_E_INVALID_STATE;
  }

  /* Restore interrupts */
  tx_interrupt_control(interrupt_status);

  return rc;
#endif
}

#endif /* _ATLK_ONCE_H */
