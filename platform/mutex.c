#include <string.h>
#include <atlk/mutex.h>
#include <platform/common.h>

#if defined ATLK_OS_POSIX
inline static void
atlk_mutex_attr_destroy(pthread_mutexattr_t *mutex_attr)
{
  int rc;

  rc = pthread_mutexattr_destroy(mutex_attr);
  BUG_ON(rc != 0);
}
#endif

atlk_rc_t
atlk_mutex_create(atlk_mutex_t *mutex, const char *name)
{
#if defined ATLK_OS_POSIX
  pthread_mutexattr_t mutex_attr;
  int rc;
#elif defined __THREADX__
  UINT rc;
#endif

  /* Check arguments validity */
  if (!mutex) {
    TR_ERROR_NO_ARGS("Mutex not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Check if the mutex is already created */
  if (mutex->created) {
    TR_ERROR_NO_ARGS("Mutex is not initialized or already created");
    return ATLK_E_INVALID_STATE;
  }

  /* Create mutex */
#if defined ATLK_OS_POSIX
  rc = pthread_mutexattr_init(&mutex_attr);
  if (rc) {
    TR_ERROR("Fail to initialize mutex, rc = %d, %s", rc, strerror(rc));
    BUG();
  }
  pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
  rc = pthread_mutex_init(&mutex->mutex, &mutex_attr);
  if (rc) {
    TR_ERROR("Fail to create mutex, rc = %d, %s", rc, strerror(rc));
    BUG();
  }
  atlk_mutex_attr_destroy(&mutex_attr);
#elif defined __THREADX__
  rc = tx_mutex_create(&mutex->mutex, (char *)name, TX_NO_INHERIT);
  BUG_ON(rc != TX_SUCCESS);
#endif

  /* Mark the mutex as created */
  mutex->created = 1;

  return ATLK_OK;   
}

void
atlk_mutex_destroy(atlk_mutex_t *mutex)
{
  if (mutex->created) {
#if defined ATLK_OS_POSIX
    int rc = pthread_mutex_destroy(&mutex->mutex);
    BUG_ON(rc);
#elif defined __THREADX__
    UINT rc = tx_mutex_delete(&mutex->mutex);
    BUG_ON(rc != TX_SUCCESS);
#endif
    mutex->created = 0; 
  }
}

