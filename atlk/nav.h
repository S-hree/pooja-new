/* Copyright (C) 2012-2015 Autotalks Ltd. */
#ifndef _ATLK_NAV_H
#define _ATLK_NAV_H

#include <stdint.h>
#include <math.h>       /* isnan, NAN */

#ifndef NAN
#define NAN (0.0/0.0)
#endif /* NAN */

#include <atlk/sdk.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   Navigation API
*/

/**
   Navigation timestamp
*/
typedef struct {
  /** Number of TAI seconds since 2004-01-01T00:00:00Z */
  double tai_seconds_since_2004;

  /**
     Net amount of UTC leap seconds between 2004-01-01T00:00:00Z
     and the point in time when fix was generated.
  */
  int16_t leap_seconds_since_2004;

  /** Set to "1" iff timestamp refers to a UTC positive leap second */
  unsigned int positive_leap_second;

} nav_time_t;

/** Navigation timestamp default initializer */
#define NAV_TIME_INIT {                         \
  .tai_seconds_since_2004 = NAN,                \
  .leap_seconds_since_2004 = 0,                 \
  .positive_leap_second = 0                     \
}

/**
   Convert navigation timestamp to POSIX time.

   POSIX time is defined as the number of seconds that have elapsed
   since 1970-01-01T00:00:00Z, not counting leap seconds.
*/
atlk_inline double
nav_time_to_posix_time(const nav_time_t *time)
{
  return time->tai_seconds_since_2004 -
    (double)time->leap_seconds_since_2004 +
    1072915200.0;
}

/** Navigation fix user data buffer size */
#define NAV_FIX_USER_DATA_SIZE 100

/** Navigation fix user data */
typedef struct {
  /** Size of data in user data buffer */
  size_t data_size;

  /** User data buffer */
  uint8_t data[NAV_FIX_USER_DATA_SIZE];

} nav_fix_user_data_t;

/* Navigation fix user data default initializer */
#define NAV_FIX_USER_DATA_INIT {    \
  .data_size = 0,                   \
  .data = { 0 }                     \
}

/** Navigation fix mode */
typedef enum {
  /** Navigation fix mode is N/A */
  NAV_FIX_MODE_NA = 0,

  /**
     No navigation fix.
     
     @note Enumeration name is inaccurate.
  */
  NAV_FIX_MODE_TIME_ONLY = 1,

  /** A 2D (two dimensional) position fix is available */
  NAV_FIX_MODE_2D = 2,

  /** A 3D (three dimensional) position fix is available */
  NAV_FIX_MODE_3D = 3,

} nav_fix_mode_t;

/** Navigation fix uses GNSS */
#define NAV_FIX_USES_GNSS (1U << 0)

/** Navigation fix uses DGNSS */
#define NAV_FIX_USES_DGNSS (1U << 1)

/** Navigation fix uses DR */
#define NAV_FIX_USES_DR (1U << 2)

/** GNSS satellites */
typedef enum {
  /** GPS satellites */
  NAV_SATELLITES_GPS = 0,

  /** GLONASS satellites */
  NAV_SATELLITES_GLONASS  = 1,

  /** The last value in nav_satellites_t */
  NAV_SATELLITES_MAX = NAV_SATELLITES_GLONASS

} nav_satellites_t;

/**
   Navigation fix data frame.

   Time, position, velocity and their error estimates.

   The reference ellipsoid used is the one defined by WGS-84.

   Any floating-point field may be set to NaN if the navigation data
   source didn't provide a value for that field. User programs should
   use the standard isnan() function to check for this condition.

   Note that a non-nan position value does not necessarily mean that GNSS 
   is in a locked state. Please check the value of ::nav_fix_t.mode
*/
typedef struct {
  /** Timestamp of the moment when fix was taken */
  nav_time_t time;

  /**
    Latitude in units of one degree.
     
    When available, its value is in range: [-90.0, 90.0]
  */
  double position_latitude_deg;

  /**
    Longitude in units of one degree.
    
    When available, its value is in range: [-180.0, 180.0]
  */
  double position_longitude_deg;

  /** Altitude above the reference ellipsoid in units of one meter */
  double position_altitude_m;

  /**
    Horizontal movement direction relative to true north (clockwise) in units
    of one degree.

    The angle between true north and the projection of the velocity vector onto
    the ground plane. Also known as "track made good" or "course over ground".

    In case of a skidding vehicle, this direction may differ from the
    vehicle's heading.

    When available, its value is in range [0.0, 360.0]
  */
  double movement_horizontal_direction_deg;

  /**
    Horizontal movement speed in units of one meter per second.

    The magnitude of the projection of the velocity vector onto
    the ground plane. Also known as "speed made good" or "speed over ground".
  */
  double movement_horizontal_speed_mps;

  /**
    Vertical movement speed in units of one meter per second.

    A positive value indicates upward movement, a negative value indicates
    downward movement. Also known as "rate of climb".
  */
  double movement_vertical_speed_mps;

  /** Time standard deviation in units of one second */
  double error_time_s;

  /**
    Horizontal position standard deviation ellipse: major axis direction
    relative to true north (clockwise) in units of one degree.

    When available, its value is in range [0.0, 360.0]
  */
  double error_position_horizontal_major_axis_direction_deg;

  /**
    Horizontal position standard deviation ellipse: semi-major axis length
    in meters.
  */
  double error_position_horizontal_semi_major_axis_length_m;

  /**
    Horizontal position standard deviation ellipse: semi-minor axis length
    in meters.
  */
  double error_position_horizontal_semi_minor_axis_length_m;

  /**
    Vertical position standard deviation in meters.
  */
  double error_position_altitude_m;

  /**
    Horizontal movement direction standard deviation in units of one degree.
  */
  double error_movement_horizontal_direction_deg;

  /**
    Horizontal movement speed standard deviation in units of one meter per
    second.
  */
  double error_movement_horizontal_speed_mps;

  /**
    Vertical movement speed standard deviation in units of one meter per
    second.
  */
  double error_movement_vertical_speed_mps;

  /** Navigation fix mode */
  nav_fix_mode_t mode;

  /** Navigation fix data source */
  uint32_t data_source;

  /** Number of satellites used per satnav system */
  uint8_t satellites_num[NAV_SATELLITES_MAX + 1];

  /** Navigation fix user data */
  nav_fix_user_data_t user_data;

} nav_fix_t;

/** Navigation fix data frame default initializer */
#define NAV_FIX_INIT {                                        \
  .time = NAV_TIME_INIT,                                      \
  .position_latitude_deg = NAN,                               \
  .position_longitude_deg = NAN,                              \
  .position_altitude_m = NAN,                                 \
  .movement_horizontal_direction_deg = NAN,                   \
  .movement_horizontal_speed_mps = NAN,                       \
  .movement_vertical_speed_mps = NAN,                         \
  .error_time_s = NAN,                                        \
  .error_position_horizontal_major_axis_direction_deg = NAN,  \
  .error_position_horizontal_semi_major_axis_length_m = NAN,  \
  .error_position_horizontal_semi_minor_axis_length_m = NAN,  \
  .error_position_altitude_m = NAN,                           \
  .error_movement_horizontal_direction_deg = NAN,             \
  .error_movement_horizontal_speed_mps = NAN,                 \
  .error_movement_vertical_speed_mps = NAN,                   \
  .mode = NAV_FIX_MODE_NA,                                    \
  .data_source = 0,                                           \
  .satellites_num = { 0 },                                    \
  .user_data = NAV_FIX_USER_DATA_INIT                         \
}

/** Navigation service instance */
typedef struct nav_service nav_service_t;

/**
   Navigation fix handler.

   Callback is called each time a new navigation fix is available.
   
   @param[in] service Navigation service instance
   @param[in] fix Navigation fix to handle

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
typedef atlk_rc_t atlk_must_check
(*nav_fix_handler_t)(nav_service_t *service,
            nav_fix_t *fix);

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_NAV_H */
