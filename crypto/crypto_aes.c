#include <tommath.h>
#include <tomcrypt.h>

#include <atlk/sdk.h>
#include <atlk/aes.h>

#include <platform/common.h>

#include <crypto/crypto_init.h>
#include <crypto/crypto_error.h>

atlk_rc_t
aes_ccm_encrypt(const aes_key_t *key,
                const aes_ccm_nonce_t *nonce,
                const void *plaintext,
                size_t plaintext_size,
                void *ciphertext,
                size_t *ciphertext_size,
                aes_ccm_authentication_tag_t *tag)
{
  size_t tag_size = sizeof(tag->value);

  return aes_ccm_encrypt_2(key,
                           nonce->value, sizeof(nonce->value),
                           NULL, 0,
                           plaintext, plaintext_size,
                           ciphertext, ciphertext_size,
                           tag->value, &tag_size);
}

atlk_rc_t
aes_ccm_decrypt(const aes_key_t *key,
                const aes_ccm_nonce_t *nonce,
                const void *ciphertext,
                size_t ciphertext_size,
                void *plaintext,
                size_t *plaintext_size,
                aes_ccm_authentication_tag_t *tag)
{
  size_t tag_size = sizeof(tag->value);

  return aes_ccm_decrypt_2(key,
                           nonce->value, sizeof(nonce->value),
                           NULL, 0,
                           ciphertext, ciphertext_size,
                           plaintext, plaintext_size,
                           tag->value, &tag_size);
}

atlk_rc_t
aes_ccm_encrypt_2(const aes_key_t *key,
            const void *nonce,
            size_t nonce_size,
            const void *header,
            size_t header_size,
            const void *plaintext,
            size_t plaintext_size,
            void *ciphertext,
            size_t *ciphertext_size,
            void *tag,
            size_t *tag_size)
{
  unsigned long taglen;
  int cipher;
  int err;

  /* Check function arguments */
  if (!key || !nonce || !nonce_size || !plaintext ||
      !ciphertext || !ciphertext_size || !tag || !tag_size) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (*ciphertext_size < plaintext_size) {
    TR_ERROR("Ciphertext size %zu smaller than plaintext size %zu",
      *ciphertext_size, plaintext_size);
    return ATLK_E_INVALID_ARG;
  }

  if (!header && header_size) {
    TR_ERROR("Invalid input: header is not specified but header_size %lu != 0",
      (unsigned long)header_size);
    return ATLK_E_INVALID_ARG;
  }  

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find AES descriptor index */
  cipher = find_cipher("aes");
  BUG_ON(cipher == -1);

  /* Initialize tag length */
  taglen = *tag_size;

  /* Encrypt using AES-CCM */
  err = ccm_memory(cipher,                             /* AES cipher index */
                   key->value, sizeof(key->value),     /* Secret key */
                   NULL,                               /* Key was not scheduled */
                   nonce, nonce_size,                  /* Session nonce */
                   header, header_size,                /* Session header */
                   (unsigned char *)plaintext,         /* Plaintext */
                   plaintext_size,
                   ciphertext,                         /* Ciphertext */
                   tag, &taglen,                       /* Authentication tag */
                   CCM_ENCRYPT);                       /* Encrypt direction */
  if (err != CRYPT_OK) {
    TR_ERROR("AES-CCM encryption failed: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  *ciphertext_size = plaintext_size;
  *tag_size = taglen;

  return ATLK_OK;
}

atlk_rc_t
aes_ccm_decrypt_2(const aes_key_t *key,
            const void *nonce,
            size_t nonce_size,
            const void *header,
            size_t header_size,
            const void *ciphertext,
            size_t ciphertext_size,
            void *plaintext,
            size_t *plaintext_size,
            void *tag,
            size_t *tag_size)
{
  unsigned long taglen;
  int cipher;
  int err;

  /* Check function arguments */
  if (!key || !nonce || !nonce_size || !plaintext || !plaintext_size ||
      !ciphertext || !tag || !tag_size) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (*plaintext_size < ciphertext_size) {
    TR_ERROR("Plaintext size %zu smaller than ciphertext size %zu",
      *plaintext_size, ciphertext_size);
    return ATLK_E_INVALID_ARG;
  }

  if (!header && header_size) {
    TR_ERROR("Invalid input: header is not specified but header_size %lu != 0",
      (unsigned long)header_size);
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find AES descriptor index */
  cipher = find_cipher("aes");
  BUG_ON(cipher == -1);

  /* Initialize tag length */
  taglen = *tag_size;

  /* Decrypt using AES-CCM */
  err = ccm_memory(cipher,                             /* AES cipher index */
                   key->value, sizeof(key->value),     /* Secret key */
                   NULL,                               /* Key was not scheduled */
                   nonce, nonce_size,                  /* Session nonce */
                   header, header_size,                /* Session header */
                   plaintext,                          /* Plaintext */
                   ciphertext_size,
                   (unsigned char *)ciphertext,        /* Ciphertext */
                   tag, &taglen,                       /* Authentication tag */
                   CCM_DECRYPT);                       /* Decrypt direction */
  if (err != CRYPT_OK) {
    TR_ERROR("AES-CCM decryption failed: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  *plaintext_size = ciphertext_size;
  *tag_size = taglen;

  return ATLK_OK;
}

atlk_rc_t
aes_ccmptmac_compute(const aes_key_t *key,
                     const void *nonce,
                     size_t nonce_size,
                     const void *header,
                     size_t header_size,
                     const void *message,
                     size_t message_size,
                     void *tag,
                     size_t *tag_size)
{
  unsigned long taglen;
  int cipher;
  int err;

  /* Check function arguments */
  if (!key || !nonce || !nonce_size || !message ||
      !tag || !tag_size) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (!header && header_size) {
    TR_ERROR("Invalid input: header is not specified but header_size %lu != 0",
      (unsigned long)header_size);
    return ATLK_E_INVALID_ARG;
  }  

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find AES descriptor index */
  cipher = find_cipher("aes");
  BUG_ON(cipher == -1);

  /* Initialize tag length */
  taglen = *tag_size;

  /* Compute AES CCM-PTMAC */
  err = ccm_ptmac(cipher,                              /* AES cipher index */
                  key->value, sizeof(key->value),      /* Secret key */
                  NULL,                                /* Key was not scheduled */
                  nonce, nonce_size,                   /* Session nonce */
                  header, header_size,                 /* Session header */
                  message, message_size,               /* Message */
                  tag, &taglen);                       /* Authentication tag */
  if (err != CRYPT_OK) {
    TR_ERROR("AES CCM-PTMAC computation failed: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  *tag_size = taglen;

  return ATLK_OK;
}

atlk_rc_t
aes_ecb_encrypt(const aes_key_t *key,
            const void *plaintext,
            size_t plaintext_size,
            void *ciphertext,
            size_t *ciphertext_size)
{
  symmetric_ECB ecb;
  int cipher;
  int err;

  /* Check function arguments */
  if (!key || !plaintext || !plaintext_size || !ciphertext ||
      !ciphertext_size) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (*ciphertext_size < plaintext_size) {
    TR_ERROR("Ciphertext size %lu smaller than plaintext size %zu",
      (unsigned long)*ciphertext_size, plaintext_size);
    return ATLK_E_INVALID_ARG;
  }

  if (plaintext_size % AES_BLOCK_SIZE) {
    TR_ERROR("Plaintext size %lu must be a multiple of %u",
      (unsigned long)plaintext_size, AES_BLOCK_SIZE);
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find AES descriptor index */
  cipher = find_cipher("aes");
  BUG_ON(cipher == -1);

  /* Initialize an ECB context */
  err = ecb_start(cipher, key->value, sizeof(key->value), 0, &ecb);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to initialize ECB context: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  /* ECB encrypt */
  err = ecb_encrypt(plaintext, ciphertext, plaintext_size, &ecb);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to ECB encrypt: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  /* Terminate ECB stream */
  err = ecb_done(&ecb);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to terminate ECB stream: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  *ciphertext_size = plaintext_size;

  return ATLK_OK;
}

atlk_rc_t
aes_ecb_decrypt(const aes_key_t *key,
            const void *ciphertext,
            size_t ciphertext_size,
            void *plaintext,
            size_t *plaintext_size)
{
  symmetric_ECB ecb;
  int cipher;
  int err;

  /* Check function arguments */
  if (!key || !plaintext || !plaintext_size || !ciphertext ||
      !ciphertext_size) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (*plaintext_size < ciphertext_size) {
    TR_ERROR("Plaintext size %lu smaller than ciphertext size %zu",
      (unsigned long)*plaintext_size, ciphertext_size);
    return ATLK_E_INVALID_ARG;
  }

  if (ciphertext_size % AES_BLOCK_SIZE) {
    TR_ERROR("Ciphertext size %lu must be a multiple of %u",
      (unsigned long)ciphertext_size, AES_BLOCK_SIZE);
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find AES descriptor index */
  cipher = find_cipher("aes");
  BUG_ON(cipher == -1);

  /* Initialize a ECB context */
  err = ecb_start(cipher, key->value, sizeof(key->value), 0, &ecb);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to initialize ECB context: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  /* ECB decrypt */
  err = ecb_decrypt(ciphertext, plaintext, ciphertext_size, &ecb);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to ECB decrypt: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  /* Terminate ECB stream */
  err = ecb_done(&ecb);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to terminate ECB stream: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  *plaintext_size = ciphertext_size;

  return ATLK_OK;
}

atlk_rc_t
aes_cbc_encrypt(const aes_key_t *key,
                const aes_cbc_iv_t *iv,
                const void *plaintext,
                size_t plaintext_size,
                void *ciphertext,
                size_t *ciphertext_size)
{
  symmetric_CBC cbc;
  int cipher;
  int err;

  /* Check function arguments */
  if (!key || !iv || !plaintext || !plaintext_size || !ciphertext ||
    !ciphertext_size) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (*ciphertext_size < plaintext_size) {
    TR_ERROR("Ciphertext size %lu smaller than plaintext size %zu",
      (unsigned long)*ciphertext_size, plaintext_size);
    return ATLK_E_INVALID_ARG;
  }

  if (plaintext_size % AES_BLOCK_SIZE) {
    TR_ERROR("Plaintext size %lu must be a multiple of %u",
      (unsigned long)plaintext_size, AES_BLOCK_SIZE);
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find AES descriptor index */
  cipher = find_cipher("aes");
  BUG_ON(cipher == -1);

  /* Initialize a CBC context */
  err = cbc_start(cipher, iv->value, key->value, sizeof(key->value), 0, &cbc);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to initialize CBC context: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  /* CBC encrypt */
  err = cbc_encrypt(plaintext, ciphertext, plaintext_size, &cbc);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to CBC encrypt: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  /* Terminate CBC stream */
  err = cbc_done(&cbc);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to terminate CBC stream: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  *ciphertext_size = plaintext_size;

  return ATLK_OK;
}

atlk_rc_t
aes_cbc_decrypt(const aes_key_t *key,
                const aes_cbc_iv_t *iv,
                const void *ciphertext,
                size_t ciphertext_size,
                void *plaintext,
                size_t *plaintext_size)
{
  symmetric_CBC cbc;
  int cipher;
  int err;

  /* Check function arguments */
  if (!key || !iv || !plaintext || !plaintext_size || !ciphertext ||
    !ciphertext_size) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (*plaintext_size < ciphertext_size) {
    TR_ERROR("Plaintext size %lu smaller than ciphertext size %zu",
      (unsigned long)*plaintext_size, ciphertext_size);
    return ATLK_E_INVALID_ARG;
  }

  if (ciphertext_size % AES_BLOCK_SIZE) {
    TR_ERROR("Ciphertext size %lu must be a multiple of %u",
      (unsigned long)ciphertext_size, AES_BLOCK_SIZE);
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find AES descriptor index */
  cipher = find_cipher("aes");
  BUG_ON(cipher == -1);

  /* Initialize a CBC context */
  err = cbc_start(cipher, iv->value, key->value, sizeof(key->value), 0, &cbc);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to initialize CBC context: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  /* CBC decrypt */
  err = cbc_decrypt(ciphertext, plaintext, ciphertext_size, &cbc);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to CBC decrypt: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  /* Terminate CBC stream */
  err = cbc_done(&cbc);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to terminate CBC stream: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  *plaintext_size = ciphertext_size;

  return ATLK_OK;
}

atlk_rc_t
aes_cmac_compute(const aes_key_t *key,
                 const void *message,
                 size_t message_size,
                 aes_cmac_tag_t *tag)
{
  unsigned long taglen;
  int cipher;
  int err;

  /* Check function arguments */
  if (!key || !message || !tag) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find AES descriptor index */
  cipher = find_cipher("aes");
  BUG_ON(cipher == -1);

  /* Initialize tag length */
  taglen = sizeof(tag->value);

  /* Compute AES-CMAC */
  err = omac_memory(cipher,                            /* AES cipher index */
                    key->value, sizeof(key->value),    /* Secret key */
                    message, message_size,             /* Message */
                    tag->value, &taglen);              /* Authentication tag */
  if (err != CRYPT_OK) {
    TR_ERROR("AES-CMAC computation failed: %s", error_to_string(err));
    return crypto_error_to_rc(err);
  }

  return ATLK_OK;
}

