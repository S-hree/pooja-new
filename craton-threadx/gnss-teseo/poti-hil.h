/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _POTI_HIL_H
#define _POTI_HIL_H

#include <atlk/sdk.h>
#include <atlk/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   C2C-CC POTI HIL (Hardware In the Loop) CAN parser.

   Aligned with DBC used in C2C-CC Test Fest 2016.

   @note: Implementation assumes little endian machine.
*/

/** POTI HIL CAN message IDs */
typedef enum {
  /** Lateral state message ID */
  POTI_HIL_CAN_ID_LATERAL_STATE = 0x106,

  /** Vehicle state message ID */
  POTI_HIL_CAN_ID_VEHICLE_STATE = 0x110,

  /** Vehicle motion 1 message ID */
  POTI_HIL_CAN_ID_VEHICLE_MOTION_1 = 0x120,
  
  /** Vehicle motion 2 message ID */
  POTI_HIL_CAN_ID_VEHICLE_MOTION_2 = 0x121

} poti_hil_can_id_t;

/**
   Get reverse gear status from message 'Vehicle state'.

   @param[in] data_ptr Pointer to start of data
   @param[in] data_size Size of data in bytes

   @return Reverse gear status or -1 on error
*/
int
poti_hil_vehicle_state_reverse_gear_status_get(const uint8_t *data_ptr,
            size_t data_size);

/** Parameters of gyro 1-axis in message 'Vehicle motion 1' */
extern const sensor_value_params_t
poti_hil_vehicle_motion_1_gyro_1axis_params;

/**
   Get gyro 1-axis (yaw) value from message 'Vehicle motion 1'.

   @param[in] data_ptr Pointer to start of data
   @param[in] data_size Size of data in bytes

   @return Gyro 1-axis value or NA value on error
*/
sensor_value_t
poti_hil_vehicle_motion_1_gyro_1axis_get(const uint8_t *data_ptr,
            size_t data_size);

/** Parameters of wheels speed in message 'Vehicle motion 2' */
extern const sensor_value_params_t 
poti_hil_vehicle_motion_2_wheel_speed_params;

/**
   Get wheels speed value from message 'Vehicle motion 2'.

   @param[in] data_ptr Pointer to start of data
   @param[in] data_size Size of data in bytes

   @return Wheels speed value or NA value on error
*/
sensor_wheels_speed_t
poti_hil_vehicle_motion_2_wheels_speed_get(const uint8_t *data_ptr,
            size_t data_size);

#ifdef __cplusplus
}
#endif

#endif /* _POTI_HIL_H */
