/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/can.h>
#include <atlk/can_service.h>
#include <atlk/gnss.h>
#include <atlk/gnss_teseo.h>
#include <atlk/mib_service.h>
#include <atlk/mibs/nav-mib.h>

#include "poti-hil.h"

/*
  CRATON ThreadX Teseo SOU Integration Example

  This example demonstrates how to integrate Teseo DR with CAN sensor data
  via Sensor-Over-Uart (SOU) API.

  CAN data format used in this example comes from C2C-CC PoTi Test Fest
  event 2016, testing is done via Hardware In the Loop (HIL) simulation.

  A thread is created in which CAN data is received, parsed and fed to Teseo
  SOU API. Averaging, scaling, SOU message creation and writing is done via
  the SOU module.

  GNSS antenna offset is calculated in the NMEA layer. Published navigation
  position is the vehicle's reference position (in this example it is done
  according to EU standards).

  Vehicle reference position is different in US and EU standards, please
  refer to: 

     - US: SAE J2745/1 - section 6.2.3.
     - EU: ETSI EN 302 637-2 - section B.19.

  Fot this example to actually work, Teseo needs to be flashed with DR
  firmware configured to the correct DR operating mode, sensor data units,
  and so on.

  A GNSS simulator with synchronized CAN playback capabilities can be used
  to test in the lab.
*/
  
/* Example thread priorities */
#define CAN_RECEIVE_THREAD_PRIORITY 40
#define TESEO_SOU_FEEDER_THREAD_PRIORITY 40

/* CAN device ID used in this example */
#define CAN_DEVICE_ID 0

/* GNSS antenna offset relative to vehicle reference point */
#define GNSS_ANTENNA_OFFSET_X_CM (-273L)
#define GNSS_ANTENNA_OFFSET_Y_CM 0L
#define GNSS_ANTENNA_OFFSET_Z_CM (-150L)

#if defined __CRATON_ARM

/* CAN receive thread */
static TX_THREAD can_receive_thread;
static uint8_t can_receive_thread_stack[0x1000];
static void can_receive_thread_entry(ULONG input);

/* CAN service */
static can_service_t *can_service = NULL;

/* CAN socket */
static can_socket_t *can_socket = NULL;

/* CAN ID filter array */
static const can_id_filter_t filter_array[] = {
  { .can_id = 0, .can_id_mask = 0 },
};

/* MIB service */
static mib_service_t *mib_service = NULL;

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* CAN socket configuration */
  can_socket_config_t socket_config = CAN_SOCKET_CONFIG_INIT;
  /* Teseo SOU configuration */
  gnss_teseo_sou_config_t sou_config = GNSS_TESEO_SOU_CONFIG_INIT;

  /* Get default MIB service instance */
  rc = mib_default_service_get(&mib_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Set GNSS antenna offset X (positive towards vehicles front) */
  rc = mib_set_navGnssAntennaOffsetX(mib_service, GNSS_ANTENNA_OFFSET_X_CM);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_navGnssAntennaOffsetX: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set GNSS antenna offset Y (positive towards vehicles right hand side) */
  rc = mib_set_navGnssAntennaOffsetY(mib_service, GNSS_ANTENNA_OFFSET_Y_CM);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_navGnssAntennaOffsetY: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set GNSS antenna offset Z (positive towards ground) */
  rc = mib_set_navGnssAntennaOffsetZ(mib_service, GNSS_ANTENNA_OFFSET_Z_CM);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_navGnssAntennaOffsetZ: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Get default CAN service instance */
  rc = can_default_service_get(&can_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "can_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set socket configuration */
  socket_config.filter_array_ptr = filter_array;
  socket_config.filter_array_size = 1;
  socket_config.device_id = CAN_DEVICE_ID;

  /* Create CAN socket */
  rc = can_socket_create(can_service, &can_socket, &socket_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "can_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Create CAN receive thread */
  trv = tx_thread_create(&can_receive_thread, "can_receive_thread",
                         can_receive_thread_entry, 0,
                         can_receive_thread_stack,
                         sizeof(can_receive_thread_stack),
                         CAN_RECEIVE_THREAD_PRIORITY,
                         CAN_RECEIVE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  /* Set Teseo SOU configuration */
  sou_config.operating_mode = GNSS_TESEO_SOU_OPERATING_MODE_20;
  sou_config.gyro_1axis_params_ptr =
    &poti_hil_vehicle_motion_1_gyro_1axis_params;
  sou_config.wheels_speed_params_ptr =
    &poti_hil_vehicle_motion_2_wheel_speed_params;
  sou_config.sched_params.priority = TESEO_SOU_FEEDER_THREAD_PRIORITY;

  /* Initialize Teseo SOU */
  rc = gnss_teseo_sou_init(&sou_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "gnss_teseo_sou_init: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  return;

error:
  /* Release allocated resources */
  can_socket_delete(can_socket);
  can_service_delete(can_service);
  mib_service_delete(mib_service);
}

static void can_receive_thread_entry(ULONG input)
{
  /* Current reverse gear status */
  int reverse_gear_status = 0;
  /* Previous reverse gear status */
  int prev_reverse_gear_status = 0;
  /* Not using input */
  (void)input;

  while (1) {
    /* Autotalks return code */
    atlk_rc_t rc = ATLK_OK;
    /* Received CAN message data */
    uint8_t data[CAN_DATA_SIZE_MAX];
    /* Received CAN message data size */
    size_t data_size = sizeof(data);
    /* Received CAN ID */
    can_id_t can_id;

    /* Receive CAN message */
    rc = can_receive(can_socket, data, &data_size, &can_id,
                     &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr, "can_receive: %s\n", atlk_rc_to_str(rc));
      continue;
    }

    switch (can_id) {
    case POTI_HIL_CAN_ID_VEHICLE_STATE:
    {
      /* Extract reverse gear status from CAN message */
      reverse_gear_status =
        poti_hil_vehicle_state_reverse_gear_status_get(data, data_size);
      if (reverse_gear_status == -1) {
        /* On failure, use previous reverse gear status */
        reverse_gear_status = prev_reverse_gear_status;
      }

      /* Feed reverse gear status to Teseo SOU */
      rc = gnss_teseo_sou_reverse_gear_data_feed(reverse_gear_status);
      if (atlk_error(rc)) {
        /* Ignore failures */
      }
      break;
    }
    case POTI_HIL_CAN_ID_VEHICLE_MOTION_1:
    {
      /* Extract gyro 1-axis status from CAN message */
      sensor_value_t gyro_1axis = 
        poti_hil_vehicle_motion_1_gyro_1axis_get(data, data_size);

      /* Feed gyro 1-axis to Teseo SOU */
      rc = gnss_teseo_sou_gyro_1axis_data_feed(gyro_1axis);
      if (atlk_error(rc)) {
        /* Ignore failures */
      }
      break;
    }
    case POTI_HIL_CAN_ID_VEHICLE_MOTION_2:
    {
      /* Extract wheels speed from CAN message */
      sensor_wheels_speed_t wheels_speed =
        poti_hil_vehicle_motion_2_wheels_speed_get(data, data_size);

      /* Feed wheels speed to Teseo SOU */
      rc = gnss_teseo_sou_wheels_speed_data_feed(wheels_speed);
      if (atlk_error(rc)) {
        /* Ignore failures */
      }
      break;
    }
    case POTI_HIL_CAN_ID_LATERAL_STATE:
    default:
      break;
    }

    if (reverse_gear_status != prev_reverse_gear_status) {
      /* If reverse gear is enabled, revese antenna offset X */
      int32_t antenna_offset_x_cm = (reverse_gear_status ?
        GNSS_ANTENNA_OFFSET_X_CM : -GNSS_ANTENNA_OFFSET_X_CM);

      /* Set GNSS antenna offset X (positive towards vehicles front) */
      rc = mib_set_navGnssAntennaOffsetX(mib_service, antenna_offset_x_cm);
      if (atlk_error(rc)) {
        fprintf(stderr, "mib_set_navGnssAntennaOffsetX: %s\n",
                atlk_rc_to_str(rc));
      }

      /* Set previous reverse gear status to current */
      prev_reverse_gear_status = reverse_gear_status;
    }
  }
}

#else /* __CRATON_ARM */

void craton_user_init(void)
{
}

#endif /* __CRATON_ARM */
