/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _CRYPTO_ECC_CURVES_H
#define _CRYPTO_ECC_CURVES_H

#include <atlk/sdk.h>
#include <atlk/ecc.h>

atlk_inline int
ecc_curve_valid(ecc_curve_t curve)
{
  switch (curve) {
    case ECC_CURVE_NIST_P224:
    case ECC_CURVE_NIST_P256:
    case ECC_CURVE_BRAINPOOL_P256t1:
    case ECC_CURVE_BRAINPOOL_P256r1:
    case ECC_CURVE_NIST_P384:
    case ECC_CURVE_BRAINPOOL_P384t1:
    case ECC_CURVE_BRAINPOOL_P384r1:
      return 1;
    default:
      return 0;
  }
}

atlk_inline size_t
ecc_curve_size(ecc_curve_t curve)
{
  switch (curve) {
    case ECC_CURVE_NIST_P224:
      return ECC_CURVE_P224_KEY_SIZE;
    case ECC_CURVE_NIST_P256:
    case ECC_CURVE_BRAINPOOL_P256t1:
    case ECC_CURVE_BRAINPOOL_P256r1:
      return ECC_CURVE_P256_KEY_SIZE;
    case ECC_CURVE_NIST_P384:
    case ECC_CURVE_BRAINPOOL_P384t1:
    case ECC_CURVE_BRAINPOOL_P384r1:
      return ECC_CURVE_P384_KEY_SIZE;
    default:
      return 0;
  }
}

atlk_inline const char *
ecc_curve_name(ecc_curve_t curve)
{
  switch (curve) {
    case ECC_CURVE_NIST_P224:
      return "NIST P-224";
    case ECC_CURVE_NIST_P256:
      return "NIST P-256";
    case ECC_CURVE_BRAINPOOL_P256t1:
      return "Brainpool P-256t1";
    case ECC_CURVE_BRAINPOOL_P256r1:
      return "Brainpool P-256r1";
    case ECC_CURVE_NIST_P384:
      return "NIST P-384";
    case ECC_CURVE_BRAINPOOL_P384t1:
      return "Brainpool P-384t1";
    case ECC_CURVE_BRAINPOOL_P384r1:
      return "Brainpool P-384r1"; 
    default:
      return "N/A";
  }
}

atlk_inline size_t
ecc_curve_to_u32_len(ecc_curve_t curve)
{
  switch (curve) {
    case ECC_CURVE_NIST_P224:
      return ECC_CURVE_P224_KEY_SIZE / sizeof(uint32_t);
    case ECC_CURVE_NIST_P256:
    case ECC_CURVE_BRAINPOOL_P256t1:
    case ECC_CURVE_BRAINPOOL_P256r1:
      return ECC_CURVE_P256_KEY_SIZE / sizeof(uint32_t);
    case ECC_CURVE_NIST_P384:
    case ECC_CURVE_BRAINPOOL_P384t1:
    case ECC_CURVE_BRAINPOOL_P384r1:
      return ECC_CURVE_P384_KEY_SIZE / sizeof(uint32_t);
    default:
      return 0;
  }
}

#endif /* _CRYPTO_ECC_CURVES_H */
