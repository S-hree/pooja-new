/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <platform/common.h>
#include <atlk/nmea_io_internal.h>

uint8_t
nmea_checksum(const char *sentence,
            const char **end_ptr)
{
  uint8_t checksum = 0x00;

  if (*sentence == '$') {
    sentence++;
  }
  BUG_ON(*sentence == '$');

  while (*sentence && *sentence != '*' && *sentence != '\r' &&
         *sentence != '\n' && isprint((unsigned char)*sentence)) {
    checksum ^= *sentence++;
  }

  if (end_ptr) {
    *end_ptr = sentence;
  }

  return checksum;
}

atlk_rc_t atlk_must_check
nmea_sentence_check(const char *sentence,
            size_t sentence_size)
{
  uint8_t checksum = 0x00;
  size_t len = sentence_size - 1;
  const char *cur = sentence;
  int checksum_is_mandatory = 1;

  BUG_ON(!sentence || sentence_size < 2);

  if (*cur++ != '$') {
    TR_DEBUG("Sentence check failed, missing '$' sign: %s", sentence);
    return ATLK_E_PROTOCOL_ERROR;
  }

  if (*cur == '$') {
    TR_DEBUG("Sentence check failed, multiple '$' signs: %s", sentence);
    return ATLK_E_PROTOCOL_ERROR;
  }
  
  if (*cur == 'P') {
    if (len < NMEA_P_LEN_MIN || len > NMEA_P_LEN_MAX) {
      TR_DEBUG("Sentence check failed, invalid length: %s", sentence);
      return ATLK_E_OUT_OF_BOUNDS;
    }

    /* Many proprietary sentences have no checksum */
    checksum_is_mandatory = 0;
  }
  else {
    if (len < NMEA_LEN_MIN || len > NMEA_LEN_MAX) {
      TR_DEBUG("Sentence check failed, invalid length: %s", sentence);
      return ATLK_E_OUT_OF_BOUNDS;
    }
  }

  checksum = nmea_checksum(cur, &cur);

  if (*cur == '*') {
    char *end = NULL;
    int actual = strtol(++cur, &end, 16); 

    if (end - cur != 2) {
      TR_DEBUG("Sentence check failed, invalid checksum field: %s", sentence);
      return ATLK_E_PROTOCOL_ERROR;
    }
    else if (checksum != actual) {
      TR_DEBUG("Actual checksum 0x%x, calculated 0x%x", actual, checksum);
      return ATLK_E_PROTOCOL_ERROR;
    }
    cur = end;
  } 
  else if (checksum_is_mandatory) {
    TR_DEBUG("Sentence check failed, missing checksum: %s", sentence);
    return ATLK_E_PROTOCOL_ERROR;
  }

  if (strcmp(cur, "\r\n")) {
    TR_DEBUG("Sentence check failed, invalid sentence end: %s", sentence);
    return ATLK_E_PROTOCOL_ERROR;
  }

  return ATLK_OK;
}
