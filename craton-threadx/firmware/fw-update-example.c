/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <tx_api.h>

#include <atlk/sdk.h>

#include <craton/fs.h>
#include <craton/nor_flash.h>
#include <craton/fw_uimage.h>

/* 
  CRATON ThreadX Firmware Update Example 
  
  This example demonstrates basic usage of NOR Flash API for code running on
  top of CRATON processor with ThreadX RTOS.

  Expected firmware image format is U-Boot uImage. For simplicty, the example
  assumes a valid firmware image is at 'A:/uImage' (i.e. on microSD).

  Warning! When 'A:/uImage' exists and is valid, this example will re-write
  main firmware image partition with this file.
*/

/* Firmware update thread priority */
#define FW_UPDATE_THREAD_PRIORITY 20

/* Firmware update thread */
static TX_THREAD fw_update_thread;
static uint8_t fw_update_thread_stack[0x8000];
static void fw_update_thread_entry(ULONG input);

/* Firmware image path */
#define FW_IMAGE_PATH "A:/uImage"

/* Firmware image max size */
#define FW_IMAGE_SIZE_MAX 0x200000

/* Firmware image buffer */
static uint8_t fw_image_buffer[FW_IMAGE_SIZE_MAX];

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;

  /* Initialize NOR flash interface */
  rc = norfl_init();
  if (atlk_error(rc)) {
    fprintf(stderr, "norfl_init: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Create firmware update thread */
  trv = tx_thread_create(&fw_update_thread, "fw_update_thread",
                         fw_update_thread_entry, 0,
                         fw_update_thread_stack,
                         sizeof(fw_update_thread_stack),
                         FW_UPDATE_THREAD_PRIORITY,
                         FW_UPDATE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;
}

static void fw_update_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* POSIX return value */
  int rv = 0;
  /* File statistics */
  struct stat st;
  /* Partition table */
  norfl_part_table_t part_table = NORFL_PART_TABLE_INIT;
  /* Size of main firmware image partition */
  uint32_t main_fw_part_size;
  /* Size of firmware image file */
  uint32_t fw_image_size;
  /* File descriptor */
  int fd = -1;
  /* Not using input */
  (void)input;

  printf("Start firmware update example...\n");

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
  fw_image_size = (uint32_t)st.st_size;

  /* Read partition table */
  rc = norfl_part_table_read(&part_table);
  if (atlk_error(rc)) {
    fprintf(stderr, "norfl_part_table_read: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Size of main firmware image partition */
  main_fw_part_size =
    part_table.part_info[NORFL_PART_FIRMWARE_MAIN].part_size;

  /* Make sure firmware image size makes sense */
  assert(fw_image_size <= main_fw_part_size);
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

  /* Make sure firmware image is a valid uImage file */
  if (!fw_uimage_valid(fw_image_buffer, fw_image_size)) {
    fprintf(stderr, "fw_uimage_valid returned false\n");
    goto exit;
  }

  /* Re-write main firmware image */
  rc = norfl_part_rewrite(NORFL_PART_FIRMWARE_MAIN,
                          fw_image_buffer, fw_image_size);
  if (atlk_error(rc)) {
    fprintf(stderr, "norfl_part_rewrite: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  printf("Firmware updated successfully!\n");

exit:
  /* Clean-up resources */
  close(fd);

  return;
}
