#ifndef _LIBSNMP_SNMP_H
#define _LIBSNMP_SNMP_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <atlk/sdk.h>
#include <atlk/mib_service.h>
#include <platform/common.h>
#include <atlk/compiler.h>
#include <atlk/eui48.h>

#include "oid.h"

#include <atlk/remote.h>
#include "mib_remote_internal.h"

/* SNMPv1 */
#define SNMP_VERSION 0
#define SNMP_COMMUNITY "public"
#define SNMP_MAX_OID_STR_LEN 50

/* Values of MIB TruthValue type */
enum mib_truthvalue {
  MIB_TRUTHVALUE_TRUE = 1,
  MIB_TRUTHVALUE_FALSE = 2
};

/* Convert MIB TruthValue to standard c boolean */
static inline int
mib_truthvalue_to_bool(enum mib_truthvalue val)
{
  return val == MIB_TRUTHVALUE_TRUE;
}

/* Convert standard c boolean to MIB TruthValue */
static inline enum mib_truthvalue
mib_truthvalue_from_bool(int val)
{
  return val ? MIB_TRUTHVALUE_TRUE : MIB_TRUTHVALUE_FALSE;
}

static inline remote_transport_t *
mib_transport_from_service(mib_service_t *mib_service)
{
  remote_transport_t *remote_transport = NULL;
  atlk_rc_t rc;

  /* Acquire remote_transport */
  rc = mib_remote_service_transport_get(mib_service,
                                        &remote_transport);
  BUG_ON(atlk_error(rc) != 0);

  return remote_transport;
}

static inline int
mib_transport_handle_from_service(mib_service_t *mib_service)
{
  int remote_transport_handle;
  atlk_rc_t rc;

  /* Acquire remote_transport */
  rc = mib_remote_service_transport_handle_get(mib_service,
                                               &remote_transport_handle);
  BUG_ON(atlk_error(rc) != 0);

  return remote_transport_handle;
}

#define SNMP_DISPLAY_STRING_SCALAR_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     char *value, \
                     size_t *size) \
  { \
    atlk_rc_t rc; \
    struct oid oid; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_string_GetRequest(mib_transport_from_service(service), \
                                   mib_transport_handle_from_service(service), \
                                   &oid, \
                                   value, \
                                   size); \
  }

#define SNMP_OCTET_MAC_ADDR_COLUMN_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     eui48_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_mac_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value); \
  }

#define SNMP_OCTET_MAC_ADDR_COLUMN_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     eui48_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_mac_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value); \
  }


#define SNMP_Unsigned16_COLUMN_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     uint16_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    uint32_t value_32bit = (uint32_t)value; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    if (value_32bit > UINT16_MAX) { \
      return ATLK_E_OUT_OF_RANGE; \
    } \
    return snmp_send_gauge_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value_32bit); \
  }

#define SNMP_Unsigned16_COLUMN_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     uint16_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    uint32_t value_32bit; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    rc = snmp_send_gauge_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  &value_32bit); \
    *value = (uint16_t)value_32bit; \
    if (value_32bit > UINT16_MAX) { \
      *value = (uint16_t)0; \
      return ATLK_E_OUT_OF_RANGE; \
    } \
    return rc; \
  }

#define SNMP_Integer32_COLUMN_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     int32_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_int32_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);  \
  }

#define SNMP_Integer32_COLUMN_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     int32_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_int32_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);   \
  }

#define SNMP_Unsigned64_COLUMN_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     uint64_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_uint64_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);  \
  }

#define SNMP_Unsigned64_COLUMN_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     uint64_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_uint64_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);   \
  }

#define SNMP_Unsigned16_SCALAR_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     uint16_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    uint32_t value_32bit; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = snmp_send_uint32_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  &value_32bit);  \
    *value = (uint16_t)value_32bit; \
    if (value_32bit > UINT16_MAX) { \
      *value = (uint16_t)0; \
      return ATLK_E_OUT_OF_RANGE; \
    } \
    return rc; \
  }

#define SNMP_Unsigned16_SCALAR_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     uint16_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    uint32_t value_32bit = (((uint32_t)value) & UINT16_MAX); \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    if (value_32bit > UINT16_MAX) { \
      return ATLK_E_OUT_OF_RANGE; \
    } \
    return snmp_send_uint32_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value_32bit);   \
  }

#define SNMP_Integer32_SCALAR_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_int32_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);  \
  }

#define SNMP_Integer32_SCALAR_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_int32_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);   \
  }

#define SNMP_ENUM_COLUMN_GET(mib_name,enum_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     enum_name *value) \
  { \
    int int_value; \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    rc = snmp_send_int32_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  &int_value);       \
    if (atlk_error(rc)) { \
      return rc; \
    } \
    *value = int_value; \
    return rc; \
  }

#define SNMP_ENUM_COLUMN_SET(mib_name,enum_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     enum_name value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_int32_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);  \
  }

#define SNMP_ENUM_SCALAR_GET(mib_name,enum_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     enum_name *value) \
  { \
    int int_value; \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = snmp_send_int32_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  &int_value); \
    if (atlk_error(rc)) { \
      return rc; \
    } \
    *value = int_value; \
    return rc; \
  }

#define SNMP_ENUM_SCALAR_SET(mib_name,enum_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     enum_name value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_int32_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);  \
  }

#define SNMP_TRUTHVALUE_COLUMN_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     int *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    int32_t truthvalue; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    rc = snmp_send_int32_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  &truthvalue); \
    if (atlk_error(rc)) { \
      return rc; \
    } \
    *value = mib_truthvalue_to_bool(truthvalue); \
    return rc; \
  }

#define SNMP_TRUTHVALUE_COLUMN_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     int value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_int32_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  mib_truthvalue_from_bool(value)); \
  }

#define SNMP_TRUTHVALUE_SCALAR_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    int32_t truthvalue; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = snmp_send_int32_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  &truthvalue); \
    if (atlk_error(rc)) { \
      return rc; \
    } \
    *value = mib_truthvalue_to_bool(truthvalue); \
    return rc; \
  }

#define SNMP_TRUTHVALUE_SCALAR_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_int32_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  mib_truthvalue_from_bool(value)); \
  }

#define SNMP_COUNTER_COLUMN_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     uint32_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_counter_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value); \
  }

#define SNMP_COUNTER_COLUMN_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     uint32_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_counter_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value); \
  }

#define SNMP_UNSIGNED32_COLUMN_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     uint32_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_gauge_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value); \
  }

#define SNMP_UNSIGNED32_COLUMN_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     uint32_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_gauge_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value); \
  }

#define SNMP_COUNTER_SCALAR_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     uint32_t *value) \
  { \
    struct oid oid = OID_INIT_VAL; \
    atlk_rc_t rc; \
    \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_counter_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);  \
  }

#define SNMP_OCTET_STRING_COLUMN_GET(mib_name, oid_str)   \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     char *value, \
                     size_t *size) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    if (unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    if (unlikely(!size)) { \
      TR_ERROR("Mandatory argument 'size' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    rc = snmp_send_string_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value, \
                                  size); \
    return rc; \
  }

#define SNMP_OCTET_STRING_COLUMN_SET(mib_name, oid_str)   \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     int32_t if_index, \
                     const char *value, \
                     size_t size) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    \
    if (unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    rc = oid_add_index(&oid, if_index); \
    if (atlk_error(rc)) { \
      TR_ERROR("Cannot add if_index to OID"); \
      return rc; \
    } \
    return snmp_send_string_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value, \
                                  size); \
  }

#define SNMP_UNSIGNED32_SCALAR_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     uint32_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_uint32_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);  \
  }

#define SNMP_UNSIGNED32_SCALAR_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     uint32_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_uint32_SetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value); \
  }

#define SNMP_IPv4_SCALAR_GET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_get_##mib_name(mib_service_t *service, \
                     uint32_t *value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    \
    if (atlk_unlikely(!value)) { \
      TR_ERROR("Mandatory argument 'value' is not specified"); \
      return ATLK_E_INVALID_ARG; \
    } \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_uint32_GetRequest(mib_transport_from_service(service), \
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);  \
  }

#define SNMP_IPv4_SCALAR_SET(mib_name,oid_str) \
  atlk_rc_t atlk_must_check \
  mib_set_##mib_name(mib_service_t *service, \
                     uint32_t value) \
  { \
    atlk_rc_t rc; \
    struct oid oid = OID_INIT_VAL; \
    \
    rc = oid_from_str(oid_str, &oid); \
    if (atlk_error(rc)) { \
      TR_ERROR("Could not resolve OID string %s", oid_str); \
      return rc; \
    } \
    return snmp_send_addr_ipv4_SetRequest(mib_transport_from_service(service),\
                                  mib_transport_handle_from_service(service), \
                                  &oid, \
                                  value);   \
  }

enum object_type {
  OBJECT_TYPE_INT32,
  OBJECT_TYPE_UINT32,
  OBJECT_TYPE_GAUGE,
  OBJECT_TYPE_OCTET_STR,
  OBJECT_TYPE_INETADDRESSIPV4,
  OBJECT_TYPE_UINT64,
  OBJECT_TYPE_COUNTER64
};

/**
 Retrieve MIB Manager
 @return Pointer to MIB manager
 */
struct mib_mgr *
snmp_get_manager(void);

/**
 Request integer value from remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[out] value Returned value

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_int32_GetRequest(remote_transport_t *remote_transport,
                           int transport_handle,
                           struct oid *oid,
                           int32_t *value);

/**
 Set integer value for remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[in] value Value to be set

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_int32_SetRequest(remote_transport_t *remote_transport,
                           int transport_handle,
                           struct oid *oid,
                           int32_t value);

/**
 Set inetaddressipv4 value for remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[in] value Value to be set

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check  atlk_rc_t
snmp_send_addr_ipv4_SetRequest(remote_transport_t *remote_transport,
                               int transport_handle,
                               struct oid *oid,
                               uint32_t value);

/**
 Request unsigned integer value from remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[out] value Returned value

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_uint32_GetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            uint32_t *value);

/**
 Set unsigned integer value for remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[in] value Value to be set

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_uint32_SetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            uint32_t value);

/**
 Request unsigned64 integer value from remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[out] value Returned value

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check  atlk_rc_t
snmp_send_uint64_GetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            uint64_t *value);

/**
 Set unsigned64 integer value for remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[in] value Value to be set

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check  atlk_rc_t
snmp_send_uint64_SetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            uint64_t value);

/**
 Request counter value from remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[out] value Returned value

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_counter_GetRequest(remote_transport_t *remote_transport,
                             int transport_handle,
                             struct oid *oid,
                             uint32_t *value);

/**
 Set counter value for remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[in] value Value to be set

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_counter_SetRequest(remote_transport_t *remote_transport,
                             int transport_handle,
                             struct oid *oid,
                             uint32_t value);

/**
 Request gauge value from remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[out] value Returned value

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_gauge_GetRequest(remote_transport_t *remote_transport,
                           int transport_handle,
                           struct oid *oid,
                           uint32_t *value);

/**
 Set gauge value for remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[in] value Value to be set

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_gauge_SetRequest(remote_transport_t *remote_transport,
                           int transport_handle,
                           struct oid *oid,
                           uint32_t value);

/**
 Request MAC address from remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[out] value Returned MAC address

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_mac_GetRequest(remote_transport_t *remote_transport,
                         int transport_handle,
                         struct oid *oid,
                         eui48_t *value);

/**
 Set MAC address for remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[in] value MAC address to be set

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_mac_SetRequest(remote_transport_t *remote_transport,
                         int transport_handle,
                         struct oid *oid,
                         eui48_t value);

/**
 Request octet string from remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[out] value Returned octet string
 @param[int, out] size Length of octet string

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_string_GetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            char *value,
                            size_t *size);

/**
 Set octet string for remote target

 @param[in] remote_transport remote_transport associated with MIB service
 @param[in] transport_handle Handle associated with the remote_transport
 @param[in] oid Object identifier of target API
 @param[in] value Octet string to be set
 @param[in] size Size of string to be set

 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
snmp_send_string_SetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            const char *value,
                            size_t size);

#endif /* _LIBSNMP_SNMP_H */
