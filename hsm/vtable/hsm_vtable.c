#include <atlk/sdk.h>
#include <platform/common.h>

#include "hsm_vtable.h"

#define HSM_SERVICE_VALIDATE(_service) do {             \
  if (!_service) {                                      \
    TR_ERROR_NO_ARGS("HSM service is not specified");   \
    return ATLK_E_INVALID_ARG;                          \
  }                                                     \
} while (0)

atlk_rc_t
hsm_service_delete(hsm_service_t *service)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->delete);
  return service->delete(service->context); 
}

atlk_rc_t
hsm_capability_info_get(hsm_service_t *service,
                        hsm_capability_info_t *capability_info)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->capability_info_get);
  return service->capability_info_get(service->context, capability_info);
}

atlk_rc_t
hsm_nvm_init(hsm_service_t *service,
             const hsm_nvm_config_t *config)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->nvm_init);
  return service->nvm_init(service->context, config);
}

atlk_rc_t
hsm_ecc_private_key_import(hsm_service_t *service,
                           hsm_cell_index_t private_key_index,
                           const hsm_ecc_private_key_info_t *private_key_info,
                           const ecc_scalar_t *private_key)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->ecc_private_key_import);
  return service->ecc_private_key_import(service->context,
    private_key_index, private_key_info, private_key);
}

atlk_rc_t
hsm_ecc_private_key_create(hsm_service_t *service,
                           hsm_cell_index_t private_key_index,
                           const hsm_ecc_private_key_info_t *private_key_info)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->ecc_private_key_create);
  return service->ecc_private_key_create(service->context,
    private_key_index, private_key_info);
}

atlk_rc_t
hsm_ecc_private_key_info_get(hsm_service_t *service,
                             hsm_cell_index_t private_key_index,
                             hsm_ecc_private_key_info_t *private_key_info)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->ecc_private_key_info_get);
  return service->ecc_private_key_info_get(service->context,
    private_key_index, private_key_info);
}

atlk_rc_t
hsm_ecc_public_key_get(hsm_service_t *service,
                       hsm_cell_index_t private_key_index,
                       ecc_point_t *public_key)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->ecc_public_key_get);
  return service->ecc_public_key_get(service->context,
    private_key_index, public_key);
}

atlk_rc_t
hsm_ecdsa_sign(hsm_service_t *service,
               hsm_cell_index_t private_key_index,
               const sha_digest_t *digest,
               ecc_fast_verification_signature_t *signature)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->ecdsa_sign);
  return service->ecdsa_sign(service->context,
    private_key_index, digest, signature);
}

atlk_rc_t
hsm_ecies_key_derive(hsm_service_t *service,
            hsm_cell_index_t private_key_index,
            const ecc_point_t *peer_public_key,
            void *key,
            size_t key_size,
            const void *kdf_param,
            size_t kdf_param_size)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->ecies_key_derive);
  return service->ecies_key_derive(service->context,
    private_key_index, peer_public_key, key, key_size,
    kdf_param, kdf_param_size);
}

atlk_rc_t
hsm_ecc_private_key_multiply_add(hsm_service_t *service,
            hsm_cell_index_t input_key_index,
            hsm_cell_index_t output_key_index,
            const ecc_scalar_t *key_addend,
            const ecc_scalar_t *key_multiplier)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->ecc_private_key_multiply_add);
  return service->ecc_private_key_multiply_add(service->context,
    input_key_index, output_key_index, key_addend, key_multiplier);
}

atlk_rc_t
hsm_csr_ecdsa_external_sign(hsm_service_t *service,
                            hsm_cell_index_t private_key_index,
                            const sha_digest_t *digest,
                            hsm_csr_random_prefix_t *prefix,
                            ecc_fast_verification_signature_t *signature)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->csr_ecdsa_external_sign);
  return service->csr_ecdsa_external_sign(service->context,
    private_key_index, digest, prefix, signature);
}

atlk_rc_t
hsm_csr_ecdsa_public_keys_sign(hsm_service_t *service,
                               hsm_cell_index_t private_key_index,
                               const hsm_cell_range_t *range_array_ptr,
                               size_t range_array_size,
                               hsm_csr_random_prefix_t *prefix,
                               ecc_fast_verification_signature_t *signature)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->csr_ecdsa_public_keys_sign);
  return service->csr_ecdsa_public_keys_sign(service->context,
    private_key_index, range_array_ptr, range_array_size, prefix, signature);
}

atlk_rc_t
hsm_host_nvm_aes_cbc_encrypt(hsm_service_t *service,
            const void *plaintext,
            size_t plaintext_size,
            aes_cbc_iv_t *iv,
            void *ciphertext,
            size_t *ciphertext_size)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->host_nvm_aes_cbc_encrypt);
  return service->host_nvm_aes_cbc_encrypt(service->context,
    plaintext, plaintext_size, iv, ciphertext, ciphertext_size);
}

atlk_rc_t
hsm_host_nvm_aes_cbc_decrypt(hsm_service_t *service,
            const aes_cbc_iv_t *iv,
            const void *ciphertext,
            size_t ciphertext_size,
            void *plaintext,
            size_t *plaintext_size)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->host_nvm_aes_cbc_decrypt);
  return service->host_nvm_aes_cbc_decrypt(service->context,
    iv, ciphertext, ciphertext_size, plaintext, plaintext_size);
}

atlk_rc_t
hsm_host_nvm_aes_cmac_compute(hsm_service_t *service,
            const void *message,
            size_t message_size,
            aes_cmac_tag_t *tag)
{
  HSM_SERVICE_VALIDATE(service);

  BUG_ON(!service->host_nvm_aes_cmac_compute);
  return service->host_nvm_aes_cmac_compute(service->context,
    message, message_size, tag);
}

