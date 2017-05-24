#include <string.h>

#include "oid.h"

atlk_must_check atlk_rc_t
oid_from_str(const char *str, struct oid *oid)
{
  char ch;
  uint32_t number = 0;
  uint32_t index = 0;

  if (!str || !oid) {
    return ATLK_E_INVALID_ARG;
  }

  memset(oid, 0x00, sizeof(*oid));

  while ((ch = *str++)) {
    if (ch >= '0' && ch <= '9') {
      number = 10 * number + ch - '0';
      continue;
    }
    if (ch == '.') {
      if (index >= OID_MAX_LENGTH) {
        TR_ERROR("OID exceeded %d elements", index);
        return ATLK_E_OUT_OF_BOUNDS;
      }
      oid->element[index++] = number;
      number = 0;
      continue;
    }
    TR_ERROR("Unknown charatcer %c (0x%02X)", ch, ch);
    return ATLK_E_INVALID_ARG;
  }
  /* Take last number */
  oid->element[index++] = number;
  /* Update OID count */
  oid->count = index;
  return ATLK_OK;  
}

atlk_must_check atlk_rc_t
oid_add_index(struct oid *oid, oid_element_t index)
{
  if (!oid) {
    return ATLK_E_INVALID_ARG;
  }
  if (oid->count >= OID_MAX_LENGTH) {
    return ATLK_E_OUT_OF_BOUNDS;
  }
  oid->element[oid->count] = index;
  oid->count++;
  return ATLK_OK;

}

atlk_must_check int
oid_is_match(const struct oid *first, const struct oid *second)
{
  int index;
  if (first->count != second->count) {
    return 0;
  }
  for (index = 0; index < first->count; index++) {
    if (first->element[index] != second->element[index]) {
      return 0;
    }
  }
  return 1;
}
