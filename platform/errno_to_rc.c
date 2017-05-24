#include <stdlib.h>

#include <platform/common.h>
#include <atlk/sdk.h>
#include <atlk/compiler.h>

/* Convert standard errno to atlk_rc_t */
atlk_rc_t atlk_must_check
errno_to_atlk_rc(int _errno)
{
  /* Static - hence all non-initialized cells are zeroed */
  static const uint8_t rc[] = {
    [0] = ATLK_OK,
    [EINVAL] = ATLK_E_INVALID_ARG,
    [EWOULDBLOCK] = ATLK_E_NOT_READY,
    [ETIMEDOUT] = ATLK_E_TIMEOUT,
    [EDOM] = ATLK_E_OUT_OF_DOMAIN,
    [ERANGE] = ATLK_E_OUT_OF_RANGE,
    [ENOMEM] = ATLK_E_OUT_OF_MEMORY,
    [EADDRINUSE] = ATLK_E_ADDRESS_IN_USE,
    [ECONNREFUSED] = ATLK_E_CONNECTION_REFUSED,
    [ECONNRESET] = ATLK_E_CONNECTION_LOST,
    [EIO] = ATLK_E_IO_ERROR,
  };

  /* Check if _errno is out of bounds */
  if (atlk_unlikely(_errno < 0 || _errno >= ARRAY_SIZE(rc))) {
    return ATLK_E_UNSPECIFIED;
  }

  /* Handle all zeroed array cells (except for index 0) */
  if (likely(_errno != 0) && rc[_errno] == 0) {
    return ATLK_E_UNSPECIFIED;
  }

  return rc[_errno];
}

