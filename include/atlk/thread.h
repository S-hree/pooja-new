/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_THREAD_H
#define _ATLK_THREAD_H

#include <platform/common.h>

#if defined ATLK_OS_POSIX
#include <pthread.h>
#elif defined __THREADX__
#include <tx_api.h>
#endif

#define ATLK_THREAD_F_ENABLE_FS 0x1

/**
   @file
   Thread API

   Provides basic thread API on POSIX and ThreadX runtime environments.
*/

typedef uint16_t atlk_thread_prio_t;

/** Thread attributes */
typedef struct {
  /** Thread name (optional) */
  const char *name;

  /** Thread stack memory area */
  void *stack;

  /** Thread stack size */
  size_t stack_size;

  /** Thread scheduling priority */
  atlk_thread_prio_t priority;

  /** Thread time slice in micro-seconds */
  uint32_t time_slice_us;

  /** Flags */
  uint32_t flags;
  
} atlk_thread_attr_t;

/** Thread attributes default initializer */
#define ATLK_THREAD_ATTR_INIT { 0 }

/** Event flag that indicates thread exit */
#define ATLK_THREAD_F_EXIT 0x1

/** Thread descriptor */
typedef struct {
  int created;
#if defined ATLK_OS_POSIX
  pthread_t thread;
#elif defined __THREADX__
  TX_THREAD thread;
  TX_EVENT_FLAGS_GROUP flags;
  int enable_fs;
#endif
} atlk_thread_t;

/** Thread descriptor static initializer */
#define ATLK_THREAD_INIT { 0 }

/** Thread descriptor dynamic initializer */
atlk_inline void
atlk_thread_init(atlk_thread_t *thread)
{
  thread->created = 0;
}

/** Thread entry function prototype */
typedef void (*atlk_thread_entry_func_t)(void *);

/**
   Create thread.

   Create a thread that starts execution immediately at the specified
   entry function. The stack, priority and time-slice are among
   the specified attributes.

   @param[in,out] thread Initialized thread descriptor
   @param[in] attr Thread attributes
   @param[in] start Thread entry function
   @param[in] arg Entry function argument

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
atlk_thread_create(atlk_thread_t *thread, const atlk_thread_attr_t *attr,
  atlk_thread_entry_func_t start, void *arg);

/**
   Join thread.

   Wait until @p thread is terminated.
   Suspend execution of the calling thread until the target @p thread
   terminates, unless the target @p thread has already terminated.
   When atlk_thread_join() returns, the target thread has been terminated.

   @param[in,out] thread Thread to wait for
*/
void
atlk_thread_join(atlk_thread_t *thread);

/**
   Cancel thread.

   @param[in,out] thread Thread to cancel
*/
void
atlk_thread_cancel(atlk_thread_t *thread);

#endif /* _ATLK_THREAD_H */
