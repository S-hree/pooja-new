/* Copyright (C) 2015 Autotalks Ltd. */
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#include <tx_api.h>

#include <atlk/nav.h>
#include <atlk/nav_service.h>

#include <atlk/mib_service.h>
#include <atlk/mibs/nav-mib.h>

#include <craton/uart_driver.h>

/*
  CRATON ThreadX GNSS Integration Example

  This example demonstrates how to integrate an arbitrary GNSS device with 
  the navigation sub-system of the Autotalks SDK.

  
  The function gnss_poll() is a "place holder" for user NMEA (or binary data)
  parsing code. The function's implementation is expected to do the following:

    1. Read from passed file descriptor.
    2. Parse standard and relevant proprietary NMEA messages (or proprietary
       binary data when relevant).
    3. Fill passed struct nav_fix_t and return the OK return code.

  In case of errors, user should return appropriate return code indicating
  error (or handle it in whichever way he sees fit).


  The navigation sub-system includes a component called "nav-update". This
  component is a subscriber of navigation fixes and is in charge of the
  following:

    1. Updating system time.
    2. Updating MIB attribute navFixAvailable.
    3. Updating MIB attribute navSysTimeLeapSeconds.
 
  Updating of the MIB attribute navSysTimeLeapSeconds is especially important
  for proper functionality of system time; specifically, it is vital for 
  gettimeofday() functionality.
  
  To update this MIB attribute, the field leap_seconds_since_2004 is expected
  to be filled in nav_fix_t.
  
  If this is not possible, it is recommended to get the value of MIB attribute
  navSysTimeLeapSeconds during init and fill this field with this fixed value.
  Note that currently the value returned (which is hard-coded) is only correct
  from July 1st 2015 and until the next leap second event occurs.


  This example assumes the following:

    1. The firmware is initialized in 'gnss' mode.
    2. GNSS device UART is connected to CRATON's 2nd UART device.
    3. GNSS device PPS output is connected to CRATON's PPS input.
*/

/* GNSS thread priority */
#define GNSS_THREAD_PRIORITY 40

#if defined __CRATON_NO_ARC 

/* GNSS thread */
static TX_THREAD gnss_thread;
static uint8_t gnss_thread_stack[0x2000];
static void gnss_thread_entry(ULONG input);

/* GNSS UART device ID */
#define UART_DEVICE_ID 1

/* GNSS UART device path */
#define UART_DEVICE_PATH "/dev/uart1"

/* GNSS UART device speed */
#define UART_SPEED_BPS UART_SPEED_230400_BPS

void craton_user_init(void)
{
  /* MIB service */
  mib_service_t *mib_service = NULL;
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* Autotalks return code*/
  atlk_rc_t rc = ATLK_OK;

  /* Get MIB service */
  rc = mib_default_service_get(&mib_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Set navigation data source to 'none' */
  rc = mib_set_navDataSource(mib_service, MIB_navDataSource_none);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_navDataSource: %s\n", atlk_rc_to_str(rc));
    goto out;
  }

  /* Create GNSS thread */
  trv = tx_thread_create(&gnss_thread, "gnss_thread",
                         gnss_thread_entry, 0,
                         gnss_thread_stack,
                         sizeof(gnss_thread_stack),
                         GNSS_THREAD_PRIORITY,
                         GNSS_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

out:
  /* Delete MIB service */
  mib_service_delete(mib_service);
  
  return;
}

/* GNSS polling function */
static atlk_rc_t atlk_must_check gnss_poll(int fd, nav_fix_t *nav_fix)
{
  /* Avoid 'unused parameter' compilation errors */
  (void)fd;
  (void)nav_fix;

  /*
     Place holder for user code (see description at top of page).

     Sleeping 50ms to simulate the behavior of a GNSS device with update
     rate of 20Hz (as well as to avoid a busy-loop in this example).
  */
  usleep(50 * 1000);
  
  return ATLK_OK;
}

/* GNSS thread entry */
static void gnss_thread_entry(ULONG input)
{
  /* Not using input */
  (void)input;
  /* Navigation service */
  nav_service_t *nav_service = NULL;
  /* File descriptor */
  int fd = -1;
  /* Autotalks return code*/
  atlk_rc_t rc = ATLK_OK;

  /* Open file descriptor for UART device */
  fd = open(UART_DEVICE_PATH, 0);
  if (fd < 0) {
    fprintf(stderr, "open failed, errno %d\n", errno);
    return;
  }

  /* Set UART device speed */
  rc = uart_speed_set(UART_DEVICE_ID, UART_SPEED_BPS);
  if (atlk_error(rc)) {
    fprintf(stderr, "uart_speed_set: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Sleep 10ms to let UART device speed settle */
  usleep(10 * 1000);

  /* Get default navigation service instance */
  rc = nav_default_service_get(&nav_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "nav_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  while(1) {
    /* Navigation data fix */
    nav_fix_t nav_fix = NAV_FIX_INIT;

    /* Poll GNSS device for next navigation data fix */
    rc = gnss_poll(fd, &nav_fix);
    if (atlk_error(rc)) {
      fprintf(stderr, "gnss_poll: %s\n", atlk_rc_to_str(rc));
      continue;
    }

    /* Publish navigation data fix to all subscribers */
    rc = nav_fix_publish(nav_service, &nav_fix);
    if (atlk_error(rc)) {
      fprintf(stderr, "nav_fix_publish: %s\n", atlk_rc_to_str(rc));
      continue;
    }
  }

error:
  /* Close file descriptor */
  close(fd);

  return;
}

#else /* __CRATON_NO_ARC */ 

void craton_user_init(void)
{
}

#endif /* __CRATON_NO_ARC */
