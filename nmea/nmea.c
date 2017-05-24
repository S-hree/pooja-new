/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <platform/common.h>
#include <atlk/nmea.h>
#include <atlk/nmea_parse.h>

atlk_rc_t atlk_must_check
nmea_str_parse(const char *field,
            nmea_str_t *value)
{
  int i = 0;

  BUG_ON(!value);
  
  if (!field) {
    *value = (nmea_str_t)NMEA_STR_NA;
    return ATLK_OK;
  }

  for (i = 0; i < NMEA_STR_SIZE - 1; ++i) {
    if (!nmea_is_field(field[i])) {
      break;
    }
  }

  if (nmea_is_field(field[i])) {
    TR_DEBUG("Str parse failed, string is too long: %s", field);
    return ATLK_E_BUFFER_TOO_SMALL;
  }
  else if (i == 0) {
    *value = (nmea_str_t)NMEA_STR_NA;
  }
  else {
    memcpy(value, field, i);
    value->c[i] = '\0';
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_char_parse(const char *field,
            nmea_char_t *value)
{
  BUG_ON(!value);

  if (!field) {
    *value = (nmea_char_t)NMEA_CHAR_NA;
    return ATLK_OK;
  }

  if (!nmea_is_field(*field)) {
    *value = (nmea_char_t)NMEA_CHAR_NA;
  }
  else if (nmea_is_field(*(field + 1))) {
    TR_DEBUG("Char parse failed, multiple chars in field: %s", field);
    return ATLK_E_INVALID_STATE;
  }
  else {
    *value = *field;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_int_parse(const char *field,
            nmea_int_t *value)
{
  char *end_ptr = NULL;
  long int integer = 0;

  BUG_ON(!value);
  
  if (!field) {
    *value = (nmea_int_t)NMEA_INT_NA;
    return ATLK_OK;
  }
  
  integer = strtol(field, &end_ptr, 10);

  if (integer == LONG_MIN) {
    TR_DEBUG("Int parse failed, value underflows: %s", field);
    return ATLK_E_OUT_OF_RANGE;
  }
  else if (integer == LONG_MAX) {
    TR_DEBUG("Int parse failed, value overflows: %s", field);
    return ATLK_E_OUT_OF_RANGE;
  }
  else if (nmea_is_field(*end_ptr)) {
    TR_DEBUG("Int parse failed, non-numeric chars after value: %s", field);
    return ATLK_E_INVALID_STATE;
  }
  else if (end_ptr == field) {
    *value = (nmea_int_t)NMEA_INT_NA;
  }
  else {
    *value = integer;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_hex_parse(const char *field,
            nmea_hex_t *value)
{
  char *end_ptr = NULL;
  long int hex = 0;

  BUG_ON(!value);
  
  if (!field) {
    *value = (nmea_hex_t)NMEA_HEX_NA;
    return ATLK_OK;
  }

  hex = strtol(field, &end_ptr, 16);

  if (hex == LONG_MIN) {
    TR_DEBUG("Hex parse failed, value underflows: %s", field);
    return ATLK_E_OUT_OF_RANGE;
  }
  else if (hex == LONG_MAX) {
    TR_DEBUG("Hex parse failed, value overflows: %s", field);
    return ATLK_E_OUT_OF_RANGE;
  }
  else if (nmea_is_field(*end_ptr)) {
    TR_DEBUG("Hex parse failed, non-numeric chars after value: %s", field);
    return ATLK_E_INVALID_STATE;
  }
  else if (end_ptr == field) {
    *value = (nmea_hex_t)NMEA_HEX_NA;
  }
  else if (hex < 0) {
    TR_DEBUG("Hex parse failed, negative value: %s", field);
    return ATLK_E_OUT_OF_DOMAIN;
  }
  else {
    *value = hex;
  }

  return ATLK_OK;
}

/* Create a null-terminated string from two chars */
#define char2(c1, c2) (char[]){c1, c2, '\0'}

/* Create a null-terminated string from four chars */
#define char4(c1, c2, c3, c4) (char[]){c1, c2, c3, c4, '\0'}

atlk_rc_t atlk_must_check
nmea_date_parse(const char *field,
            nmea_date_t *value)
{
  int i = 0;

  BUG_ON(!value);

  if (!field) {
    *value = (nmea_date_t)NMEA_DATE_NA;
    return ATLK_OK;
  }
  
  for (i = 0; i < 8; ++i) {
    if (!isdigit((unsigned char)field[i])) {
      break;
    }
  }

  if (i != 0 && i != 6 && i != 8) {
    TR_DEBUG("Date parse failed, invalid number of numeric chars: %s", field);
    return ATLK_E_INVALID_STATE;
  }
  else if (nmea_is_field(field[i])) {
    TR_DEBUG("Date parse failed, non-numeric chars after value: %s", field);
    return ATLK_E_INVALID_STATE;
  }
  else if (i == 0) {
    *value = (nmea_date_t)NMEA_DATE_NA;
  }
  else {
    const char *f = field;
    nmea_date_t d = NMEA_DATE_NA;

    d.day = strtol(char2(f[0], f[1]), NULL, 10);
    d.month = strtol(char2(f[2], f[3]), NULL, 10);
    d.year = (i == 6) ?
      strtol(char2(f[4], f[5]), NULL, 10) + 2000 : 
      strtol(char4(f[4], f[5], f[6], f[7]), NULL, 10);

    if (!nmea_date_valid(&d)) {
      TR_DEBUG("Date parse failed, field values out of range: %s", field);
      return ATLK_E_OUT_OF_RANGE;
    }

    *value = d;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_time_parse(const char *field,
            nmea_time_t *value)
{
  int i = 0;

  BUG_ON(!value);
  
  if (!field) {
    *value = (nmea_time_t)NMEA_TIME_NA;
    return ATLK_OK;
  }
  
  for (i = 0; i < 6; ++i) {
    if (!isdigit((unsigned char)field[i])) {
      break;
    }
  }

  if (i != 0 && i != 6) {
    TR_DEBUG("Time parse failed, invalid number of numeric chars: %s", field);
    return ATLK_E_INVALID_STATE;
  }
  else if (i == 0) {
    if (nmea_is_field(field[i])) {
      TR_DEBUG("Time parse failed, non-numeric chars: %s", field);
      return ATLK_E_INVALID_STATE;
    }
    *value = (nmea_time_t)NMEA_TIME_NA;
  }
  else {
    const char *f = field;
    nmea_time_t t = NMEA_TIME_NA;

    t.hours = strtol(char2(f[0], f[1]), NULL, 10);
    t.minutes = strtol(char2(f[2], f[3]), NULL, 10);
    t.seconds = strtol(char2(f[4], f[5]), NULL, 10);

    f += 6;

    if (nmea_is_field(*f) && *f != '.') {
      TR_DEBUG("Time parse failed, invalid char after value: %s", field);
      return ATLK_E_INVALID_STATE;
    }

    if (*f++ == '.') {
      int microseconds = 0;
      int scale = 1000000;

      while (isdigit((unsigned char)*f) && scale > 1) {
        microseconds = (microseconds * 10) + (*f++ - '0');
        scale /= 10;
      }
      t.microseconds = microseconds * scale;
    } 
    else {
      t.microseconds = 0;
    }

    if (!nmea_time_valid(&t)) {
      TR_DEBUG("Time parse failed, field values out of range: %s", field);
      return ATLK_E_OUT_OF_RANGE;
    }

    *value = t;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_float_parse(const char *field,
            nmea_float_t *value)
{
  int sign = 0;
  nmea_float_t f = NMEA_FLOAT_NA;

  BUG_ON(!value);
  
  if (!field) {
    *value = (nmea_float_t)NMEA_FLOAT_NA;
    return ATLK_OK;
  }

  while (nmea_is_field(*field)) {
    if (*field == '+' && sign == 0 && f.value == -1) {
      sign = 1;
    } 
    else if (*field == '-' && sign == 0 && f.value == -1) {
      sign = -1;
    } 
    else if (isdigit((unsigned char)*field)) {
      int digit = *field - '0';

      if (f.value == -1) {
        f.value = 0;
      }
      else if (f.value > (INT64_MAX - digit) / 10) {
        if (f.scale) {
          break;
        } 
        else {
          TR_DEBUG("Float parse failed, value overflows: %s", field);
          return ATLK_E_OUT_OF_RANGE;
        }
      }

      f.value = (10 * f.value) + digit;
      if (f.scale) {
        f.scale *= 10;
      }
    } 
    else if (*field == '.' && f.scale == 0) {
      f.scale = 1;
    } 
    else if (*field == ' ') {
      if (sign != 0 || f.value != -1 || f.scale != 0) {
        TR_DEBUG("Float parse failed, non-preceding space char: %s", field);
        return ATLK_E_INVALID_STATE;
      }
    } 
    else {
      TR_DEBUG("Float parse failed, invalid char in field: %s", field);
      return ATLK_E_INVALID_STATE;
    }
    field++;
  }

  if ((sign != 0 || f.scale != 0) && f.value == -1) {
    TR_DEBUG("Float parse failed, invalid field format: %s", field);
    return ATLK_E_INVALID_STATE;
  }

  if (f.value == -1) {
    f = (nmea_float_t)NMEA_FLOAT_NA;
  } 
  else if (f.scale == 0) {
    f.scale = 1;
  }

  if (sign != 0) {
    f.value *= sign;
  }

  *value = f;
  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_address(nmea_address_t *address,
            const char *sentence)
{
  size_t i = 0;
  nmea_str_t type = NMEA_STR_NA;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!address || !sentence);

  rc = nmea_str_parse(sentence, &type);
  if (atlk_error(rc)) {
    return rc;
  }

  if (type.c[1] == 'P') {
    return ATLK_E_UNSUPPORTED;
  }
  else if (strlen((const char *)&type) != 6) {
    TR_DEBUG("Parse address failed, invalid address length: %s", sentence);
    return ATLK_E_PROTOCOL_ERROR;
  }

  address->type = NMEA_TYPE_PARAMETRIC;

  for (i = 0; i < NMEA_TALKER_ID_SIZE - 1; ++i) {
    if (!strncmp((const char *)&type.c[1], nmea_talker_id[i], 2)) {
      break;
    }
  }
  address->sentence_id = i;

  for (i = 0; i < NMEA_FORMATTER_SIZE - 1; ++i) {
    if (!strncmp((const char *)&type.c[3], nmea_formatter[i], 3)) {
      break;
    }
  }
  address->formatter = i;

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_parse_address_stm(nmea_address_t *address,
            const char *sentence)
{
  size_t i = 0;
  nmea_str_t type = NMEA_STR_NA;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!address || !sentence);

  rc = nmea_str_parse(sentence, &type);
  if (atlk_error(rc)) {
    return rc;
  }

  if (strncmp((const char *)&type.c[1], "PSTM", 4)) {
    return ATLK_E_UNSUPPORTED;
  }
  else if (strlen((const char *)&type) < 6) {
    TR_DEBUG("STM parse address failed, invalid address length: %s", sentence);
    return ATLK_E_PROTOCOL_ERROR;
  }

  address->type = NMEA_TYPE_PROPRIETARY;
  address->sentence_id = NMEA_SENTENCE_ID_STM;

  for (i = 0; i < NMEA_FORMATTER_STM_SIZE - 1; ++i) {
    if (!strcmp((const char *)&type.c[5], nmea_formatter_stm[i])) {
      break;
    }
  }
  address->formatter = i;

  return ATLK_OK;
}

static const int nmea_formatter_ubx_msg_id[] = {
  [NMEA_FORMATTER_UBX_TIME] = 4,
  [NMEA_FORMATTER_UBX_NA] = -1
};

atlk_rc_t atlk_must_check
nmea_parse_address_ubx(nmea_address_t *address,
            const char *sentence)
{
  size_t i = 0;
  nmea_str_t type = NMEA_STR_NA;
  nmea_int_t msg_id = NMEA_INT_NA;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!address || !sentence);

  rc = nmea_str_parse(sentence, &type);
  if (atlk_error(rc)) {
    return rc;
  }

  if (strncmp((const char *)&type.c[1], "PUBX", 4)) {
    return ATLK_E_UNSUPPORTED;
  }
  else if (type.c[5] != '\0') {
    TR_DEBUG("UBX parse address failed, invalid address length: %s", sentence);
    return ATLK_E_PROTOCOL_ERROR;
  }

  rc = nmea_int_parse(sentence + 6, &msg_id);
  if (atlk_error(rc)) {
    return rc;
  }

  address->type = NMEA_TYPE_PROPRIETARY;
  address->sentence_id = NMEA_SENTENCE_ID_UBX;

  for (i = 0; i < ARRAY_SIZE(nmea_formatter_ubx_msg_id) - 1; ++i) {
    if (msg_id == nmea_formatter_ubx_msg_id[i]) {
      break;
    }
  }
  address->formatter = i;

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_gns_fix_mode_parse(const nmea_str_t *fix_mode,
            nmea_char_t *gps_mode,
            nmea_char_t *glonass_mode)
{
  const char *field = NULL;

  if (atlk_unlikely(!fix_mode || !gps_mode || !glonass_mode)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  field = (const char *)fix_mode;
  if (strnlen(field, 3) < 2) {
    TR_DEBUG("Parse failed, field is too short: %s", field);
    return ATLK_E_UNSPECIFIED;
  }

  *gps_mode = field[0];
  *glonass_mode = field[1];

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_data_ubx_time_leap_sec_parse(const nmea_str_t *leap_sec,
            nmea_int_t *utc_offset_sec,
            nmea_int_t *fw_default)
{
  const char *field = NULL;
  char *end_ptr = NULL;
  long int i = 0;

  if (atlk_unlikely(!leap_sec || !utc_offset_sec || !fw_default)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  field = (const char *)leap_sec;
  i = strtol(field, &end_ptr, 10);

  if (!end_ptr || end_ptr == field) {
    TR_DEBUG("Parse failed, non-numeric chars: %s", field);
    return ATLK_E_UNSPECIFIED;
  }
  else if (*end_ptr == '\0') {
    *fw_default = 0;
  }
  else if (*end_ptr == 'D') {
    *fw_default = 1;
  }
  else {
    TR_DEBUG("Parse failed, invalid char after value: %s", field);
    return ATLK_E_UNSPECIFIED;
  }

  *utc_offset_sec = i;

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_init(nmea_t *nmea)
{
  if (atlk_unlikely(!nmea)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  nmea->parametric = (nmea_callback_data_t)NMEA_CALLBACK_DATA_INIT;

  for (size_t i = 0; i < NMEA_SENTENCE_ID_MAX; ++i) {
    for (size_t j = 0; j < NMEA_FORMATTER_P_MAX; ++j) {
      nmea->proprietary[i][j] = (nmea_callback_data_t)NMEA_CALLBACK_DATA_INIT;
    }
  }

  return ATLK_OK;
}

typedef atlk_rc_t atlk_must_check
(*nmea_parse_address_t)(nmea_address_t *address,
            const char *sentence);

static const nmea_parse_address_t parse_address[] = {
  nmea_parse_address,
  nmea_parse_address_stm,
  nmea_parse_address_ubx
};

/* TODO: Add the below to auto-generation */

#define HANDLE(nmea_data_x_t, nmea_parse_x) {                               \
    nmea_data_x_t data;                                                     \
    rc = nmea_parse_x(&data, sentence);                                     \
    if (!atlk_error(rc)) {                                                  \
      return callback(sentence, address, (const void *)&data, context);     \
    } break; }

static atlk_rc_t atlk_must_check
nmea_parse_parametric(const char *sentence,
            const nmea_address_t *address,
            const nmea_callback_t callback,
            void *context)
{
  atlk_rc_t rc = ATLK_OK;

  switch (address->formatter) {
  case NMEA_FORMATTER_RMC:
    HANDLE(nmea_data_rmc_t, nmea_parse_rmc);
  case NMEA_FORMATTER_GGA:
    HANDLE(nmea_data_gga_t, nmea_parse_gga);
  case NMEA_FORMATTER_GSA:
    HANDLE(nmea_data_gsa_t, nmea_parse_gsa);
  case NMEA_FORMATTER_GLL:
    HANDLE(nmea_data_gll_t, nmea_parse_gll);
  case NMEA_FORMATTER_GST:
    HANDLE(nmea_data_gst_t, nmea_parse_gst);
  case NMEA_FORMATTER_GSV:
    HANDLE(nmea_data_gsv_t, nmea_parse_gsv);
  case NMEA_FORMATTER_VTG:
    HANDLE(nmea_data_vtg_t, nmea_parse_vtg);
  case NMEA_FORMATTER_GNS:
    HANDLE(nmea_data_gns_t, nmea_parse_gns);
  case NMEA_FORMATTER_NA:
    break;
  default:
    BUG();
  }

  return rc;
}

static atlk_rc_t atlk_must_check
nmea_parse_proprietary(const char *sentence,
            const nmea_address_t *address,
            const nmea_callback_t callback,
            void *context)
{
  atlk_rc_t rc = ATLK_OK;

  switch (address->sentence_id) {
  case NMEA_SENTENCE_ID_STM:
  {
    switch (address->formatter) {
    case NMEA_FORMATTER_STM_CPU:
      HANDLE(nmea_data_stm_cpu_t, nmea_parse_stm_cpu);
    case NMEA_FORMATTER_STM_KFCOV:
      HANDLE(nmea_data_stm_kfcov_t, nmea_parse_stm_kfcov);
    case NMEA_FORMATTER_STM_UTC:
      HANDLE(nmea_data_stm_utc_t, nmea_parse_stm_utc);
    case NMEA_FORMATTER_STM_TG:
      HANDLE(nmea_data_stm_tg_t, nmea_parse_stm_tg);
    case NMEA_FORMATTER_STM_FWUPGRADEOK:
    case NMEA_FORMATTER_STM_FWUPGRADEERROR:
    case NMEA_FORMATTER_STM_GETPARERROR:
      HANDLE(nmea_data_no_data_t, nmea_parse_no_data);
    case NMEA_FORMATTER_STM_SETPAR:
      HANDLE(nmea_data_stm_setpar_t, nmea_parse_stm_setpar);
    case NMEA_FORMATTER_STM_DRCAL:
      HANDLE(nmea_data_stm_drcal_t, nmea_parse_stm_drcal);
    case NMEA_FORMATTER_STM_DRCOV:
      HANDLE(nmea_data_stm_drcov_t, nmea_parse_stm_drcov);
    case NMEA_FORMATTER_STM_NA:
      break;
    default:
      BUG();
    }
    break;
  }
  case NMEA_SENTENCE_ID_UBX:
  {
    switch (address->formatter) {
    case NMEA_FORMATTER_UBX_TIME:
      HANDLE(nmea_data_ubx_time_t, nmea_parse_ubx_time);
    case NMEA_FORMATTER_UBX_NA:
      break;
    default:
      BUG();
    }
    break;
  }
  case NMEA_SENTENCE_ID_NA:
    break;
  default:
    BUG();
  }

  return rc;
}

#undef HANDLE /* No longer required */

atlk_rc_t atlk_must_check
nmea_sentence_process(nmea_t *nmea,
            const char *sentence)
{
  nmea_address_t address = NMEA_ADDRESS_INIT;
  const nmea_callback_data_t *callback_data = NULL;
  nmea_callback_t callback = NULL;
  void *context = NULL;
  size_t i = 0;
  atlk_rc_t rc = ATLK_OK;

  if (atlk_unlikely(!nmea || !sentence)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  for (i = 0; i < ARRAY_SIZE(parse_address); ++i) {
    rc = parse_address[i](&address, sentence);
    if (rc != ATLK_E_UNSUPPORTED) {
      break;
    }
  }

  if (i == ARRAY_SIZE(parse_address)) {
    address.type = NMEA_TYPE_PROPRIETARY;
    address.sentence_id = NMEA_SENTENCE_ID_NA;
    address.formatter = 0;
  }
  else if (atlk_error(rc)) {
    return rc;
  }

  switch (address.type) {
  case NMEA_TYPE_PARAMETRIC:
  {
    callback = nmea->parametric.callback;
    context = nmea->parametric.context;

    if (!callback) {
      return ATLK_OK;
    }

    rc = nmea_parse_parametric(sentence, &address, callback, context);
    if (atlk_error(rc)) {
      return rc;
    }
    break;
  }
  case NMEA_TYPE_PROPRIETARY:
  {
    callback_data = nmea->proprietary[address.sentence_id];
    callback = callback_data[address.formatter].callback;
    context = callback_data[address.formatter].context;

    if (!callback) {
      return ATLK_OK;
    }

    rc = nmea_parse_proprietary(sentence, &address, callback, context);
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