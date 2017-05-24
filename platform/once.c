#if defined ATLK_OS_POSIX
#include <pthread.h>
#elif defined __THREADX__
#include <tx_api.h>
#endif

#include <atlk/sdk.h>
#include <platform/common.h>
#include <atlk/once.h>

atlk_rc_t
atlk_once_run(atlk_once_t *once, atlk_once_routine_t routine)
{
#if defined ATLK_OS_POSIX
  int rc;

  rc = pthread_once(once, routine);
  if (rc) {
    return ATLK_E_UNSPECIFIED;
  }

  return ATLK_OK;
#elif defined __THREADX__
  UINT interrupt_status;
  UINT trv;
  UINT old_threshold;
  UINT num_waiting;
  TX_THREAD *thread;

  if (!once || !routine) {
    return ATLK_E_INVALID_ARG;
  }

  /* Disable interrupts */
  interrupt_status = tx_interrupt_control(TX_INT_DISABLE);

  /* Check once control state */
  switch (once->state) {
    case ATLK_ONCE_S_NOT_STARTED:
      /* Set state to running */
      once->state = ATLK_ONCE_S_RUNNING;

      /* Create semaphore so that subsequent callers can wait */
      trv = tx_semaphore_create(&once->semaphore, "Once semaphore", 0);
      BUG_ON(trv != TX_SUCCESS);

      /* Run routine with restored interrupts */
      tx_interrupt_control(interrupt_status);

      routine();

      /* Disable interrupts */
      interrupt_status = tx_interrupt_control(TX_INT_DISABLE);

      /* Set state to "finished" and pick up number of waiting threads */
      once->state = ATLK_ONCE_S_FINISHED;
      num_waiting = once->num_waiting;

      /* Restore interrupts */
      tx_interrupt_control(interrupt_status);

      /* Set preemption threshold to 0 */
      thread = tx_thread_identify();
      trv = tx_thread_preemption_change(thread, 0, &old_threshold);
      BUG_ON(trv != TX_SUCCESS);

      /* Wake up all waiting threads */
      while (num_waiting) {
        trv = tx_semaphore_put(&once->semaphore);
        BUG_ON(trv != TX_SUCCESS);
        num_waiting--;
      }

      /* Restore preemption threshold */
      trv = tx_thread_preemption_change(thread, old_threshold, &old_threshold);
      BUG_ON(trv != TX_SUCCESS);
      break;

    case ATLK_ONCE_S_RUNNING:
      /* Increment number of waiting threads */
      once->num_waiting++;

      /* Restore interrupts */
      tx_interrupt_control(interrupt_status);

      /* Wait for routine completion */
      trv = tx_semaphore_get(&once->semaphore, TX_WAIT_FOREVER);
      BUG_ON(trv != TX_SUCCESS);
      break;

    case ATLK_ONCE_S_FINISHED:
      /* Restore interrupts */
      tx_interrupt_control(interrupt_status);
      break;

    default:
      BUG();
      break;
  }

  return ATLK_OK;
#endif
}

