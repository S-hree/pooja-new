/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <platform/common.h>
#include <atlk/mutex.h>
#include <atlk/cond.h>
#include <atlk/gnss.h>
#include <atlk/gnss_internal.h>
#include <atlk/gnss_teseo.h>
#include <atlk/gnss_teseo_internal.h>

#include "teseo_commands.h"

/* TODO: A more "generic" NMEA callback to command communication method is
 * required (or the below code will be re-copied all over the place) */

/* TESEO reset procedure */
static gnss_hw_reset_t teseo_hw_reset = NULL;

/* Command to get Teseo text string */
#define GET_TEXT_MSG_CMD "$PSTMGETPAR,2500\r\n"

/* Teseo firmware version container */
struct teseo_fw_version {
  char buf[NMEA_STR_SIZE];
  atlk_mutex_t mutex;
  atlk_cond_t cond;
};

/* Teseo firmware version container default initializer */
#define TESEO_FW_VERSION_INIT {     \
  .buf = { 0 },                     \
  .mutex = ATLK_MUTEX_INIT,         \
  .cond = ATLK_COND_INIT            \
}

/* Create Teseo firmware version container */
static atlk_rc_t atlk_must_check
teseo_firmware_version_create(struct teseo_fw_version *version)
{
  atlk_rc_t rc = ATLK_OK;

  *version = (struct teseo_fw_version)TESEO_FW_VERSION_INIT;

  rc = atlk_mutex_create(&version->mutex, "Teseo: FW version");
  if (atlk_error(rc)) {
    TR_ERROR("Mutex creation failed (rc=%d)", rc);
    return rc;
  }

  rc = atlk_cond_create(&version->cond, "Teseo: FW version");
  if (atlk_error(rc)) {
    TR_ERROR("Condition variable creation failed (rc=%d)", rc);
    atlk_mutex_destroy(&version->mutex);
  }
  
  return rc;
}

/* Teseo firmware version */
static struct teseo_fw_version teseo_fw_version = TESEO_FW_VERSION_INIT;

static atlk_rc_t atlk_must_check
nmea_callback_stm_setpar(const char *sentence,
            const nmea_address_t *address,
            const void *data,
            void *context)
{
  (void)data;
  (void)context;

  switch (address->formatter) {
  case NMEA_FORMATTER_STM_GETPARERROR:
  {
    TR_ERROR("Received error reply from Teseo: %s", sentence);
    break;
  }
  case NMEA_FORMATTER_STM_SETPAR:
  {
    nmea_data_stm_setpar_t *setpar = (nmea_data_stm_setpar_t *)data;

    if (setpar->config_block_id != 2500) {
      TR_ERROR("Unexpected config block and ID: %d", setpar->config_block_id);
      break;
    }

    atlk_mutex_lock(&teseo_fw_version.mutex);       /* Lock! */
    strncpy(teseo_fw_version.buf, (const char *)&setpar->value1,
            sizeof(teseo_fw_version.buf));
    atlk_mutex_unlock(&teseo_fw_version.mutex);     /* Unlock! */
    break;
  }
  default:
    TR_ERROR("Recived address-formatter %d", address->formatter); 
    BUG();
  }

  atlk_cond_signal(&teseo_fw_version.cond);
  return ATLK_OK;
}

atlk_rc_t atlk_must_check
teseo_fw_version_get(char *fw_version,
            size_t *fw_version_size,
            const atlk_wait_t *wait,
            gnss_nmea_t *gnss_nmea)
{
  struct timeval tv;
  size_t len = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!fw_version || !fw_version_size || !gnss_nmea);

  /* Optimization - check whether we already got Teseo firmware version */
  if (teseo_fw_version.buf[0] != '\0') {
    goto copy;
  }
  else if (!wait) {
    TR_ERROR_NO_ARGS("Cannot retrieve version with specified wait option");
    return ATLK_E_UNSPECIFIED;
  }

  tv.tv_sec = wait->wait_usec / MICRO_PER_UNIT;
  tv.tv_usec = wait->wait_usec % MICRO_PER_UNIT;

  /* Get CDB-ID 500 ("Text message") from default configuration block */
  rc = nmea_io_write_sentence(&gnss_nmea->nmea_io, GET_TEXT_MSG_CMD,
                              sizeof(GET_TEXT_MSG_CMD), &atlk_wait_forever);
  if (atlk_error(rc)) {
    return rc;
  }

  atlk_mutex_lock(&teseo_fw_version.mutex);       /* Lock! */
  
  /* Wait for Teseo reply up to passed wait option */
  rc = atlk_cond_rel_timedwait(&teseo_fw_version.cond,
                               &teseo_fw_version.mutex, &tv);
  if (rc == ATLK_E_TIMEOUT) {
    TR_DEBUG_NO_ARGS("Condition wait timed-out");
  }

  atlk_mutex_unlock(&teseo_fw_version.mutex);     /* Unlock! */

  /* Exit early */
  if (atlk_error(rc)) {
    return rc;
  }

copy:
  /* Get version string length */
  len = strnlen(teseo_fw_version.buf, sizeof(teseo_fw_version.buf) - 1);

  /* We are strict; we don't copy partial version string */
  if (len >= *fw_version_size) {
    TR_ERROR("Buffer is too small, version string length: %zu", len + 1);
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  /* Copy and null-terminate it */
  strncpy(fw_version, teseo_fw_version.buf, len);
  fw_version[len] = '\0';
  *fw_version_size = len + 1;

  return ATLK_OK;
}

/* Command to perform hot start */
#define HOT_START_CMD "$PSTMHOT\r\n"

/* Command to perform warm start */
#define WARM_START_CMD "$PSTMWARM\r\n"

/* Command to perform cold start */
#define COLD_START_CMD_FMT "$PSTMCOLD,%d\r\n"

/* Cold start commad mask */
#define COLD_START_CMD_MASK_CLEAR_ALMANAC 0x1
#define COLD_START_CMD_MASK_CLEAR_EPHEMERIS 0x2
#define COLD_START_CMD_MASK_CLEAR_POSITION 0x4
#define COLD_START_CMD_MASK_CLEAR_TIME 0x8

/* Comamnd to perform SW reset */
#define SW_RESET_CMD "$PSTMSRR\r\n"

static int
get_teseo_cold_start_flags(uint32_t flags)
{
  int mask = 0;

  mask |= (flags & GNSS_COLD_START_F_CLEAR_ALMANAC) ?
    COLD_START_CMD_MASK_CLEAR_ALMANAC : 0;
  mask |= (flags & GNSS_COLD_START_F_CLEAR_EPHEMERIS) ?
    COLD_START_CMD_MASK_CLEAR_EPHEMERIS : 0;
  mask |= (flags & GNSS_COLD_START_F_CLEAR_POSITION) ?
    COLD_START_CMD_MASK_CLEAR_POSITION : 0;
  mask |= (flags & GNSS_COLD_START_F_CLEAR_TIME) ?
    COLD_START_CMD_MASK_CLEAR_TIME : 0;

  TR_DEBUG("Teseo cold restart flags: 0x%u", mask);

  return mask;
}

atlk_rc_t atlk_must_check
teseo_reset(const gnss_reset_params_t *params,
            const atlk_wait_t *wait,
            gnss_nmea_t *gnss_nmea)
{
  (void)wait;
  char data[32] = { 0 };
  size_t data_size = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!params || !gnss_nmea);

  if (params->reset_type == GNSS_RESET_TYPE_HW) {
    if (!teseo_hw_reset) {
      TR_ERROR("Teseo HW reset not currently supported");
      return ATLK_E_UNSUPPORTED;
    }
    return teseo_hw_reset();
  }

  if (params->start_type != GNSS_START_TYPE_COLD &&
      params->cold_start_flags) {
    TR_ERROR("Flags supported only when choosing start type cold");
    return ATLK_E_UNSUPPORTED;
  }

  switch (params->start_type) {
  case GNSS_START_TYPE_HOT:
    memcpy(data, HOT_START_CMD, sizeof(HOT_START_CMD));
    data_size = sizeof(HOT_START_CMD);
    break;
  case GNSS_START_TYPE_WARM:
    memcpy(data, WARM_START_CMD, sizeof(WARM_START_CMD));
    data_size = sizeof(WARM_START_CMD);
    break;
  case GNSS_START_TYPE_COLD:
  {
    int flags = get_teseo_cold_start_flags(params->cold_start_flags);
    data_size = snprintf(data, sizeof(data), COLD_START_CMD_FMT, flags);
    break;
  }
  case GNSS_START_TYPE_NA:
  default:
    BUG();
  }

  rc = nmea_io_write_sentence(&gnss_nmea->nmea_io, data, data_size,
                              &atlk_wait_forever);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to set GNSS start (rc=%d)", rc);
    return rc;
  }

  switch (params->reset_type) {
  case GNSS_RESET_TYPE_SW:
    break;
  case GNSS_RESET_TYPE_GNSS_ONLY:
    /* We are done */
    return ATLK_OK;
  case GNSS_RESET_TYPE_HW:
  case GNSS_RESET_TYPE_NA:
  default:
    BUG();
  }

  rc = nmea_io_write_sentence(&gnss_nmea->nmea_io, SW_RESET_CMD,
                              sizeof(SW_RESET_CMD), &atlk_wait_forever);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to reset GNSS device (rc=%d)", rc);
  }

  return rc;
}

static struct drcal_report {
  int dr_is_calib;
  int enabled;
}
drcal_report = { 
  .dr_is_calib = 0,
  .enabled = 1
};

void
teseo_drcal_report_set(int enabled)
{
  drcal_report.enabled = enabled;
}

static atlk_rc_t atlk_must_check
nmea_callback_stm_drcal(const char *sentence,
            const nmea_address_t *address,
            const void *data,
            void *context)
{ 
  nmea_data_stm_drcal_t *drcal = (nmea_data_stm_drcal_t *)data;
  (void)context;
  
  BUG_ON(address->formatter != NMEA_FORMATTER_STM_DRCAL);

  if (!drcal_report.enabled) {
    return ATLK_OK;
  }
  
  if (drcal->dr_is_calib && !drcal_report.dr_is_calib) {
    TR_INFO("DR is fully calibrated");
    drcal_report.dr_is_calib = 1;
  }
  else if (!drcal->dr_is_calib) {
    TR_INFO("DR is NOT fully calibrated: %s", sentence);
    drcal_report.dr_is_calib = 0;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
teseo_commands_init(nmea_t *nmea,
            gnss_hw_reset_t hw_reset)
{
  nmea_callback_data_t callback_data = {
    .callback = nmea_callback_stm_setpar,
    .context = NULL
  };
  nmea_address_t address = NMEA_ADDRESS_INIT;
  atlk_rc_t rc = ATLK_OK;

  if (atlk_unlikely(!nmea)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  teseo_hw_reset = hw_reset;

  /* Create Teseo firmware version container */
  rc = teseo_firmware_version_create(&teseo_fw_version);
  if (atlk_error(rc)) {
    return rc;
  }

  /* Register STM parameter callbacks */
  address.type = NMEA_TYPE_PROPRIETARY;
  address.sentence_id = NMEA_SENTENCE_ID_STM;
  address.formatter = NMEA_FORMATTER_STM_GETPARERROR;
  nmea_callback_register(nmea, &address, callback_data);
  address.formatter = NMEA_FORMATTER_STM_SETPAR;
  nmea_callback_register(nmea, &address, callback_data);

  /* Register DR calibration reporting callback */
  callback_data.callback = nmea_callback_stm_drcal;
  address.formatter = NMEA_FORMATTER_STM_DRCAL;
  nmea_callback_register(nmea, &address, callback_data);

  return ATLK_OK;
}
