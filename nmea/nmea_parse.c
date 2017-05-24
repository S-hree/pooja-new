/* Copyright (C) 2012-2016 Autotalks Ltd. */
/* File generated automatically; do not edit. */
/* Generated: 2016-05-22 19:08:27.244996. */

#include <platform/common.h>
#include <atlk/nmea_parse.h>

atlk_inline atlk_rc_t atlk_must_check
nmea_next_field(const char *sentence,
            const char **field,
            int optional)
{
  const char *cur = sentence;

  while (nmea_is_field(*cur)) {
    cur++;
  }

  if (*cur == ',') {
    cur++;
    *field = cur;
  }
  else {
    if (!optional) {
      TR_DEBUG_NO_ARGS("Non-optional field missing");
      return ATLK_E_NOT_FOUND;
    }
    *field = NULL;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_str_parse(const char *field,
            nmea_str_t *value);

atlk_rc_t atlk_must_check
nmea_char_parse(const char *field,
            nmea_char_t *value);

atlk_rc_t atlk_must_check
nmea_int_parse(const char *field,
            nmea_int_t *value);

atlk_rc_t atlk_must_check
nmea_hex_parse(const char *field,
            nmea_hex_t *value);

atlk_rc_t atlk_must_check
nmea_date_parse(const char *field,
            nmea_date_t *value);

atlk_rc_t atlk_must_check
nmea_time_parse(const char *field,
            nmea_time_t *value);

atlk_rc_t atlk_must_check
nmea_float_parse(const char *field,
            nmea_float_t *value);

atlk_rc_t atlk_must_check
nmea_parse_rmc(nmea_data_rmc_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_time_parse(field, &data->utc_time);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing time 'utc_time' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->fix_status);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'fix_status' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->latitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'latitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->latitude_direction);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'latitude_direction' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->longitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'longitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->longitude_direction);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'longitude_direction' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->speed);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'speed' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->course);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'course' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_date_parse(field, &data->utc_date);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing date 'utc_date' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->variation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'variation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->variation_direction);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'variation_direction' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_gga(nmea_data_gga_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_time_parse(field, &data->utc_time);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing time 'utc_time' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->latitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'latitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->latitude_direction);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'latitude_direction' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->longitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'longitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->longitude_direction);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'longitude_direction' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->fix_quality);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'fix_quality' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites_in_use);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites_in_use' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->hdop);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'hdop' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->altitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'altitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->altitude_units);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'altitude_units' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->geoidal_separation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'geoidal_separation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->geoidal_separation_units);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'geoidal_separation_units' failed (rc=%d)", rc);
    return rc;
  }

  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->_dgps_data_age);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float '_dgps_data_age' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_dgps_ref_station_id);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_dgps_ref_station_id' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_gsa(nmea_data_gsa_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->fix_mode);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'fix_mode' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->fix_type);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'fix_type' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[0]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[0]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[1]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[1]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[2]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[2]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[3]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[3]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[4]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[4]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[5]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[5]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[6]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[6]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[7]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[7]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[8]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[8]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[9]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[9]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[10]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[10]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->satellites[11]);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'satellites[11]' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->pdop);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'pdop' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->hdop);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'hdop' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->vdop);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'vdop' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_gll(nmea_data_gll_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->latitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'latitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->latitude_direction);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'latitude_direction' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->longitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'longitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->longitude_direction);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'longitude_direction' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_time_parse(field, &data->utc_time);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing time 'utc_time' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->fix_status);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'fix_status' failed (rc=%d)", rc);
    return rc;
  }

  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->_fix_mode);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char '_fix_mode' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_gst(nmea_data_gst_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_time_parse(field, &data->utc_time);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing time 'utc_time' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->rms_deviation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'rms_deviation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->semi_major_deviation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'semi_major_deviation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->semi_minor_deviation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'semi_minor_deviation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->semi_major_orientation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'semi_major_orientation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->latitude_error_deviation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'latitude_error_deviation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->longitude_error_deviation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'longitude_error_deviation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->altitude_error_deviation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'altitude_error_deviation' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_gsv(nmea_data_gsv_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->total_sentences);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'total_sentences' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->sentence_number);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'sentence_number' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->total_satellites);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'total_satellites' failed (rc=%d)", rc);
    return rc;
  }

  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[0].number);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[0].number' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[0].elevation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[0].elevation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[0].azimuth);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[0].azimuth' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[0].snr);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[0].snr' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[1].number);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[1].number' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[1].elevation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[1].elevation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[1].azimuth);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[1].azimuth' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[1].snr);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[1].snr' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[2].number);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[2].number' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[2].elevation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[2].elevation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[2].azimuth);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[2].azimuth' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[2].snr);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[2].snr' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[3].number);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[3].number' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[3].elevation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[3].elevation' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[3].azimuth);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[3].azimuth' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_satellites[3].snr);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_satellites[3].snr' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_vtg(nmea_data_vtg_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->course_true);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'course_true' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->t);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 't' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->course_magnetic);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'course_magnetic' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->m);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'm' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->speed_knots);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'speed_knots' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->n);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'n' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->speed_kph);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'speed_kph' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->k);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'k' failed (rc=%d)", rc);
    return rc;
  }

  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->_fix_mode);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char '_fix_mode' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_gns(nmea_data_gns_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_time_parse(field, &data->utc_time);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing time 'utc_time' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->latitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'latitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->latitude_direction);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'latitude_direction' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->longitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'longitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->longitude_direction);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char 'longitude_direction' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_str_parse(field, &data->fix_mode);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing str 'fix_mode' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->total_satellites);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'total_satellites' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->hdop);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'hdop' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->altitude);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'altitude' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->geoidal_separation);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'geoidal_separation' failed (rc=%d)", rc);
    return rc;
  }

  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->_dgps_data_age);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float '_dgps_data_age' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_dgps_ref_station_id);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_dgps_ref_station_id' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_stm_cpu(nmea_data_stm_cpu_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->cpu_usage);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'cpu_usage' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->pll_on_off);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'pll_on_off' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->cpu_speed);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'cpu_speed' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_stm_kfcov(nmea_data_stm_kfcov_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->postd);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'postd' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->posncov);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'posncov' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->posecov);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'posecov' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->posvcov);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'posvcov' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->velstd);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'velstd' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->valncov);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'valncov' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->velecov);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'velecov' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->velvcov);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'velvcov' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_stm_utc(nmea_data_stm_utc_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_time_parse(field, &data->utc_time);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing time 'utc_time' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_date_parse(field, &data->utc_date);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing date 'utc_date' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->utc_timestamp);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'utc_timestamp' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->utc_offset);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'utc_offset' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->utc_offset_validity);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'utc_offset_validity' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_stm_tg(nmea_data_stm_tg_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->week);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'week' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->tow);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'tow' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->totsat);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'totsat' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->cputime);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'cputime' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->timevalid);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'timevalid' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->nco);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'nco' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_hex_parse(field, &data->kf_config_status);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing hex 'kf_config_status' failed (rc=%d)", rc);
    return rc;
  }

  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_constellation_mask);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_constellation_mask' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_time_best_sat_type);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_time_best_sat_type' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_time_master_sat_type);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_time_master_sat_type' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_time_aux_sat_type);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_time_aux_sat_type' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_time_master_week_n);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_time_master_week_n' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->_time_master_tow);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float '_time_master_tow' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_time_master_validity);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_time_master_validity' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_time_aux_week_n);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_time_aux_week_n' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->_time_aux_tow);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float '_time_aux_tow' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_time_aux_validity);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_time_aux_validity' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_stm_setpar(nmea_data_stm_setpar_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->config_block_id);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'config_block_id' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_str_parse(field, &data->value1);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing str 'value1' failed (rc=%d)", rc);
    return rc;
  }

  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_str_parse(field, &data->_value2);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing str '_value2' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_str_parse(field, &data->_value3);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing str '_value3' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_str_parse(field, &data->_value4);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing str '_value4' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_stm_drcal(nmea_data_stm_drcal_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->dr_is_calib);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'dr_is_calib' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->odo_is_calib);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'odo_is_calib' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->gyro_gain_is_calib);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'gyro_gain_is_calib' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->gyro_offset_is_calib);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'gyro_offset_is_calib' failed (rc=%d)", rc);
    return rc;
  }

  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_hex_parse(field, &data->_imu_flag);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing hex '_imu_flag' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->_gyro_integrity_flag);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int '_gyro_integrity_flag' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_stm_drcov(nmea_data_stm_drcov_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->lat_std_dev);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'lat_std_dev' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->lon_std_dev);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'lon_std_dev' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->heading_std_dev);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'heading_std_dev' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->gyro_offset_std_dev);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'gyro_offset_std_dev' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->gyro_gain_std_dev);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'gyro_gain_std_dev' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->odo_scale_std_dev);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'odo_scale_std_dev' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->gyro_ovst_std_dev);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'gyro_ovst_std_dev' failed (rc=%d)", rc);
    return rc;
  }

  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->_acc_offset_std_dev);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float '_acc_offset_std_dev' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->_height_std_dev);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float '_height_std_dev' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_ubx_time(nmea_data_ubx_time_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->msg_id);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'msg_id' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_time_parse(field, &data->time);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing time 'time' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_date_parse(field, &data->date);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing date 'date' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->utc_tow);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'utc_tow' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->utc_wk);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'utc_wk' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_str_parse(field, &data->leap_sec);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing str 'leap_sec' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->clk_bias);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'clk_bias' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_float_parse(field, &data->clk_drift);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing float 'clk_drift' failed (rc=%d)", rc);
    return rc;
  }

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_int_parse(field, &data->tp_gran);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing int 'tp_gran' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_no_data(nmea_data_no_data_t *data, 
            const char *sentence)
{
  const char *field = sentence;
  int optional = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!data || !sentence);
  
  /* Remaining fields are optional */
  optional = 1;

  rc = nmea_next_field(field, &field, optional);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_char_parse(field, &data->_dummy);
  if (atlk_error(rc)) {
    TR_DEBUG("Parsing char '_dummy' failed (rc=%d)", rc);
    return rc;
  }

  return ATLK_OK;
}

const char *nmea_talker_id[NMEA_TALKER_ID_SIZE] = {
  [NMEA_TALKER_ID_GP] = "GP",
  [NMEA_TALKER_ID_GL] = "GL",
  [NMEA_TALKER_ID_GN] = "GN",
  [NMEA_TALKER_ID_GA] = "GA",
  [NMEA_TALKER_ID_BD] = "BD",
  [NMEA_TALKER_ID_QZ] = "QZ",
  [NMEA_TALKER_ID_NA] = "NA"
};

const char *nmea_formatter[NMEA_FORMATTER_SIZE] = {
  [NMEA_FORMATTER_RMC] = "RMC",
  [NMEA_FORMATTER_GGA] = "GGA",
  [NMEA_FORMATTER_GSA] = "GSA",
  [NMEA_FORMATTER_GLL] = "GLL",
  [NMEA_FORMATTER_GST] = "GST",
  [NMEA_FORMATTER_GSV] = "GSV",
  [NMEA_FORMATTER_VTG] = "VTG",
  [NMEA_FORMATTER_GNS] = "GNS",
  [NMEA_FORMATTER_NA] = "NA"
};

const char *nmea_formatter_stm[NMEA_FORMATTER_STM_SIZE] = {
  [NMEA_FORMATTER_STM_CPU] = "CPU",
  [NMEA_FORMATTER_STM_KFCOV] = "KFCOV",
  [NMEA_FORMATTER_STM_UTC] = "UTC",
  [NMEA_FORMATTER_STM_TG] = "TG",
  [NMEA_FORMATTER_STM_FWUPGRADEOK] = "FWUPGRADEOK",
  [NMEA_FORMATTER_STM_FWUPGRADEERROR] = "FWUPGRADEERROR",
  [NMEA_FORMATTER_STM_GETPARERROR] = "GETPARERROR",
  [NMEA_FORMATTER_STM_SETPAR] = "SETPAR",
  [NMEA_FORMATTER_STM_DRCAL] = "DRCAL",
  [NMEA_FORMATTER_STM_DRCOV] = "DRCOV",
  [NMEA_FORMATTER_STM_NA] = "NA"
};

const char *nmea_formatter_ubx[NMEA_FORMATTER_UBX_SIZE] = {
  [NMEA_FORMATTER_UBX_TIME] = "TIME",
  [NMEA_FORMATTER_UBX_NA] = "NA"
};
