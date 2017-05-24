/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdlib.h>
#include <stdio.h>

#include <platform/common.h>
#include <atlk/nmea.h>
#include <atlk/nmea_nav.h>
#include <atlk/coord.h>
#include <atlk/coord_internal.h>
#include <atlk/timestd.h>

/* Conversion factor from knots to meters/sec */
#define KNOTS_TO_MPS 0.514444444

/* Default time error (half of GNSS timestamp resolution) */
#define ERROR_TIME_S_DEFAULT 0.0005

/* Time error when leap seconds are not yet known (arbitrary value) */
#define ERROR_TIME_S_NO_LEAP_SEC 10.0

atlk_rc_t atlk_must_check
nmea_to_nav_fix_time(nav_time_t *time,
            const nmea_timestamp_t *timestamp)
{
  int positive_leap_second = 0, value = 0;
  static const int days_so_far[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  
  BUG_ON(!time || !timestamp);

  if (!nmea_date_valid(&timestamp->utc_date)) {
    TR_DEBUG_NO_ARGS("Invalid UTC date value");
    return ATLK_E_INVALID_ARG;
  }

  if (!nmea_time_valid(&timestamp->utc_time)) {
    TR_DEBUG_NO_ARGS("Invalid UTC time value");
    return ATLK_E_INVALID_ARG;
  }

  if (timestamp->utc_offset_sec == NMEA_INT_NA) {
    TR_DEBUG_NO_ARGS("UTC offset sec is N/A");
    return ATLK_E_INVALID_ARG;
  }

  if (timestamp->utc_offset_sec <
      LEAP_SECONDS_FROM_GPS_1980JAN6_TO_TAI_2004JAN1) {
    TR_DEBUG("Invalid UTC offset sec value %d", timestamp->utc_offset_sec);
    return ATLK_E_INVALID_ARG;
  }

  positive_leap_second = (timestamp->utc_time.seconds == 60 ? 1 : 0);

  value = (timestamp->utc_date.year - 1970) * 365 +
    days_so_far[timestamp->utc_date.month - 1];
  value += (timestamp->utc_date.year - 1968) / 4;
  value -= (timestamp->utc_date.year - 1900) / 100;
  value += (timestamp->utc_date.year - 1600) / 400;
  if (((timestamp->utc_date.year % 100) != 0 ||
       (timestamp->utc_date.year % 400) == 0) &&
      (timestamp->utc_date.year % 4) == 0 && 
      (timestamp->utc_date.month - 1) < 2) {
    value--;
  }

  value += timestamp->utc_date.day - 1;
  value *= 24;
  value += timestamp->utc_time.hours;
  value *= 60;
  value += timestamp->utc_time.minutes;
  value *= 60;
  value += timestamp->utc_time.seconds - positive_leap_second;
  value -= NONLEAP_SECONDS_FROM_1970JAN1_TO_2004JAN1;
  value += timestamp->utc_offset_sec;
  value -= LEAP_SECONDS_FROM_GPS_1980JAN6_TO_TAI_2004JAN1;

  time->tai_seconds_since_2004 =
    (double)value + (double)timestamp->utc_time.microseconds / MICRO_PER_UNIT;
  time->leap_seconds_since_2004 = timestamp->utc_offset_sec - 
    LEAP_SECONDS_FROM_GPS_1980JAN6_TO_TAI_2004JAN1;
  time->positive_leap_second = positive_leap_second;

  return ATLK_OK;
}

atlk_inline void
fix_mode_to_nav_fix(nmea_char_t mode,
            nav_fix_t *fix)
{
  if (mode == NMEA_CHAR_NA) {
    return;
  }

  switch (mode) {
  case NMEA_FIX_MODE_AUTOMATIC:
    fix->data_source = NAV_FIX_USES_GNSS;
    break;
  case NMEA_FIX_MODE_DIFFERENTIAL:
    fix->data_source = NAV_FIX_USES_DGNSS;
    break;
  case NMEA_FIX_MODE_ESTIMATED:
    fix->data_source = NAV_FIX_USES_DR;
    break;
  case NMEA_FIX_MODE_MANUAL:
  case NMEA_FIX_MODE_SIMULATED:
  case NMEA_FIX_MODE_NOT_VALID:
    fix->data_source = 0;
    break;
  }
}

atlk_rc_t atlk_must_check
nmea_data_rmc_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_rmc_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->timestamp.utc_time = data->utc_time;
  nmea_nav->timestamp.utc_date = data->utc_date;

  nmea_nav->fix.position_latitude_deg =
    nmea_float_to_coord(&data->latitude, data->latitude_direction);

  nmea_nav->fix.position_longitude_deg =
    nmea_float_to_coord(&data->longitude, data->longitude_direction);

  nmea_nav->fix.movement_horizontal_direction_deg = 
    fmod(nmea_float_to_double(&data->course) + 360.0, 360.0);

  nmea_nav->fix.movement_horizontal_speed_mps = 
    nmea_float_to_double(&data->speed) * KNOTS_TO_MPS;

  if (!isnan(nmea_nav->fix.movement_horizontal_direction_deg) &&
      !isnan(nmea_nav->fix.movement_horizontal_speed_mps) &&
      nmea_nav->fix.movement_horizontal_speed_mps > nmea_nav->min_speed_mps) {
    nmea_nav->last_heading_deg =
      nmea_nav->fix.movement_horizontal_direction_deg;
  }

  return ATLK_OK;
}

atlk_inline void
fix_quality_to_nav_fix(nmea_int_t quality,
            nav_fix_t *fix)
{
  if (quality == NMEA_INT_NA) {
    return;
  }

  switch (quality) {
  case NMEA_FIX_QUALITY_SPS_MODE:
    fix->data_source = NAV_FIX_USES_GNSS;
    break;
  case NMEA_FIX_QUALITY_DIFFERENTIAL:
    fix->data_source = NAV_FIX_USES_DGNSS;
    break;
  case NMEA_FIX_QUALITY_ESTIMATED:
    fix->data_source = NAV_FIX_USES_DR;
    break;
  case NMEA_FIX_QUALITY_NOT_AVAILABLE:
  case NMEA_FIX_QUALITY_PPS_MODE:
  case NMEA_FIX_QUALITY_RTK_INTEGER:
  case NMEA_FIX_QUALITY_RTK_FLOAT:
  case NMEA_FIX_QUALITY_MANUAL:
  case NMEA_FIX_QUALITY_SIMULATOR:
    fix->data_source = 0;
    break;
  }
}

atlk_rc_t atlk_must_check
nmea_data_gga_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_gga_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->timestamp.utc_time = data->utc_time;

  nmea_nav->fix.position_latitude_deg =
    nmea_float_to_coord(&data->latitude, data->latitude_direction);

  nmea_nav->fix.position_longitude_deg =
    nmea_float_to_coord(&data->longitude, data->longitude_direction);

  fix_quality_to_nav_fix(data->fix_quality, &nmea_nav->fix);

  if (data->altitude_units == 'M' && data->geoidal_separation_units == 'M') {
    nmea_nav->fix.position_altitude_m = nmea_float_to_double(&data->altitude) +
      nmea_float_to_double(&data->geoidal_separation);
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_gsa_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_gsa_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  fix_mode_to_nav_fix(data->fix_mode, &nmea_nav->fix);

  switch (data->fix_type) {
  case NMEA_FIX_TYPE_NOT_AVAILABLE:  
    nmea_nav->fix.mode = NAV_FIX_MODE_TIME_ONLY;
    break;
  case NMEA_FIX_TYPE_2D:
    nmea_nav->fix.mode = NAV_FIX_MODE_2D;
    break;
  case NMEA_FIX_TYPE_3D:
  case NMEA_FIX_TYPE_TESEO_DR:
    nmea_nav->fix.mode = NAV_FIX_MODE_3D;
    break;
  default:
    nmea_nav->fix.mode = NAV_FIX_MODE_NA;
    break;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_gll_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_gll_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->fix.position_latitude_deg =
    nmea_float_to_coord(&data->latitude, data->latitude_direction);

  nmea_nav->fix.position_longitude_deg =
    nmea_float_to_coord(&data->longitude, data->longitude_direction);

  nmea_nav->timestamp.utc_time = data->utc_time;

  fix_mode_to_nav_fix(data->_fix_mode, &nmea_nav->fix);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_gst_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_gst_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->timestamp.utc_time = data->utc_time;

  nmea_nav->fix.error_position_horizontal_semi_major_axis_length_m =
    nmea_float_to_double(&data->semi_major_deviation);

  nmea_nav->fix.error_position_horizontal_semi_minor_axis_length_m = 
    nmea_float_to_double(&data->semi_minor_deviation);

  nmea_nav->fix.error_position_horizontal_major_axis_direction_deg = 
    fmod(nmea_float_to_double(&data->semi_major_orientation) + 360.0, 360.0);

  nmea_nav->fix.error_position_altitude_m = 
    nmea_float_to_double(&data->altitude_error_deviation);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_gsv_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_gsv_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  if (data->total_satellites != NMEA_INT_NA) {
    switch (address->sentence_id) {
    case NMEA_TALKER_ID_GP:
      if (data->sentence_number == 1) {
        nmea_nav->fix.satellites_num[NAV_SATELLITES_GPS] =
          data->total_satellites;
      }
      break;
    case NMEA_TALKER_ID_GL:
      if (data->sentence_number == 1) {
        nmea_nav->fix.satellites_num[NAV_SATELLITES_GLONASS] =
          data->total_satellites;
      }
      break;
    case NMEA_TALKER_ID_GN:
      /* TODO: In this case we need to look at PRNs */
      break;
    case NMEA_TALKER_ID_GA:
    case NMEA_TALKER_ID_BD:
    case NMEA_TALKER_ID_QZ:
    case NMEA_TALKER_ID_NA:
      break;
    }
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_vtg_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_vtg_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  if (data->t == 'T') {
    nmea_nav->fix.movement_horizontal_direction_deg = 
      fmod(nmea_float_to_double(&data->course_true) + 360.0, 360.0);
  }

  if (data->n == 'N') {
    nmea_nav->fix.movement_horizontal_speed_mps = 
      nmea_float_to_double(&data->speed_knots) * KNOTS_TO_MPS;
  }

  fix_mode_to_nav_fix(data->_fix_mode, &nmea_nav->fix);

  if (!isnan(nmea_nav->fix.movement_horizontal_direction_deg) &&
      !isnan(nmea_nav->fix.movement_horizontal_speed_mps) &&
      nmea_nav->fix.movement_horizontal_speed_mps > nmea_nav->min_speed_mps) {
    nmea_nav->last_heading_deg =
      nmea_nav->fix.movement_horizontal_direction_deg;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_gns_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_gns_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->timestamp.utc_time = data->utc_time;

  nmea_nav->fix.position_latitude_deg =
    nmea_float_to_coord(&data->latitude, data->latitude_direction);

  nmea_nav->fix.position_longitude_deg =
    nmea_float_to_coord(&data->longitude, data->longitude_direction);

  /* TODO: Handle fix mode */
  
  if (data->total_satellites != NMEA_INT_NA) {
    switch (address->sentence_id) {
    case NMEA_TALKER_ID_GP:
      nmea_nav->fix.satellites_num[NAV_SATELLITES_GPS] =
        data->total_satellites;
      break;
    case NMEA_TALKER_ID_GL:
      nmea_nav->fix.satellites_num[NAV_SATELLITES_GLONASS] =
        data->total_satellites;
      break;
    case NMEA_TALKER_ID_GN:
      /* Ignore it, includes satellites from all constellations */
      break;
    case NMEA_TALKER_ID_GA:
    case NMEA_TALKER_ID_BD:
    case NMEA_TALKER_ID_QZ:
    case NMEA_TALKER_ID_NA:
      break;
    }
  }

  nmea_nav->fix.position_altitude_m = nmea_float_to_double(&data->altitude) +
    nmea_float_to_double(&data->geoidal_separation);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_stm_utc_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_stm_utc_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->timestamp.utc_time = data->utc_time;
  nmea_nav->timestamp.utc_date = data->utc_date;
  nmea_nav->timestamp.utc_offset_sec = data->utc_offset;

  switch (data->utc_offset_validity) {
  case NMEA_STM_UTC_OFFSET_VALIDITY_VALID:
    nmea_nav->timestamp.utc_offset_status = NMEA_UTC_OFFSET_STATUS_VALID;
    break;
  case NMEA_STM_UTC_OFFSET_VALIDITY_READ_FROM_NVM:
    nmea_nav->timestamp.utc_offset_status =
      NMEA_UTC_OFFSET_STATUS_READ_FROM_NVM;
    break;
  case NMEA_STM_UTC_OFFSET_VALIDITY_NOT_VALID:
    nmea_nav->timestamp.utc_offset_status = NMEA_UTC_OFFSET_STATUS_NOT_VALID;
    break;
  default:
    break;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_stm_tg_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_stm_tg_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->stm_tg_used = 1;

  if (data->timevalid > NMEA_STM_TG_TIME_VALID_TIME_ACCURATE) {
    nmea_nav->fix.error_time_s = ERROR_TIME_S_DEFAULT;
  }
  else {
    nmea_nav->fix.error_time_s = NAN;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_stm_kfcov_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_stm_kfcov_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->fix.error_movement_horizontal_speed_mps =
    nmea_float_to_double(&data->velstd);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_stm_drcov_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_stm_drcov_t *data)
{
  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->fix.error_position_altitude_m = 
    nmea_float_to_double(&data->_height_std_dev);

  nmea_nav->fix.error_movement_horizontal_direction_deg =
    nmea_float_to_double(&data->heading_std_dev);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_ubx_time_to_nav_fix(nmea_nav_t *nmea_nav,
            const nmea_address_t *address,
            const nmea_data_ubx_time_t *data)
{
  nmea_int_t utc_offset_sec = NMEA_INT_NA, fw_default = NMEA_INT_NA;
  nmea_utc_offset_status_t status = NMEA_UTC_OFFSET_STATUS_NA;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!nmea_nav || !address || !data);

  nmea_nav->timestamp.utc_time = data->time;
  nmea_nav->timestamp.utc_date = data->date;

  rc = nmea_data_ubx_time_leap_sec_parse(&data->leap_sec, &utc_offset_sec,
                                         &fw_default);
  if (atlk_error(rc)) {
    return rc;
  }
  status = nmea_nav->timestamp.utc_offset_status;

  nmea_nav->timestamp.utc_offset_sec = utc_offset_sec;
  if (!fw_default) {
    nmea_nav->timestamp.utc_offset_status = 
      NMEA_UTC_OFFSET_STATUS_VALID;
  }
  else if (status != NMEA_UTC_OFFSET_STATUS_SET_BY_USER) {
    nmea_nav->timestamp.utc_offset_status = 
      NMEA_UTC_OFFSET_STATUS_FIRMWARE_DEFAULT;
  }

  return ATLK_OK;
}

atlk_inline int
antenna_offset_enabled(const coord_ego_t *ant_offset)
{
  return fabs(ant_offset->x_m) > 0.01 ||
    fabs(ant_offset->y_m) > 0.01 || 
    fabs(ant_offset->z_m) > 0.01;
}

static atlk_rc_t atlk_must_check
cycle_ender_handle(nmea_nav_t *nmea_nav)
{
  atlk_rc_t rc = ATLK_OK;

  /* If PSTMTG disabled or non-Teseo device, assume 3D fix means valid time */
  if (!nmea_nav->stm_tg_used) {
    nmea_nav->fix.error_time_s = 
      (nmea_nav->fix.mode == NAV_FIX_MODE_3D) ? ERROR_TIME_S_DEFAULT : NAN;
  }

  switch (nmea_nav->timestamp.utc_offset_status) {
  case NMEA_UTC_OFFSET_STATUS_READ_FROM_NVM:
  case NMEA_UTC_OFFSET_STATUS_SET_BY_USER:
  case NMEA_UTC_OFFSET_STATUS_VALID:
    break;
  case NMEA_UTC_OFFSET_STATUS_NOT_VALID:
  case NMEA_UTC_OFFSET_STATUS_FIRMWARE_DEFAULT:
    /* Ugly: Override whatever was set and force an arbitrary timestamp error */
    nmea_nav->fix.error_time_s = ERROR_TIME_S_NO_LEAP_SEC;
    break;
  case NMEA_UTC_OFFSET_STATUS_NA:
    TR_DEBUG("UTC offset status %d on cycle ender",
             nmea_nav->timestamp.utc_offset_status);
    return ATLK_E_INVALID_STATE;
  default:
    BUG();
  }

  rc = nmea_to_nav_fix_time(&nmea_nav->fix.time, &nmea_nav->timestamp);
  if (atlk_error(rc)) {
    return rc;
  }

  if (!isnan(nmea_nav->last_heading_deg) &&
      antenna_offset_enabled(&nmea_nav->ant_offset)) {
    coord_lla_t lla = COORD_LLA_INIT;
    const coord_lla_t ref_lla = {
      .latitude_deg = nmea_nav->fix.position_latitude_deg,
      .longitude_deg = nmea_nav->fix.position_longitude_deg,
      .altitude_m = nmea_nav->fix.position_altitude_m
    };
    const coord_ego_t ego = {
      .x_m = -nmea_nav->ant_offset.x_m,
      .y_m = -nmea_nav->ant_offset.y_m,
      .z_m = nmea_nav->ant_offset.z_m
    };

    rc = coord_displace(&lla, &ref_lla, nmea_nav->last_heading_deg, &ego);
    if (atlk_error(rc)) {
      /* Trace and continue, i.e. use nmea_nav->fix with no displacement */
      TR_DEBUG("Failed to calculate position reference (rc=%d)", rc);
    }
    else {
      nmea_nav->fix.position_latitude_deg = lla.latitude_deg;
      nmea_nav->fix.position_longitude_deg = lla.longitude_deg;
      nmea_nav->fix.position_altitude_m = lla.altitude_m;
    }
  }

  return nmea_nav->handler(nmea_nav->service, &nmea_nav->fix);
}

atlk_inline int
is_cycle_ender(const nmea_address_t *a, 
            const nmea_address_t *e)
{
  switch (a->type) {
  case NMEA_TYPE_PARAMETRIC:
    return (e->sentence_id == NMEA_TALKER_ID_NA ||
            a->sentence_id == e->sentence_id) && a->formatter == e->formatter;
  case NMEA_TYPE_PROPRIETARY:
    return a->sentence_id == e->sentence_id && a->formatter == e->formatter;
  case NMEA_TYPE_NA:
  default:
    BUG();
    return 0;
  }
}

#define HANDLE(nmea_data_x_t, nmea_data_x_to_nav_fix) {     \
  nmea_data_x_t *data_x = (nmea_data_x_t *)data;            \
  rc = nmea_data_x_to_nav_fix(nmea_nav, address, data_x);   \
  if (atlk_error(rc)) { break; }                            \
  if (is_cycle_ender(address, &nmea_nav->cycle_ender)) {    \
    return cycle_ender_handle(nmea_nav);                    \
  } break; }

static atlk_rc_t atlk_must_check
nav_fix_callback_parametric(const nmea_address_t *address,
            const void *data,
            nmea_nav_t *nmea_nav)
{
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!address || !data || !nmea_nav);
  
  switch (address->formatter) {
  case NMEA_FORMATTER_RMC:
    HANDLE(nmea_data_rmc_t, nmea_data_rmc_to_nav_fix);
  case NMEA_FORMATTER_GGA:
    HANDLE(nmea_data_gga_t, nmea_data_gga_to_nav_fix);
  case NMEA_FORMATTER_GSA:
    HANDLE(nmea_data_gsa_t, nmea_data_gsa_to_nav_fix);
  case NMEA_FORMATTER_GLL:
    HANDLE(nmea_data_gll_t, nmea_data_gll_to_nav_fix);
  case NMEA_FORMATTER_GST:
    HANDLE(nmea_data_gst_t, nmea_data_gst_to_nav_fix);
  case NMEA_FORMATTER_GSV:
    HANDLE(nmea_data_gsv_t, nmea_data_gsv_to_nav_fix);
  case NMEA_FORMATTER_VTG:
    HANDLE(nmea_data_vtg_t, nmea_data_vtg_to_nav_fix);
  case NMEA_FORMATTER_GNS:
    HANDLE(nmea_data_gns_t, nmea_data_gns_to_nav_fix);
  case NMEA_FORMATTER_NA:
  default:
    BUG();
  }

  return rc;
}

static atlk_rc_t atlk_must_check
nav_fix_callback_proprietary(const nmea_address_t *address,
            const void *data,
            nmea_nav_t *nmea_nav)
{
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!address || !data || !nmea_nav);
  
  switch (address->sentence_id) {
  case NMEA_SENTENCE_ID_STM:
    switch (address->formatter) {
    case NMEA_FORMATTER_STM_UTC:
      HANDLE(nmea_data_stm_utc_t, nmea_data_stm_utc_to_nav_fix);
    case NMEA_FORMATTER_STM_TG:
      HANDLE(nmea_data_stm_tg_t, nmea_data_stm_tg_to_nav_fix);
    case NMEA_FORMATTER_STM_KFCOV:
      HANDLE(nmea_data_stm_kfcov_t, nmea_data_stm_kfcov_to_nav_fix);
    case NMEA_FORMATTER_STM_DRCOV:
      HANDLE(nmea_data_stm_drcov_t, nmea_data_stm_drcov_to_nav_fix);
    case NMEA_FORMATTER_STM_CPU:
    case NMEA_FORMATTER_STM_DRCAL:
    case NMEA_FORMATTER_STM_NA:
    default:
      BUG();
    }
    break;
  case NMEA_SENTENCE_ID_UBX:
    switch (address->formatter) {
    case NMEA_FORMATTER_UBX_TIME:
      HANDLE(nmea_data_ubx_time_t, nmea_data_ubx_time_to_nav_fix);
    case NMEA_FORMATTER_UBX_NA:
    default:
      BUG();
    }
    break;
  }

  return rc;
}

#undef HANDLE /* No longer required */

static atlk_rc_t atlk_must_check
nav_fix_callback(const char *sentence,
            const nmea_address_t *address,
            const void *data,
            void *context)
{
  nmea_nav_t *nmea_nav = NULL;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!sentence || !address || !context);
  
  nmea_nav = (nmea_nav_t *)context;

  switch (address->type) {
  case NMEA_TYPE_PARAMETRIC: {
    rc = nav_fix_callback_parametric(address, data, nmea_nav);
    if (atlk_error(rc)) {
      return rc;
    }
    break;
  }
  case NMEA_TYPE_PROPRIETARY: {
    rc = nav_fix_callback_proprietary(address, data, nmea_nav);
    if (atlk_error(rc)) {
      return rc;
    }
    break;
  }
  case NMEA_TYPE_NA:
  default:
    BUG();
  }

  return rc;
}

atlk_inline void
nmea_timestamp_init(nmea_timestamp_t *timestamp,
            nmea_int_t utc_offset_sec)
{
  timestamp->utc_time = (nmea_time_t)NMEA_TIME_NA;
  timestamp->utc_date = (nmea_date_t)NMEA_DATE_NA;
  timestamp->utc_offset_sec = utc_offset_sec;
  
  if (utc_offset_sec != NMEA_INT_NA) {
    timestamp->utc_offset_status = NMEA_UTC_OFFSET_STATUS_SET_BY_USER;
  }
  else {
    timestamp->utc_offset_status = NMEA_UTC_OFFSET_STATUS_NA;
  }
}

atlk_rc_t atlk_must_check
nmea_nav_init(nmea_nav_t *nmea_nav,
            const nmea_nav_config_t *config)
{
  const nmea_callback_data_t callback_data = {
    .callback = nav_fix_callback,
    .context = (void *)nmea_nav
  };
  nmea_address_t address = NMEA_ADDRESS_INIT;

  if (atlk_unlikely(!nmea_nav || !config)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  nmea_nav->cycle_ender = config->cycle_ender;
  nmea_nav->fix = (nav_fix_t)NAV_FIX_INIT;
  nmea_timestamp_init(&nmea_nav->timestamp, config->utc_offset_sec);
  nmea_nav->ant_offset = config->ant_offset;
  nmea_nav->last_heading_deg = NAN;
  nmea_nav->min_speed_mps = config->min_speed_mps;
  nmea_nav->handler = config->handler;
  nmea_nav->service = config->service;

  /* Register callback for parametric sentences */
  address.type = NMEA_TYPE_PARAMETRIC;
  nmea_callback_register(config->nmea, &address, callback_data);

  /* Register callback for proprietary STM sentences */
  address.type = NMEA_TYPE_PROPRIETARY;
  address.sentence_id = NMEA_SENTENCE_ID_STM;
  address.formatter = NMEA_FORMATTER_STM_UTC;
  nmea_callback_register(config->nmea, &address, callback_data);
  address.formatter = NMEA_FORMATTER_STM_TG;
  nmea_callback_register(config->nmea, &address, callback_data);
  address.formatter = NMEA_FORMATTER_STM_KFCOV;
  nmea_callback_register(config->nmea, &address, callback_data);
  address.formatter = NMEA_FORMATTER_STM_DRCOV;
  nmea_callback_register(config->nmea, &address, callback_data);

  /* Register callback for proprietary UBX sentences */
  address.type = NMEA_TYPE_PROPRIETARY;
  address.sentence_id = NMEA_SENTENCE_ID_UBX;
  address.formatter = NMEA_FORMATTER_UBX_TIME;
  nmea_callback_register(config->nmea, &address, callback_data);

  return ATLK_OK;
}
