/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_WORK_QUEUE_H
#define _ATLK_WORK_QUEUE_H

#include <platform/common.h>
#include <atlk/list.h>
#include <atlk/thread.h>
#include <atlk/mutex.h>
#include <atlk/cond.h>

/**
   @file
   Work queue API
*/

typedef struct atlk_work atlk_work_t;

/** Work function prototype */
typedef void (*atlk_work_func_t)(atlk_work_t *work);

/** Work descriptor */
struct atlk_work {
  atlk_work_func_t work_func;
  struct list_head list;
};

/** Work initializer */
#define ATLK_WORK_INIT {      \
  .work_func = NULL,          \
  .list = LIST_HEAD_INVALID   \
}

/** Internal work queue states */
typedef enum {
  ATLK_WORK_QUEUE_INITIALIZED,
  ATLK_WORK_QUEUE_RUNNING,
  ATLK_WORK_QUEUE_DESTROYED
} atlk_work_queue_state_t;

/** Work queue attributes */
typedef struct{
  atlk_thread_attr_t thread_attr;
} atlk_work_queue_attr_t;

/** Work queue attributes default initializer */
#define ATLK_WORK_QUEUE_ATTR_INIT { ATLK_THREAD_ATTR_INIT }

/** Work queue descriptor */
typedef struct {
  atlk_work_queue_state_t state;
  struct list_head queue;
  atlk_cond_t event;
  atlk_mutex_t lock;
  atlk_thread_t work_thread;
} atlk_work_queue_t;

/** Work queue static initializer */
#define ATLK_WORK_QUEUE_INIT {              \
  .state = ATLK_WORK_QUEUE_INITIALIZED,     \
  .queue = LIST_HEAD_INVALID,               \
  .event = ATLK_COND_INIT,                  \
  .lock = ATLK_MUTEX_INIT,                  \
  .work_thread = ATLK_THREAD_INIT           \
}

/** Work queue dynamic initializer */
atlk_inline void
atlk_work_queue_init(atlk_work_queue_t *wq)
{
  *wq = (atlk_work_queue_t)ATLK_WORK_QUEUE_INIT;
}

/**
   Prepare work object.

   @param[in,out] work Work object reference to prepare
   @param[in] func Work function
*/
atlk_inline void
atlk_work_prepare(atlk_work_t *work, atlk_work_func_t func)
{
  work->work_func = func;
}

/**
   Enqueue work to work queue.

   Note: must be called from a thread context!

   @param[in,out] wq Initialized work queue
   @param[in] work Prepared work
*/
void
atlk_work_enqueue(atlk_work_queue_t *wq, atlk_work_t *work);

/**
   Destroy work queue.

   @param[in,out] wq Work queue
*/
void
atlk_work_queue_destroy(atlk_work_queue_t *wq);

/**
   Create work queue.

   @param[in,out] wq Initialized work queue
   @param[in] attr Work queue attributes

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t
atlk_work_queue_create(atlk_work_queue_t *wq,
  const atlk_work_queue_attr_t *attr);

#endif /* _ATLK_WORK_QUEUE_H */
