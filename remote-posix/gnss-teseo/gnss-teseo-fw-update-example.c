/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <atlk/sdk.h>
#include <atlk/gnss.h>
#include <atlk/gnss_teseo.h>

/*
  POSIX Teseo FW Update Example

  This example demonstrates basic usage of Teseo firmware update API for code
  running externally to CRATON processor with an OS supporting POSIX.

  Expected firmware image format is a Teseo UPG image. Image path should be
  passed to the executable.
*/

/* Implementation of the navigation service instance */
static struct nav_service {
  int __dummy;
} nav_service;

/* Registered GNSS navigation fix handler */
static atlk_rc_t
nav_fix_publish(nav_service_t *service, nav_fix_t *fix)
{
  (void)service;
  (void)fix;
  return ATLK_OK;
}

/* GNSS thread priority */
#define GNSS_THREAD_PRIORITY 20

/* GNSS device name */
#define GNSS_DEV_NAME "/dev/ttyUSB0"

/* Firmware update thread priority */
#define TESEO_FW_UPDATE_THREAD_PRIORITY 20

/* Firmware image max size */
#define FW_IMAGE_SIZE_MAX 0x100000

/* Firmware image buffer */
static uint8_t fw_image_buffer[FW_IMAGE_SIZE_MAX];

int main(int argc, char *argv[])
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* POSIX return value */
  int rv = 0;
  /* File statistics */
  struct stat st;
  /* Size of firmware image file */
  uint32_t fw_image_size;
  /* File descriptor */
  int fd = -1;
  /* GNSS configuration parameters */
  gnss_config_t config = GNSS_CONFIG_INIT;
  /* Teseo firmware update parameters */
  gnss_teseo_fw_update_params_t params = GNSS_TESEO_FW_UPDATE_PARAMS_INIT;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s IMAGE_PATH\n", argv[0]);
    return EXIT_FAILURE;
  }

  printf("Start Teseo firmware update example...\n");

  /* Set GNSS configuration parameters */
  config.model = GNSS_MODEL_STMICRO_TESEO_II;
  config.dev_name = GNSS_DEV_NAME;
  config.sched_params.priority = GNSS_THREAD_PRIORITY;
  config.handler = nav_fix_publish;
  config.service = &nav_service;
 
  /* Initialize GNSS */
  rc = gnss_init(&config);
  if (atlk_error(rc)) {
    fprintf(stderr, "gnss_init: %s\n", atlk_rc_to_str(rc));
    return EXIT_FAILURE;
  }

  /* Get firmware image file statistics */
  rv = stat(argv[1], &st);
  if (rv == -1) {
    fprintf(stderr, "stat failed, errno: %d\n", errno);
    return EXIT_FAILURE;
  }

  /* Size of firmware image file */
  assert(st.st_size <= INT32_MAX);
  fw_image_size = (uint32_t)st.st_size;

  /* Make sure firmware image size makes sense */
  assert(fw_image_size <= sizeof(fw_image_buffer));

  /* Open firmware image in read-only mode */
  fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "open failed, errno %d\n", errno);
    return EXIT_FAILURE;
  }

  /* Read image into buffer */
  rv = read(fd, fw_image_buffer, fw_image_size);
  if (rv == -1) {
    fprintf(stderr, "read failed, errno %d\n", errno);
    goto exit;
  }
  assert((uint32_t)rv == fw_image_size);

  /* Set Teseo firmware update parameters */
  params.fw_image = fw_image_buffer;
  params.fw_image_size = fw_image_size;
  params.nmea_speed_bps = UART_SPEED_230400_BPS;
  params.download_speed_bps = UART_SPEED_921600_BPS;
  params.erase_nvm_area = 1;
  params.nvm_area_size_kb = 0;
  params.recovery_mode = 0;
  params.sched_params.priority = TESEO_FW_UPDATE_THREAD_PRIORITY;

  /* Update Teseo firmware */
  rc = gnss_teseo_fw_update(&params);
  if (atlk_error(rc)) {
    fprintf(stderr, "gnss_teseo_fw_update failed: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  printf("Firmware updated successfully!\n");

exit:
  /* Clean-up resources */
  close(fd);

  return EXIT_SUCCESS;
}
