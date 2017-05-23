/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <atlk/sdk.h>
#include <atlk/aes.h>
#include <atlk/hsm_service.h>
#include <atlk/hsm_emulator.h>

#if defined __CRATON_NO_ARC || defined __CRATON_ARC1

/*
  CRATON ThreadX Secure Storage Example

  This example demonstrates usage of the HSM API to implement
  secure certificate storage using non-secure NVM for code running on top
  of CRATON processor with ThreadX RTOS.

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

/* Number of NVM cells to configure for HSM */
#define SECURE_STORAGE_EXAMPLE_HSM_NVM_NUM_CELLS 128

/* Helper macros */
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define ROUND_UP(n, d) (DIV_ROUND_UP(n, d) * (d))

/* Helper function for converting buffer to hex */
static void
buffer_to_line(const void *buf, size_t buf_len, char *line)
{
  const uint8_t *ptr = buf;
  char *pos = &line[0];
  size_t i;

  if (!buf_len) {
    *pos = '\0';
    return;
  }

  if (buf_len > 16) {
    buf_len = 16;
  }

  for (i = 0; i < buf_len - 1; i++) {
    pos += sprintf(pos, "%02x ", ptr[i]);
  }
  pos += sprintf(pos, "%02x", ptr[i]);
}

/* Print buffer to standard output */
static void
buffer_print(const void *buf, size_t len)
{
  const uint8_t *ptr = buf;
  size_t i, line_len, remaining = len;
  char line[80];

  for (i = 0; i < len; i += 16) {
    line_len = remaining < 16 ? remaining : 16;
    remaining -= 16;

    buffer_to_line(ptr + i, line_len, line);
    printf("  %.8lx: %s\n", (unsigned long)i, line);
  }
}

static atlk_rc_t
secure_storage_example(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* HSM NVM configuration */
  hsm_nvm_config_t hsm_nvm_config = HSM_NVM_CONFIG_INIT;
  /* Dummy root certificate */
  const char root_cert[] = "Dummy Root Certificate";
  /* Dummy pseudonym certificate */
  const char pseudonym_cert[] = "Dummy Pseudonym Certificate";
  /* AES-CMAC authentication tag of root certificate */
  aes_cmac_tag_t root_cert_tag = AES_CMAC_TAG_INIT;
  /* Buffer to store plaintext/ciphertext */
  uint8_t buf[64];
  /* Buffer size */
  size_t buf_size;
  /* AES-CBC initialization vector */
  aes_cbc_iv_t iv;

  printf("Initializing NVM to contain %u cells\n",
    SECURE_STORAGE_EXAMPLE_HSM_NVM_NUM_CELLS);

  hsm_nvm_config.num_of_cells = SECURE_STORAGE_EXAMPLE_HSM_NVM_NUM_CELLS;

  /* Initialize HSM NVM */
  rc = hsm_nvm_init(example_hsm_service, &hsm_nvm_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_nvm_init: %s", atlk_rc_to_str(rc));
    return rc;
  }

  printf("Root certificate:\n");
  buffer_print(root_cert, sizeof(root_cert));

  /* Compute AES-CMAC for the root certificate */
  rc = hsm_host_nvm_aes_cmac_compute(example_hsm_service,
                                     root_cert,
                                     sizeof(root_cert),
                                     &root_cert_tag);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_host_nvm_aes_cmac_compute: %s\n", atlk_rc_to_str(rc));
    return rc;
  }

  printf("Root certificate authentication tag:\n");
  buffer_print(root_cert_tag.value, sizeof(root_cert_tag.value));

  /* Now the root certificate + tag can be stored on a non-secure NVM */

  printf("Pseudonym certificate:\n");
  buffer_print(pseudonym_cert, sizeof(pseudonym_cert));

  /*
   * Pad the pseudonym certificate so the total size will be a multiple of
   * the AES block size (= 16).
   */
  memset(buf, 0, sizeof(buf));
  memcpy(buf, pseudonym_cert, sizeof(pseudonym_cert));
  buf_size = ROUND_UP(sizeof(pseudonym_cert), 16);

  /* Encrypt the pseudonym certificate with AES-CBC */
  rc = hsm_host_nvm_aes_cbc_encrypt(example_hsm_service,
                                    buf, buf_size, &iv, buf, &buf_size);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_host_nvm_aes_cbc_encrypt: %s\n", atlk_rc_to_str(rc));
    return rc;
  }

  printf("AES-CBC initialization vector:\n");
  buffer_print(iv.value, sizeof(iv.value));

  printf("AES-CBC encrypted pseudonym certificate (with padding):\n");
  buffer_print(buf, buf_size);

  /* Decrypt the pseudonym certificate with AES-CBC */
  rc = hsm_host_nvm_aes_cbc_decrypt(example_hsm_service,
                                    &iv, buf, buf_size, buf, &buf_size);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_host_nvm_aes_cbc_decrypt: %s\n", atlk_rc_to_str(rc));
    return rc;
  }

  printf("AES-CBC decrypted pseudonym certificate (with padding):\n");
  buffer_print(buf, buf_size);

  printf("AES-CBC decrypted pseudonym certificate (without padding):\n");
  buffer_print(buf, sizeof(pseudonym_cert));

  return ATLK_OK;
}

void
craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* HSM emulator configuration */
  hsm_emulator_config_t hsm_emulator_config = HSM_EMULATOR_CONFIG_INIT;

  /* Get default ECC service */
  rc = ecc_default_service_get(&example_ecc_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "ecc_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto out;
  }

  /* Initialize the HSM emulator configuration */
  hsm_emulator_config.ecc_service_ptr = example_ecc_service;
  hsm_emulator_config.nvm_file_path = NULL;

  /* Initialize AES keys to be used for secure certificate storage */
  memset(hsm_emulator_config.host_nvm_authentication_key.value, 0xAA,
         sizeof(hsm_emulator_config.host_nvm_authentication_key.value));
  memset(hsm_emulator_config.host_nvm_encryption_key.value, 0xBB,
         sizeof(hsm_emulator_config.host_nvm_encryption_key.value));

  /* Create HSM emulator service */
  rc = hsm_emulator_create(&hsm_emulator_config, &example_hsm_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "hsm_emulator_create: %s\n", atlk_rc_to_str(rc));
    goto out;
  }

  /* Run example */
  rc = secure_storage_example();
  if (rc) {
    goto out;
  }

out:
  if (atlk_error(rc)) {
    fprintf(stderr, "ERROR\n");
  }

  /* Delete HSM emulator service */
  hsm_service_delete(example_hsm_service);

  /* Delete ECC service */
  ecc_service_delete(example_ecc_service);
}

#else /* __CRATON_NO_ARC || __CRATON_ARC1 */

void craton_user_init(void)
{
}

#endif /* __CRATON_NO_ARC || __CRATON_ARC1 */
