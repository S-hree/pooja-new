#include <tommath.h>
#include <tomcrypt.h>

#include <atlk/sdk.h>
#include <atlk/ecies.h>

#include <platform/common.h>

#include <crypto/ecc_curves.h>
#include <crypto/crypto_init.h>
#include <crypto/crypto_sha.h>
#include <crypto/crypto_error.h>

atlk_rc_t
ecies_encrypt(sha_algorithm_t sha_algorithm,
              const void *key,
              size_t key_size,
              const void *plaintext,
              size_t plaintext_size,
              void *ciphertext,
              size_t *ciphertext_size,
              ecies_authentication_tag_t *tag)
{
  unsigned long tag_len;
  const uint8_t *key_ptr;
  const uint8_t *plaintext_ptr;
  uint8_t *ciphertext_ptr;
  int i;
  int hash;
  int err;

  /* Validate arguments */
  if (!key || !key_size || !plaintext || !plaintext_size ||
      !ciphertext || !ciphertext_size || !tag) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (!sha_algorithm_valid(sha_algorithm)) {
    TR_ERROR("Invalid SHA algorithm %d", sha_algorithm);
    return ATLK_E_INVALID_ARG;
  }

  /* In IEEE Std 1609.2-2016, ECIES is only used to encrypt AES symmetic keys */
  if (plaintext_size > ECIES_MAX_TEXT_SIZE) {
    TR_ERROR("Unsupported plaintext size %zu", plaintext_size);
    return ATLK_E_UNSUPPORTED;
  }

  if (*ciphertext_size < plaintext_size) {
    TR_ERROR("Ciphertext size %zu smaller than plaintext size %zu",
      *ciphertext_size, plaintext_size);
    return ATLK_E_INVALID_ARG;
  }

  /*
   * Make sure HMAC key size is large enough.
   * Reference: IEEE Std. 1363a-2004, section 14.4.1.
   */
  if (key_size < plaintext_size + sha_algorithm_digest_size(sha_algorithm)/2) {
    TR_ERROR("ECIES key size (%zu) must be greater or equal to the addition "
             "of the plaintext size (%zu) and half the hash digest size (%zu)",
             key_size,
             plaintext_size,
             sha_algorithm_digest_size(sha_algorithm)/2);
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Encrypt */
  key_ptr = key;
  plaintext_ptr = plaintext;
  ciphertext_ptr = ciphertext;
  for (i = 0; i < plaintext_size; i++) {
    ciphertext_ptr[i] = plaintext_ptr[i] ^ key_ptr[i];
  }

  *ciphertext_size = plaintext_size;

  /* Find hash */
  hash = find_hash(sha_algorithm_name(sha_algorithm));
  BUG_ON(hash == -1);

  /* Compute message authentication code */
  tag_len = sizeof(*tag);
  err = hmac_memory(hash,
                    key_ptr + plaintext_size, key_size - plaintext_size,
                    ciphertext, *ciphertext_size,
                    tag->value, &tag_len);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to compute message authentication code: %s",
      error_to_string(err));
    return crypto_error_to_rc(err);
  }

  return ATLK_OK;
}

atlk_rc_t
ecies_decrypt(sha_algorithm_t sha_algorithm,
              const void *key,
              size_t key_size,
              const void *ciphertext,
              size_t ciphertext_size,
              void *plaintext,
              size_t *plaintext_size,
              ecies_authentication_tag_t *tag)
{
  unsigned long tag_len;
  const uint8_t *key_ptr;
  const uint8_t *ciphertext_ptr;
  uint8_t *plaintext_ptr;
  int i;
  int hash;
  int err;

  /* Validate arguments */
  if (!key || !key_size || !ciphertext || !ciphertext_size ||
      !plaintext || !plaintext_size || !tag) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (!sha_algorithm_valid(sha_algorithm)) {
    TR_ERROR("Invalid SHA algorithm %d", sha_algorithm);
    return ATLK_E_INVALID_ARG;
  }

  /* In IEEE Std 1609.2-2016, ECIES is only used to encrypt AES symmetic keys */
  if (ciphertext_size > ECIES_MAX_TEXT_SIZE) {
    TR_ERROR("Unsupported ciphertext size %zu", ciphertext_size);
    return ATLK_E_UNSUPPORTED;
  }

  if (*plaintext_size < ciphertext_size) {
    TR_ERROR("Plaintext size %zu smaller than ciphertext size %zu",
	     *plaintext_size, ciphertext_size);
    return ATLK_E_INVALID_ARG;
  }

  /*
   * Make sure HMAC key size is large enough.
   * Reference: IEEE Std. 1363a-2004, section 14.4.1.
   */
  if (key_size < ciphertext_size + sha_algorithm_digest_size(sha_algorithm)/2) {
    TR_ERROR("ECIES key size (%zu) must be greater or equal to the addition "
             "of the plaintext size (%zu) and half the hash digest size (%zu)",
             key_size,
             ciphertext_size,
             sha_algorithm_digest_size(sha_algorithm)/2);
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Decrypt */
  key_ptr = key;
  ciphertext_ptr = ciphertext;
  plaintext_ptr = plaintext;
  for (i = 0; i < ciphertext_size; i++) {
    plaintext_ptr[i] = ciphertext_ptr[i] ^ key_ptr[i];
  }

  *plaintext_size = ciphertext_size;

  /* Find hash */
  hash = find_hash(sha_algorithm_name(sha_algorithm));
  BUG_ON(hash == -1);

  /* Compute message authentication code */
  tag_len = sizeof(*tag);
  err = hmac_memory(hash,
                    key_ptr + ciphertext_size, key_size - ciphertext_size,
                    ciphertext, ciphertext_size,
                    tag->value, &tag_len);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to compute message authentication code: %s",
      error_to_string(err));
    return crypto_error_to_rc(err);
  }

  return ATLK_OK;
}

