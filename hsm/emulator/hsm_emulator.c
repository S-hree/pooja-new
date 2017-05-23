/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <atlk/sdk.h>
#include <atlk/hsm.h>
#include <atlk/hsm_service.h>
#include <atlk/rng.h>
#include <atlk/ecc.h>
#include <atlk/ecc_math.h>
#include <atlk/ecdsa.h>
#include <atlk/ecc_service.h>
#include <atlk/hsm_emulator.h>

#include <platform/common.h>
#include <platform/osal.h>

#include <crypto/ecc_curves.h>
#include <crypto/crypto_fmt.h>
#include <crypto/crypto_ecc.h>
#include <crypto/crypto_sha.h>
#include <crypto/crypto_kdf.h>

#include "hsm_emulator_storage.h"
#include "../vtable/hsm_vtable.h"

/* Maximum cell ranges supported by ::hsm_emulator_csr_ecdsa_public_keys_sign */
#define HSM_EMULATOR_MAX_CSR_CELL_RANGES 16

/* Maximum ECIES key size supported by ::hsm_emulator_ecies_key_derive */
#define HSM_EMULATOR_MAX_ECIES_KEY_SIZE 256

/* HSM emulator descriptor */
typedef struct {

  /* ECC service used by HSM service */
  ecc_service_t *ecc_service;

  /* ECC socket used by HSM service */
  ecc_socket_t *ecc_socket;

  /* Key pair storage lock */
  osal_mutex_t storage_lock;

  /* Number of used storage cells */
  unsigned int storage_num_cells;

  /* NVM filename */
  const char *nvm_file_path;

  /* AES-128 key to provide host NVM storage integrity */
  aes_key_t host_nvm_authentication_key;

  /* AES-128 key to provide host NVM storage confidentiality */
  aes_key_t host_nvm_encryption_key;

  /* HSM emulator storage vtable */
  const hsm_emulator_storage_vtable_t *storage;

  /* HSM service */
  hsm_service_t *hsm_service;

} hsm_emulator_t;

static void
hsm_emulator_close(hsm_emulator_t *emulator);

atlk_rc_t
hsm_emulator_service_delete(void *context)
{
  hsm_emulator_t *emulator = context;
  hsm_service_t *service = NULL;

  /* Validate arguments */
  BUG_ON(!emulator);

  service = emulator->hsm_service;

  hsm_emulator_close(emulator);
  free(emulator);
  free(service);

  return ATLK_OK;
}

atlk_rc_t
hsm_emulator_capability_info_get(void *context,
                       hsm_capability_info_t *capability_info)
{
  hsm_emulator_t *emulator = context;
  atlk_rc_t rc;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!capability_info) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  capability_info->max_num_of_cells = HSM_EMULATOR_STORAGE_NUM_CELLS_MAX;
  capability_info->current_num_of_cells = emulator->storage_num_cells;
  capability_info->max_num_of_cell_ranges_for_csr = \
    HSM_EMULATOR_MAX_CSR_CELL_RANGES;
  capability_info->max_ecies_key_size = HSM_EMULATOR_MAX_ECIES_KEY_SIZE;
  capability_info->max_ecies_kdf_param_size = KDF2_MAX_PARAM_SIZE;

  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return ATLK_OK;
}

atlk_rc_t
hsm_emulator_nvm_init(void *context, const hsm_nvm_config_t *config)
{
  atlk_rc_t rc;
  hsm_emulator_t *emulator = context;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!config) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (config->num_of_cells > HSM_EMULATOR_STORAGE_NUM_CELLS_MAX) {
    TR_ERROR("Invalid number of cells %" PRIu32 ", larger than maximum %u",
      config->num_of_cells, HSM_EMULATOR_STORAGE_NUM_CELLS_MAX);
    return ATLK_E_INVALID_ARG;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Set new number of cells */
  emulator->storage_num_cells = config->num_of_cells;

  rc = emulator->storage->init(emulator->nvm_file_path,
                            &emulator->storage_num_cells);
  if (atlk_error(rc)) {
    TR_ERROR("Storage initialization failed (rc=%d)", rc);
    goto out;
  }

out:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return ATLK_OK;
}

static int
hsm_emulator_public_key_algorithm_valid(hsm_public_key_algorithm_t algorithm)
{
  return algorithm <= HSM_PUBLIC_KEY_ALGORITHM_ECIES;
}

atlk_rc_t
hsm_emulator_ecc_private_key_import(void *context,
                          hsm_cell_index_t private_key_index,
                          const hsm_ecc_private_key_info_t *private_key_info,
                          const ecc_scalar_t *private_key)
{
  hsm_emulator_t *emulator = context;
  hsm_emulator_storage_cell_t cell = HSM_EMULATOR_STORAGE_CELL_INIT;
  ecc_point_t public_key = ECC_POINT_INIT;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!private_key_info || !private_key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (!ecc_curve_valid(private_key_info->key_curve)) {
    TR_ERROR("Invalid elliptic curve %d", private_key_info->key_curve);
    return ATLK_E_INVALID_ARG;
  }

  if (private_key_info->key_type != HSM_PRIVATE_KEY_TYPE_ISOLATED) {
    TR_ERROR("Invalid private key type %d", private_key_info->key_type);
    return ATLK_E_INVALID_ARG;
  }

  if (!hsm_emulator_public_key_algorithm_valid(private_key_info->key_algorithm)) {
    TR_ERROR("Invalid private key algorithm %d",
             private_key_info->key_algorithm);
    return ATLK_E_INVALID_ARG;
  }

  /* Derive public key */
  rc = ecc_derive_public_key(private_key_info->key_curve,
                             private_key,
                             &public_key);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to derive public key (rc=%d)", rc);
    return rc;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Make sure cell index is valid */
  if (private_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid cell index %" PRIu32, private_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Fill storage cell */
  cell.used = 1;
  cell.private_key_info = *private_key_info;
  cell.private_key = *private_key;
  cell.public_key = public_key;

  /* Write storage cell + flush to NVM */
  rc = emulator->storage->cell_write(private_key_index, &cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to write storage cell to index %" PRIu32 " (rc=%d)",
             private_key_index, rc);
    goto unlock;
  }

unlock:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return rc;
}
                           
atlk_rc_t
hsm_emulator_ecc_private_key_create(void *context,
                          hsm_cell_index_t private_key_index,
                          const hsm_ecc_private_key_info_t *private_key_info)
{
  hsm_emulator_t *emulator = context;
  hsm_emulator_storage_cell_t cell = HSM_EMULATOR_STORAGE_CELL_INIT;
  ecc_scalar_t private_key = ECC_SCALAR_INIT;
  ecc_point_t public_key = ECC_POINT_INIT;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!private_key_info) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_STATE;
  } 

  if (!ecc_curve_valid(private_key_info->key_curve)) {
    TR_ERROR("Invalid elliptic curve %d", private_key_info->key_curve);
    return ATLK_E_INVALID_ARG;
  }

  switch (private_key_info->key_type){
    case HSM_PRIVATE_KEY_TYPE_ISOLATED:
    case HSM_PRIVATE_KEY_TYPE_CSR_MEMBER:
    case HSM_PRIVATE_KEY_TYPE_CSR_SIGNER:
    case HSM_PRIVATE_KEY_TYPE_MA_INPUT:
      break;
    default:
      TR_ERROR("Invalid private key type %d", private_key_info->key_type);
      return ATLK_E_INVALID_ARG;
  }

  if (!hsm_emulator_public_key_algorithm_valid(private_key_info->key_algorithm)) {
    TR_ERROR("Invalid private key algorithm %d",
             private_key_info->key_algorithm);
    return ATLK_E_INVALID_ARG;
  }

  if (private_key_info->key_type == HSM_PRIVATE_KEY_TYPE_CSR_SIGNER) {
    if (ecc_curve_size(private_key_info->key_curve) <=
        ECC_CURVE_P224_KEY_SIZE) {
      TR_ERROR("Invalid elliptic curve %d for private key type %d",
        private_key_info->key_curve, private_key_info->key_type);
      return ATLK_E_INVALID_ARG;
    }

    if (private_key_info->key_algorithm != HSM_PUBLIC_KEY_ALGORITHM_ECDSA) {
      TR_ERROR("Invalid algorithm %d for private key type %d",
        private_key_info->key_algorithm, private_key_info->key_type);
      return ATLK_E_INVALID_ARG;
    }
  }

  /* Generate ECC key pair */
  rc = ecc_generate_key_pair(private_key_info->key_curve,
                             &private_key,
                             &public_key);
  if (atlk_error(rc)) {
    return rc;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Make sure cell index is valid */
  if (private_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid cell index %" PRIu32, private_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Fill storage cell */
  cell.used = 1;
  cell.private_key_info = *private_key_info;
  cell.private_key = private_key;
  cell.public_key = public_key;

  /* Write storage cell and flush to NVM */
  rc = emulator->storage->cell_write(private_key_index, &cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to write storage cell to index %" PRIu32 " (rc=%d)",
             private_key_index, rc);
    goto unlock;
  }

unlock:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return rc;
}

atlk_rc_t
hsm_emulator_ecc_private_key_info_get(void *context,
                            hsm_cell_index_t private_key_index,
                            hsm_ecc_private_key_info_t *private_key_info)
{
  hsm_emulator_t *emulator = context;
  hsm_emulator_storage_cell_t storage_cell;
  const hsm_emulator_storage_cell_t *cell;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!private_key_info) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Make sure cell index is valid */
  if (private_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid cell index %" PRIu32, private_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Get storage cell */
  rc = emulator->storage->cell_read(private_key_index, &storage_cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to read storage cell from index %" PRIu32 " (rc=%d)",
             private_key_index, rc);
    goto unlock;
  }

  /* Verify cell is not empty */
  cell = &storage_cell;
  if (!cell->used) {
    TR_ERROR("No private key at cell index %" PRIu32, private_key_index);
    rc = ATLK_E_NOT_FOUND;
    goto unlock;
  }

  /* Return the key information */
  *private_key_info = cell->private_key_info;

unlock:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return rc;
}

atlk_rc_t
hsm_emulator_ecc_public_key_get(void *context,
                      hsm_cell_index_t private_key_index,
                      ecc_point_t *public_key)
{
  hsm_emulator_t *emulator = context;
  hsm_emulator_storage_cell_t storage_cell;
  const hsm_emulator_storage_cell_t *cell;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!public_key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Make sure cell index is valid */
  if (private_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid cell index %" PRIu32, private_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Get storage cell */
  rc = emulator->storage->cell_read(private_key_index, &storage_cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to read storage cell from index %" PRIu32 " (rc=%d)",
             private_key_index, rc);
    goto unlock;
  }

  /* Verify cell is not empty */
  cell = &storage_cell;
  if (!cell->used) {
    TR_ERROR("No private key at cell index %" PRIu32, private_key_index);
    rc = ATLK_E_NOT_FOUND;
    goto unlock;
  }

  /* Return the public key */
  *public_key = cell->public_key;

unlock:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return rc;
}

#define HSM_EMULATOR_REQUEST_ID 1

static atlk_rc_t
hsm_emulator_ecdsa_sign_execute(hsm_emulator_t *emulator,
                        ecc_curve_t curve,
                        const ecc_scalar_t *private_key,
                        const sha_digest_t *digest,
                        ecc_fast_verification_signature_t *signature)
{
  ecc_request_t request = ECC_REQUEST_INIT;
  ecc_response_t response = ECC_RESPONSE_INIT;
  atlk_rc_t rc;

  /* Populate request */
  request.context.request_id = HSM_EMULATOR_REQUEST_ID;
  request.context.request_type = ECC_REQUEST_TYPE_SIGN;
  request.context.curve = curve;
  request.params.sign_params.private_key = *private_key;
  request.params.sign_params.digest = *digest;

  /* Send request */
  rc = ecc_request_send(emulator->ecc_socket, &request, NULL);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to send request (rc=%d)", rc);
    return rc;
  }

  /* Wait for response (blocking) */
  rc = ecc_response_receive(emulator->ecc_socket,
                            &response,
                            &atlk_wait_forever);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to receive response (rc=%d)", rc);
    return rc;
  }

  /* Return signature if valid */
  if (response.rc == ECC_OK) {
    *signature = response.result.sign_result;
  }
  else {
    TR_ERROR("Failed to ECDSA sign (rc=%d)", response.rc);
    return ATLK_E_UNSPECIFIED;
  }

  return ATLK_OK;
}

atlk_rc_t
hsm_emulator_ecdsa_sign(void *context,
              hsm_cell_index_t private_key_index,
              const sha_digest_t *digest,
              ecc_fast_verification_signature_t *signature)
{
  hsm_emulator_t *emulator = context;
  hsm_emulator_storage_cell_t storage_cell;
  const hsm_emulator_storage_cell_t *cell;
  const hsm_ecc_private_key_info_t *private_key_info;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!digest || !signature) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (digest->value_size > sizeof(digest->value)) {
    TR_ERROR("Invalid digest size %zu (> %zu)",
      digest->value_size, sizeof(digest->value));
    return ATLK_E_INVALID_ARG;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Make sure cell index is valid */
  if (private_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid cell index %" PRIu32, private_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Get storage cell and private key information */
  rc = emulator->storage->cell_read(private_key_index, &storage_cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to read storage cell from index %" PRIu32 " (rc=%d)",
             private_key_index, rc);
    goto unlock;
  }

  /* Verify cell is not empty */
  cell = &storage_cell;
  if (!cell->used) {
    TR_ERROR("No private key at cell index %" PRIu32, private_key_index);
    rc = ATLK_E_NOT_FOUND;
    goto unlock;
  }

  /* Verify key type */
  private_key_info = &cell->private_key_info;
  switch (private_key_info->key_type) {
    case HSM_PRIVATE_KEY_TYPE_ISOLATED:
    case HSM_PRIVATE_KEY_TYPE_CSR_MEMBER:
    case HSM_PRIVATE_KEY_TYPE_MA_OUTPUT:
      break;
    default:
      TR_ERROR("Private key at cell index %" PRIu32 " is of type %d",
        private_key_index, private_key_info->key_type);
      rc = ATLK_E_INVALID_ARG;
      goto unlock;
  }

  /* Verify key is intended for ECDSA */
  if (private_key_info->key_algorithm != HSM_PUBLIC_KEY_ALGORITHM_ECDSA) {
    TR_ERROR("Private key at cell index %" PRIu32 " is not intended for ECDSA",
      private_key_index);
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  rc = hsm_emulator_ecdsa_sign_execute(emulator,
                               private_key_info->key_curve,
                               &cell->private_key,
                               digest,
                               signature);
  if (atlk_error(rc)) {
    goto unlock;
  }

unlock:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return rc;
}

atlk_rc_t
hsm_emulator_ecies_key_derive(void *context,
                    hsm_cell_index_t private_key_index,
                    const ecc_point_t *peer_public_key,
                    void *key,
                    size_t key_size,
                    const void *kdf_param,
                    size_t kdf_param_size)
{
  hsm_emulator_t *emulator = context;
  hsm_emulator_storage_cell_t storage_cell;
  const hsm_emulator_storage_cell_t *cell;
  const hsm_ecc_private_key_info_t *private_key_info;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!peer_public_key || !key || !key_size) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (key_size > HSM_EMULATOR_MAX_ECIES_KEY_SIZE) {
    TR_ERROR("Unsupported key size %lu (> %u)",
      (unsigned long)key_size, HSM_EMULATOR_MAX_ECIES_KEY_SIZE);
    return ATLK_E_UNSUPPORTED;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Make sure cell index is valid */
  if (private_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid cell index %" PRIu32, private_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Get storage cell and private key information */
  rc = emulator->storage->cell_read(private_key_index, &storage_cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to read storage cell from index %" PRIu32 " (rc=%d)",
             private_key_index, rc);
    goto unlock;
  }

  /* Verify cell is not empty */
  cell = &storage_cell;
  if (!cell->used) {
    TR_ERROR("No private key at cell index %" PRIu32, private_key_index);
    rc = ATLK_E_NOT_FOUND;
    goto unlock;
  }

  /* Verify private key belongs to at least a 256-bit elliptic curve */
  private_key_info = &cell->private_key_info;
  if (ecc_curve_size(private_key_info->key_curve) <= ECC_CURVE_P224_KEY_SIZE) {
    TR_ERROR("Invalid elliptic curve %s",
             ecc_curve_name(private_key_info->key_curve));
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  /* Verify key type */
  switch (private_key_info->key_type) {
    case HSM_PRIVATE_KEY_TYPE_ISOLATED:
    case HSM_PRIVATE_KEY_TYPE_CSR_MEMBER:
    case HSM_PRIVATE_KEY_TYPE_MA_OUTPUT:
      break;
    default:
      TR_ERROR("Private key at cell index %" PRIu32 " is of type %d",
        private_key_index, private_key_info->key_type);
      rc = ATLK_E_INVALID_ARG;
      goto unlock;
  }

  /* Verify private key is intended for ECIES */
  if (private_key_info->key_algorithm != HSM_PUBLIC_KEY_ALGORITHM_ECIES) {
    TR_ERROR("Private key at cell index %" PRIu32 " is not intended for ECIES",
      private_key_index);
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  /* Derive ECIES key */
  rc = ecies_key_derive(private_key_info->key_curve,
                        &cell->private_key,
                        peer_public_key,
                        key,
                        key_size,
                        kdf_param,
                        kdf_param_size);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to derive ECIES key (rc=%d)", rc);
    goto unlock;
  }

unlock:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return rc;
}

atlk_rc_t
hsm_emulator_ecc_private_key_multiply_add(void *context,
                                hsm_cell_index_t input_key_index,
                                hsm_cell_index_t output_key_index,
                                const ecc_scalar_t *key_addend,
                                const ecc_scalar_t *key_multiplier)
{
  hsm_emulator_t *emulator = context;
  hsm_emulator_storage_cell_t input_storage_cell;
  const hsm_emulator_storage_cell_t *input_cell;
  hsm_emulator_storage_cell_t output_cell;
  const hsm_ecc_private_key_info_t *private_key_info;
  ecc_scalar_t private_key = ECC_SCALAR_INIT;
  ecc_point_t public_key = ECC_POINT_INIT;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!key_addend || !key_multiplier) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /*
   * We don't allow the input index be equal to the output index due to
   * the idempotency requirement; i.e. invoking a procedure once and invoking
   * the same procedure twice or more with the same inputs (without any other
   * intervening procedure invocations) should be indistinguishable to the user.
   */
  if (input_key_index == output_key_index) {
    TR_ERROR("Input key index %" PRIu32 " is equal to output key index",
      input_key_index);
    return ATLK_E_INVALID_ARG;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Make sure input cell index is valid */
  if (input_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid input cell index %" PRIu32, input_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Make sure output cell index is valid */
  if (output_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid output cell index %" PRIu32, output_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Get input storage cell and private key information */
  rc = emulator->storage->cell_read(input_key_index, &input_storage_cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to read storage cell from index %" PRIu32 " (rc=%d)",
             input_key_index, rc);
    goto unlock;
  }

  /* Verify input cell is not empty */
  input_cell = &input_storage_cell;
  if (!input_cell->used) {
    TR_ERROR("No private key at input cell index %" PRIu32, input_key_index);
    rc = ATLK_E_NOT_FOUND;
    goto unlock;
  }

  /* Verify key type */
  private_key_info = &input_cell->private_key_info;
  if (private_key_info->key_type != HSM_PRIVATE_KEY_TYPE_MA_INPUT) {
    TR_ERROR("Invalid type %d for key at input index %" PRIu32,
      private_key_info->key_type, input_key_index);
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  /*
   * Transform private key.
   * This function makes sure that the key multiplier is not a multiple
   * of the curve order.
   */
  rc = ecc_private_key_multiply_add(private_key_info->key_curve,
                                    &input_cell->private_key,
                                    key_addend,
                                    key_multiplier,
                                    &private_key);
  if (atlk_error(rc)) {
    goto unlock;
  }

  /* Derive public key */
  rc = ecc_derive_public_key(private_key_info->key_curve,
                             &private_key, &public_key);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to derive public key (rc=%d)", rc);
    goto unlock;
  }

  /* Fill storage entry */
  output_cell.used = 1;
  output_cell.private_key_info.key_curve = private_key_info->key_curve;
  output_cell.private_key_info.key_type = HSM_PRIVATE_KEY_TYPE_MA_OUTPUT;
  output_cell.private_key_info.key_algorithm = private_key_info->key_algorithm;
  output_cell.private_key = private_key;
  output_cell.public_key = public_key;

  /* Write storage cell and flush to NVM */
  rc = emulator->storage->cell_write(output_key_index, &output_cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to write storage cell to index %" PRIu32 " (rc=%d)",
             output_key_index, rc);
    goto unlock;
  }

unlock:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return rc;
}

/* Top 16 bytes of SHA-256 digest of '0x00' */
static const uint8_t hsm_emulator_csr_internal_prefix[] = {
  0x6e, 0x34, 0x0b, 0x9c, 0xff, 0xb3, 0x7a, 0x98,
  0x9c, 0xa5, 0x44, 0xe6, 0xbb, 0x78, 0x0a, 0x2c
};

/* Top 16 bytes of SHA-256 digest of '0x01' */
static const uint8_t hsm_emulator_csr_external_prefix[] = {
  0x4b, 0xf5, 0x12, 0x2f, 0x34, 0x45, 0x54, 0xc5,
  0x3b, 0xde, 0x2e, 0xbb, 0x8c, 0xd2, 0xb7, 0xe3,
};

atlk_rc_t
hsm_emulator_csr_ecdsa_external_sign(void *context,
                           hsm_cell_index_t private_key_index,
                           const sha_digest_t *digest,
                           hsm_csr_random_prefix_t *prefix,
                           ecc_fast_verification_signature_t *signature)
{
  hsm_emulator_t *emulator = context;
  hsm_emulator_storage_cell_t storage_cell;
  const hsm_emulator_storage_cell_t *cell;
  const hsm_ecc_private_key_info_t *private_key_info;
  sha_sw_state_t state = SHA_SW_STATE_INIT;
  sha_digest_t external_digest = SHA_DIGEST_INIT;
  sha_algorithm_t sha_algorithm;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!digest || !prefix || !signature) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (digest->value_size > sizeof(digest->value)) {
    TR_ERROR("Invalid digest size %zu (> %zu)",
      digest->value_size, sizeof(digest->value));
    return ATLK_E_INVALID_ARG;
  }

  /* 1. Get CSR random prefix */
  rc = rng_data_get(prefix->value, sizeof(prefix->value));
  if (atlk_error(rc)) {
    TR_ERROR("Failed to get %lu random bytes for CSR random prefix (rc=%d)",
      (unsigned long)sizeof(*prefix), rc);
    return rc;
  }

  /* 2. Compute CSR SHA external digest */

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Make sure cell index is valid */
  if (private_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid cell index %" PRIu32, private_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Get storage cell and private key information */
  rc = emulator->storage->cell_read(private_key_index, &storage_cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to read storage cell from index %" PRIu32 " (rc=%d)",
             private_key_index, rc);
    goto unlock;
  }

  /* Verify cell is not empty */
  cell = &storage_cell;
  if (!cell->used) {
    TR_ERROR("No private key at cell index %" PRIu32, private_key_index);
    rc = ATLK_E_NOT_FOUND;
    goto unlock;
  }

  /*
   * You might notice that part of the following checks are redundant since
   * it is enough just to check the key type (all the keys with type equal to
   * HSM_PRIVATE_KEY_TYPE_CSR_SIGNER are created with an over 256-bit elliptic
   * curve and ECDSA algorithm).
   * In any case, we avoid implicit assumptions and check all the necessary
   * conditions anyway.
   */

  /* Verify that private key belongs to at least a 256-bit elliptic curve */
  private_key_info = &cell->private_key_info;
  if (ecc_curve_size(private_key_info->key_curve) <= ECC_CURVE_P224_KEY_SIZE) {
    TR_ERROR("Invalid elliptic curve %s",
             ecc_curve_name(private_key_info->key_curve));
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  /* Verify key type */
  if (private_key_info->key_type != HSM_PRIVATE_KEY_TYPE_CSR_SIGNER) {
    TR_ERROR("Invalid key type %d for key at index %" PRIu32 " (expected %d)",
      private_key_info->key_type, private_key_index,
      HSM_PRIVATE_KEY_TYPE_CSR_SIGNER);
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  /* Verify key is intended for ECDSA */
  if (private_key_info->key_algorithm != HSM_PUBLIC_KEY_ALGORITHM_ECDSA) {
    TR_ERROR("Private key at cell index %" PRIu32 " is not intended for ECDSA",
      private_key_index);
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  /* Determine SHA algorithm according to the elliptic curve size */
  switch (ecc_curve_size(private_key_info->key_curve)) {
    case ECC_CURVE_P256_KEY_SIZE:
      sha_algorithm = SHA_256;
      break;
    case ECC_CURVE_P384_KEY_SIZE:
      sha_algorithm = SHA_384;
      break;
    default:
      BUG();
  }

  /* Initialize SHA state */
  rc = sha_sw_init(sha_algorithm, &state);
  BUG_ON(atlk_error(rc));

  /* CSR external prefix */
  rc = sha_sw_process(&state,
                      hsm_emulator_csr_external_prefix,
                      sizeof(hsm_emulator_csr_external_prefix));
  BUG_ON(atlk_error(rc));

  /* CSR random prefix */
  rc = sha_sw_process(&state, prefix, sizeof(*prefix));
  BUG_ON(atlk_error(rc));

  /* CSR message digest */
  rc = sha_sw_process(&state, digest->value, digest->value_size);
  BUG_ON(atlk_error(rc));

  /* Get CSR external digest */
  rc = sha_sw_finalize(&state, &external_digest);
  BUG_ON(atlk_error(rc));

  /* 3. ECDSA sign */
  rc = hsm_emulator_ecdsa_sign_execute(emulator,
                               private_key_info->key_curve,
                               &cell->private_key,
                               &external_digest,
                               signature);
  if (atlk_error(rc)) {
    goto unlock;
  }

unlock:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return rc;
}

atlk_rc_t
hsm_emulator_csr_ecdsa_public_keys_sign(void *context,
                              hsm_cell_index_t private_key_index,
                              const hsm_cell_range_t *range_array_ptr,
                              size_t range_array_size,
                              hsm_csr_random_prefix_t *prefix,
                              ecc_fast_verification_signature_t *signature)
{
  hsm_emulator_t *emulator = context;
  hsm_emulator_storage_cell_t private_key_cell;
  const hsm_emulator_storage_cell_t *cell;
  const hsm_ecc_private_key_info_t *private_key_info;
  sha_sw_state_t state = SHA_SW_STATE_INIT;
  sha_digest_t public_keys_digest = SHA_DIGEST_INIT;
  sha_digest_t internal_digest = SHA_DIGEST_INIT;
  sha_algorithm_t sha_algorithm;
  int i, j;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!range_array_ptr || !range_array_size ||
      !prefix || !signature) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (range_array_size > HSM_EMULATOR_MAX_CSR_CELL_RANGES) {
    TR_ERROR("Unsupported cell ranges array size %zu", range_array_size);
    return ATLK_E_UNSUPPORTED;
  }

  /* Lock storage */
  rc = osal_mutex_lock(&emulator->storage_lock);
  BUG_ON(atlk_error(rc));

  /* Make sure cell index is valid */
  if (private_key_index >= emulator->storage_num_cells) {
    TR_ERROR("Invalid cell index %" PRIu32, private_key_index);
    rc = ATLK_E_OUT_OF_BOUNDS;
    goto unlock;
  }

  /* Get storage cell and private key information */
  rc = emulator->storage->cell_read(private_key_index, &private_key_cell);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to read storage cell from index %" PRIu32 " (rc=%d)",
             private_key_index, rc);
    goto unlock;
  }

  /* Verify cell is not empty */
  cell = &private_key_cell;
  if (!cell->used) {
    TR_ERROR("No private key at cell index %" PRIu32, private_key_index);
    rc =  ATLK_E_NOT_FOUND;
    goto unlock;
  }

  /*
   * You might notice that part of the following checks are redundant since
   * it is enough just to check the key type (all the keys with type equal to
   * HSM_PRIVATE_KEY_TYPE_CSR_SIGNER are created with an over 256-bit elliptic
   * curve and ECDSA algorithm).
   * In any case, we avoid implicit assumptions and check all the necessary
   * conditions anyway.
   */

  /* Verify that private key belongs to at least a 256-bit elliptic curve */
  private_key_info = &cell->private_key_info;
  if (ecc_curve_size(private_key_info->key_curve) <= ECC_CURVE_P224_KEY_SIZE) {
    TR_ERROR("Invalid elliptic curve %s",
             ecc_curve_name(private_key_info->key_curve));
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  /* Verify key type */
  if (private_key_info->key_type != HSM_PRIVATE_KEY_TYPE_CSR_SIGNER) {
    TR_ERROR("Invalid key type %d for key at index %" PRIu32 " (expected %d)",
      private_key_info->key_type, private_key_index,
      HSM_PRIVATE_KEY_TYPE_CSR_SIGNER);
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  /* Verify key is intended for ECDSA */
  if (private_key_info->key_algorithm != HSM_PUBLIC_KEY_ALGORITHM_ECDSA) {
    TR_ERROR("Private key at cell index %" PRIu32 " is not intended for ECDSA",
      private_key_index);
    rc = ATLK_E_INVALID_ARG;
    goto unlock;
  }

  /* 1. Compute public keys SHA digest */

  /* Determine SHA algorithm according to the elliptic curve size */
  switch (ecc_curve_size(private_key_info->key_curve)) {
    case ECC_CURVE_P256_KEY_SIZE:
      sha_algorithm = SHA_256;
      break;
    case ECC_CURVE_P384_KEY_SIZE:
      sha_algorithm = SHA_384;
      break;
    default:
      BUG();
  }

  /* Initialize SHA state */
  rc = sha_sw_init(sha_algorithm, &state);
  BUG_ON(atlk_error(rc));

  for (i = 0; i < range_array_size; i++) {
    const hsm_cell_range_t *range = &range_array_ptr[i];

    for (j = 0; j < range->num_of_cells; j++) {
      hsm_emulator_storage_cell_t storage_cell = HSM_EMULATOR_STORAGE_CELL_INIT;
      hsm_cell_index_t index = range->first_cell_index + j;
      uint8_t coordinate[sizeof(ecc_scalar_t)] = { 0 };
      size_t u32_len;
      size_t size;

      /* Make sure cell index is valid */
      if (index >= emulator->storage_num_cells) {
        TR_ERROR("Invalid cell index %" PRIu32 " (range #%d)", index, i);
        rc = ATLK_E_OUT_OF_BOUNDS;
        goto unlock;
      }

      /* Get storage cell and private key information */
      rc = emulator->storage->cell_read(index, &storage_cell);
      if (atlk_error(rc)) {
        TR_ERROR("Failed to read storage cell from index %" PRIu32 " (rc=%d)",
                 index, rc);
        goto unlock;
      }

      /* Verify cell is not empty */
      cell = &storage_cell;
      if (!cell->used) {
        TR_ERROR("No private key at cell index %" PRIu32 " (range #%d)",
          index, i);
        rc =  ATLK_E_NOT_FOUND;
        goto unlock;
      }

      /* Verify key type */
      private_key_info = &cell->private_key_info;
      if (private_key_info->key_type != HSM_PRIVATE_KEY_TYPE_CSR_MEMBER) {
        TR_ERROR("Invalid key type %d for cell index %" PRIu32 "(expected %d)",
          private_key_info->key_type, index, HSM_PRIVATE_KEY_TYPE_CSR_MEMBER);
        rc = ATLK_E_INVALID_ARG;
        goto unlock;
      }

      u32_len = ecc_curve_to_u32_len(private_key_info->key_curve);
      size = ecc_curve_size(private_key_info->key_curve);

      /* Convert public key X coordinate to octet string */
      fmt_i2osp(cell->public_key.x_coordinate.value, coordinate, u32_len);

      /* Process X coordinate */
      rc = sha_sw_process(&state, coordinate, size);
      BUG_ON(atlk_error(rc));

      /* Convert public key Y coordinate to octet string */
      fmt_i2osp(cell->public_key.y_coordinate.value, coordinate, u32_len);

      /* Process Y coordinate */
      rc = sha_sw_process(&state, coordinate, size);
      BUG_ON(atlk_error(rc));
    } 
  }

  /* Get public keys SHA digest */
  rc = sha_sw_finalize(&state, &public_keys_digest);
  BUG_ON(atlk_error(rc));

  /* 2. Get CSR random prefix */
  rc = rng_data_get(prefix, sizeof(*prefix));
  if (atlk_error(rc)) {
    TR_ERROR("Failed to get %lu random bytes for CSR random prefix (rc=%d)",
      (unsigned long)sizeof(*prefix), rc);
    goto unlock;
  }

  /* 3. Compute CSR SHA internal digest */

  /* Initialize SHA state */
  rc = sha_sw_init(sha_algorithm, &state);
  BUG_ON(atlk_error(rc));

  /* CSR internal prefix */
  rc = sha_sw_process(&state,
                      hsm_emulator_csr_internal_prefix,
                      sizeof(hsm_emulator_csr_internal_prefix));
  BUG_ON(atlk_error(rc));

  /* CSR random prefix */
  rc = sha_sw_process(&state, prefix, sizeof(*prefix));
  BUG_ON(atlk_error(rc));

  /* CSR public keys digest */
  rc = sha_sw_process(&state, public_keys_digest.value,
                      public_keys_digest.value_size);
  BUG_ON(atlk_error(rc));

  /* Get CSR SHA internal digest */
  rc = sha_sw_finalize(&state, &internal_digest);
  BUG_ON(atlk_error(rc));

  /* 4. ECDSA sign */

  cell = &private_key_cell;
  private_key_info = &cell->private_key_info;

  rc = hsm_emulator_ecdsa_sign_execute(emulator,
                               private_key_info->key_curve,
                               &cell->private_key,
                               &internal_digest,
                               signature);
  if (atlk_error(rc)) {
    goto unlock;
  }

unlock:
  /* Unlock storage */
  osal_mutex_unlock(&emulator->storage_lock);

  return rc;
}

atlk_rc_t
hsm_emulator_host_nvm_aes_cbc_encrypt(void *context,
                                      const void *plaintext,
                                      size_t plaintext_size,
                                      aes_cbc_iv_t *iv,
                                      void *ciphertext,
                                      size_t *ciphertext_size)
{
  hsm_emulator_t *emulator = context;
  atlk_rc_t rc;

  /* Validate arguments */
  BUG_ON(!emulator);

  if (!iv) {
    TR_ERROR("AES-CBC initialization vector is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Get random AES-CBC initialization vector  */
  rc = rng_data_get(iv->value, sizeof(iv->value));
  if (atlk_error(rc)) {
    return rc;
  }

  /* Encrypt with AES-CBC */
  rc = aes_cbc_encrypt(&emulator->host_nvm_encryption_key, iv,
                       plaintext, plaintext_size,
                       ciphertext, ciphertext_size);
  if (atlk_error(rc)) {
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t
hsm_emulator_host_nvm_aes_cbc_decrypt(void *context,
                                      const aes_cbc_iv_t *iv,
                                      const void *ciphertext,
                                      size_t ciphertext_size,
                                      void *plaintext,
                                      size_t *plaintext_size)
{
  hsm_emulator_t *emulator = context;
  atlk_rc_t rc;

  /* Validate arguments */
  BUG_ON(!emulator);

  /* Decrypt with AES-CBC */
  rc = aes_cbc_decrypt(&emulator->host_nvm_encryption_key, iv,
                       ciphertext, ciphertext_size,
                       plaintext, plaintext_size);
  if (atlk_error(rc)) {
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t
hsm_emulator_host_nvm_aes_cmac_compute(void *context,
                                       const void *message,
                                       size_t message_size,
                                       aes_cmac_tag_t *tag)
{
  hsm_emulator_t *emulator = context;
  atlk_rc_t rc;

  /* Validate arguments */
  BUG_ON(!emulator);

  /* Compute AES-CMAC  */
  rc = aes_cmac_compute(&emulator->host_nvm_authentication_key,
                        message, message_size, tag);
  if (atlk_error(rc)) {
    return rc;
  }

  return ATLK_OK;
}

static void
hsm_emulator_close(hsm_emulator_t *emulator)
{
  osal_mutex_delete(&emulator->storage_lock);
  ecc_socket_delete(emulator->ecc_socket);
  (void)memset(emulator, 0, sizeof(*emulator));
}

static atlk_rc_t
hsm_emulator_init(const hsm_emulator_config_t *config, hsm_emulator_t *emulator)
{
  atlk_rc_t rc = ATLK_OK;

  /* Initialize HSM emulator with default values */
  (void)memset(emulator, 0, sizeof(*emulator));

  /* Use ECC service from user configuration */
  emulator->ecc_service = config->ecc_service_ptr;

  /* Initialize filename */
  emulator->nvm_file_path = config->nvm_file_path;

  /* Initialize AES-128 keys */
  emulator->host_nvm_authentication_key = config->host_nvm_authentication_key;
  emulator->host_nvm_encryption_key = config->host_nvm_encryption_key;

  /* Create ECC socket */
  rc = ecc_socket_create(emulator->ecc_service,
                         &emulator->ecc_socket);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to create ECC socket (rc=%d)", rc);
    goto out;
  }

  /* Create HSM emulator storage lock */
  osal_mutex_create(&emulator->storage_lock,
                    "HSM emulator storage lock");

  /* Create the storage - RAM or FILE based */
  if (emulator->nvm_file_path != NULL) {
    rc = hsm_emulator_storage_file_vtable_get(&emulator->storage);
    if (atlk_error(rc)) {
      TR_ERROR("Failed to get storage file vtable (rc=%d)", rc);
      goto destroy;
    }
  }
  else {
    rc = hsm_emulator_storage_ram_vtable_get(&emulator->storage);
    if (atlk_error(rc)) {
      TR_ERROR("Failed to get storage ram vtable (rc=%d)", rc);
      goto destroy;
    }
  }

  /* Initialize the storage, Set to auto-detect size */
  emulator->storage_num_cells = 0;
  rc = emulator->storage->init(emulator->nvm_file_path,
                                            &emulator->storage_num_cells);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to create HSM emulator storage object (rc=%d)", rc);
    goto destroy;
  }

  return ATLK_OK;

destroy:
  osal_mutex_delete(&emulator->storage_lock);
  ecc_socket_delete(emulator->ecc_socket);
out:
  return rc;
}

static void
hsm_emulator_service_init(hsm_service_t *service, hsm_emulator_t *emulator)
{
  service->delete = &hsm_emulator_service_delete;
  service->capability_info_get = &hsm_emulator_capability_info_get;
  service->nvm_init = &hsm_emulator_nvm_init;
  service->ecc_private_key_import = &hsm_emulator_ecc_private_key_import;
  service->ecc_private_key_create = &hsm_emulator_ecc_private_key_create;
  service->ecc_private_key_info_get = &hsm_emulator_ecc_private_key_info_get;
  service->ecc_public_key_get = &hsm_emulator_ecc_public_key_get;
  service->ecdsa_sign = &hsm_emulator_ecdsa_sign;
  service->ecies_key_derive = &hsm_emulator_ecies_key_derive;
  service->ecc_private_key_multiply_add = &hsm_emulator_ecc_private_key_multiply_add;
  service->csr_ecdsa_external_sign = &hsm_emulator_csr_ecdsa_external_sign;
  service->csr_ecdsa_public_keys_sign = &hsm_emulator_csr_ecdsa_public_keys_sign;
  service->host_nvm_aes_cbc_encrypt = &hsm_emulator_host_nvm_aes_cbc_encrypt;
  service->host_nvm_aes_cbc_decrypt = &hsm_emulator_host_nvm_aes_cbc_decrypt;
  service->host_nvm_aes_cmac_compute= &hsm_emulator_host_nvm_aes_cmac_compute;
  service->context = emulator;
}

atlk_rc_t
hsm_emulator_create(const hsm_emulator_config_t *config,
                    hsm_service_t **service_ptr)
{
  hsm_service_t *service = NULL;
  hsm_emulator_t *emulator = NULL;
  atlk_rc_t rc = ATLK_OK;

  /* Check function arguments */
  if (!service_ptr) {
    TR_ERROR_NO_ARGS("HSM emulator service is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (!config) {
    TR_ERROR_NO_ARGS("HSM emulator configuration was not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (!config->ecc_service_ptr) {
    TR_ERROR_NO_ARGS("ECC service was not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Allocate HSM emulator */
  emulator = malloc(sizeof(*emulator));
  if (!emulator) {
    TR_ERROR("Failed to allocate %zu bytes for HSM emulator",
      sizeof(*emulator));
    return ATLK_E_OUT_OF_MEMORY;
  }

  /* Initialize HSM emulator (cannot fail) */
  rc = hsm_emulator_init(config, emulator);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to create HSM emulator (rc=%d)", rc);
    goto error;
  }

  service = malloc(sizeof(*service));
  if (!service) {
     TR_ERROR("Failed to allocate %zu bytes for HSM emulator",
      sizeof(*emulator));
    rc = ATLK_E_OUT_OF_MEMORY;
    goto close;
  }

  hsm_emulator_service_init(service, emulator);
  emulator->hsm_service = service;

  *service_ptr = service;

  return ATLK_OK;

close:
  hsm_emulator_close(emulator);
error:
  free(emulator);
  free(service);
  return rc;
}

