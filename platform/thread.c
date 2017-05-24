#include <atlk/thread.h>
#include <platform/common.h>

#ifdef __THREADX__

#if defined __CRATON_ARM
#define USE_SAFEFAT
#endif /* __CRATON_ARM */

#ifdef USE_SAFEFAT
#include <api/api_fat.h>
#endif /* USE_SAFEFAT */

static void
atlk_thread_entry_exit_callback(TX_THREAD *thread_ptr, UINT event)
{
  if (event == TX_THREAD_EXIT) {
    atlk_thread_t *thread = container_of(thread_ptr, atlk_thread_t, thread);
    UINT rc;

    rc = tx_event_flags_set(&thread->flags, ATLK_THREAD_F_EXIT, TX_OR);
    BUG_ON(rc != TX_SUCCESS);
  }
#ifdef USE_SAFEFAT
  else if (event == TX_THREAD_ENTRY) {
    atlk_thread_t *thread = container_of(thread_ptr, atlk_thread_t, thread);
    if (thread->enable_fs) {
      int _rc = f_enterFS();
      BUG_ON(_rc != F_NO_ERROR);
    }
  }
#endif /* USE_SAFEFAT */
}
#endif /* __THREADX__ */

atlk_rc_t
atlk_thread_create(atlk_thread_t *thread, const atlk_thread_attr_t *attr,
                   atlk_thread_entry_func_t start, void *arg)
{
#if defined ATLK_OS_POSIX
  int rc;
#elif defined __THREADX__
  UINT rc;
#endif

  /* Check arguments validity */
  if (!thread || !attr || !start) {
    TR_ERROR_NO_ARGS("Mandatory function argument not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (!attr->stack) {
    TR_ERROR_NO_ARGS("Thread stack not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Check if thread is already created */
  if (thread->created) {
    TR_ERROR_NO_ARGS("Thread is not initialized or already created");
    return ATLK_E_INVALID_STATE;
  }

#if defined ATLK_OS_POSIX

  /* Create thread */
  rc = pthread_create(&thread->thread, NULL, (void * (*)(void *))start, arg);
  BUG_ON(rc);

#elif defined __THREADX__

  /* Make sure the stack size is large enough */
  if (attr->stack_size < TX_MINIMUM_STACK) {
    TR_ERROR("Thread stack size %zu is too small (< %u)",
      attr->stack_size, TX_MINIMUM_STACK);
    return ATLK_E_INVALID_ARG;
  }

  /* Make sure the thread priority is legal */
  if (attr->priority >= TX_MAX_PRIORITIES) {
    TR_ERROR("Invalid thread priority %u", attr->priority);
    return ATLK_E_INVALID_ARG;
  }

  thread->enable_fs = (attr->flags & ATLK_THREAD_F_ENABLE_FS) ? 1 : 0;

  /* Create thread event flags */
  rc = tx_event_flags_create(&thread->flags, (char *)attr->name);
  BUG_ON(rc != TX_SUCCESS);

  /* Create thread */
  rc = tx_thread_create(&thread->thread, (char *)attr->name, 
                        (void (*) (ULONG))start, (ULONG)arg, 
                        attr->stack, attr->stack_size,
                        attr->priority, attr->priority,
                        usec_to_tick_round_up((uint64_t)attr->time_slice_us),
                        TX_AUTO_START);
  BUG_ON(rc != TX_SUCCESS);

  /* Register thread exit notification */
  rc = tx_thread_entry_exit_notify(&thread->thread,
                                   atlk_thread_entry_exit_callback);
  BUG_ON(rc != TX_SUCCESS);

#endif

  /* Mark the thread as created */
  thread->created = 1;

  return ATLK_OK;
}

void
atlk_thread_join(atlk_thread_t *thread)
{
#if defined ATLK_OS_POSIX
  int rc;
#elif defined __THREADX__
  uint32_t flags;
  UINT rc;
#endif

  if (!thread->created) {
    return;
  }

#if defined ATLK_OS_POSIX
  rc = pthread_join(thread->thread, NULL);
  BUG_ON(rc);
#elif defined __THREADX__
  /* Wait for the thread to exit */
  rc = tx_event_flags_get(&thread->flags, ATLK_THREAD_F_EXIT, 
                          TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);
  BUG_ON(rc != TX_SUCCESS);

  rc = tx_event_flags_delete(&thread->flags);
  BUG_ON(rc != TX_SUCCESS);

  rc = tx_thread_delete(&thread->thread);
  BUG_ON(rc != TX_SUCCESS);
#endif

  /* Unset thread creation so it can be re-used */
  thread->created = 0;
}

void
atlk_thread_cancel(atlk_thread_t *thread)
{
#if defined ATLK_OS_POSIX
  int rc;
#elif defined __THREADX__
  UINT rc;
#endif

  if (!thread->created) {
    return;
  }

#if defined ATLK_OS_POSIX
  rc = pthread_cancel(thread->thread);
  BUG_ON(rc);
#elif defined __THREADX__
  rc = tx_thread_entry_exit_notify(&thread->thread, TX_NULL);
  BUG_ON(rc != TX_SUCCESS);

  rc = tx_event_flags_delete(&thread->flags);
  BUG_ON(rc != TX_SUCCESS);

  rc = tx_thread_terminate(&thread->thread);
  BUG_ON(rc != TX_SUCCESS);
  
  rc = tx_thread_delete(&thread->thread);
  BUG_ON(rc != TX_SUCCESS);
#endif

  /* Unset thread creation so it can be re-used */
  thread->created = 0;
}
