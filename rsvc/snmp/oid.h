#ifndef _LIBSNMP_OID_H
#define _LIBSNMP_OID_H

#include <stdint.h>

#include <atlk/sdk.h>
#include <platform/common.h>

#define OID_MAX_LENGTH 20

#define OID_INIT_VAL {.count = 0,}

typedef uint16_t oid_element_t;

/* Representation of SNMP object identifier (OID) */
struct oid {
  /* Elements of OID */
  oid_element_t element[OID_MAX_LENGTH];
  /* Number of elements */
  size_t count;
};

/**
 Convert object identifier string into OID structure
 @param[in] str String representing the object identifier
 @param[out] oid Object identifier structure
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
oid_from_str(const char *str, struct oid *oid);

/**
 Add index to existing object identifier
 @param[in, out] oid Object identifier structure
 @param[in] index Index to be appended to object identifier
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
oid_add_index(struct oid *oid, oid_element_t index);

/**
 Check if two OIDs match
 @param[in] first First object identifier structure
 @param[in] first Second object identifier structure
 @return 1 if the OIDs match, 0 if do not match
 */
atlk_must_check int
oid_is_match(const struct oid *first, const struct oid *second);

#endif /* _LIBSNMP_OID_H */
