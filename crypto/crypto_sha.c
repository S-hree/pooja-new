#include <tommath.h>
#include <tomcrypt.h>

#include <atlk/sdk.h>
#include <atlk/sha.h>
#include <atlk/sha_sw.h>

#include <platform/common.h>

#include <crypto/crypto_init.h>
#include <crypto/crypto_sha.h>
#include <crypto/crypto_error.h>

atlk_rc_t
sha_sw_init(sha_algorithm_t algorithm,
            sha_sw_state_t *state)
{
  int err;

  /* Check function arguments */
  if (!state) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (!sha_algorithm_valid(algorithm)) {
    TR_ERROR("Invalid SHA algorithm %d", algorithm);
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialization */
  crypto_init();

  /* Find the hash */
  state->hash = find_hash(sha_algorithm_name(algorithm));
  BUG_ON(state->hash == -1);

  /* Initialize hash state (should not fail) */
  err = hash_descriptor[state->hash].init(&state->state);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to initialize hash state: %s", error_to_string(err));
    BUG();
  }

  return ATLK_OK;
}

atlk_rc_t
sha_sw_sha224_init(sha_sw_state_t *state)
{
  return sha_sw_init(SHA_224, state);
}

atlk_rc_t
sha_sw_sha256_init(sha_sw_state_t *state)
{
  return sha_sw_init(SHA_256, state);
}

atlk_rc_t
sha_sw_sha384_init(sha_sw_state_t *state)
{
  return sha_sw_init(SHA_384, state);
}

atlk_rc_t
sha_sw_process(sha_sw_state_t *state,
               const void *data_ptr,
               size_t data_size)
{
  int err;

  /* Check function arguments */
  if (!state || !data_ptr) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (state->hash < 0) {
    TR_ERROR_NO_ARGS("Uninitialized SHA state");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialization */
  crypto_init();

  /* Process hash (should not fail) */
  err = hash_descriptor[state->hash].process(&state->state,
                                             data_ptr,
                                             data_size);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to process hash: %s", error_to_string(err));
    BUG();
  }

  return ATLK_OK;
}

atlk_rc_t
sha_sw_finalize(sha_sw_state_t *state,
                sha_digest_t *digest)
{
  int err;

  /* Check function arguments */
  if (!state || !digest) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (state->hash < 0) {
    TR_ERROR_NO_ARGS("Uninitialized SHA state");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialization */
  crypto_init();

  /* Initialize digest */
  *digest = (sha_digest_t)SHA_DIGEST_INIT;

  /* Get hash digest (should not fail) */
  err = hash_descriptor[state->hash].done(&state->state, digest->value);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to finish hash: %s", error_to_string(err));
    BUG();
  }

  /* Save hash digest size */
  digest->value_size = hash_descriptor[state->hash].hashsize;

  return ATLK_OK;
}

atlk_rc_t
sha_sw_compute(sha_algorithm_t algorithm,
               const void *data_ptr,
               size_t data_size,
               sha_digest_t *digest)
{
  int hash;
  unsigned long len = sizeof(digest->value);
  int err;

  /* Check function arguments */
  if (!data_ptr || !digest) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (!sha_algorithm_valid(algorithm)) {
    TR_ERROR("Invalid SHA algorithm %d", algorithm);
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialization */
  crypto_init();

  /* Find the hash */
  hash = find_hash(sha_algorithm_name(algorithm));
  BUG_ON(hash == -1);

  /* Initialize digest */
  *digest = (sha_digest_t)SHA_DIGEST_INIT;

  /* Calculate hash */
  err = hash_memory(hash, data_ptr, data_size, digest->value, &len);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to calculate hash: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  /* Save hash digest size */
  digest->value_size = hash_descriptor[hash].hashsize;

  return ATLK_OK;
}

atlk_rc_t
sha_sw_sha224_compute(const void *data_ptr,
                      size_t data_size,
                      sha_digest_t *digest)
{
  return sha_sw_compute(SHA_224, data_ptr, data_size, digest);
}

atlk_rc_t
sha_sw_sha256_compute(const void *data_ptr,
                      size_t data_size,
                      sha_digest_t *digest)
{
  return sha_sw_compute(SHA_256, data_ptr, data_size, digest);
}

atlk_rc_t
sha_sw_sha384_compute(const void *data_ptr,
                      size_t data_size,
                      sha_digest_t *digest)
{
  return sha_sw_compute(SHA_384, data_ptr, data_size, digest);
}

