#ifndef _HSM_VTABLE_H
#define _HSM_VTABLE_H

#include <atlk/hsm_service.h>

/** HSM service descriptor */
struct hsm_service {
  atlk_rc_t (*delete)(void *);

  atlk_rc_t (*capability_info_get)(void *, hsm_capability_info_t *);

  atlk_rc_t (*nvm_init)(void *, const hsm_nvm_config_t *);

  atlk_rc_t (*ecc_private_key_import)(void *, hsm_cell_index_t,
    const hsm_ecc_private_key_info_t *, const ecc_scalar_t *);

  atlk_rc_t (*ecc_private_key_create)(void *, hsm_cell_index_t,
    const hsm_ecc_private_key_info_t *);

  atlk_rc_t (*ecc_private_key_info_get)(void *, hsm_cell_index_t,
    hsm_ecc_private_key_info_t *);

  atlk_rc_t (*ecc_public_key_get)(void *, hsm_cell_index_t ,
    ecc_point_t *);

  atlk_rc_t (*ecdsa_sign)(void *, hsm_cell_index_t,
   const sha_digest_t *, ecc_fast_verification_signature_t *);

  atlk_rc_t (*ecies_key_derive)(void *, hsm_cell_index_t,
    const ecc_point_t *, void *, size_t, const void *, size_t);

  atlk_rc_t (*ecc_private_key_multiply_add)(void *, hsm_cell_index_t,
    hsm_cell_index_t, const ecc_scalar_t *, const ecc_scalar_t *);

  atlk_rc_t (*csr_ecdsa_external_sign)(void *, hsm_cell_index_t,
    const sha_digest_t *, hsm_csr_random_prefix_t *,
    ecc_fast_verification_signature_t *);

  atlk_rc_t (*csr_ecdsa_public_keys_sign)(void *, hsm_cell_index_t,
    const hsm_cell_range_t *, size_t, hsm_csr_random_prefix_t *,
    ecc_fast_verification_signature_t *);

  atlk_rc_t (*host_nvm_aes_cbc_encrypt)(void *, const void *, size_t,
    aes_cbc_iv_t *, void *, size_t *);

  atlk_rc_t (*host_nvm_aes_cbc_decrypt)(void *, const aes_cbc_iv_t *,
    const void *, size_t, void *, size_t *);

  atlk_rc_t (*host_nvm_aes_cmac_compute)(void *, const void *, size_t,
    aes_cmac_tag_t *tag);

  void *context;
};

#endif /* _HSM_VTABLE_H */
