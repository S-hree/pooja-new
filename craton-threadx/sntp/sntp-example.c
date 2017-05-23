/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

#include <time.h>
#include <sys/time.h>

#include <tx_api.h>

#include <atlk/sntp_client.h>
#include <atlk/mib_service.h>
#include <atlk/mibs/nav-mib.h>

/*
  CRATON SNTP Client Example

  This example demonstrates basic usage of SNTP client API for code running
  on top of CRATON processor with ThreadX RTOS.
*/

/* SNTP client thread priority */
#define SNTP_CLIENT_PRIORITY 20

/* IP address of NTP server used in this example */
#define SERVER_ADDRESS (10 << 24 | 10 << 16 | 1 << 8 | 110 << 0)

#ifdef __CRATON_ARM

/* Example callback that will be invoked on every NTP update received */
static void
sntp_update_callback(const sntp_info_t *info)
{
  /* NTP time */
  double ntp_time = 0.0;
  /* System time */
  struct timeval sys_time;
  /* Buffer for datetime string */
  char buf[64];

  /* Convert NTP time to struct timeval */
  ntp_time = sntp_time_to_posix_time(info);
  sys_time.tv_sec = floor(ntp_time);
  sys_time.tv_usec = ntp_time - (double)sys_time.tv_sec;

  /* Set system time */
  settimeofday(&sys_time, NULL);

  /* Get system time */
  gettimeofday(&sys_time, NULL);

  /* Print system time as datetime string */
  strftime(buf, sizeof(buf), "%b %d %X %Y", localtime(&sys_time.tv_sec));
  printf("Received update! System time set to: %s\n", buf);
}

/* Disable syncing of system time with external 1-PPS */
static atlk_rc_t
disable_pps_sync(void)
{
  /* MIB service */
  mib_service_t *service = NULL;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;

  /* Get default MIB service instance */
  rc = mib_default_service_get(&service);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Disable syncing of system time with external 1-PPS */
  rc = mib_set_navSysTimePpsSyncEnabled(service, 0);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_navSysTimePpsSyncEnabled: %s\n",
            atlk_rc_to_str(rc));
    goto error;
  }

error:
  /* Cleanup resources */
  mib_service_delete(service);

  return rc;
}

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* SNTP client configuration parameters */
  sntp_client_config_t config = SNTP_CLIENT_CONFIG_INIT;

  printf("Starting SNTP client example...\n");

  /* Disable syncing of system time with external 1-PPS */
  rc = disable_pps_sync();
  if (atlk_error(rc)) {
    fprintf(stderr, "Failed to disable syncing with 1-PPS\n"); 
    return;
  }

  /* Set SNTP client configuration parameters */
  config.sched_params.priority = SNTP_CLIENT_PRIORITY;
  config.update_handler = sntp_update_callback;
  config.ntp_server_address = SERVER_ADDRESS;
  config.type = SNTP_CONNECTION_TYPE_UNICAST;
  config.max_root_dispersion_us = 0;
  config.min_server_stratum = 5;
  config.unicast_poll_interval_s = 15;

  /* Initialize SNTP client */
  rc = sntp_client_init(&config);
  if (atlk_error(rc)) {
    fprintf(stderr, "sntp_client_init: %s\n", atlk_rc_to_str(rc));
    return;
  }

  printf("Waiting for NTP update...\n");
}


#else /*__CRATON_ARM */

void craton_user_init(void)
{
}

#endif /* __CRATON_ARM */
