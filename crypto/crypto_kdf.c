#include <tommath.h>
#include <tomcrypt.h>

#include <atlk/sdk.h>
#include <atlk/sha.h>

#include <platform/common.h>

#include <crypto/crypto_fmt.h>
#include <crypto/crypto_init.h>
#include <crypto/crypto_sha.h>
#include <crypto/crypto_kdf.h>

/* Reference: IEEE Std 1363a-2004, section 13.2. */
atlk_rc_t
kdf2_compute(sha_algorithm_t algorithm,
             const void *secret,
             size_t secret_len,
             const void *param,
             size_t param_len,
             uint8_t *output,
             size_t output_len)
{
  size_t threshold;
  uint8_t *counter_pos;
  uint32_t counter;
  uint8_t local[KDF2_MAX_SECRET_SIZE + \
                sizeof(counter) +             \
                KDF2_MAX_PARAM_SIZE] = { 0 };

  /* Validate arguments */
  if (!secret || !secret_len || !output || !output_len) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (secret_len > KDF2_MAX_SECRET_SIZE) {
    TR_ERROR("Unsupported shared secret length %zu", secret_len);
    return ATLK_E_UNSUPPORTED;
  }

  if (param_len > KDF2_MAX_PARAM_SIZE) {
    TR_ERROR("Unsupported key derivation parameter length %zu", param_len);
    return ATLK_E_UNSUPPORTED;
  }

  if (!param && param_len) {
    TR_ERROR("Key derivation parameter is not specified but length %zu > 0",
      param_len);
    return ATLK_E_INVALID_ARG;
  }

  if (!sha_algorithm_valid(algorithm)) {
    TR_ERROR("Invalid SHA algorithm %d", algorithm);
    return ATLK_E_INVALID_ARG;
  }

  threshold = DIV_ROUND_UP(output_len, sha_algorithm_digest_size(algorithm));
  memcpy(local, secret, secret_len);
  counter_pos = &local[secret_len];

  if (param) {
    memcpy(&local[secret_len + sizeof(counter)], param, param_len);
  }

  for (counter = 1; counter <= threshold; counter++) {
    sha_digest_t digest = SHA_DIGEST_INIT;
    size_t size;
    atlk_rc_t rc;

    /* Set counter */
    fmt_i2osp(&counter, counter_pos, 1);

    /* Compute hash digest */
    rc = sha_sw_compute(algorithm,
                        local,
                        secret_len + sizeof(counter) + param_len,
                        &digest);
    if (atlk_error(rc)) {
      return rc;
    }

    /* Copy digest to output */
    size = min(sha_algorithm_digest_size(algorithm), output_len);
    memcpy(output, digest.value, size);

    /* Update */
    output += size;
    output_len -= size;
  }

  return ATLK_OK;
}

