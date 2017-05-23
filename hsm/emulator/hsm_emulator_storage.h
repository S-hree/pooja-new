#ifndef _HSM_EMULATOR_STORAGE_H
#define _HSM_EMULATOR_STORAGE_H

#include <atlk/sdk.h>
#include <platform/common.h>
#include <atlk/hsm.h>
#include <atlk/hsm_service.h>
#include <atlk/hsm_emulator.h>

/*
 * IMPORTANT:
 * The HSM storage API is not thread safe!
 */

/* Maximum number of available storage cells (10 years, 20 keys per week) */
#define HSM_EMULATOR_STORAGE_NUM_CELLS_MAX 10436

/* Invalid file handle */
#define HSM_EMULATOR_INVALID_FILE_HANDLE (-1)

/* Storage cell descriptor */
typedef struct {
  /* Is this cell used? */
  int used;

  /* Private key information */
  hsm_ecc_private_key_info_t private_key_info;

  /* Private key */
  ecc_scalar_t private_key;

  /* Public key */
  ecc_point_t  public_key;

} hsm_emulator_storage_cell_t;

/* Storage cell default static initializer */
#define HSM_EMULATOR_STORAGE_CELL_INIT {                \
  .used = 0,                                            \
  .private_key_info = HSM_ECC_PRIVATE_KEY_INFO_INIT,    \
  .private_key = ECC_SCALAR_INIT,                       \
  .public_key = ECC_POINT_INIT                          \
}

/* This structure is used as a vtable for the HSM emulator storage */
typedef struct {
  /* Initializes a storage file media
   * if storage_num_cells_max == 0 ==> Auto-detect the storage size.
   * if storage_num_cells_max != 0 and the file doesn't exist ==> Create the
   *   storage with size ==  storage_num_cells_max * sizeof(record).
   *
   * @param[in] nvm_file_path NVM file path
   * @param[in,out] storage_num_cells_max Max number of cells
   *
   * @retval ::ATLK_OK for success
   * @return Error code if failed
   */
  atlk_rc_t atlk_must_check
  (*init)(const char *nvm_file_path, unsigned int *storage_num_cells_max);

  /* Writes a single cell to the media (and flushes afterwards if required)
   *
   * @param[in] cell_index cell index
   * @param[in] cell Pointer to cell data
   *
   * @retval ::ATLK_OK for success
   * @return Error code if failed
   */
  atlk_rc_t atlk_must_check
  (*cell_write)(uint32_t cell_index, const hsm_emulator_storage_cell_t *cell);

  /* Reads a single cell from the media
   *
   * @param[in] cell_index cell index
   * @param[out] cell Pointer to cell data
   *
   * @retval ::ATLK_OK for success
   * @return Error code if failed
   */
  atlk_rc_t atlk_must_check
  (*cell_read)(uint32_t cell_index, hsm_emulator_storage_cell_t *cell);

} hsm_emulator_storage_vtable_t;

/* Gets the vtable for a file based storage
 *
 * @param[out] vtable storage vtable
 *
 * @retval ::ATLK_OK for success
 * @return Error code if failed
 */
atlk_rc_t atlk_must_check
hsm_emulator_storage_file_vtable_get(
    const hsm_emulator_storage_vtable_t **vtable);

/* Gets the vtable for a RAM based storage
 *
 * @param[out] vtable storage vtable
 *
 * @retval ::ATLK_OK for success
 * @return Error code if failed
 */
atlk_rc_t atlk_must_check
hsm_emulator_storage_ram_vtable_get(
    const hsm_emulator_storage_vtable_t **vtable);

#endif /* _HSM_EMULATOR_STORAGE_H */
