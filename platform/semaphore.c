#include <atlk/semaphore.h>
#include <platform/common.h>

atlk_rc_t
atlk_sem_create(atlk_sem_t *sem, const char *name, unsigned int value)
{
#if defined ATLK_OS_POSIX
  int rc;
#elif defined __THREADX__
  UINT rc;
#endif

  /* Validate arguments */
  if (!sem) {
    TR_ERROR_NO_ARGS("Semaphore not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Check if the semaphore is already created */
  if (sem->created) {
    TR_ERROR_NO_ARGS("Semaphore is not initialized or already created");
    return ATLK_E_INVALID_STATE;
  }

  /* Create the semaphore */
#if defined ATLK_OS_POSIX
  rc = sem_init(&sem->semaphore, 0, value);
  BUG_ON(rc);
#elif defined __THREADX__
  rc =  tx_semaphore_create(&sem->semaphore, (char *)name, value);
  BUG_ON(rc);
#endif

  /* Mark the semaphore as created */
  sem->created = 1;

  return ATLK_OK;
}

