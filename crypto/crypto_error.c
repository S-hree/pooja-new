#include <tomcrypt.h>

#include <atlk/sdk.h>

#include <platform/common.h>

/* Convert LibTomCrypt error to atlk_rc_t */
atlk_rc_t
crypto_error_to_rc(int error)
{
  /* Static - hence all non-initialized cells are zeroed */
  static const uint8_t rc[] = {
    [CRYPT_OK] = ATLK_OK,
    [CRYPT_ERROR] = ATLK_E_UNSPECIFIED,
    [CRYPT_NOP] = ATLK_E_UNSPECIFIED,
    [CRYPT_INVALID_KEYSIZE] = ATLK_E_INVALID_ARG,
    [CRYPT_INVALID_ROUNDS] = ATLK_E_INVALID_ARG,
    [CRYPT_FAIL_TESTVECTOR] = ATLK_E_UNSPECIFIED,
    [CRYPT_BUFFER_OVERFLOW] = ATLK_E_BUFFER_TOO_SMALL,
    [CRYPT_INVALID_PACKET] = ATLK_E_INVALID_ARG,
    [CRYPT_INVALID_PRNGSIZE] = ATLK_E_INVALID_ARG,
    [CRYPT_ERROR_READPRNG] = ATLK_E_IO_ERROR,
    [CRYPT_INVALID_CIPHER] = ATLK_E_INVALID_ARG,
    [CRYPT_INVALID_HASH] = ATLK_E_INVALID_ARG,
    [CRYPT_INVALID_PRNG] = ATLK_E_INVALID_ARG,
    [CRYPT_MEM] = ATLK_E_OUT_OF_MEMORY,
    [CRYPT_PK_TYPE_MISMATCH] = ATLK_E_UNSPECIFIED,
    [CRYPT_PK_NOT_PRIVATE] = ATLK_E_INVALID_ARG,
    [CRYPT_INVALID_ARG] = ATLK_E_INVALID_ARG,
    [CRYPT_FILE_NOTFOUND] = ATLK_E_NOT_FOUND,
    [CRYPT_PK_INVALID_TYPE] = ATLK_E_INVALID_ARG,
    [CRYPT_PK_INVALID_SYSTEM] = ATLK_E_INVALID_ARG,
    [CRYPT_PK_DUP] = ATLK_E_EXISTS,
    [CRYPT_PK_NOT_FOUND] = ATLK_E_NOT_FOUND,
    [CRYPT_PK_INVALID_SIZE] = ATLK_E_INVALID_ARG,
    [CRYPT_INVALID_PRIME_SIZE] = ATLK_E_INVALID_ARG,
    [CRYPT_PK_INVALID_PADDING] = ATLK_E_INVALID_ARG,
  };

  /* Check if error is out of bounds */
  if (atlk_unlikely(error < 0 || error >= ARRAY_SIZE(rc))) {
    return ATLK_E_UNSPECIFIED;
  }

  /* Handle all zeroed array cells (except for index 0) */
  if (likely(error != 0) && rc[error] == 0) {
    return ATLK_E_UNSPECIFIED;
  }

  return rc[error];
}

