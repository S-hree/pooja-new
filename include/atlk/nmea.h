/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _ATLK_NMEA_H
#define _ATLK_NMEA_H

#include <math.h>
#include <limits.h>

#ifndef NAN
#define NAN (0.0/0.0)
#endif /* NAN */

#include <atlk/sdk.h>

#ifdef __cplusplus
extern "C" {
#endif

/** NMEA string size */
#define NMEA_STR_SIZE 76

/** String value */
typedef struct {
  char c[NMEA_STR_SIZE];
} nmea_str_t;

/** String value is N/A */
#define NMEA_STR_NA {       \
  .c[0] = '\0'              \
}

/** Character value */
typedef char nmea_char_t;

/** Character value is N/A */
#define NMEA_CHAR_NA CHAR_MIN

/** Integer value */
typedef int nmea_int_t;

/** Integer value is N/A */
#define NMEA_INT_NA INT_MAX

/** Hexadecimal value */
typedef unsigned int nmea_hex_t;

/** Hexadecimal value is N/A */
#define NMEA_HEX_NA UINT_MAX

/** Date value */
typedef struct {
  int day;
  int month;
  int year;
} nmea_date_t;

/** Date value is N/A */
#define NMEA_DATE_NA {      \
  .day = -1,                \
  .month = -1,              \
  .year = -1                \
}

/**
   Return whether date value is valid.
   
   @note Dates before TAI 2004-01-01T00:00:00Z are not supported.

   @param[in] value Date value

   @return 1 if @p value is valid, 0 otherwise
*/
atlk_inline int
nmea_date_valid(const nmea_date_t *value)
{
  return value->day >= 1 && value->day <= 31 &&
    value->month >= 1 && value->month <= 12 &&
    value->year >= 2004 && value->year <= 2099;
}

/** Time value */
typedef struct {
  int hours;
  int minutes;
  int seconds;
  int microseconds;
} nmea_time_t;

/** Time value is N/A */
#define NMEA_TIME_NA {      \
  .hours = -1,              \
  .minutes = -1,            \
  .seconds = -1,            \
  .microseconds = -1        \
}

/**
   Return whether time value is valid.
   
   @param[in] value Time value

   @return 1 if @p value is valid, 0 otherwise
*/
atlk_inline int
nmea_time_valid(const nmea_time_t *value)
{
  return value->hours >= 0 && value->hours <= 23 &&
    value->minutes >= 0 && value->minutes <= 59 &&
    value->seconds >= 0 && value->seconds <= 60 &&
    value->microseconds >= 0 && value->microseconds <= 999999;
}

/** Float value */
typedef struct {
  int64_t value;
  int64_t scale;
} nmea_float_t;

/** Float value is N/A */
#define NMEA_FLOAT_NA {     \
  .value = -1,              \
  .scale = 0                \
}

/**
   Convert float value to double.

   @param[in] value Value to convert

   @retval NAN on invalid scale
   @return Converted value
*/
atlk_inline double
nmea_float_to_double(const nmea_float_t *value)
{
  if (value->scale == 0) {
    return NAN;
  }

  return (double)value->value / (double)value->scale;
}

/**
   Convert float value (with direction) to decimal degrees.

   @param[in] value Value to convert
   @param[in] direction Coordinate direction

   @retval NAN on invalid scale or direction
   @return Converted value in degrees
*/
atlk_inline double
nmea_float_to_coord(const nmea_float_t *value,
            nmea_char_t direction)
{
  int sign = 0;

  if (value->scale == 0) {
    return NAN;
  }

  switch (direction) {
  case 'N':
  case 'E':
    sign = 1;
    break;
  case 'S':
  case 'W':
    sign = -1;
    break;
  default:
    return NAN;
  }

  return (double)((sign * value->value) / (value->scale * 100)) + \
    (double)((sign * value->value) % (value->scale * 100)) / \
    (60 * value->scale);
}

/** NMEA sentence type */
typedef enum {
  /** Parametric NMEA sentence */
  NMEA_TYPE_PARAMETRIC = 0,

  /** Proprietary NMEA sentence */
  NMEA_TYPE_PROPRIETARY = 1,

  /** NMEA sentence type is NA */
  NMEA_TYPE_NA = 2

} nmea_type_t;

/** NMEA sentence address */
typedef struct {
  /** NMEA sentence type */
  nmea_type_t type;

  /**
     NMEA sentence ID.
  
     If @p type is nmea_type_t::NMEA_TYPE_PARAMETRIC then this is the
     Talker ID (e.g. "GP").

     If @p type is nmea_type_t::NMEA_TYPE_PROPRIETARY then this is the
     Manufacturer's Mnemonic code (e.g. "STM").
  */
  uint8_t sentence_id;

  /**
     NMEA sentence formatter.
     
     If @p type is nmea_type_t::NMEA_TYPE_PARAMETRIC then this is the
     sentence formatter (e.g. "GLL").

     If @p type is nmea_type_t::NMEA_TYPE_PROPRIETARY then this is the
     Manufacturer's sentence formatter (e.g. "UTC").
  */
  uint8_t formatter;
   
} nmea_address_t;

/** NMEA sentence address default initializer */
#define NMEA_ADDRESS_INIT {     \
  .type = NMEA_TYPE_NA,         \
  .sentence_id = UINT8_MAX,     \
  .formatter = UINT8_MAX        \
}

/** Parametric NMEA sentence ID (Talker iD) */
typedef enum {
  NMEA_TALKER_ID_GP = 0,
  NMEA_TALKER_ID_GL,
  NMEA_TALKER_ID_GN,
  NMEA_TALKER_ID_GA,
  NMEA_TALKER_ID_BD,
  NMEA_TALKER_ID_QZ,
  NMEA_TALKER_ID_NA,
  NMEA_TALKER_ID_MAX = NMEA_TALKER_ID_NA + 1
} nmea_talker_id_t;

/** Parametric NMEA sentence formatter */
typedef enum {
  NMEA_FORMATTER_RMC = 0,
  NMEA_FORMATTER_GGA,
  NMEA_FORMATTER_GSA,
  NMEA_FORMATTER_GLL,
  NMEA_FORMATTER_GST,
  NMEA_FORMATTER_GSV,
  NMEA_FORMATTER_VTG,
  NMEA_FORMATTER_GNS,
  NMEA_FORMATTER_NA,
  NMEA_FORMATTER_MAX = NMEA_FORMATTER_NA + 1
} nmea_formatter_t;

/** NMEA fix status (RMC, GLL status) */
typedef enum {
  NMEA_FIX_STATUS_VALID = 'A',
  NMEA_FIX_STATUS_NOT_VALID = 'V'
} nmea_fix_status_t;

/** NMEA fix mode (GSA, GLL, VTG, GNS mode) */
typedef enum {
  NMEA_FIX_MODE_AUTOMATIC = 'A',
  NMEA_FIX_MODE_DIFFERENTIAL = 'D',
  NMEA_FIX_MODE_ESTIMATED = 'E',
  NMEA_FIX_MODE_MANUAL = 'M',
  NMEA_FIX_MODE_SIMULATED = 'S',
  NMEA_FIX_MODE_NOT_VALID = 'N',
  NMEA_FIX_MODE_PRECISE = 'P',
  NMEA_FIX_MODE_RTK = 'R',
  NMEA_FIX_MODE_FLOAT_RTK = 'F'
} nmea_fix_mode_t;

/** NMEA fix type (GSA status) */
typedef enum {
  NMEA_FIX_TYPE_NOT_AVAILABLE = 1,
  NMEA_FIX_TYPE_2D = 2,
  NMEA_FIX_TYPE_3D = 3,

  /** Some Teseo binary versions set this value when in DR (undocumented) */
  NMEA_FIX_TYPE_TESEO_DR = 6
} nmea_fix_type_t;

/** NMEA fix quality (GGA quality) */
typedef enum {
  NMEA_FIX_QUALITY_NOT_AVAILABLE = 0,
  NMEA_FIX_QUALITY_SPS_MODE = 1,
  NMEA_FIX_QUALITY_DIFFERENTIAL = 2,
  NMEA_FIX_QUALITY_PPS_MODE = 3,
  NMEA_FIX_QUALITY_RTK_INTEGER = 4,
  NMEA_FIX_QUALITY_RTK_FLOAT = 5,
  NMEA_FIX_QUALITY_ESTIMATED = 6,
  NMEA_FIX_QUALITY_MANUAL = 7,
  NMEA_FIX_QUALITY_SIMULATOR = 8
} nmea_fix_quality_t;

/** RMC sentence data */
typedef struct {
  nmea_time_t utc_time;
  nmea_char_t fix_status;
  nmea_float_t latitude;
  nmea_char_t latitude_direction;
  nmea_float_t longitude;
  nmea_char_t longitude_direction;
  nmea_float_t speed;
  nmea_float_t course;
  nmea_date_t utc_date;
  nmea_float_t variation;
  nmea_char_t variation_direction;
} nmea_data_rmc_t;

/** GGA sentence data */
typedef struct {
  nmea_time_t utc_time;
  nmea_float_t latitude;
  nmea_char_t latitude_direction;
  nmea_float_t longitude;
  nmea_char_t longitude_direction;
  nmea_int_t fix_quality;
  nmea_int_t satellites_in_use;
  nmea_float_t hdop;
  nmea_float_t altitude;
  nmea_char_t altitude_units;
  nmea_float_t geoidal_separation;
  nmea_char_t geoidal_separation_units;
  nmea_float_t _dgps_data_age;
  nmea_int_t _dgps_ref_station_id;
} nmea_data_gga_t;

/** GSA sentence data */
typedef struct {
  nmea_char_t fix_mode;
  nmea_int_t fix_type;
  nmea_int_t satellites[12];
  nmea_float_t pdop;
  nmea_float_t hdop;
  nmea_float_t vdop;
} nmea_data_gsa_t;

/** GLL sentence data */
typedef struct {
  nmea_float_t latitude;
  nmea_char_t latitude_direction;
  nmea_float_t longitude;
  nmea_char_t longitude_direction;
  nmea_time_t utc_time;
  nmea_char_t fix_status;
  nmea_char_t _fix_mode;
} nmea_data_gll_t;

/** GST sentence data */
typedef struct {
  nmea_time_t utc_time;
  nmea_float_t rms_deviation;
  nmea_float_t semi_major_deviation;
  nmea_float_t semi_minor_deviation;
  nmea_float_t semi_major_orientation;
  nmea_float_t latitude_error_deviation;
  nmea_float_t longitude_error_deviation;
  nmea_float_t altitude_error_deviation;
} nmea_data_gst_t;

/** GSV satellites info */
typedef struct {
  nmea_int_t number;
  nmea_int_t elevation;
  nmea_int_t azimuth;
  nmea_int_t snr;
} nmea_gsv_satellite_info_t;

/** GSV sentence data */
typedef struct {
  nmea_int_t total_sentences;
  nmea_int_t sentence_number;
  nmea_int_t total_satellites;
  nmea_gsv_satellite_info_t _satellites[4];
} nmea_data_gsv_t;

/** VTG sentence data */
typedef struct {
  nmea_float_t course_true;
  nmea_char_t t;
  nmea_float_t course_magnetic;
  nmea_char_t m;
  nmea_float_t speed_knots;
  nmea_char_t n;
  nmea_float_t speed_kph;
  nmea_char_t k;
  nmea_char_t _fix_mode;
} nmea_data_vtg_t;

/** GNS sentence data */
typedef struct {
  nmea_time_t utc_time;
  nmea_float_t latitude;
  nmea_char_t latitude_direction;
  nmea_float_t longitude;
  nmea_char_t longitude_direction;
  nmea_str_t fix_mode;
  nmea_int_t total_satellites;
  nmea_float_t hdop;
  nmea_float_t altitude;
  nmea_float_t geoidal_separation;
  nmea_float_t _dgps_data_age;
  nmea_int_t _dgps_ref_station_id;
} nmea_data_gns_t;

/**
   GNS sentence fix mode parse.

   @param[in] fix_mode Field ::nmea_data_gns_t.fix_mode
   @param[out] gps_mode GPS mode indicator
   @param[out] glonass_mode GLONASS mode indicator

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_data_gns_fix_mode_parse(const nmea_str_t *fix_mode,
            nmea_char_t *gps_mode,
            nmea_char_t *glonass_mode);

/** Proprietary sentence ID (Manufacturer's Mnemonic Code) */
typedef enum {
  NMEA_SENTENCE_ID_STM = 0,
  NMEA_SENTENCE_ID_UBX,
  NMEA_SENTENCE_ID_NA,
  NMEA_SENTENCE_ID_MAX = NMEA_SENTENCE_ID_NA + 1
} nmea_sentence_id_t;
 
/** STM sentence formatter */
typedef enum {
  NMEA_FORMATTER_STM_CPU = 0,
  NMEA_FORMATTER_STM_KFCOV,
  NMEA_FORMATTER_STM_UTC,
  NMEA_FORMATTER_STM_TG,
  NMEA_FORMATTER_STM_FWUPGRADEOK,
  NMEA_FORMATTER_STM_FWUPGRADEERROR,
  NMEA_FORMATTER_STM_GETPARERROR,
  NMEA_FORMATTER_STM_SETPAR,
  NMEA_FORMATTER_STM_DRCAL,
  NMEA_FORMATTER_STM_DRCOV,
  NMEA_FORMATTER_STM_NA,
  NMEA_FORMATTER_STM_MAX = NMEA_FORMATTER_STM_NA + 1
} nmea_formatter_stm_t;

/** STM CPU sentence data */
typedef struct {
  nmea_float_t cpu_usage;
  nmea_int_t pll_on_off;
  nmea_int_t cpu_speed;
} nmea_data_stm_cpu_t;

/** STM KFCOV sentence data */
typedef struct {
  nmea_float_t postd;
  nmea_float_t posncov;
  nmea_float_t posecov;
  nmea_float_t posvcov;
  nmea_float_t velstd;
  nmea_float_t valncov;
  nmea_float_t velecov;
  nmea_float_t velvcov;
} nmea_data_stm_kfcov_t;

/** STM UTC offset validity */
typedef enum {
  NMEA_STM_UTC_OFFSET_VALIDITY_NOT_VALID = 0,
  NMEA_STM_UTC_OFFSET_VALIDITY_READ_FROM_NVM = 1,
  NMEA_STM_UTC_OFFSET_VALIDITY_VALID = 2
} nmea_stm_utc_offset_validity_t;

/** STM UTC sentence data */
typedef struct {
  nmea_time_t utc_time;
  nmea_date_t utc_date;
  nmea_int_t utc_timestamp;
  nmea_int_t utc_offset;
  nmea_int_t utc_offset_validity;
} nmea_data_stm_utc_t;

/** STM TG time valid */
typedef enum {
  NMEA_STM_TG_TIME_VALID_NO_TIME = 0,
  NMEA_STM_TG_TIME_VALID_TIME_READ_FROM_FLASH = 1,
  NMEA_STM_TG_TIME_VALID_TIME_SET_BY_USER = 2,
  NMEA_STM_TG_TIME_VALID_TIME_SET_USER_RTC = 3,
  NMEA_STM_TG_TIME_VALID_RTC_TIME = 4,
  NMEA_STM_TG_TIME_VALID_RTC_TIME_ACCURATE = 5,
  NMEA_STM_TG_TIME_VALID_TIME_APPROXIMATE = 6,
  NMEA_STM_TG_TIME_VALID_NOT_USED = 7,
  NMEA_STM_TG_TIME_VALID_TIME_ACCURATE = 8,
  NMEA_STM_TG_TIME_VALID_POSITION_TIME = 9,
  NMEA_STM_TG_TIME_VALID_EPHEMERIS_TIME = 10
} nmea_stm_tg_time_valid_t;

/** STM TG sentence data */
typedef struct {
  nmea_int_t week;
  nmea_float_t tow;
  nmea_int_t totsat;
  nmea_int_t cputime;
  nmea_int_t timevalid;
  nmea_float_t nco;
  nmea_hex_t kf_config_status;
  nmea_int_t _constellation_mask;
  nmea_int_t _time_best_sat_type;
  nmea_int_t _time_master_sat_type;
  nmea_int_t _time_aux_sat_type;
  nmea_int_t _time_master_week_n;
  nmea_float_t _time_master_tow;
  nmea_int_t _time_master_validity;
  nmea_int_t _time_aux_week_n;
  nmea_float_t _time_aux_tow;
  nmea_int_t _time_aux_validity;
} nmea_data_stm_tg_t;

/** STM SETPAR sentence data */
typedef struct {
  nmea_int_t config_block_id;
  nmea_str_t value1;
  nmea_str_t _value2;
  nmea_str_t _value3;
  nmea_str_t _value4;
} nmea_data_stm_setpar_t;

/** STM DRCAL sentence data */
typedef struct {
  nmea_int_t dr_is_calib;
  nmea_int_t odo_is_calib;
  nmea_int_t gyro_gain_is_calib;
  nmea_int_t gyro_offset_is_calib;
  nmea_hex_t _imu_flag;
  nmea_int_t _gyro_integrity_flag;
} nmea_data_stm_drcal_t;

/** STM DRCOV sentence data */
typedef struct {
  nmea_float_t lat_std_dev;
  nmea_float_t lon_std_dev;
  nmea_float_t heading_std_dev;
  nmea_float_t gyro_offset_std_dev;
  nmea_float_t gyro_gain_std_dev;
  nmea_float_t odo_scale_std_dev;
  nmea_float_t gyro_ovst_std_dev;
  nmea_float_t _acc_offset_std_dev;
  nmea_float_t _height_std_dev;
} nmea_data_stm_drcov_t;

/** UBX sentence formatter */
typedef enum {
  NMEA_FORMATTER_UBX_TIME = 0,
  NMEA_FORMATTER_UBX_NA,
  NMEA_FORMATTER_UBX_MAX = NMEA_FORMATTER_UBX_NA + 1
} nmea_formatter_ubx_t;

/** UBX TIME sentence data */
typedef struct {
  nmea_int_t msg_id;
  nmea_time_t time;
  nmea_date_t date;
  nmea_float_t utc_tow;
  nmea_int_t utc_wk;
  nmea_str_t leap_sec;
  nmea_int_t clk_bias;
  nmea_float_t clk_drift;
  nmea_int_t tp_gran;
} nmea_data_ubx_time_t;

/**
   UBX TIME sentence leap seconds parse.

   @param[in] leap_sec Field ::nmea_data_ubx_time_t.leap_sec
   @param[out] utc_offset_sec UTC to GPS time offset in seconds
   @param[out] fw_default Whether value is the default firmware value

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_data_ubx_time_leap_sec_parse(const nmea_str_t *leap_sec,
            nmea_int_t *utc_offset_sec,
            nmea_int_t *fw_default);

/** Used when parsing sentences with no data */
typedef struct {
  nmea_char_t _dummy;
} nmea_data_no_data_t;

/**
   NMEA sentence callback.

   Callback is called on every parsed sentence matching @p address.

   @param[in] sentence NMEA sentence
   @param[in] address Sentence address
   @param[in] data Parsed sentence data
   @param[in,out] context Callback context (optional)

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
typedef atlk_rc_t atlk_must_check
(*nmea_callback_t)(const char *sentence,
            const nmea_address_t *address,
            const void *data,
            void *context);

/** NMEA sentence callback data */
typedef struct {
  /** NMEA sentence callback */
  nmea_callback_t callback;

  /** Callback context */
  void *context;

} nmea_callback_data_t;

/** NMEA sentence callback data default initializer */
#define NMEA_CALLBACK_DATA_INIT {   \
  .callback = NULL,                 \
  .context = NULL                   \
}

/** Maximum supported proprietary sentence formatters */
#define NMEA_FORMATTER_P_MAX \
  ((size_t)NMEA_FORMATTER_STM_MAX > (size_t)NMEA_FORMATTER_UBX_MAX ? \
   (size_t)NMEA_FORMATTER_STM_MAX : (size_t)NMEA_FORMATTER_UBX_MAX)

/** NMEA object */
typedef struct {
  /** Parametric sentence callback data */
  nmea_callback_data_t parametric;

  /** Processed proprietary sentence callback data */
  nmea_callback_data_t proprietary[NMEA_SENTENCE_ID_MAX][NMEA_FORMATTER_P_MAX];

} nmea_t;

/** NMEA object default initializer */
#define NMEA_INIT {                                 \
  .parametric = NMEA_CALLBACK_DATA_INIT,            \
  .proprietary = {{ NMEA_CALLBACK_DATA_INIT, }},    \
}

/**
   Register sentence callback data.

   @warning Registration should not be done concurrently with a call to
   ::nmea_sentence_process. This is not checked in code!

   @warning Sentence address is not checked for validity!

   @param[in,out] nmea NMEA object
   @param[in] address NMEA sentence address
   @param[in] callback_data Sentence callback data
*/
atlk_inline void
nmea_callback_register(nmea_t *nmea,
            const nmea_address_t *address,
            nmea_callback_data_t callback_data)
{
  switch (address->type) {
  case NMEA_TYPE_PARAMETRIC:
    nmea->parametric = callback_data;
    return;
  case NMEA_TYPE_PROPRIETARY:
    nmea->proprietary[address->sentence_id][address->formatter] = callback_data;
    return;
  case NMEA_TYPE_NA:
  default:
    return;
  }
}

/**
   Initialize NMEA object.

   @param[out] nmea NMEA object to init

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_init(nmea_t *nmea);

/**
   Process NMEA sentence.

   @param[in] nmea NMEA object
   @param[in] sentence NMEA sentence to process
   
   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_sentence_process(nmea_t *nmea,
            const char *sentence);

/**
   NMEA sentence handler.
   
   @param[in,out] nmea NMEA object
   @param[out] sentence NMEA sentence

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
typedef atlk_rc_t atlk_must_check
(*nmea_sentence_handler_t)(nmea_t *nmea,
            const char *sentence);

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_NMEA_H */
