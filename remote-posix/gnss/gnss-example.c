/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include <atlk/uart.h>
#include <atlk/gnss.h>

/*
  POSIX GNSS  Example

  This example demonstrates basic usage of GNSS API for code running
  externally to CRATON processor with an OS supporting POSIX.
*/

/* GNSS thread priority */
#define GNSS_THREAD_PRIORITY 20

/* GNSS device name */
#define GNSS_DEV_NAME "/dev/ttyUSB0"

/* Implementation of the navigation service instance */
static struct nav_service {
  int __dummy;
} nav_service;

/* Registered GNSS navigation fix handler */
static atlk_rc_t
nav_fix_publish(nav_service_t *service, nav_fix_t *fix)
{
  (void)service;

  printf("nav_fix_publish called!\n");
  
  printf("time: %.1f sec (err: %.3f sec), leap: %d sec%s\n",
         fix->time.tai_seconds_since_2004, fix->error_time_s,
         fix->time.leap_seconds_since_2004,
         fix->time.positive_leap_second ? " (positive leap second)" : "");

  printf("lat: %.7f deg, lon: %.7f deg, alt: %.1f m (err: %.1f m)\n",
         fix->position_latitude_deg, fix->position_longitude_deg,
         fix->position_altitude_m, fix->error_position_altitude_m);

  printf("err ellipse: hdg: %.1f deg, major len: %.1f m, minor len: %.1f m\n",
         fix->error_position_horizontal_major_axis_direction_deg,
         fix->error_position_horizontal_semi_major_axis_length_m,
         fix->error_position_horizontal_semi_minor_axis_length_m);

  printf("heading: %.2f deg (err: %.2f deg)\n",
         fix->movement_horizontal_direction_deg,
         fix->error_movement_horizontal_direction_deg);

  printf("speed: %.1f mps (err: %.1f mps), v-speed: %.1f mps (err: %.1f mps)\n",
         fix->movement_horizontal_speed_mps,
         fix->error_movement_horizontal_speed_mps,
         fix->movement_vertical_speed_mps,
         fix->error_movement_vertical_speed_mps);

  printf("mode: %d, data source: 0x%" PRIu32 ", GP cnt: %d, GL cnt: %d\n",
         fix->mode, fix->data_source,
         fix->satellites_num[NAV_SATELLITES_GPS],
         fix->satellites_num[NAV_SATELLITES_GLONASS]);

  return ATLK_OK;
}

int main(int argc, char *argv[])
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* GNSS configuration parameters */
  gnss_config_t config = GNSS_CONFIG_INIT;
  /* Not using argc */
  (void)argc;
  /* Not using argv */
  (void)argv;

  printf("Start GNSS example...\n");

  /* Set GNSS configuration parameters */
  config.model = GNSS_MODEL_STMICRO_TESEO_II;
  config.dev_name = GNSS_DEV_NAME;
  config.nmea_speed_bps = UART_SPEED_230400_BPS;
  config.sched_params.priority = GNSS_THREAD_PRIORITY;
  config.handler = nav_fix_publish;
  config.service = &nav_service;

  /* Initialize GNSS */
  rc = gnss_init(&config);
  if (atlk_error(rc)) {
    fprintf(stderr, "gnss_init: %s\n", atlk_rc_to_str(rc));
    return EXIT_FAILURE;
  }

  while (1) {
    usleep(1000000);
  }

  return EXIT_SUCCESS;
}
