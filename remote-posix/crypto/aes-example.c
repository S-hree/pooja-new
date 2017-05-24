/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <atlk/sdk.h>
#include <atlk/aes.h>

/*
  CRATON Remote AES Example

  This example demonstrates the usage of the AES-CBC encryption/decryption and
  AES-CMAC generation APIs for code running on top of CRATON processor
  with ThreadX RTOS.
*/

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

/*
 * AES-CBC example test vectors were taken from:
 *   NIST Special Publication 800-38A:
 *   Recommendation for Block Cipher Modes of Operation:
 *   Methods and Techniques,
 *   Appendix F.2
 */

/* Example AES key used for AES-CBC encryption/decryption */
static const aes_key_t aes_cbc_key = {
  { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }
};

/* Example initialization vector used for AES-CBC encryption/decryption */
static const aes_cbc_iv_t aes_cbc_iv = {
  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f }
};

/* Example plaintext used for AES-CBC encryption */
static const uint8_t aes_cbc_plaintext[] = {
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
  0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
  0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
  0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
  0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
  0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
  0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};

/* Example ciphertext used for AES-CBC decryption */
static const uint8_t aes_cbc_ciphertext[] = {
  0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
  0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
  0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee,
  0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
  0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b,
  0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
  0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09,
  0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7
};

static void
aes_cbc_encrypt_example(void)
{
  /* Buffer for storing ciphertext */
  uint8_t ciphertext[sizeof(aes_cbc_plaintext)];
  /* Size of ciphertext */
  size_t ciphertext_size = sizeof(ciphertext);
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;

  printf("AES-CBC encryption example\n");

  printf("Encryption key:\n");
  buffer_print(&aes_cbc_key, sizeof(aes_cbc_key));

  printf("Initialization vector:\n");
  buffer_print(&aes_cbc_iv, sizeof(aes_cbc_iv));

  printf("Plaintext:\n");
  buffer_print(aes_cbc_plaintext, sizeof(aes_cbc_plaintext));

  /* Encrypt plaintext with AES-CBC */
  rc = aes_cbc_encrypt(&aes_cbc_key,
                       &aes_cbc_iv,
                       aes_cbc_plaintext, sizeof(aes_cbc_plaintext),
                       ciphertext, &ciphertext_size);
  if (atlk_error(rc)) {
    fprintf(stderr, "aes_cbc_encrypt: %s\n", atlk_rc_to_str(rc));
    return;
  }

  printf("Ciphertext:\n");
  buffer_print(ciphertext, ciphertext_size);

  /* Make sure ciphertext is correct */
  if (memcmp(aes_cbc_ciphertext, ciphertext, ciphertext_size) == 0) {
    printf("AES-CBC encryption succeeded\n");
  }
  else {
    printf("AES-CBC encryption failed\n");
  }
  printf("\n");
}

static void
aes_cbc_decrypt_example(void)
{
  /* Buffer for storing plaintext */
  uint8_t plaintext[sizeof(aes_cbc_ciphertext)];
  /* Size of plaintext */
  size_t plaintext_size = sizeof(plaintext);
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;

  printf("AES-CBC decryption example\n");

  printf("Decryption key:\n");
  buffer_print(&aes_cbc_key, sizeof(aes_cbc_key));

  printf("Initialization vector:\n");
  buffer_print(&aes_cbc_iv, sizeof(aes_cbc_iv));

  printf("Ciphertext:\n");
  buffer_print(aes_cbc_ciphertext, sizeof(aes_cbc_ciphertext));

  /* Decrypt ciphertext with AES-CBC */
  rc = aes_cbc_decrypt(&aes_cbc_key,
                       &aes_cbc_iv,
                       aes_cbc_ciphertext, sizeof(aes_cbc_ciphertext),
                       plaintext, &plaintext_size);
  if (atlk_error(rc)) {
    fprintf(stderr, "aes_cbc_decrypt: %s\n", atlk_rc_to_str(rc));
    return;
  }

  printf("Plaintext:\n");
  buffer_print(plaintext, plaintext_size);

  /* Make sure plaintext is correct */
  if (memcmp(aes_cbc_plaintext, plaintext, plaintext_size) == 0) {
    printf("AES-CBC decryption succeeded\n");
  }
  else {
    printf("AES-CBC decryption failed\n");
  }
  printf("\n");
}

/*
 * AES-CMAC example test vectors were taken from:
 *   NIST Special Publication 800-38B:
 *   Recommendation for Block Cipher Modes of Operation:
 *   The CMAC Mode for Authentication,
 *   Appendix D.1
 */

/* Example AES key used for AES-CMAC tag generation */
static const aes_key_t aes_cmac_key = {
  { 0x2b, 0x7e, 0x15 ,0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }
};

/* Example message for AES-CMAC tag generation */
static const uint8_t aes_cmac_msg[] = {
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
};

/* Expected AES-CMAC tag */
static const aes_cmac_tag_t aes_cmac_tag = {
  { 0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44,
    0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c }
};

static void
aes_cmac_example(void)
{
  /* AES-CMAC tag */
  aes_cmac_tag_t tag = AES_CMAC_TAG_INIT;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;

  printf("AES-CMAC example:\n");

  printf("Key:\n");
  buffer_print(&aes_cmac_key, sizeof(aes_cmac_key));

  printf("Message:\n");
  buffer_print(aes_cmac_msg, sizeof(aes_cmac_msg));

  /* Compute AES-CMAC tag */
  rc = aes_cmac_compute(&aes_cmac_key, aes_cmac_msg, sizeof(aes_cmac_msg), &tag);
  if (atlk_error(rc)) {
    fprintf(stderr, "aes_cmac_compute: %s\n", atlk_rc_to_str(rc));
    return;
  }

  printf("AES-CMAC Tag:\n");
  buffer_print(&tag, sizeof(tag));

  /* Make sure tag is correct */
  if (memcmp(&aes_cmac_tag, &tag, sizeof(tag)) == 0) {
    printf("AES-CMAC generation succeeded\n");
  }
  else {
    printf("AES-CBC generation failed\n");
  }
  printf("\n");
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  /* AES-CBC encryption example */
  aes_cbc_encrypt_example();

  /* AES-CBC decryption example */
  aes_cbc_decrypt_example();

  /* AES-CMAC example */
  aes_cmac_example();

  return 0;
}

