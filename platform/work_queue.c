#include <atlk/sdk.h>
#include <platform/common.h>
#include <atlk/list.h>
#include <atlk/thread.h>
#include <atlk/mutex.h>
#include <atlk/cond.h>
#include <atlk/work_queue.h>

static atlk_work_t *
atlk_work_dequeue(atlk_work_queue_t *wq)
{
  struct list_head *entry;

  list_del_head(&wq->queue, &entry);
  if (!entry) {
    return NULL;
  }

  return container_of(entry, atlk_work_t, list);
}

void
atlk_work_enqueue(atlk_work_queue_t *wq, atlk_work_t *work)
{
  BUG_ON(!wq || !work);

  atlk_mutex_lock(&wq->lock);
  list_add_tail(&work->list, &wq->queue);
  atlk_cond_signal(&wq->event);
  atlk_mutex_unlock(&wq->lock);
}

static void
atlk_work_queue_thread(void *queue)
{
  atlk_work_queue_t *wq = queue;

  while (1) {
    atlk_work_t *work;

    atlk_mutex_lock(&wq->lock);

    /* Check the work queue has not been closed */
    if (wq->state == ATLK_WORK_QUEUE_DESTROYED) {
      goto out;
    }

    while (list_empty(&wq->queue)) {
      /* Go to sleep... */
      atlk_cond_wait(&wq->event, &wq->lock);

      /* Woken up! */

      /* Check the work queue has not been closed */
      if (wq->state == ATLK_WORK_QUEUE_DESTROYED) {
        goto out;
      }
    }

    work = atlk_work_dequeue(wq);
    BUG_ON(!work);

    atlk_mutex_unlock(&wq->lock);

    /* Do the work (work object may become invalid after this point) */
    work->work_func(work);
  }

out:
  atlk_mutex_unlock(&wq->lock);
}

void
atlk_work_queue_destroy(atlk_work_queue_t *wq)
{
  if (!wq) {
    return;
  }

  atlk_mutex_lock(&wq->lock);

  /* Mark the work queue as 'destroyed' */
  wq->state = ATLK_WORK_QUEUE_DESTROYED;

  /* Wake up the work thread */
  atlk_cond_signal(&wq->event);

  atlk_mutex_unlock(&wq->lock);

  /* Wait for the work thread to finish */
  atlk_thread_join(&wq->work_thread);

  /* Destroy event */
  atlk_cond_destroy(&wq->event);

  /* Destroy mutex */
  atlk_mutex_destroy(&wq->lock);

  *wq = (atlk_work_queue_t)ATLK_WORK_QUEUE_INIT;
}

atlk_rc_t
atlk_work_queue_create(atlk_work_queue_t *wq,
  const atlk_work_queue_attr_t *attr)
{
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!wq || !attr) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Check work queue state */
  if (wq->state != ATLK_WORK_QUEUE_INITIALIZED) {
    TR_ERROR_NO_ARGS("Work queue is not initialized or is already created");
    return ATLK_E_INVALID_STATE;
  }

  /* Create queue */
  INIT_LIST_HEAD(&wq->queue);

  /* Create mutex */
  rc = atlk_mutex_create(&wq->lock, attr->thread_attr.name);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to create mutex (rc=%d)", rc);
    goto out;
  }

  /* Create event */
  rc = atlk_cond_create(&wq->event, attr->thread_attr.name);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to create condition variable (rc=%d)", rc);
    goto out;
  }

  /* Mark the work queue as 'running' */
  wq->state = ATLK_WORK_QUEUE_RUNNING;

  /* Create work thread (keep this step last) */
  rc = atlk_thread_create(&wq->work_thread, &attr->thread_attr,
    atlk_work_queue_thread, wq);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to create work thread (rc=%d)", rc);
    goto out;
  }

out:

  if (atlk_error(rc)) {
    atlk_cond_destroy(&wq->event);
    atlk_mutex_destroy(&wq->lock);
    *wq = (atlk_work_queue_t)ATLK_WORK_QUEUE_INIT;
  }

  return rc;
}

