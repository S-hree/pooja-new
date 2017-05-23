#include <unistd.h>
#include <assert.h>

#include <tx_posix.h>

/*
  POSIX Compliancy Wrapper API Example

  This example demonstrates basic usage of the POSIX Compliancy Wrapper
  API for code running on top of CRATON processor with ThreadX RTOS.

  It includes examples of six pthreads of different priorities, using a
  message queue, semaphore and mutex. Two additional threads are created
  to demonstrate thread control.

  NOTE: This example is based on "posix_demo.c" which is included with
        the POSIX Compliancy Wrapper for ThreadX package.

  @todo: This example is not currently supported in multi-core SDK.
*/

#define     MAX_MESSAGE_SIZE        50
#define     DEMO_BYTE_POOL_SIZE     9120
#define     EXIT_COUNT              50 
#define     THREAD0_EXIT_COUNT      (EXIT_COUNT / 10) 

/* Set stack size for each thread */ 
#define EXAMPLE_STACK_SIZE (1 << 14)

/* Define the POSIX pthread object control blocks ... */
pthread_t               pthread_0;
pthread_t               pthread_0_join_0;
pthread_t               pthread_0_join_1;
pthread_t               pthread_1;
pthread_t               pthread_2;
pthread_t               pthread_3;
pthread_t               pthread_4;
pthread_t               pthread_5;
pthread_t               pthread_6;
pthread_t               pthread_7;
pthread_t               pthread_8;

/* Define pthread attributes objects */
pthread_attr_t          ptattr0;
pthread_attr_t          ptattr0_join0;
pthread_attr_t          ptattr0_join1;
pthread_attr_t          ptattr1;
pthread_attr_t          ptattr2;
pthread_attr_t          ptattr3;
pthread_attr_t          ptattr4;
pthread_attr_t          ptattr5;
pthread_attr_t          ptattr6;
pthread_attr_t          ptattr7;
pthread_attr_t          ptattr8;


/* Define the message queue attribute.  */
struct mq_attr          queue_atrr;

/* Define a queue descriptor.          */
mqd_t                   q_des;

/* Define a semaphore.                 */
sem_t                   *sem;

/* Define an unnamed semaphore.       */
sem_t                   unnamed_sem;

/* Define a mutex                     */
pthread_mutex_t         mutex1, mutex2;

/* Define a mutex attributes object   */
pthread_mutexattr_t     mta2;

/* Conditional variable               */ 
pthread_cond_t         cond; 

/* Define the counters used in this demo application...  */
unsigned int     pthread_0_counter;
unsigned int     pthread_0_join_0_counter;
unsigned int     pthread_0_join_1_counter;
unsigned int     pthread_1_counter;
unsigned int     pthread_2_counter;
unsigned int     pthread_3_counter;
unsigned int     pthread_4_counter;
unsigned int     pthread_5_counter;
unsigned int     pthread_6_counter;
unsigned int     pthread_7_counter;
unsigned int     pthread_8_counter;
unsigned int     pthread_1_message_sent;
unsigned int     pthread_2_message_received;


/* Define pthread function prototypes.  */
void    *pthread_0_entry(void *);
void    *pthread_0_join_0_entry(void *);
void    *pthread_0_join_1_entry(void *);
void    *pthread_1_entry(void *);
void    *pthread_2_entry(void *);
void    *pthread_3_entry(void *);
void    *pthread_4_entry(void *);
void    *pthread_5_entry(void *);
void    *pthread_6_entry(void *);
void    *pthread_7_entry(void *);
void    *pthread_8_entry(void *);


/* Message to be sent.  */
char *msg0 = "This is a test message";
char msg0_priority = 3;

/* Memory pool for POSIX internal objects and thread stacks. */ 
static char pthread_0_stack[EXAMPLE_STACK_SIZE]; 
static char pthread_1_stack[EXAMPLE_STACK_SIZE]; 
static char pthread_2_stack[EXAMPLE_STACK_SIZE]; 
static char pthread_3_stack[EXAMPLE_STACK_SIZE]; 
static char pthread_4_stack[EXAMPLE_STACK_SIZE]; 
static char pthread_5_stack[EXAMPLE_STACK_SIZE]; 
static char pthread_6_stack[EXAMPLE_STACK_SIZE]; 


/* Define what the initial system looks like.  */
/** 
 *
 * Name: craton_user_init 
 * 
 * Description: User initial system starting point 
 *    Start the main demo threads each thread, or thread couple, 
 *    will demonstrate different POSIX API and properties. 
 *
 * Includes examples for the following POSIX API: 
 * - pthread_attr_init 
 * - pthread_attr_setschedparam 
 * - pthread_attr_setstackaddr
 * - pthread_attr_setstacksize
 * - pthread_create
 * - mq_open
 * - sem_open
 * - pthread_mutex_init
 * - pthread_cond_init
 *
 * */
void craton_user_init(void)
{
  struct sched_param  param;

  queue_atrr.mq_maxmsg  = 124;
  queue_atrr.mq_msgsize = MAX_MESSAGE_SIZE;

  /* Put system definition stuff in here, e.g. pthread creates and 
   * other assoerted create information. */

  /* Create pthread attributes for pthread 0 to pthread 5 */
  pthread_attr_init(&ptattr0);
  pthread_attr_init(&ptattr1);
  pthread_attr_init(&ptattr2);
  pthread_attr_init(&ptattr3);
  pthread_attr_init(&ptattr4);
  pthread_attr_init(&ptattr5);  
  pthread_attr_init(&ptattr6);  

  /* Create a sched_param structure */
  memset(&param, 0, sizeof(param));

  /* Now create all pthreads , firstly modify respective ptheread
     attribute with desired priority and stack start 
     address and then create the pthread */

  /* Create pthread 0.  */
  param.sched_priority = 10;
  pthread_attr_setschedparam(&ptattr0, &param);
  pthread_attr_setstackaddr(&ptattr0, pthread_0_stack);
  pthread_attr_setstacksize(&ptattr0, sizeof(pthread_0_stack));
  pthread_create(&pthread_0, &ptattr0,pthread_0_entry,NULL);

  /* Create pthread 1.  */
  param.sched_priority = 15;
  pthread_attr_setschedparam(&ptattr1, &param);
  pthread_attr_setstackaddr(&ptattr1, pthread_1_stack);
  pthread_attr_setstacksize(&ptattr1, sizeof(pthread_1_stack));
  pthread_create (&pthread_1, &ptattr1,pthread_1_entry,NULL);

  /* Create pthread 2.  */
  param.sched_priority = 20;
  pthread_attr_setschedparam(&ptattr2, &param);
  pthread_attr_setstackaddr(&ptattr2, pthread_2_stack);
  pthread_attr_setstacksize(&ptattr2, sizeof(pthread_2_stack));
  pthread_create (&pthread_2, &ptattr2,pthread_2_entry,NULL);

  /* Create pthread 3.  */
  param.sched_priority = 25;
  pthread_attr_setschedparam(&ptattr3, &param);
  pthread_attr_setstackaddr(&ptattr3, pthread_3_stack);
  pthread_attr_setstacksize(&ptattr3, sizeof(pthread_3_stack));
  pthread_create (&pthread_3, &ptattr3,pthread_3_entry,NULL);

  /* Create pthread 4.  */
  param.sched_priority = 24;
  pthread_attr_setschedparam(&ptattr4, &param);
  pthread_attr_setstackaddr(&ptattr4, pthread_4_stack);
  pthread_attr_setstacksize(&ptattr4, sizeof(pthread_4_stack));
  pthread_create(&pthread_4, &ptattr4,pthread_4_entry,NULL);

  /* Create pthread 5.  */
  param.sched_priority = 30;
  pthread_attr_setschedparam(&ptattr5, &param);
  pthread_attr_setstackaddr(&ptattr5, pthread_5_stack);
  pthread_attr_setstacksize(&ptattr5, sizeof(pthread_5_stack));
  pthread_create (&pthread_5, &ptattr5,pthread_5_entry,NULL);

  /* Create pthread 6.  */
  param.sched_priority = 5;
  pthread_attr_setschedparam(&ptattr6, &param);
  pthread_attr_setstackaddr(&ptattr6, pthread_6_stack);
  pthread_attr_setstacksize(&ptattr6, sizeof(pthread_6_stack));
  pthread_attr_setdetachstate(&ptattr6, PTHREAD_CREATE_DETACHED); 
  pthread_create (&pthread_6, &ptattr6,pthread_6_entry,NULL);

  /* Create pthread 7.  */
  /* Use default values for thread attribute*/
  pthread_attr_init(&ptattr7);  
  pthread_attr_setschedpolicy(&ptattr7, SCHED_FIFO); 
  pthread_create (&pthread_7, &ptattr7, pthread_7_entry, NULL);

  /* Create pthread 8.  */
  /* Use default values for thread attribute*/
  pthread_create (&pthread_8, NULL, pthread_8_entry, NULL);

  /* Create a Message queue.  */
  q_des = mq_open("Queue",O_CREAT|O_RDWR,0,&queue_atrr);

  /* Create a Semaphore.  */
  sem = sem_open("Sem0", O_CREAT | O_EXCL,0,1);

  /* Create an unnamed Semaphore.  */
  sem_init(&unnamed_sem ,0 ,0);

  /* Create a Mutex */
  pthread_mutex_init(&mutex1, NULL);

  /* Create mutex attribute */ 
  pthread_mutexattr_init(&mta2); 
  pthread_mutexattr_setprotocol(&mta2, PTHREAD_PRIO_INHERIT);
  pthread_mutexattr_setpshared(&mta2, PTHREAD_PROCESS_SHARED); 
  pthread_mutexattr_settype(&mta2, PTHREAD_MUTEX_RECURSIVE);  

  /* Create a Mutex */
  pthread_mutex_init(&mutex2, &mta2);

  /* Create a Cond var */ 
  pthread_cond_init(&cond, NULL); 
}

/* Global thread status variables.  */ 
int pt0_status = 0;
int pt0_j0_status = 0;
int pt0_j1_status = 0;
int pt1_status = 0;
int pt2_status = 0;
int pt3_status = 0;
int pt4_status = 0;
int pt5_status = 0;
int pt6_status = 0;
int pt7_status = 0;
int pt8_status = 0;

static int thread_completed_count = 0; 

/** 
 *
 * Name: increase_thread_complete_count 
 * 
 * Description: Detect the completion of all threads excluding thread0. 
 *
 * Includes examples for the following POSIX API: 
 * - pthread_cond_signal/broadcast
 *
 * */
static void increase_thread_complete_count(void)
{
  
  pthread_mutex_lock(&mutex2);
  thread_completed_count++; 
  printf("Thread complete count is %d\n", thread_completed_count);

  if (thread_completed_count == 8) {
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex2);
}


static void pthread_example_clean_up(void) 
{
  int status; 

  /* Destroy the mutex */ 
  status = pthread_mutex_destroy(&mutex1); 
  assert(status == 0); 
  status = pthread_mutex_destroy(&mutex2); 
  assert(status == 0); 

  /* Destroy the mutex attribute */ 
  status = pthread_mutexattr_destroy(&mta2); 
  assert(status == 0); 

  /* Unlink message queue*/ 
  status = mq_unlink("Queue"); 
  assert(status == 0); 

  /* Close message queue*/ 
  status = mq_close(q_des); 
  assert(status == 0); 

  /* Unlink the named semaphore */ 
  status = sem_unlink("Sem0"); 
  assert(status == 0);

  /* Destroy the semaphore */ 
  status = sem_close(sem); 
  assert(status == 0);

  /* Destroy the semaphore */ 
  status = sem_destroy(&unnamed_sem); 
  assert(status == 0);

}

/** 
 *
 * Name: pthread_0_entry 
 * 
 * Description: Demonstrate flow thread flow control - i.e. wait for thread 
 *    completion and thread termination. 
 *
  * Includes examples for the following POSIX API: 
 * - pthread_attr_setstack 
 * - pthread_join 
 * - pthread_cancel 
 * - pthread_attr_destroy
 *
 * Note! 
 * pthread_join can ONLY work from a POSIX thread, if called 
 * from a ThreadX thread call will fail. 
 *
 * */
void    *pthread_0_entry(void *pthread0_input)
{
  int trv; 
  (void)pthread0_input; 

  /* This pthread simply sits in while-forever-sleep loop */ 
  printf("Entered %s\n", __func__); 

  /* Create pthread attributes for child threads */
  pthread_attr_init(&ptattr0_join0);
  pthread_attr_init(&ptattr0_join1);

  /* Create pthread 0.  */
  pthread_attr_setinheritsched(&ptattr0_join0, PTHREAD_INHERIT_SCHED);
  pthread_create(&pthread_0_join_0, &ptattr0_join0, 
                 pthread_0_join_0_entry,NULL);

  /* Create pthread 1.  */
  pthread_attr_setinheritsched(&ptattr0_join1, PTHREAD_INHERIT_SCHED);
  pthread_create(&pthread_0_join_1, &ptattr0_join1, 
                 pthread_0_join_1_entry,NULL);

  printf("%s waiting on join\n", __func__);
  trv = pthread_join(pthread_0_join_0, NULL);
  if (trv) {
    printf("pthread_join in %s failed\n", __func__); 
  }

  printf("%s is trying to cancel pthread_0_join_1_entry \n", __func__);
  trv =  pthread_cancel(pthread_0_join_1);
  if (trv) {
    printf("pthread_join in %s failed\n", __func__); 
  }
  pthread_detach(pthread_0_join_1);

  /* Create pthread attributes for child threads */
  pthread_attr_destroy(&ptattr0_join0);
  pthread_attr_destroy(&ptattr0_join1);

  while(1) 
  {
    /* Increment the pthread counter.*/
    pthread_0_counter++;
    printf("%s sleep %u\n", __func__, pthread_0_counter); 

    /* sleep for a while  */
    pt0_status=sleep(2);
    if(pt0_status)
      break;

    if (pthread_0_counter == THREAD0_EXIT_COUNT) {
      struct timespec time = {1, 0}; 

      /* Wait on timeout for condition */ 
      pthread_mutex_lock(&mutex2);
      pt6_status = pthread_cond_timedwait(&cond, &mutex2, &time);
      pthread_mutex_unlock(&mutex2);

      /* Wait for all threads to complete */ 
      pthread_mutex_lock(&mutex2);
      printf("Waiting for all threads to complete\n");
      pt6_status = pthread_cond_wait(&cond, &mutex2);
      pthread_mutex_unlock(&mutex2);

      pthread_example_clean_up(); 

      printf("Example ended\n"); 

      /* Terminate the thread */ 
      pthread_exit(&pt0_status); 
    }
  }

  return(&pt0_status);
}

/** 
 *
 * Name: pthread_0_join_0_entry 
 * 
 * Description: Simple forever-sleep loop which is exited 
 *    after EXIT_COUNT iterations. 
 *
 * Includes examples for the following POSIX API: 
 * - pthread_self 
 * - pthread_equal 
 * - pthread_exit
 * - nanosleep
 *
 * */
void    *pthread_0_join_0_entry(void *pthread0_join0_input)
{
  struct timespec thread_0_join_0_sleep_time={0,0};
  (void)pthread0_join0_input; 

  printf("Entered %s\n", __func__); 
  while(1) 
  {
    /* Increment the pthread counter.*/
    pthread_0_join_0_counter++;
    printf("%s sleep %u\n", __func__, pthread_0_join_0_counter); 

    if (pthread_0_join_0_counter == EXIT_COUNT) {
      /* Compare pthread ID's */ 
      pt0_j0_status = pthread_equal(pthread_self() ,pthread_0_join_0); 

      /* Verify that received pthread id is the correct one for this thread */ 
      if (!pt0_j0_status) {
        printf("\n Incorrect pthread id in pthread_0_join_0 \n");
        assert(0); 
      }
      /* Terminate the thread */ 
      pthread_exit(&pt0_j0_status); 
    }

    /* sleep for a while  */
    thread_0_join_0_sleep_time.tv_nsec =  9999999;
    pt0_status=nanosleep(&thread_0_join_0_sleep_time,0);
    if(pt0_j0_status)
      break;
  }
  printf("Completed %s\n", __func__); 
  return(&pt0_j0_status);
}

/** 
 *
 * Name: pthread_0_join_1_entry 
 * 
 * Description: Simple forever-sleep loop. 
 *    Terminated on parent thread request. 
 *
 * Includes examples for the following POSIX API: 
 * - pthread_testcancel 
 * - pthread_setcancelstate
 * - pthread_setcanceltype
 *
 * */
void    *pthread_0_join_1_entry(void *pthread0_join1_input)
{
  int old_state = 0; 
  int trv = 0; 
  (void)pthread0_join1_input; 

  struct timespec thread_0_join_1_sleep_time={0,0};

  printf("Entered %s\n", __func__); 
  printf("Cancel state set to ENABLE\n");
  trv = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&old_state);
  if (trv) {
    printf("%s was not able to set cancelstate\n", __func__); 
  }

  trv = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&old_state);
  if (trv) {
    printf("%s was not able to set cancelstate\n", __func__); 
  }

  while(1) 
  {
    /* Increment the pthread counter.*/
    pthread_0_join_1_counter++;
    printf("%s sleep %u\n", __func__, pthread_0_join_1_counter); 

    /* Place a cancel check point to allow parent thread synchronized thread 
     * cancellation  */ 
    pthread_testcancel(); 

    /* sleep for a while  */
    thread_0_join_1_sleep_time.tv_nsec =  9999999; 
    thread_0_join_1_sleep_time.tv_sec =   0; 
    pt0_j1_status=nanosleep(&thread_0_join_1_sleep_time,0);
    if(pt0_j1_status)
      break;
  }
  printf("Completed %s\n", __func__); 
  return(&pt0_j1_status);
}


/** 
 *
 * Name: pthread_1_entry 
 * 
 * Description: Simple example for message queue use. 
 *    Forever loop which sends a message every time thread resumes. 
 *
 * Includes examples for the following POSIX API: 
 * - mq_send 
 *
 * */
void    *pthread_1_entry(void *pthread1_input)
{
  (void)pthread1_input; 

  struct timespec thread_1_sleep_time={0,0};

  printf("Entered %s\n", __func__); 
  /* This thread simply sends a messages to a queue shared by pthread 2.  */
  while(1)
  {

    /* Increment the thread counter.  */
    pthread_1_counter++;
    printf("pthread_1_entry send message %u\n", pthread_1_counter); 

    /* Send message to queue 0.  */
    pt1_status = mq_send(q_des,msg0,strlen(msg0),msg0_priority);
    /* check status.  */
    if(pt1_status)
      break;

    /* Increment the message sent.  */
    pthread_1_message_sent++;

    /* sleep for a while  */
    thread_1_sleep_time.tv_nsec = 900000000;
    nanosleep(&thread_1_sleep_time,0);

    if (pthread_1_counter == EXIT_COUNT) {
      increase_thread_complete_count(); 
      /* Terminate thread */ 
      pthread_exit(&pt1_status); 
    }
  }
  assert(0); 
  return(&pt1_status);
}

/** 
 *
 * Name: pthread_2_entry 
 * 
 * Description: Simple example for message queue use. 
 *    Forever loop which receives a message every time thread resumes. 
 *
 * Includes examples for the following POSIX API: 
 * - mq_receive 
 *
 * */
void    *pthread_2_entry(void *pthread2_input)
{

  char    msgr0[MAX_MESSAGE_SIZE]; 
  unsigned int   priority;
  struct timespec thread_2_sleep_time={0,0};

  (void)pthread2_input; 

  printf("Entered %s\n", __func__); 
  /* This pthread retrieves messages placed on the queue by pthread 1.  */
  while(1 )
  {
    /* Increment the thread counter.  */
    pthread_2_counter++;
    pt2_status = 
        (unsigned int)mq_receive(q_des, msgr0, MAX_MESSAGE_SIZE, 
                                 (ULONG *)&priority);
    printf("pthread_2_entry receive message %u\n", pthread_2_counter); 

    /* Check received data size */ 
    if ((unsigned int)pt2_status != strlen(msg0)) {
      printf("ERROR: Message length for received message is incorrect\n"); 
      break;
    }
    
    /* Check receive message priority */ 
    if (priority != msg0_priority) {
      printf("ERROR: Message priority for received message is incorrect\n"); 
      break;
    }

    if(pt2_status == ERROR)
      break;

    /* Otherwise, it is OK to increment the received message count.  */
    pthread_2_message_received++;
    /* sleep for a while  */
    thread_2_sleep_time.tv_nsec = 900000000;
    nanosleep(&thread_2_sleep_time,0);

    if (pthread_2_counter == EXIT_COUNT) {
      increase_thread_complete_count(); 
      pthread_exit(&pt2_status); 
    }
  }
  assert(0); 
  return(&pt2_status);
}

/** 
 *
 * Name: pthread_3_entry 
 * 
 * Description: Simple example of semaphore use. 
 *    Forever loop requests and releases a semaphore. 
 *
 * Includes examples for the following POSIX API: 
 * - sem_post 
 *
 * */
void    *pthread_3_entry(void *pthread3_input)
{
  struct timespec thread_3_sleep_time={0,0};
  int rv; 
  (void)pthread3_input; 

  printf("Entered %s\n", __func__); 

  /* Wait with timeout for the unnamed semaphore should return with error 
   * as semaphore is not released */
  rv = sem_trywait(&unnamed_sem); 
  if (rv != -1 && posix_errno != EAGAIN) {
    assert(0); 
  }

  /* This function compete for ownership of semaphore_0.  */
  while(1)
  {

    /* Increment the thread counter.  */
    pthread_3_counter++;

    /* Sleep for a while to hold the semaphore.  */
    thread_3_sleep_time.tv_nsec = 900000000;
    nanosleep(&thread_3_sleep_time,0);

    /* Release the semaphore.  */
    pt3_status = sem_post(sem);
    printf("pthread_3_entry release sem\n"); 

    /* Check status.  */
    if (pt3_status )
      break;

    if (pthread_3_counter == EXIT_COUNT) {
      increase_thread_complete_count(); 
      pthread_exit(&pt3_status); 
    }

  }
  assert(0); 
  return(&pt3_status);
}

/** 
 *
 * Name: pthread_4_entry 
 * 
 * Description: Simple example of semaphore use. 
 *    Forever loop requests and releases a semaphore. 
 *
 * Includes examples for the following POSIX API: 
 * - sem_wait 
 * - sem_getvalue
 *
 * */
void    *pthread_4_entry(void *pthread4_input)
{
  ULONG sem_val; 
  struct timespec thread_4_sleep_time={0,0};
  (void)pthread4_input; 

  printf("Entered %s\n", __func__); 

  /* This function compete for ownership of semaphore_0.  */
  while(1)
  {

    /* Increment the thread counter.  */
    pthread_4_counter++;
    printf("pthread_4_entry lock sem %u\n", pthread_4_counter); 

    /* Check the semaphore value */ 
    pt4_status = sem_getvalue(sem, &sem_val);
  
    /* Check status.  */
    if ((pt4_status) && (!sem_val))
      break;

    /* Get the semaphore with suspension.  */ 
    pt4_status = sem_wait(sem);

    /* Check status.  */
    if (pt4_status)
      break;

    /* Sleep for a while to hold the semaphore.  */
    thread_4_sleep_time.tv_nsec = 900000000;
    nanosleep(&thread_4_sleep_time,0);

    if (pthread_4_counter == EXIT_COUNT) {
      increase_thread_complete_count(); 
      pthread_exit(&pt4_status); 
    }

  }
  assert(0); 
  return(&pt4_status);
}

/** 
 *
 * Name: pthread_5_entry 
 * 
 * Description: Simple example of mutex use. 
 *    Forever loop requesting and releasing a mutex. 
 *    pthread_5_entry competes with pthread_6_entry for the mutex. 
 *
 * Includes examples for the following POSIX API: 
 * - pthread_mutex_lock 
 * - pthread_mutex_unlock 
 * - pthread_cond_signal/broadcast
 *
 * */
void    *pthread_5_entry(void *pthread5_input)
{

  struct timespec thread_5_sleep_time={0,0};
  (void)pthread5_input; 

  printf("Entered %s\n", __func__); 
  while(1)
  {
    /* Increment the thread counter.  */
    pthread_5_counter++;
    printf("pthread_5_entry wait on mutex lock %u\n", pthread_5_counter); 

    /* now lock the mutex */
    pt5_status = pthread_mutex_lock(&mutex1);
    if (pt5_status != OK)
      break;

    printf("pthread_5_entry locked mutex %u\n", pthread_5_counter); 

    /* sleep for a while  */
    thread_5_sleep_time.tv_nsec = 900000000;
    nanosleep(&thread_5_sleep_time,0);

    pt5_status = pthread_mutex_unlock(&mutex1);
    printf("pthread_5_entry release mutex %u\n", pthread_5_counter); 
    if (pt5_status != OK)
      break;

    if (pthread_5_counter == EXIT_COUNT) {
      increase_thread_complete_count(); 
      pthread_exit(&pt5_status); 
    }
  }
  assert(0); 
  return(&pt5_status);
}

/** 
 *
 * Name: print_once 
 * 
 * Description: This function should only run once. 
 *
 * Includes examples for the following POSIX API: 
 * - pthread_mutex_timedlock 
 *
 * */
static void print_once(void)
{
  /* Flag to mark that the function was run only once */ 
  static int function_count = 0; 
  struct timespec time={0, 50000000};
  int rv; 

  if (function_count > 1) {
    printf("Error: The print_once function has been called twice \n"); 
    assert(0); 
  }

  function_count++; 
  printf("\n");
  printf(" POSIX Exmaple  \n");
  printf(" Example of pthread_once  \n");
  printf(" This should not be printed again  \n");
  printf("\n");

  rv = pthread_mutex_timedlock(&mutex1, &time);      
  if (rv != ETIMEDOUT) {
    printf("Got mutex1 although it should be locked (rv = %d)\n", rv); 
    assert(0); 
  }
}

/** 
 *
 * Name: pthread_6_entry 
 * 
 * Description: Simple example of mutex use. 
 *    Forever loop requesting and releasing a mutex. 
 *    pthread_5_entry competes with pthread_6_entry for the mutex. 
 *
 * Includes examples for the following POSIX API: 
 *  - pthread_once 
 *
 * */
void    *pthread_6_entry(void *pthread6_input)
{

  unsigned int     try_lock_count = 0;
  struct timespec thread_6_sleep_time={0,0};
  pthread_once_t once_control = PTHREAD_ONCE_INIT; 
  (void)pthread6_input; 

  printf("Entered %s\n", __func__); 
  while(1)
  {
    thread_6_sleep_time.tv_nsec = 50000000;
    nanosleep(&thread_6_sleep_time,0);

    /* Increment the thread counter. */
    printf("pthread_6_entry trylock mutex %u (try #%u)\n", 
           pthread_6_counter, try_lock_count); 

    if (try_lock_count < 10) {
      /* Try to lock the mutex */
      pt6_status = pthread_mutex_trylock(&mutex1);

      if (pt6_status == EBUSY) {
        /* Mutex is locked by another thread sleep and try again  */ 
        try_lock_count++; 
        continue; 
      }
    }
    else {
      /* Insist on locking mutex if lock was not acquired 10 times in a row */ 
      printf("pthread_6_entry wait to lock mutex %u\n", pthread_6_counter); 
      pt6_status = pthread_mutex_lock(&mutex1);

      try_lock_count = 0;
    }

    if (pt6_status != OK)
      break;

    pthread_6_counter++;
    printf("pthread_6_entry locked mutex %u\n", pthread_6_counter); 

    /* sleep for a while  */
    thread_6_sleep_time.tv_nsec = 50000000;
    nanosleep(&thread_6_sleep_time,0);
    pt6_status = pthread_mutex_unlock(&mutex1);
    printf("pthread_6_entry release mutex %u\n", pthread_6_counter); 
    if (pt6_status != OK)
      break;

    if (pthread_6_counter > 10) {
      /* Call on a message routine  */ 
      pthread_once(&once_control, print_once);
    }

    if (pthread_6_counter == EXIT_COUNT) {
      increase_thread_complete_count(); 
      pthread_exit(&pt6_status); 
    }
  }
  assert(0); 
  return(&pt6_status);
}


/** 
 *
 * Name: pthread_7_entry 
 * 
 * Description:
 *
 * Includes examples for the following POSIX API: 
 * - 
 *
 * */
void    *pthread_7_entry(void *pthread7_input)
{
  int policy; 
  struct sched_param param; 
  (void)pthread7_input; 

  printf("Entered %s\n", __func__); 
  pthread_getschedparam(pthread_self(), &policy, &param); 

  while(1)
  {
    /* Increment the thread counter.  */
    pthread_7_counter++;
    printf("pthread_7_entry iteration %u\n", pthread_7_counter); 

    if (pthread_7_counter ==  EXIT_COUNT) {
      pthread_yield();
      pthread_setschedparam(pthread_self(), SCHED_RR, &param);      
    }

    if (pthread_7_counter == 7 * EXIT_COUNT) {
      param.sched_priority = 1;
      pthread_setschedparam(pthread_self(), SCHED_RR, &param);      
    }

    if (pthread_7_counter == 10 * EXIT_COUNT) {
      increase_thread_complete_count(); 
      pthread_exit(&pt7_status); 
    }
  }
  assert(0); 
  return(&pt7_status);
}

/** 
 *
 * Name: pthread_8_entry 
 * 
 * Description:
 *
 * Includes examples for the following POSIX API: 
 * - 
 *
 * */
void    *pthread_8_entry(void *pthread8_input)
{
  (void)pthread8_input; 

  printf("Entered %s\n", __func__); 
  while(1)
  {
    /* Increment the thread counter.  */
    pthread_8_counter++;
    printf("pthread_8_entry iteration %u\n", pthread_8_counter); 

    if (pthread_8_counter == 5 * EXIT_COUNT) {
      int policy; 
      struct sched_param param; 

      pthread_yield();
      pthread_getschedparam(pthread_self(), &policy, &param); 
      pthread_setschedparam(pthread_self(), SCHED_RR, &param);      
    }

    if (pthread_8_counter == 10 * EXIT_COUNT) {
      increase_thread_complete_count(); 
      pthread_exit(&pt8_status); 
    }
  }
  assert(0); 
  return(&pt8_status);
}



