/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _CRYPTO_CRYPTO_KDF
#define _CRYPTO_CRYPTO_KDF

#include <atlk/sdk.h>
#include <atlk/sha.h>

/** Maximum supported shared secret size for key derivation function */
#define KDF2_MAX_SECRET_SIZE 128

/** Maximum supported parameter size for key derivation function */
#define KDF2_MAX_PARAM_SIZE 64

/**
   KDF2 implementation based on IEEE Std 1363a-2004 (clause 13.2)

   @param[in] algorithm SHA algorithm
   @param[in] secret Shared secret
   @param[in] secret_len Shared secret length in bytes
   @param[in] param Key derivation parameter (optional)
   @param[in] param_size Key derivation parameter size in octets
   @param[out] output Shared key
   @param[in] output_len Shared key length in bytes

   @retval ::ATLK_OK if succeeded
   @return Error code if failed   
*/
atlk_rc_t atlk_must_check
kdf2_compute(sha_algorithm_t algorithm,
             const void *secret,
             size_t secret_len,
             const void *param,
             size_t param_len,
             uint8_t *output,
             size_t output_len);

#endif /* _CRYPTO_CRYPTO_KDF */
