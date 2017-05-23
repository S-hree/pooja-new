/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/nav_service.h>

/*
  CRATON ThreadX Navigation Example

  This example demonstrates basic usage of Navigation API for code running on
  top of CRATON processor with ThreadX RTOS.

  A single threads is created in which to receive navigation data fixes. A
  navigation service is retrieved and a navigation fix subscriber is created.

  Fixes are received in a loop with a non-blocking call to nav_fix_receive.
  When the return code indicates that a new fix is not available, the thread
  sleeps before calling nav_fix_receive again.

  Received fixes are printed.
*/

/* Example thread priority */
#define EXAMPLE_THREAD_PRIORITY 40

/* Example thread */
static TX_THREAD example_thread;
static uint8_t example_thread_stack[0x1000];
static void example_thread_entry(ULONG input);

/* Navigation service */
static nav_service_t *example_nav_service = NULL;

/* Navigation fix subscriber */
static nav_fix_subscriber_t *example_nav_fix_subscriber = NULL;

/* Cleanup any allocated resources */
static void example_cleanup(void)
{
  nav_fix_subscriber_delete(example_nav_fix_subscriber);
  nav_service_delete(example_nav_service);
}

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;

  /* Get default navigation service instance */
  rc = nav_default_service_get(&example_nav_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "nav_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Create a navigation fix subscriber */
  rc = nav_fix_subscriber_create(example_nav_service,
                                 &example_nav_fix_subscriber);
  if (atlk_error(rc)) {
    fprintf(stderr, "nav_fix_subscriber_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Create example thread */
  trv = tx_thread_create(&example_thread, "example_thread",
                         example_thread_entry, 0,
                         example_thread_stack,
                         sizeof(example_thread_stack),
                         EXAMPLE_THREAD_PRIORITY,
                         EXAMPLE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;

error:
  example_cleanup();
}

void example_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Navigation data fix */
  nav_fix_t nav_fix = NAV_FIX_INIT;
  /* Not using input */
  (void)input;

  while (1) {
    /* Receive a navigation fix */
    rc = nav_fix_receive(example_nav_fix_subscriber, &nav_fix, NULL);

    if (rc == ATLK_E_NOT_READY) {
      /* New data has not arrived, try again */
      printf("No new navigation fix\n");
    }
    else if (atlk_error(rc)) {
      /* Unexpected error occurred */
      fprintf(stderr, "nav_fix_receive: %s\n", atlk_rc_to_str(rc));
      goto error;
    }
    else {
      /* Print reported latitude and longitude */
      printf("Navigation fix: latitude %.7lf, longitude %.7lf\n",
	     nav_fix.position_latitude_deg,
	     nav_fix.position_longitude_deg);
    }

    /* Sleep 100 ms */
    usleep(100000);
  }

error:
  example_cleanup();
}
