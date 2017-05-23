/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <inttypes.h>
#include <string.h>
#include <assert.h>

#include <craton/syslog.h>

#include "poti-hil.h"

/**
   BO_ 272 Vehicle_state: 8 MOVE

   SG_ Gear : 28|4@1+ (1,0) [0|16] "" Vector__XXX

   VAL_ 272 Gear 0 "Park" 1 "Reverse" 2 "Neutral" 3 "Drive" 4 "Motor Brake";
*/
int
poti_hil_vehicle_state_reverse_gear_status_get(const uint8_t *data_ptr,
            size_t data_size)
{
  int reverse_gear_status = 0;
  uint8_t gear = 0;

  assert(data_ptr);

  if (atlk_unlikely(data_size != 8)) {
    syslog(LOG_ERR, "Unexpecetd data size: %zu", data_size);
    return -1;
  }

  gear = (data_ptr[3] & 0xf0) >> 4;
  reverse_gear_status = (gear == 1) ? 1 : 0;

  syslog(LOG_DEBUG, "Got reverse gear status: %d", reverse_gear_status);

  return reverse_gear_status;
}

const sensor_value_params_t
poti_hil_vehicle_motion_1_gyro_1axis_params = {
  .units = SENSOR_UNITS_RADPS,
  .min = -2048,                     /* = -4.096 / 0.002 */
  .max = 2048,                      /* = 4.096 / 0.002 */
  .inverse_scaling = 500            /* = 1 / 0.002 */
};

/**
   BO_ 288 Vehicle_motion_1: 8 MOVE

   SG_ psiD : 40|12@1- (0.002,0) [-4.096|4.096] "rad/sec" Vector__XXX

   CM_ SG_ 288 psiD "Positive number means rotation to the left";
*/
sensor_value_t
poti_hil_vehicle_motion_1_gyro_1axis_get(const uint8_t *data_ptr,
            size_t data_size)
{
  sensor_value_t gyro_1axis = SENSOR_VALUE_NA;
 
  assert(data_ptr);
 
  if (atlk_unlikely(data_size != 8)) {
    syslog(LOG_ERR, "Unexpecetd data size: %zu", data_size);
    return gyro_1axis;
  }

  gyro_1axis = ((uint32_t)data_ptr[5] | ((uint32_t)data_ptr[6] << 8)) & 0xfff;
  if (gyro_1axis & (1 << 11)) {
    gyro_1axis |= 0xfffff000;
  }
  gyro_1axis *= -1;

  syslog(LOG_DEBUG, "Got gyro 1-axis: %" PRIi32, gyro_1axis);
  
  return gyro_1axis;
}

const sensor_value_params_t 
poti_hil_vehicle_motion_2_wheel_speed_params = {
  .units = SENSOR_UNITS_MPS,
  .min = -32768,                    /* = -327.68 / 0.01 */
  .max = 32768,                     /* = 327.68 / 0.01 */
  .inverse_scaling = 100            /* = 1 / 0.01 */
};

/**
   BO_ 289 Vehicle_motion_2: 8 MOVE

   SG_ vx_wheel_FL : 0|16@1- (0.01,0) [-327.68|327.68] "m/s" Vector__XXX
   SG_ vx_wheel_FR : 16|16@1- (0.01,0) [-327.68|327.68] "m/s" Vector__XXX
   SG_ vx_wheel_RL : 32|16@1- (0.01,0) [-327.68|327.68] "m/s" Vector__XXX
   SG_ vx_wheel_RR : 48|16@1- (0.01,0) [-327.68|327.68] "m/s" Vector__XXX
    
   CM_ SG_ 289 vx_wheel_FL "Wheel speed front left";
   CM_ SG_ 289 vx_wheel_FR "Wheel speed front right";
   CM_ SG_ 289 vx_wheel_RL "Wheelspeed rear left";
   CM_ SG_ 289 vx_wheel_RR "Wheelspeed rear right";
*/
sensor_wheels_speed_t
poti_hil_vehicle_motion_2_wheels_speed_get(const uint8_t *data_ptr,
            size_t data_size)
{
  sensor_wheels_speed_t wheels_speed = SENSOR_WHEELS_SPEED_INIT;
  int16_t speed = 0;

  assert(data_ptr);

  if (atlk_unlikely(data_size != 8)) {
    syslog(LOG_ERR, "Unexpecetd data size: %zu", data_size);
    return wheels_speed;
  }

  memcpy(&speed, &data_ptr[0], sizeof(speed));
  wheels_speed.front_left = speed;

  memcpy(&speed, &data_ptr[2], sizeof(speed));
  wheels_speed.front_right = speed;

  memcpy(&speed, &data_ptr[4], sizeof(speed));
  wheels_speed.rear_left = speed;

  memcpy(&speed, &data_ptr[6], sizeof(speed));
  wheels_speed.rear_right = speed;

  syslog(LOG_DEBUG, "Got wheels speed FL: %" PRIi32 ", FR: %"
         PRIi32 ", RL: %" PRIi32 ", RR: %" PRIi32,
         wheels_speed.front_left, wheels_speed.front_right,
         wheels_speed.rear_left, wheels_speed.rear_right);

  return wheels_speed;
}
