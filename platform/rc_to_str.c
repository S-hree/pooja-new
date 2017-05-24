#include <platform/common.h>
#include <atlk/sdk.h>
#include <atlk/compiler.h>

/* Convert atlk_rc_t to human-readable error message */
const char *
atlk_rc_to_str(atlk_rc_t rc)
{
  static const char *err_str[] = {
    [ATLK_OK] = "Operation successful",
    [ATLK_E_UNSPECIFIED] = "Unspecified error",
    [ATLK_E_INVALID_ARG] = "Invalid argument",
    [ATLK_E_UNSUPPORTED] = "Operation not supported",
    [ATLK_E_INVALID_STATE] = "Object in invalid state",
    [ATLK_E_NOT_FOUND] = "Object not found",
    [ATLK_E_EXISTS] = "Object already exists",
    [ATLK_E_NOT_READY] = "Not ready to perform operation",
    [ATLK_E_TIMEOUT] = "Operation timed out",
    [ATLK_E_OUT_OF_DOMAIN] = "Numerical argument out of domain",
    [ATLK_E_OUT_OF_RANGE] = "Numerical result out of range",
    [ATLK_E_OUT_OF_MEMORY] = "Failed to allocate memory",
    [ATLK_E_ADDRESS_IN_USE] = "Address already in use",
    [ATLK_E_CONNECTION_REFUSED] = "Connection refused",
    [ATLK_E_CONNECTION_LOST] = "Connection lost",
    [ATLK_E_PROTOCOL_ERROR] = "Protocol error",
    [ATLK_E_PROTOCOL_MISMATCH] = "Protocol version mismatch",
    [ATLK_E_OUT_OF_BOUNDS] = "Array access out of bounds",
    [ATLK_E_BAD_ALIGNMENT] = "Address not aligned as required",
    [ATLK_E_BUFFER_TOO_SMALL] = "Buffer is too small",
    [ATLK_E_IO_ERROR] = "Input/output error",
  };

  /* Check if rc is out of bounds */
  if (atlk_unlikely(rc >= ARRAY_SIZE(err_str))) {
    return "Unknown error code";
  }

  return err_str[rc];
}
