#include <platform/common.h>
#include <atlk/cond.h>

#ifdef __THREADX__
#include <tx_api.h>
#endif

atlk_rc_t
atlk_cond_create(atlk_cond_t *cond, const char *name)
{
#if defined ATLK_OS_POSIX
  int rc;
#elif defined __THREADX__
  UINT rc;
#endif

  /* Validate arguments */
  if (!cond) {
    TR_ERROR_NO_ARGS("Condition variable not specified");
    return ATLK_E_INVALID_ARG;
  }  

  /* Make sure the condition variable is not already created */
  if (cond->created) {
    TR_ERROR_NO_ARGS("Condition variable is not initialized or already created");
    return ATLK_E_INVALID_STATE;
  }

  /* Create the condition variable */
#if defined ATLK_OS_POSIX
  rc = pthread_cond_init(&cond->cond, NULL);
  if (rc) {
    return errno_to_atlk_rc(rc);
  }
#elif defined __THREADX__
  /* Create the internal semaphore for this condition variable */
  rc = tx_semaphore_create(&cond->semaphore, (char *)name, 0);
  BUG_ON(rc != TX_SUCCESS);
#endif

  /* Mark the condition variable as created */
  cond->created = 1;

  return ATLK_OK;
}

void
atlk_cond_broadcast(atlk_cond_t *cond)
{
#if defined ATLK_OS_POSIX
  int rc = pthread_cond_broadcast(&cond->cond);
  BUG_ON(rc);

#elif defined __THREADX__
  TX_SEMAPHORE *semaphore;
  TX_THREAD *thread;
  UINT rc;
  ULONG sem_count;
  UINT old_threshold, dummy;

  semaphore = &cond->semaphore;
  sem_count = semaphore->tx_semaphore_suspended_count;

  if (!sem_count) {
    return;
  }

  rc = tx_semaphore_prioritize(semaphore);
  BUG_ON(rc != TX_SUCCESS);

  thread = tx_thread_identify();

  rc = tx_thread_preemption_change(thread, 0, &old_threshold); 
  BUG_ON(rc != TX_SUCCESS);

  while (sem_count) {
    rc = tx_semaphore_put(semaphore);
    BUG_ON(rc != TX_SUCCESS);
    sem_count--;
  }

  rc = tx_thread_preemption_change(thread, old_threshold, &dummy);
  BUG_ON(rc != TX_SUCCESS);
#endif
}

atlk_rc_t atlk_must_check
atlk_cond_rel_timedwait(atlk_cond_t *cond,
            atlk_mutex_t *mutex,
            const struct timeval *tv)
{
#if defined ATLK_OS_POSIX
  struct timespec ts, ts_now;
  uint32_t ns;
  int rc;

  clock_gettime(CLOCK_REALTIME, &ts_now);
  ns = ts_now.tv_nsec + tv->tv_usec * NANO_PER_MICRO;
  ts.tv_sec = ts_now.tv_sec + tv->tv_sec + ns / NANO_PER_UNIT;
  ts.tv_nsec = ns % NANO_PER_UNIT;

  rc = pthread_cond_timedwait(&cond->cond, &mutex->mutex, &ts);
  BUG_ON(rc && rc != ETIMEDOUT);

  return errno_to_atlk_rc(rc);

#elif defined __THREADX__
  TX_SEMAPHORE *semaphore;
  ULONG wait_option, time_ms;
  UINT rc, rc_sem, old_threshold, dummy;
  TX_THREAD *thread;

  thread = tx_thread_identify();
  rc = tx_thread_preemption_change(thread, 0, &old_threshold);
  BUG_ON(rc != TX_SUCCESS);

  atlk_mutex_unlock(mutex);

  semaphore = &cond->semaphore;
  time_ms = (tv->tv_usec / MICRO_PER_MILLI) + (tv->tv_sec * MILLI_PER_UNIT);
  wait_option = DIV_ROUND_UP(time_ms, (MILLI_PER_UNIT / TX_TICK_RATE));

  rc_sem = tx_semaphore_get(semaphore, wait_option);
  BUG_ON(rc != TX_SUCCESS && rc != TX_NO_INSTANCE);

  rc = tx_thread_preemption_change(thread, old_threshold, &dummy);
  BUG_ON(rc != TX_SUCCESS);

  if (rc_sem == TX_NO_INSTANCE) {
    atlk_mutex_lock(mutex);
    return ATLK_E_TIMEOUT;
  }

  rc = tx_semaphore_prioritize(semaphore);
  BUG_ON(rc != TX_SUCCESS);

  atlk_mutex_lock(mutex);

  return ATLK_OK;
#endif
}
