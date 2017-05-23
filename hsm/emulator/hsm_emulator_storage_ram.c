/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <atlk/sdk.h>
#include <platform/common.h>

#include "hsm_emulator_storage.h"

typedef struct {
  /* Pointer to storage */
  hsm_emulator_storage_cell_t *table;

  /* Max number of storage cells */
  unsigned int storage_num_cells_max;

} hsm_emulator_storage_ram_t;

/* HSM emulator storage file default static initializer */
#define HSM_EMULATOR_STORAGE_RAM_INIT {            \
  .table = NULL,                                   \
  .storage_num_cells_max = 0                       \
}

static hsm_emulator_storage_ram_t storage_ram =
    HSM_EMULATOR_STORAGE_RAM_INIT;

static void
hsm_emulator_storage_ram_cells_reset(void)
{
  hsm_emulator_storage_cell_t cell = HSM_EMULATOR_STORAGE_CELL_INIT;
  unsigned int i;

  BUG_ON(storage_ram.table == NULL);

  for (i = 0; i < storage_ram.storage_num_cells_max; i++) {
    storage_ram.table[i] = cell;
  }
}

static atlk_rc_t atlk_must_check
hsm_emulator_storage_ram_init(const char *nvm_file_path,
                              unsigned int *storage_num_cells_max)
{
  BUG_ON(nvm_file_path);
  BUG_ON(!storage_num_cells_max);

  if (*storage_num_cells_max != 0) {
    size_t total_bytes;

    if (storage_ram.table) {
      free(storage_ram.table);
    }

    total_bytes = *storage_num_cells_max * sizeof(*storage_ram.table);
    storage_ram.table = malloc(total_bytes);
    if (!storage_ram.table) {
      TR_ERROR("Cannot allocate storage ram buffer of size %zu",
               total_bytes);
      return ATLK_E_OUT_OF_MEMORY;
    }

    storage_ram.storage_num_cells_max = *storage_num_cells_max;

    hsm_emulator_storage_ram_cells_reset();
  }
  else {
    /*
     * Do nothing here.
     * If the size is 0 ==> Still don't return an error - give the user
     * a chance to use the API to build it.
     */
  }

  return ATLK_OK;
}

static atlk_rc_t atlk_must_check
hsm_emulator_storage_cell_ram_write(uint32_t cell_index,
                                    const hsm_emulator_storage_cell_t *cell)
{
  BUG_ON(storage_ram.table == NULL);
  BUG_ON(cell_index >= storage_ram.storage_num_cells_max);
  BUG_ON(!cell);

  memcpy(&storage_ram.table[cell_index], cell, sizeof(*cell));

  return ATLK_OK;
}

static atlk_rc_t atlk_must_check
hsm_emulator_storage_cell_ram_read(uint32_t cell_index,
                                   hsm_emulator_storage_cell_t *cell)
{
  BUG_ON(storage_ram.table == NULL);
  BUG_ON(cell_index >= storage_ram.storage_num_cells_max);
  BUG_ON(!cell);

  memcpy(cell, &storage_ram.table[cell_index], sizeof(*cell));

  return ATLK_OK;
}

static const hsm_emulator_storage_vtable_t hsm_emulator_ram_vtable = {
  .init = hsm_emulator_storage_ram_init,              \
  .cell_write = hsm_emulator_storage_cell_ram_write,  \
  .cell_read = hsm_emulator_storage_cell_ram_read,    \
};

atlk_rc_t atlk_must_check
hsm_emulator_storage_ram_vtable_get(
    const hsm_emulator_storage_vtable_t **vtable)
{
  BUG_ON(vtable == NULL);

  *vtable = &hsm_emulator_ram_vtable;

  return ATLK_OK;
}
