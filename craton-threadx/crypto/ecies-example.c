/* Copyright (C) 2014-2016 Autotalks Ltd. */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/sdk.h>
#include <atlk/rng.h>
#include <atlk/sha.h>
#include <atlk/ecc.h>
#include <atlk/ecies.h>
#include <atlk/aes.h>
#include <atlk/hsm_service.h>
#include <atlk/hsm_emulator.h>

#if defined __CRATON_NO_ARC || defined __CRATON_ARC1

/*
  CRATON ThreadX ECIES Example

  This example demonstrates a basic ECIES and AES-CCM encryption/decryption
  scenario inspired by IEEE Std. 1609.2-2016 using the HSM API, ECC API and
  RNG API for code running on top of CRATON processor with ThreadX RTOS.

  The device used in this example is a "HSM emulator", a term used to describe
  an emulated HSM device. The differences between an emulated HSM service
  and a real one are:
  - The emulated HSM service is created via hsm_emulator_create().
  - The implementation is not tamper-resistant because it uses general purpose
    hardware instead of tamper-resistant hardware.

  The purpose of the emulated HSM is basic software integration with
  the HSM API on a hardware platform that doesn't have a working HSM chip.
*/

/* HSM emulator service */
static hsm_service_t *example_hsm_service = NULL;

/* ECC service */
static ecc_service_t *example_ecc_service = NULL;

/* Format string for ECC scalar */
#define ECC_SCALAR_FMT \
  "0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx,0x%08lx," \
  "0x%08lx,0x%08lx,0x%08lx,0x%08lx"

/* Format argument list for ecc_scalar_t */
#define ECC_SCALAR_FMT_ARGS(x)                      \
  x.value[0], x.value[1], x.value[2], x.value[3],   \
  x.value[4], x.value[5], x.value[6], x.value[7],   \
  x.value[8], x.value[9], x.value[10], x.value[11]

/* ECIES HMAC key size in octets */
#define ECIES_HMAC_KEY_SIZE 32

/* Number of NVM cells to configure for HSM */
#define ECIES_EXAMPLE_HSM_NVM_NUM_CELLS 128

/* HSM emulator filename */
#define HSM_EMULATOR_FILENAME "B:/hsm-emu.dat"

/* ECIES example shared infomation between Alice and Bob */
typedef struct {
  /* Elliptic curve used */
  ecc_curve_t curve;

  /* Bob ECC public key */
  ecc_point_t bob_public_key;

} ecies_example_shared_info_t;

static ecies_example_shared_info_t ecies_example_shared_info = {
  .curve = ECC_CURVE_NIST_P224,
  .bob_public_key = ECC_POINT_INIT
};

/* ECIES example Bob's private information */
typedef struct {
  /* Index of the ECC private key */
  hsm_cell_index_t private_key_index;

} ecies_example_bob_info_t;

static ecies_example_bob_info_t ecies_example_bob_info = {
  .private_key_index = HSM_CELL_INDEX_NA
};

/* ECIES example maximum message size in octets */
#define ECIES_EXAMPLE_MESSAGE_MAX_SIZE 64

/* ECIES example message */
typedef struct {
  /* Ephemeral public key used for ECIES encryption */
  ecc_point_t ecies_ephemeral_public_key;

  /* AES-CCM key encrypted using ECIES */
  uint8_t ecies_encrypted_aes_key[AES_KEY_SIZE];

  /* ECIES authentication tag */
  ecies_authentication_tag_t ecies_authentication_tag;

  /* Ciphertext encrypted using AES-CCM */
  uint8_t aes_ccm_ciphertext[ECIES_EXAMPLE_MESSAGE_MAX_SIZE];

  /* Ciphertext size in octets */
  size_t aes_ccm_ciphertext_size;

  /* AES-CCM nonce */
  aes_ccm_nonce_t aes_ccm_nonce;

  /* AES-CCM authentication tag */
  aes_ccm_authentication_tag_t aes_ccm_tag;

} ecies_example_message_t;

static void
ecies_example_print_buffer(const uint8_t *buf, size_t buf_len)
{
  size_t i;

  for (i = 0; i < buf_len; i++) {
    printf("%02x", buf[i]);
  }
}

static atlk_rc_t
ecies_example_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Private key information */
  hsm_ecc_private_key_info_t private_key_info = HSM_ECC_PRIVATE_KEY_INFO_INIT;
  /* HSM cell index to store Bob's private key */
  hsm_cell_index_t private_key_index = HSM_CELL_INDEX_NA;
  /* Bob's ECC public key */
  ecc_point_t bob_public_key = ECC_POINT_INIT;
  /* HSM capability information */
  hsm_capability_info_t hsm_capability_info = HSM_CAPABILITY_INFO_INIT;
  /* HSM NVM configuration */
  hsm_nvm_config_t hsm_nvm_config = HSM_NVM_CONFIG_INIT;

  printf("\n>>> Initialization\n");

  /* Get HSM capability information */
  rc = hsm_capability_info_get(example_hsm_service, &hsm_capability_info);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_capability_info_get: %s", atlk_rc_to_str(rc));
    return rc;
  }

  printf("HSM capability information:\n");
  printf("  Maximum number of NVM cells: %lu\n",
    hsm_capability_info.max_num_of_cells);
  printf("  Current number of NVM cells: %lu\n",
    hsm_capability_info.current_num_of_cells);
  printf("  Maximum number of cell ranges supported by "
         "hsm_csr_ecdsa_public_keys_sign(): %lu\n",
    hsm_capability_info.max_num_of_cell_ranges_for_csr);

  printf("Initializing NVM to contain %u cells\n",
    ECIES_EXAMPLE_HSM_NVM_NUM_CELLS);

  hsm_nvm_config.num_of_cells = ECIES_EXAMPLE_HSM_NVM_NUM_CELLS;

  /* Initialize HSM NVM */
  rc = hsm_nvm_init(example_hsm_service, &hsm_nvm_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_nvm_init: %s", atlk_rc_to_str(rc));
    return rc;
  }
 
  /* Use NIST P-256 elliptic curve and an Isolated key for ECIES algorithm */
  private_key_info.key_curve = ECC_CURVE_NIST_P256;
  private_key_info.key_type = HSM_PRIVATE_KEY_TYPE_ISOLATED;
  private_key_info.key_algorithm = HSM_PUBLIC_KEY_ALGORITHM_ECIES;

  printf("Using elliptic curve ID: %u\n", private_key_info.key_curve);
  printf("Using key_type ID: %u\n", private_key_info.key_type);
  printf("Using key_algorithm ID: %u\n", private_key_info.key_algorithm);

  /* Use the first available cell to store Bob's private key */
  private_key_index = 0;

  printf("Using HSM cell index: %lu\n", private_key_index);

  /* Create Bob's private key */
  rc = hsm_ecc_private_key_create(example_hsm_service,
                                  private_key_index,
                                  &private_key_info);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_ecc_private_key_create: %s", atlk_rc_to_str(rc));
    return rc;
  }

  printf("Bob's ECC private key created\n");

  /* Get Bob's public key */
  rc = hsm_ecc_public_key_get(example_hsm_service,
                              private_key_index,
                              &bob_public_key);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_ecc_public_key_get: %s", atlk_rc_to_str(rc));
    return rc;
  }

  assert(bob_public_key.point_type == ECC_POINT_UNCOMPRESSED);

  /* Print retrieved ECC public key */
  printf("Bob's ECC public key created:\n");
  printf("  x: " ECC_SCALAR_FMT "\n",
    ECC_SCALAR_FMT_ARGS(bob_public_key.x_coordinate));
  printf("  y: " ECC_SCALAR_FMT "\n",
    ECC_SCALAR_FMT_ARGS(bob_public_key.y_coordinate));

  /* Store shared information */
  ecies_example_shared_info.curve = private_key_info.key_curve;
  ecies_example_shared_info.bob_public_key = bob_public_key;

  /* Store Bob's private information */
  ecies_example_bob_info.private_key_index = private_key_index;

  return ATLK_OK;
}

static atlk_rc_t
ecies_example_alice(ecies_example_message_t *msg)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* AES key */
  aes_key_t aes_key = AES_KEY_INIT;
  /* AES-CCM nonce */
  aes_ccm_nonce_t aes_ccm_nonce = AES_CCM_NONCE_INIT;
  /* AES-CCM authentication tag */
  aes_ccm_authentication_tag_t aes_ccm_tag = AES_CCM_AUTHENTICATION_TAG_INIT;
  /* AES-CCM ciphertext */
  uint8_t ciphertext[ECIES_EXAMPLE_MESSAGE_MAX_SIZE] = { 0 };
  /* AES-CCM ciphertext size in octets */
  size_t ciphertext_size = sizeof(ciphertext);
  /* Example message */
  static const char example_msg[] =
    "Autotalks - The Confidence of Knowing Ahead";
  /* ECIES key */
  uint8_t ecies_key[sizeof(aes_key) + ECIES_HMAC_KEY_SIZE] = { 0 };
  /* ECIES ephemeral public key */
  ecc_point_t ephemeral_public_key = ECC_POINT_INIT;
  /* AES key encrypted using ECIES */
  uint8_t encrypted_aes_key[AES_KEY_SIZE] = { 0 };
  /* AES key size in octets */
  size_t encrypted_aes_key_size = sizeof(encrypted_aes_key);
  /* ECIES authentication tag */
  ecies_authentication_tag_t ecies_authentication_tag =
    ECIES_AUTHENTICATION_TAG_INIT;

  printf("\n>>> Alice\n");

  /* Print the message data */
  printf("Message: %s\n", example_msg);
  printf("Message size: %lu\n", (long unsigned int)sizeof(example_msg));

  /* Get random AES key */
  rc = rng_data_get(&aes_key, sizeof(aes_key));
  if (atlk_error(rc)) {
    fprintf(stderr, "rng_data_get: %s", atlk_rc_to_str(rc));
    return rc;
  }

  /* Print random AES key */
  printf("AES key: ");
  ecies_example_print_buffer(aes_key.value, sizeof(aes_key));
  printf("\n");

  /* Get random AES-CCM nonce */
  rc = rng_data_get(&aes_ccm_nonce, sizeof(aes_ccm_nonce));
  if (atlk_error(rc)) {
    fprintf(stderr, "rng_data_get: %s", atlk_rc_to_str(rc));
    return rc;
  }

  /* Print random AES-CCM nonce */
  printf("AES-CCM nonce: ");
  ecies_example_print_buffer(aes_ccm_nonce.value, sizeof(aes_ccm_nonce));
  printf("\n");

  /* Encrypt message with AES-CCM */
  rc = aes_ccm_encrypt(&aes_key,
                       &aes_ccm_nonce,
                       example_msg,
                       sizeof(example_msg),
                       ciphertext,
                       &ciphertext_size,
                       &aes_ccm_tag);
  if (atlk_error(rc)) {
    fprintf(stderr, "aes_ccm_encrypt: %s", atlk_rc_to_str(rc));
    return rc;
  }

  /* Print AES-CCM encrypted message and authentication tag */
  printf("AES-CCM encrypted message: ");
  ecies_example_print_buffer(ciphertext, ciphertext_size);
  printf("\n");

  printf("AES-CCM authentication tag: ");
  ecies_example_print_buffer(aes_ccm_tag.value, sizeof(aes_ccm_tag));
  printf("\n");

  /* ECIES key size should be equal to: plaintext size + HMAC key size */

  /* Create ECIES key and ephemeral public key */
  rc = ecies_key_create(ecies_example_shared_info.curve,
                        &ecies_example_shared_info.bob_public_key,
                        &ephemeral_public_key,
                        ecies_key,
                        sizeof(ecies_key),
                        NULL,
                        0);
  if (atlk_error(rc)) {
    fprintf(stderr, "ecdh_secret_create: %s", atlk_rc_to_str(rc));
    return rc;
  }

  assert(ephemeral_public_key.point_type == ECC_POINT_UNCOMPRESSED);

  /* Print ephemeral public key */
  printf("ECC ephemeral public key created:\n");
  printf("  x: " ECC_SCALAR_FMT "\n",
    ECC_SCALAR_FMT_ARGS(ephemeral_public_key.x_coordinate));
  printf("  y: " ECC_SCALAR_FMT "\n",
    ECC_SCALAR_FMT_ARGS(ephemeral_public_key.y_coordinate));

  /* Print ECIES key */
  printf("ECIES key: ");
  ecies_example_print_buffer(ecies_key, sizeof(ecies_key));
  printf("\n");

  /* Encrypt AES key using ECIES */
  rc = ecies_encrypt(SHA_256,
                     ecies_key,
                     sizeof(ecies_key),
                     &aes_key,
                     sizeof(aes_key),
                     encrypted_aes_key,
                     &encrypted_aes_key_size,
                     &ecies_authentication_tag);
  if (atlk_error(rc)) {
    fprintf(stderr, "ecies_encrypt: %s", atlk_rc_to_str(rc));
    return rc;
  }

  /* Print ECIES encrypted AES key and authentication tag */
  printf("ECIES encrypted AES key: ");
  ecies_example_print_buffer(encrypted_aes_key, encrypted_aes_key_size);
  printf("\n");

  printf("ECIES authentication tag: ");
  ecies_example_print_buffer(ecies_authentication_tag.value,
    sizeof(ecies_authentication_tag));
  printf("\n");

  /* Produce message */
  memcpy(msg->aes_ccm_ciphertext, ciphertext, ciphertext_size);
  msg->aes_ccm_ciphertext_size = ciphertext_size;
  msg->aes_ccm_nonce = aes_ccm_nonce;
  msg->aes_ccm_tag = aes_ccm_tag;
  memcpy(msg->ecies_encrypted_aes_key, &encrypted_aes_key, AES_KEY_SIZE);
  msg->ecies_authentication_tag = ecies_authentication_tag;
  msg->ecies_ephemeral_public_key = ephemeral_public_key;

  return ATLK_OK;
}

static atlk_rc_t
ecies_example_bob(const ecies_example_message_t *msg)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* ECIES decrypted AES key */
  aes_key_t aes_key = AES_KEY_INIT;
  /* AES key size in octets */
  size_t aes_key_size = sizeof(aes_key);
  /* ECIES key */
  uint8_t ecies_key[sizeof(aes_key) + ECIES_HMAC_KEY_SIZE] = { 0 };
  /* ECIES authentication tag */
  ecies_authentication_tag_t ecies_authentication_tag =
    ECIES_AUTHENTICATION_TAG_INIT;
  /* AES-CCM authentication tag */
  aes_ccm_authentication_tag_t aes_ccm_tag = AES_CCM_AUTHENTICATION_TAG_INIT;
  /* AES-CCM plaintext */
  uint8_t plaintext[ECIES_EXAMPLE_MESSAGE_MAX_SIZE] = { 0 };
  /* AES-CCM plaintext size in octets */
  size_t plaintext_size = sizeof(plaintext);
  /* Example failure indication */
  int failed = 1;

  printf("\n>>> Bob\n");

  /* Derive ECIES key */
  rc = hsm_ecies_key_derive(example_hsm_service,
                            ecies_example_bob_info.private_key_index,
                            &msg->ecies_ephemeral_public_key,
                            ecies_key,
                            sizeof(ecies_key),
                            NULL,
                            0);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_ecdh_secret_derive: %s", atlk_rc_to_str(rc));
    return rc;
  }

  /* Print ECIES key */
  printf("Derived ECIES key: ");
  ecies_example_print_buffer(ecies_key, sizeof(ecies_key));
  printf("\n");

  /* Decrypt AES key with ECIES */
  rc = ecies_decrypt(SHA_256,
              ecies_key,
              sizeof(ecies_key),
              msg->ecies_encrypted_aes_key,
              sizeof(msg->ecies_encrypted_aes_key),
              aes_key.value,
              &aes_key_size,
              &ecies_authentication_tag);
  if (atlk_error(rc)) {
    fprintf(stderr, "ecies_decrypt: %s", atlk_rc_to_str(rc));
    return rc;
  }

  /* Print ECIES decrypted AES key and authentication tag */
  printf("ECIES decrypted AES key: ");
  ecies_example_print_buffer(aes_key.value, aes_key_size);
  printf("\n");

  printf("ECIES authentication tag: ");
  ecies_example_print_buffer(ecies_authentication_tag.value,
    sizeof(ecies_authentication_tag));
  printf("\n");

  /* Compare ECIES authentication tags */
  if (memcmp(&ecies_authentication_tag, &msg->ecies_authentication_tag,
      sizeof(ecies_authentication_tag)) != 0) {
    printf("ECIES encryption/decryption failed\n");
    goto out;
  }
  else {
    printf("ECIES encryption/decryption succeeded\n");
  }

  /* Decrypt message using AES-CCM */
  rc = aes_ccm_decrypt(&aes_key,
                       &msg->aes_ccm_nonce,
                       msg->aes_ccm_ciphertext,
                       msg->aes_ccm_ciphertext_size,
                       plaintext,
                       &plaintext_size,
                       &aes_ccm_tag);
  if (atlk_error(rc)) {
    fprintf(stderr, "aes_ccm_decrypt: %s", atlk_rc_to_str(rc));
    return rc;
  }

  /* Print AES-CCM decrypted message and authentication tag */
  printf("Decrypted message: %s\n", plaintext);
  printf("Decrypted message size: %lu\n", (long unsigned int)plaintext_size);

  printf("AES-CCM authentication tag: ");
  ecies_example_print_buffer(aes_ccm_tag.value, sizeof(aes_ccm_tag));
  printf("\n");

  /* Compare AES-CCM authentication tags */
  if (memcmp(&aes_ccm_tag, &msg->aes_ccm_tag, sizeof(aes_ccm_tag)) != 0) {
    printf("AES-CCM encryption/decryption failed\n");
    goto out;
  }
  else {
    printf("AES-CCM encryption/decryption succeeded\n");
  }

  /* Set failure indication flag */ 
  failed = 0;

out:
  printf("%s\n", failed ? "FAILURE" : "SUCCESS");

  return ATLK_OK;
}

/* ECIES example thread */
static TX_THREAD ecies_example_thread;
static uint8_t ecies_example_thread_stack[0x8000];

/* ECIES example thread priority */
#define ECIES_EXAMPLE_THREAD_PRIORITY 20

static void
ecies_example_thread_entry(ULONG input)
{
  /* Not using input */
  (void)input;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* ECDSA example message */
  ecies_example_message_t message;
  /* HSM emulator configuration */
  hsm_emulator_config_t hsm_emulator_config = HSM_EMULATOR_CONFIG_INIT;

  /* Initialize the HSM emulator configuration */
  rc = ecc_default_service_get(&example_ecc_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "ecc_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto out;
  }

  hsm_emulator_config.ecc_service_ptr = example_ecc_service;
#ifdef FS_EXIST
  hsm_emulator_config.nvm_file_path = HSM_EMULATOR_FILENAME;
#else
  hsm_emulator_config.nvm_file_path = NULL;
#endif /* FS_EXIST */
  /* Create HSM emulator service */
  rc = hsm_emulator_create(&hsm_emulator_config, &example_hsm_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_emulator_create: %s\n", atlk_rc_to_str(rc));
    goto out;
  }

  /* Initialize example message */
  rc = ecies_example_init();
  if (atlk_error(rc)) {
    goto out;
  }

  /* Produce example message by Alice */
  rc = ecies_example_alice(&message);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Consume example message by Bob */
  rc = ecies_example_bob(&message);
  if (atlk_error(rc)) {
    goto out;
  }

out:
  if (atlk_error(rc)) {
    fprintf(stderr, "ERROR\n");
  }
  
  /* Delete HSM emulator service */
  hsm_service_delete(example_hsm_service);
}

void
craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  
  /* Create example thread */
  trv = tx_thread_create(&ecies_example_thread, "ecies_example_thread",
                         ecies_example_thread_entry, 0,
                         ecies_example_thread_stack,
                         sizeof(ecies_example_thread_stack),
                         ECIES_EXAMPLE_THREAD_PRIORITY,
                         ECIES_EXAMPLE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;
}

#else /* __CRATON_NO_ARC || __CRATON_ARC1 */

void craton_user_init(void)
{
}

#endif /* __CRATON_NO_ARC || __CRATON_ARC1 */

