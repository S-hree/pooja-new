/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>

#include <tx_api.h>

#include <atlk/sntp_client.h>
#include <atlk/mib_service.h>
#include <atlk/mibs/nav-mib.h>

/*
  CRATON System Time Benchmark

  This benchmark enables testing CRATON system time accuracy (set via GNSS
  source and synced with a GNSS device's 1PPS output) against a NTP-based
  reference time source using a SNTP client on CRATON.


  SNTP client configuration

  SNTP client is configured to a unicast polling interval of one second.
  Note that this is technically not allowed according to RFC-4330 (minimum
  allowed polling interval is 15 seconds). We do so under the assumption
  that the user is polling a local server (i.e. a server he has set up in his
  own Network). Please don't poll public NTP servers directly at this polling
  interval!

  The client is configured to a minimum server stratum of 3. Given that the
  user is expected to test against his own NTP server, his server is expected
  to  receive updates from a server of stratum 1 or 2 (stratum 2 servers are
  widely available; stratum 1 less so).

  Max root server dispersion is configured to 100000us. Although this seems
  high, most NTP servers tested against seem to have root dispersion values
  of this order. This value represents the maximal observed error between the
  NTP server and the root server; it does not mean that the error is actually
  100ms (disclaimer: this is my current understanding).


  NTP server configuration

  How to set-up a NTP server on Ubuntu Linux desktop is detailed at:
  
     https://help.ubuntu.com/lts/serverguide/NTP.html

  Tips:

  1. A list of NTP servers in your area is available at: http://www.pool.ntp.org
  2. Configuration:
      * Specifying a server as 'iburst' should speed up NTP time aquision.
      * Make sure to allow clients to synchronize with the server (this is
        disabled by default).
  3. Trouble-shooting:
      * Make sure to restart ntp after editing '/etc/ntp.conf'.
      * Initial NTP time acquisition can take ~minutes, please be patient.
      * Run 'ntpq -p' to see the list of NTP servers and their parameters.
      * Run 'ntpq -c rl' to see additional stats including root dispersion.
      * Make sure a firewall is not blocking NTP traffic.
*/

/* SNTP client thread priority */
#define SNTP_CLIENT_PRIORITY 20

/* IP address of NTP server used in this example */
#define SERVER_ADDRESS (10 << 24 | 10 << 16 | 1 << 8 | 110 << 0)

#ifdef __CRATON_ARM

/* MIB service */
static mib_service_t *mib_service = NULL;

/* Return a textual description for mib_navSysTimeStatus_t */
static const char *
sys_time_status_to_str(mib_navSysTimeStatus_t status)
{
  switch (status) {
  case MIB_navSysTimeStatus_notSet:
    return "not set";
  case MIB_navSysTimeStatus_set:
    return "set";
  default:
    return "unknown";
  }
}

/* Callback that will be invoked on every NTP update received */
static void
sntp_update_callback(const sntp_info_t *info)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* NTP time */
  double ntp_time = 0.0;
  /* System time */
  struct timeval sys_time;
  /* Whether fix is available */
  int fix_available = 0;
  /* System time status */
  mib_navSysTimeStatus_t sys_time_status = MIB_navSysTimeStatus_notSet;
  /* Difference between system time and NTP time */
  int64_t diff_us = 0; 

  /* Get current system time */
  gettimeofday(&sys_time, NULL);
  
  /* Get whether fix is available */
  rc = mib_get_navFixAvailable(mib_service, &fix_available);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_get_navFixAvailable: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Get system time status */
  rc = mib_get_navSysTimeStatus(mib_service, &sys_time_status);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_get_navSysTimeStatus: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Calculate difference between system time and NTP time */
  ntp_time = sntp_time_to_posix_time(info);
  diff_us = (int64_t)sys_time.tv_sec * 1000000 +
    sys_time.tv_usec - ntp_time * 1000000.0;

  /* Print results */
  printf("Fix is %s, system time is %s, diff from NTP time is %lld us.\n",
         fix_available ? "available" : "not available",
         sys_time_status_to_str(sys_time_status), diff_us);
}

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* SNTP client configuration parameters */
  sntp_client_config_t config = SNTP_CLIENT_CONFIG_INIT;

  printf("Starting system time benchmark...\n");

  /* Get default MIB service instance */
  rc = mib_default_service_get(&mib_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Set SNTP client configuration parameters */
  config.sched_params.priority = SNTP_CLIENT_PRIORITY;
  config.update_handler = sntp_update_callback;
  config.ntp_server_address = SERVER_ADDRESS;
  config.type = SNTP_CONNECTION_TYPE_UNICAST;
  config.max_root_dispersion_us = 100000;
  config.min_server_stratum = 3;
  config.unicast_poll_interval_s = 1;

  /* Print filtering parameters used */
  printf("Rejecting NTP updates from NTP server that does not hold to:\n");
  printf("- Max root dispersion: %luus\n", config.max_root_dispersion_us);
  printf("- Min server stratum: %u\n", config.min_server_stratum);

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
