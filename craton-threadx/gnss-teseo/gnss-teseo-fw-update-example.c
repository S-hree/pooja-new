/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <tx_api.h>

#include <atlk/sdk.h>
#include <atlk/gnss.h>
#include <atlk/gnss_teseo.h>

#include <craton/fs.h>

/*
  CRATON ThreadX Teseo FW Update Example

  This example demonstrates basic usage of Teseo firmware update API for code
  running on top of CRATON processor with ThreadX RTOS.

  Expected firmware image format is a Teseo UPG image. For simplicty, the
  example assumes a valid firmware image is at 'A:/image.bin' (i.e. on microSD).
*/

/* Firmware update thread priority */
#define TESEO_FW_UPDATE_THREAD_PRIORITY 20

/* Firmware update thread */
static TX_THREAD gnss_teseo_fw_update_thread;
static uint8_t gnss_teseo_fw_update_thread_stack[0x8000];
static void gnss_teseo_fw_update_thread_entry(ULONG input);

/* Firmware image path */
#define FW_IMAGE_PATH "A:/image.bin"

/* Firmware image max size */
#define FW_IMAGE_SIZE_MAX 0x100000

/* Firmware image buffer */
static uint8_t fw_image_buffer[FW_IMAGE_SIZE_MAX];

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;

  /* Create firmware update thread */
  trv = tx_thread_create(&gnss_teseo_fw_update_thread,
                         "gnss_teseo_fw_update_thread",
                         gnss_teseo_fw_update_thread_entry, 0,
                         gnss_teseo_fw_update_thread_stack,
                         sizeof(gnss_teseo_fw_update_thread_stack),
                         TESEO_FW_UPDATE_THREAD_PRIORITY,
                         TESEO_FW_UPDATE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;
}

static void gnss_teseo_fw_update_thread_entry(ULONG input)
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
  /* Teseo firmware update parameters */
  gnss_teseo_fw_update_params_t params = GNSS_TESEO_FW_UPDATE_PARAMS_INIT;
  /* Not using input */
  (void)input;

  printf("Start Teseo firmware update example...\n");

  /* Enable the usage of file system in this thread */
  rv = fs_thread_enable();
  if (rv == -1) {
    fprintf(stderr, "fs_thread_enable failed, errno: %d\n", errno);
    return;
  }

  /* Get firmware image file statistics */
  rv = stat(FW_IMAGE_PATH, &st);
  if (rv == -1) {
    fprintf(stderr, "stat failed, errno: %d\n", errno);
    return;
  }

  /* Size of firmware image file */
  assert(st.st_size <= INT32_MAX);
  fw_image_size = (uint32_t)st.st_size;

  /* Make sure firmware image size makes sense */
  assert(fw_image_size <= sizeof(fw_image_buffer));

  /* Open firmware image in read-only mode */
  fd = open(FW_IMAGE_PATH, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "open failed, errno %d\n", errno);
    return;
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

  return;
}
