/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _CRYPTO_CRYPTO_ECC
#define _CRYPTO_CRYPTO_ECC

#include <atlk/sdk.h>
#include <atlk/sha.h>
#include <atlk/ecc.h>
#include <atlk/ecdsa.h>

#include <platform/common.h>

int
ecc_private_key_valid(ecc_curve_t curve, const ecc_scalar_t *private_key);

int
ecc_key_pair_valid(ecc_curve_t curve,
                   const ecc_scalar_t *private_key,
                   const ecc_point_t *public_key);

atlk_rc_t
ecc_point_add(ecc_curve_t curve,
              const ecc_point_t *P,
              const ecc_point_t *R,
              ecc_point_t *Q);

atlk_rc_t
ecc_generate_private_key(ecc_curve_t curve,
                         ecc_scalar_t *private_key);

atlk_rc_t
ecc_generate_key_pair(ecc_curve_t curve,
                      ecc_scalar_t *private_key,
                      ecc_point_t *public_key);

atlk_rc_t
ecc_derive_public_key(ecc_curve_t curve,
                      const ecc_scalar_t *private_key,
                      ecc_point_t *public_key);

atlk_rc_t
ecies_key_derive(ecc_curve_t curve,
                 const ecc_scalar_t *private_key,
                 const ecc_point_t *peer_public_key,
                 void *key,
                 size_t key_size,
                 const void *kdf_param,
                 size_t kdf_param_size);

#endif /* _CRYPTO_CRYPTO_ECC */
