/* Copyright (C) 2013-2015 Autotalks Ltd. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <atlk/sdk.h>
#include <platform/common.h>

#ifdef __CRATON__
#include <craton/fs.h>
#endif

#include "hsm_emulator_storage.h"

/* HSM emulator storage file default static initializer */
#define HSM_EMULATOR_STORAGE_FILE_INIT {           \
  .nvm_file_path = NULL,                           \
  .file_handle = HSM_EMULATOR_INVALID_FILE_HANDLE, \
  .storage_num_cells_max = 0                       \
}

typedef struct {
  /* NVM file path */
  const char *nvm_file_path;

  /* File handle */
  int file_handle;

  /* Max number of storage cells */
  unsigned int storage_num_cells_max;

} hsm_emulator_storage_file_t;

#if !defined(__CRATON__) || defined(__arm__)

static hsm_emulator_storage_file_t storage_file =
    HSM_EMULATOR_STORAGE_FILE_INIT;

atlk_inline void
hsm_emulator_storage_file_access_enable(void)
{
#if defined(__CRATON__) && !defined(CONFIG_USE_ARC)
  /* Enable thread access to file system */
  fs_thread_enable();
#endif
}

/* Closes the file */
static void
hsm_emulator_storage_file_close(void)
{
  if (storage_file.file_handle != HSM_EMULATOR_INVALID_FILE_HANDLE) {
    close(storage_file.file_handle);
    storage_file.file_handle = HSM_EMULATOR_INVALID_FILE_HANDLE;
  }
}

/* Opens the media
 *
 * @retval ::ATLK_OK for success
 * @return atlk return code
 */
static atlk_rc_t atlk_must_check
hsm_emulator_storage_file_open(void)
{
  /* File shouldn't be open at this point */
  if (storage_file.file_handle != HSM_EMULATOR_INVALID_FILE_HANDLE) {
    TR_WARNING("File already open...");
    return ATLK_OK;
  }

  /* Re-open the file, with the read/write flags only */
  storage_file.file_handle = open(storage_file.nvm_file_path, O_RDWR);
  if (storage_file.file_handle == -1) {
    /* No message here, message is printed to log if needed by
     * the caller of this function.
     */
    return ATLK_E_IO_ERROR;
  }

  return ATLK_OK;
}

/* Rewrites the media (i.e. zeros all its cells).
 *
 * @retval ::ATLK_OK for success
 * @return atlk return code
 */
static atlk_rc_t atlk_must_check
hsm_emulator_storage_file_create(void)
{
  int len;
  unsigned int i;
  hsm_emulator_storage_cell_t cell = HSM_EMULATOR_STORAGE_CELL_INIT;
  atlk_rc_t rc = ATLK_OK;

  /*
   * Open the file, rewrite it (reinitialize), create it, read/write
   * Then close it and re-open it with (O_RDWR) flags.
   * The reason why we use it is a workaround, since the open
   * function fails when using the (O_RDWR | O_CREAT | O_TRUNC) flags together.
   *
   */
  storage_file.file_handle = open(storage_file.nvm_file_path,
                                  O_WRONLY | O_CREAT | O_TRUNC,
                                  S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  if (storage_file.file_handle == -1) {
    TR_ERROR("Cannot open file for re-write: %s, errno: %d",
             storage_file.nvm_file_path, errno);
    rc = ATLK_E_IO_ERROR;
    goto failure;
  }

#ifndef __CRATON__
  /* Change mode to read/write for user, only-only for group */
  int ret = chmod(storage_file.nvm_file_path, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  if (ret != 0) {
    TR_ERROR("Cannot change file attributes: %s, errno: %d",
             storage_file.nvm_file_path, errno);
    rc = ATLK_E_IO_ERROR;
    goto failure;
  }
#endif

  /* Create a new file with the correct total size of records */
  for (i = 0; i < storage_file.storage_num_cells_max; i++) {
    len = write(storage_file.file_handle,
                &cell,
                sizeof(cell));
    if (len != sizeof(cell)) {
      TR_ERROR("Failed to write to the file: len: %d, errno: %d",
               len,
               errno);
      rc = ATLK_E_IO_ERROR;
      goto failure;
    }
  }

  /* Close the file */
  hsm_emulator_storage_file_close();

  /* Re-open the file */
  rc = hsm_emulator_storage_file_open();
  if (atlk_error(rc)) {
    TR_ERROR("Opening a new file failed %d", rc);
    goto failure;
  }

  return ATLK_OK;

failure:
  hsm_emulator_storage_file_close();
  return rc;
}

#endif /* !__CRATON__ || __arm__ */

static atlk_rc_t atlk_must_check
hsm_emulator_storage_file_init(const char *nvm_file_path,
                               unsigned int *storage_num_cells_max)
{
#if defined(__CRATON__) && !defined(__arm__)
  return ATLK_E_UNSUPPORTED;
#else
  int ret;
  atlk_rc_t rc = ATLK_OK;
  struct stat file_status;

  BUG_ON(!nvm_file_path);
  BUG_ON(!storage_num_cells_max);

  /* Enable file access */
  hsm_emulator_storage_file_access_enable();

  /* Close previously open file (if needed) */
  hsm_emulator_storage_file_close();

  storage_file.nvm_file_path = nvm_file_path;
  storage_file.storage_num_cells_max = *storage_num_cells_max;

  /* Create a new file? */
  if (storage_file.storage_num_cells_max != 0) {
    rc = hsm_emulator_storage_file_create();
    if (atlk_error(rc)) {
      TR_ERROR("Creating a new file failed %d", rc);
      return rc;
    }

    return ATLK_OK;
  }
  else {
    /* File exists? */
    rc = hsm_emulator_storage_file_open();
    if (atlk_error(rc)) {
      /*
       * Return ATLK_OK since we give the user a chance to create the storage
       * after initializing the HSM emulator storage.
       * Also, we do not print any error here to the log:
       * If the user starts using any API before initializing the NVM
       * and the file does not exist => Error will be printed.
       */
      return ATLK_OK;
    }

    /* Get file size */
    ret = stat(storage_file.nvm_file_path, &file_status);
    if (ret == -1) {
      TR_WARNING("Error getting size of file: %s does not exist, ret=%d",
                 storage_file.nvm_file_path,
                 ret);
      rc = ATLK_E_IO_ERROR;
      goto failure;
    }

    if ((file_status.st_size % sizeof(hsm_emulator_storage_cell_t)) != 0) {
      TR_ERROR("Size of file: %s (size = %lu) is not a multiple of %zu",
                 storage_file.nvm_file_path,
                 (unsigned long)file_status.st_size,
                 sizeof(hsm_emulator_storage_cell_t));
      rc = ATLK_E_IO_ERROR;
      goto failure;
    }

    /* Calculate the file size */
    storage_file.storage_num_cells_max =
     (unsigned int)(file_status.st_size / sizeof(hsm_emulator_storage_cell_t));

    /* Update number of cells */
    *storage_num_cells_max = storage_file.storage_num_cells_max;

    return ATLK_OK;

failure:
    /* Close the file */
    hsm_emulator_storage_file_close();
    return rc;
  }
#endif /* __CRATON__ && !__arm__ */
}

static atlk_rc_t atlk_must_check
hsm_emulator_storage_cell_file_write(uint32_t cell_index,
                                     const hsm_emulator_storage_cell_t *cell)
{
#if defined(__CRATON__) && !defined(__arm__)
  return ATLK_E_UNSUPPORTED;
#else
  off_t offset;
  ssize_t len;
  int rv;

  /* Enable file access */
  hsm_emulator_storage_file_access_enable();

  /* The file should already be open at this stage */
  if (storage_file.file_handle == HSM_EMULATOR_INVALID_FILE_HANDLE) {
    TR_ERROR_NO_ARGS("File is not open");
    return ATLK_E_INVALID_STATE;
  }

  BUG_ON(!cell);

  if (cell_index >= storage_file.storage_num_cells_max) {
    TR_ERROR("cell_index (%"PRIu32 ") not in the range of storage_num_cells"
             "(%u)",
             cell_index,
             storage_file.storage_num_cells_max);
    return ATLK_E_OUT_OF_BOUNDS;
  }

  offset = cell_index * sizeof(hsm_emulator_storage_cell_t);

  offset = lseek(storage_file.file_handle, offset, SEEK_SET);
  if (offset == -1) {
    TR_ERROR("lseek failed, errno=%d", errno);
    return ATLK_E_IO_ERROR;
  }

  len = write(storage_file.file_handle,
              cell,
              sizeof(*cell));
  if (len != sizeof(*cell)) {
    TR_ERROR("Failed to write to the file: len: %d, errno: %d",
             (int)len, errno);
    return ATLK_E_IO_ERROR;
  }

  /* Sync file-system */
  rv = fsync(storage_file.file_handle);
  if (rv == -1) {
    TR_ERROR("Failed to synchronize file (errno=%d)", errno);
    return ATLK_E_IO_ERROR;
  }

  return ATLK_OK;
#endif /* __CRATON__ && !__arm__ */
}

static atlk_rc_t atlk_must_check
hsm_emulator_storage_cell_file_read(uint32_t cell_index,
                                    hsm_emulator_storage_cell_t *cell)
{
#if defined(__CRATON__) && !defined(__arm__)
  return ATLK_E_UNSUPPORTED;
#else
  off_t offset;
  ssize_t len;

  /* Enable file access */
  hsm_emulator_storage_file_access_enable();

  /* The file should already be open at this stage */
  if (storage_file.file_handle == HSM_EMULATOR_INVALID_FILE_HANDLE) {
    TR_ERROR_NO_ARGS("File is not open");
    return ATLK_E_INVALID_STATE;
  }

  BUG_ON(!cell);

  if (cell_index >= storage_file.storage_num_cells_max) {
    TR_ERROR("cell_index (%"PRIu32 ") not in the range of storage_num_cells"
             "(%u)",
             cell_index,
             storage_file.storage_num_cells_max);
    return ATLK_E_OUT_OF_BOUNDS;
  }

  offset = cell_index * sizeof(*cell);

  offset = lseek(storage_file.file_handle, offset, SEEK_SET);
  if (offset == -1) {
    TR_ERROR("lseek failed, errno: %d", errno);
    return ATLK_E_IO_ERROR;
  }

  len = read(storage_file.file_handle,
             cell,
             sizeof(*cell));
  if (len != sizeof(*cell)) {
    TR_ERROR("Failed to read from the file: len: %d, errno: %d",
             (int)len,
             errno);
    return ATLK_E_IO_ERROR;
  }

  return ATLK_OK;
#endif /* __CRATON__ && !__arm__ */
}

static const hsm_emulator_storage_vtable_t hsm_emulator_file_vtable = {
  .init = hsm_emulator_storage_file_init,             \
  .cell_write = hsm_emulator_storage_cell_file_write, \
  .cell_read = hsm_emulator_storage_cell_file_read,   \
};

atlk_rc_t atlk_must_check
hsm_emulator_storage_file_vtable_get(
    const hsm_emulator_storage_vtable_t **vtable)
{
  BUG_ON(vtable == NULL);

  *vtable = &hsm_emulator_file_vtable;

  return ATLK_OK;
}
