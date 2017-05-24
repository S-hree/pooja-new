#include <string.h>
#include <errno.h>

#include <atlk/compiler.h>
#include <atlk/mib_remote.h>
#include <atlk/remote_internal.h>

#include "snmp.h"
#include "asn1.h"
#include "mib_remote_internal.h"

#define SNMP_MAX_PDU 2000
#define SNMP_MAX_VAR_SIZE 1500
#define SNMP_REQUEST_ID_BASE 0x10000

#define UDP_TRANSPORT_SNMP_PORT 161

struct sequence {
  uint8_t *size_ptr;
  uint8_t *start_offset;
};

/* Request type (SetRequest/GetRequest) */
enum snmp_request_type {
  SNMP_REQUEST_SET,
  SNMP_REQUEST_GET
};

struct snmp_object {
  /* Pointer to object to be set */
  uint8_t *ptr;
  /* Object type */
  enum object_type type;
  /* Object size in bytes */
  size_t size;
  /* Sequence number of get/set request */
  uint32_t seq_num;
  /* Sequence number of get/set request */
  enum snmp_request_type request_type;
};

static uint32_t snmp_request_id = SNMP_REQUEST_ID_BASE;

static atlk_rc_t
snmp_compose_request(uint8_t *buffer,
                     size_t *buffer_size,
                     const struct snmp_object *obj,
                     struct oid *oid)
{
  struct sequence seq_all, seq_req, seq_vars, seq_oid;
  int32_t int_value;
  uint64_t uint64_value;
  struct memio memio;
  atlk_rc_t rc;

  if (!buffer || !buffer_size || !oid) {
    TR_ERROR("Invalid arguments");
    return ATLK_E_INVALID_ARG;
  }

  memio_init(&memio, buffer, *buffer_size);

  rc = asn1_encode_sequence(&memio, &seq_all.size_ptr); /* Sequence */
  if (atlk_error(rc)) {
    TR_ERROR("Encode primary sequence failed");
    return rc;
  }

  seq_all.start_offset = memio.next;
  rc = asn1_encode_integer(&memio, 0); /* SNMPv1 version */
  if (atlk_error(rc)) {
    TR_ERROR("Encode SNMP version failed");
    return rc;
  }

  rc = asn1_encode_octet_string(&memio, (uint8_t *)SNMP_COMMUNITY, sizeof(SNMP_COMMUNITY) - 1); /* Community */
  if (atlk_error(rc)) {
    TR_ERROR("Encode community failed");
    return rc;
  }
  if (obj->request_type == SNMP_REQUEST_GET) {
    rc = asn1_encode_snmp_GetRequest(&memio, &seq_req.size_ptr); /* SNMP get request */
  }
  else {
    rc = asn1_encode_snmp_SetRequest(&memio, &seq_req.size_ptr); /* SNMP set request */
  }
  if (atlk_error(rc)) {
    TR_ERROR("Encode get/set request failed");
    return rc;
  }

  seq_req.start_offset = memio.next;
  rc = asn1_encode_integer(&memio, obj->seq_num); /* Request ID */
  if (atlk_error(rc)) {
    TR_ERROR("Encode request ID failed");
    return rc;
  }

  rc = asn1_encode_integer(&memio, 0); /* Error status */
  if (atlk_error(rc)) {
    TR_ERROR("Encode error status failed");
    return rc;
  }

  rc = asn1_encode_integer(&memio, 0); /* Error index */
  if (atlk_error(rc)) {
    TR_ERROR("Encode error index failed");
    return rc;
  }

  rc = asn1_encode_sequence(&memio, &seq_vars.size_ptr); /* Variables sequence */
  if (atlk_error(rc)) {
    TR_ERROR("Encode vairiables sequence failed");
    return rc;
  }

  seq_vars.start_offset = memio.next;
  rc = asn1_encode_sequence(&memio, &seq_oid.size_ptr); /* OID sequence */
  if (atlk_error(rc)) {
    TR_ERROR("Encode OID sequence failed");
    return rc;
  }

  seq_oid.start_offset = memio.next;
  rc = asn1_encode_oid(&memio, oid);
  if (atlk_error(rc)) {
    TR_ERROR("Encode OID failed");
    return rc;
  }

  if (obj->request_type == SNMP_REQUEST_GET) {
    rc = asn1_encode_termination(&memio);
    if (atlk_error(rc)) {
      TR_ERROR("Encode termination failed");
      return rc;
    }
  }
  else {
    switch(obj->type) {
    case OBJECT_TYPE_INETADDRESSIPV4:
    case OBJECT_TYPE_INT32:
    case OBJECT_TYPE_UINT32:
    case OBJECT_TYPE_GAUGE:
      if (obj->size != sizeof(int_value)) {
        TR_ERROR("Wrong size for integer");
        return ATLK_E_PROTOCOL_ERROR;
      }
      memcpy(&int_value, obj->ptr, sizeof(int_value));
      if (obj->type != OBJECT_TYPE_GAUGE) {
        if (obj->type == OBJECT_TYPE_INETADDRESSIPV4) {
          rc = asn1_encode_inetaddressipv4(&memio, int_value);
        }
        else {
          rc = asn1_encode_integer(&memio, int_value);
        }
      }
      else {
        rc = asn1_encode_gauge(&memio, int_value);
      }
      if (atlk_error(rc)) {
        TR_ERROR("Encode integer failed");
        return rc;
      }
      memcpy(&int_value, obj->ptr, sizeof(int_value));
      break;

    case OBJECT_TYPE_UINT64:
    case OBJECT_TYPE_COUNTER64:
      if (obj->size != sizeof(uint64_value)) {
        TR_ERROR("Wrong size for counter64");
        return ATLK_E_PROTOCOL_ERROR;
      }

      memcpy(&uint64_value, obj->ptr, sizeof(uint64_value));

      rc = asn1_encode_counter64(&memio, uint64_value);
      if (atlk_error(rc)) {
        TR_ERROR("Encode integer failed");
        return rc;
      }
      memcpy(&uint64_value, obj->ptr, sizeof(uint64_value));
      break;

    case OBJECT_TYPE_OCTET_STR:
      if (atlk_unlikely(obj->size > SNMP_MAX_VAR_SIZE)) {
        TR_ERROR("Invalid size of octet string");
        return ATLK_E_PROTOCOL_ERROR;
      }
      rc = asn1_encode_octet_string(&memio, obj->ptr, obj->size);
      if (atlk_error(rc)) {
        TR_ERROR("Encode integer failed");
        return rc;
      }
      break;
    default:
      TR_ERROR("Invalid object type (%d)", obj->type);
      return ATLK_E_PROTOCOL_ERROR;
      break;
    }
  }
  /* Update sizes */
  rc = asn1_fill_field_size(seq_all.size_ptr, (uint8_t*)memio.next - seq_all.start_offset);
  if (atlk_error(rc)) {
    TR_ERROR("Fill sequence size failed");
    return rc;
  }

  rc = asn1_fill_field_size(seq_req.size_ptr, (uint8_t*)memio.next - seq_req.start_offset);
  if (atlk_error(rc)) {
    TR_ERROR("Fill request sequence size failed");
    return rc;
  }

  rc = asn1_fill_field_size(seq_vars.size_ptr, (uint8_t*)memio.next - seq_vars.start_offset);
  if (atlk_error(rc)) {
    TR_ERROR("Fill variables sequence size failed");
    return rc;
  }

  rc = asn1_fill_field_size(seq_oid.size_ptr, (uint8_t*)memio.next - seq_oid.start_offset);
  if (atlk_error(rc)) {
    TR_ERROR("Fill OID sequence size failed");
    return rc;
  }

  *buffer_size = (uint8_t*)memio.next - buffer;
  return ATLK_OK;
}

static atlk_rc_t
snmp_decode_response(const uint8_t *buffer,
                     size_t in_size,
                     struct snmp_object *obj,
                     struct oid *oid)
{
  size_t len;
  int version;
  char *octet_str;
  int err_status;
  int seq_num;
  struct memio memio;
  atlk_rc_t rc;

  if (!oid) {
    TR_ERROR("Invalid OID argument");
    return ATLK_E_INVALID_ARG;
  }

  memio_init(&memio, (void*)buffer, in_size);
  /* Decode total sequence */
  rc = asn1_decode_sequence(&memio, &len);
  if (atlk_error(rc)) {
    TR_ERROR("Decode sequence failed");
    return rc;
  }
  /* Check sequence length */
  if (len != in_size - ((uint8_t*)memio.next - buffer)) {
    TR_ERROR("Wrong sequence: Expected %zu, got %zu",
             in_size - ((uint8_t*)memio.next - buffer),
             len);
    return ATLK_E_PROTOCOL_ERROR;
  }
  /* Check version */
  rc = asn1_decode_integer(&memio, &version);
  if (atlk_error(rc)) {
    TR_ERROR("Decode version failed");
    return rc;
  }
  /* Currently SNMPv1 is supported */
  if (version != SNMP_VERSION) {
    TR_ERROR("Incorrect SNMP version, expected %d (SNMPv1), got %d",
             SNMP_VERSION,
             version);
    return ATLK_E_PROTOCOL_ERROR;
  }
  /* Check community */
  rc = asn1_decode_octet_string(&memio, &octet_str, &len);
  if (atlk_error(rc)) {
    TR_ERROR("Decode community failed");
    return ATLK_E_PROTOCOL_ERROR;
  }
  if (memcmp(SNMP_COMMUNITY, octet_str, sizeof(SNMP_COMMUNITY) - 1)) {
    TR_ERROR("Expected community %s, starts with %02X", SNMP_COMMUNITY, octet_str[0]);
    return ATLK_E_PROTOCOL_ERROR;
  }

  rc = asn1_decode_snmp_GetResponse(&memio, &len);
  if (atlk_error(rc)) {
    TR_ERROR("Decode get response failed");
    return rc;
  }
  /* Check response length */
  if (len != in_size - ((uint8_t*)memio.next - buffer)) {
    TR_ERROR("Wrong sequence: Expected %zu, got %zu",
             in_size - ((uint8_t*)memio.next - buffer),
             len);
    return ATLK_E_PROTOCOL_ERROR;
  }
  /* Decode sequence number */
  rc = asn1_decode_integer(&memio, &seq_num);
  if (atlk_error(rc)) {
    TR_ERROR("Decode seuqnce number failed");
    return rc;
  }
  obj->seq_num = (uint32_t)seq_num;
  /* Decode error status */
  rc = asn1_decode_integer(&memio, &err_status);
  if (atlk_error(rc)) {
    TR_ERROR("Decode error status failed");
    return rc;
  }
  if (err_status != 0) {
    TR_ERROR("SNMP error (%d)", err_status);
    /* TODO: This is a valid state when operation not permitted on target */
    return ATLK_E_PROTOCOL_ERROR;
  }
  /* Decode error index */
  rc = asn1_decode_integer(&memio, &err_status);
  if (atlk_error(rc)) {
    TR_ERROR("Decode error index failed");
    return rc;
  }
  /* Decode variable sequence */
  rc = asn1_decode_sequence(&memio, &len);
  if (atlk_error(rc)) {
    TR_ERROR("Decode variables sequence failed");
    return rc;
  }
  /* Check sequence length */
  if (len != in_size - ((uint8_t*)memio.next - buffer)) {
    TR_ERROR("Wrong sequence: Expected %zu, got %zu",
             in_size - ((uint8_t*)memio.next - buffer),
             len);
    return ATLK_E_PROTOCOL_ERROR;
  }
  /* Decode first (and only) variable sequence */
  rc = asn1_decode_sequence(&memio, &len);
  if (atlk_error(rc)) {
    TR_ERROR( "Decode variable sequence failed");
    return rc;
  }
  /* Check sequence length */
  if (len != in_size - ((uint8_t*)memio.next - buffer)) {
    TR_ERROR("Wrong sequence: Expected %zu, got %zu",
             in_size - ((uint8_t*)memio.next - buffer),
             len);
    return ATLK_E_PROTOCOL_ERROR;
  }
  /* Decode object identifier (OID) */
  rc = asn1_decode_oid(&memio, oid);
  if (atlk_error(rc)) {
    TR_ERROR("Decode OID failed");
    return rc;
  }
  rc = asn1_decode_variable(&memio, obj->ptr, &obj->size);
  if (atlk_error(rc)) {
    TR_ERROR("Decode variable failed");
    return rc;
  }
  return ATLK_OK;
}

static atlk_rc_t
snmp_send_GetRequest(remote_transport_t *remote_transport,
                     int transport_handle,
                     struct oid *oid,
                     void *resp,
                     size_t *resp_size)
{
  uint8_t buffer[SNMP_MAX_PDU];
  size_t buffer_size;
  atlk_rc_t rc;
  struct snmp_object req_obj, resp_obj;
  struct oid response_oid;
  uint16_t remote_port;

  if (!remote_transport || !oid || !resp || !resp_size) {
    TR_ERROR("Invalid arguments");
    return ATLK_E_INVALID_ARG;
  }

  memset(&req_obj, 0x00, sizeof(req_obj));
  req_obj.seq_num = snmp_request_id;
  snmp_request_id++;
  req_obj.request_type = SNMP_REQUEST_GET;
  buffer_size = sizeof(buffer);
  rc = snmp_compose_request(buffer, &buffer_size, &req_obj, oid);
  if (atlk_error(rc)) {
    TR_ERROR("Failed composing SNMP get request, error %d", rc);
    return rc;
  }

  /* Send the request */
  rc = remote_transport_send(remote_transport,
                             transport_handle,
                             UDP_TRANSPORT_SNMP_PORT,
                             buffer,
                             &buffer_size);
  if (atlk_error(rc)) {
    TR_ERROR("Could not send PDU, rc = %d, buffer_size = %d",
              rc,
              (int)buffer_size);
    return rc;
  }

  /* Get response */
  buffer_size = sizeof(buffer);
  rc = remote_transport_receive(remote_transport,
                                transport_handle,
                                buffer,
                                &buffer_size,
                                REMOTE_TRANSPORT_DEFAULT_RTT,
                                &remote_port);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to receive PDU, rc = %d, buffer_size = %d",
                rc,
                (int)buffer_size);
    return rc;
  }

  resp_obj.ptr = resp;
  resp_obj.size = *resp_size;
  rc = snmp_decode_response(buffer,
                            buffer_size,
                            &resp_obj,
                            &response_oid);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to decode response PDU for get request");
    return rc;
  }

  *resp_size = resp_obj.size;
  if (!oid_is_match(oid, &response_oid)) {
    TR_ERROR("Object identifiers do not match for get request");
    return ATLK_E_PROTOCOL_ERROR;
  }
  if (req_obj.seq_num != resp_obj.seq_num) {
    TR_ERROR("Request ID doesn't match (%u vs %u)",
             req_obj.seq_num, resp_obj.seq_num);
    return ATLK_E_PROTOCOL_ERROR;
  }
  return rc;
}

static atlk_rc_t
snmp_send_SetRequest(remote_transport_t *remote_transport,
                     int transport_handle,
                     struct oid *oid,
                     enum object_type type,
                     const void *value,
                     size_t value_size,
                     void *resp,
                     size_t *resp_size)
{
  uint8_t buffer[SNMP_MAX_PDU];
  size_t buffer_size;
  atlk_rc_t rc;
  struct snmp_object req_obj, resp_obj;
  struct oid response_oid;
  uint16_t remote_port;

  if (!remote_transport || !oid || !value || !resp || !resp_size) {
    TR_ERROR("Invalid arguments");
    return ATLK_E_INVALID_ARG;
  }

  req_obj.ptr = (uint8_t *)value;
  req_obj.size = value_size;
  req_obj.type = type;
  req_obj.request_type = SNMP_REQUEST_SET;
  req_obj.seq_num = snmp_request_id;
  snmp_request_id++;

  if (atlk_unlikely(value_size > SNMP_MAX_VAR_SIZE)) {
    TR_ERROR("Value size exceeded limit");
    return ATLK_E_OUT_OF_RANGE;
  }

  buffer_size = sizeof(buffer);
  rc = snmp_compose_request(buffer, &buffer_size, &req_obj, oid);
  if (atlk_error(rc)) {
    TR_ERROR("Failed composing SNMP get request");
    return rc;
  }

  /* Send the request */
  rc = remote_transport_send(remote_transport,
                             transport_handle,
                             UDP_TRANSPORT_SNMP_PORT,
                             buffer,
                             &buffer_size);
  if (atlk_error(rc)) {
    TR_ERROR("Could not send PDU, rc = %d, buffer_size = %d",
              rc,
              (int)buffer_size);
    return rc;
  }

  /* Get response */
  buffer_size = sizeof(buffer);
  rc = remote_transport_receive(remote_transport,
                                transport_handle,
                                buffer,
                                &buffer_size,
                                REMOTE_TRANSPORT_DEFAULT_RTT,
                                &remote_port);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to receive PDU, rc = %d, buffer_size = %d",
                rc,
                (int)buffer_size);
    return rc;
  }

  resp_obj.ptr = resp;
  resp_obj.size = *resp_size;
  rc = snmp_decode_response(buffer,
                            buffer_size,
                            &resp_obj,
                            &response_oid);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to decode response PDU for set request");
    return rc;
  }

  *resp_size= resp_obj.size;
  if (!oid_is_match(oid, &response_oid)) {
    TR_ERROR("Object identifiers do not match for set request");
    return ATLK_E_PROTOCOL_ERROR;
  }

  if (req_obj.seq_num != resp_obj.seq_num) {
    TR_ERROR("Request ID doesn't match (%u vs %u)",
             req_obj.seq_num, resp_obj.seq_num);
    return ATLK_E_PROTOCOL_ERROR;
  }

  return rc;
}

atlk_must_check atlk_rc_t
snmp_send_int32_GetRequest(remote_transport_t *remote_transport,
                           int transport_handle,
                           struct oid *oid,
                           int32_t *value)
{
  size_t int_size;

  int_size = sizeof(*value);

  return snmp_send_GetRequest(remote_transport,
                              transport_handle,
                              oid,
                              value,
                              &int_size);
}

atlk_must_check  atlk_rc_t
snmp_send_int32_SetRequest(remote_transport_t *remote_transport,
                           int transport_handle,
                           struct oid *oid,
                           int32_t value)
{
  int32_t returned_value;
  size_t int_size = sizeof(returned_value);
  atlk_rc_t rc;

  rc = snmp_send_SetRequest(remote_transport,
                            transport_handle,
                            oid,
                            OBJECT_TYPE_UINT32,
                            &value,
                            sizeof(value),
                            &returned_value,
                            &int_size);
  if (atlk_error(rc)) {
    return rc;
  }
  if (value != returned_value) {
    TR_ERROR("Could not set value (%u). Retrieved %u",
             value,
             returned_value);
    return ATLK_E_PROTOCOL_ERROR;
  }
  return ATLK_OK;
}

atlk_must_check  atlk_rc_t
snmp_send_uint32_GetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            uint32_t *value)
{
  size_t int_size;

  int_size = sizeof(*value);

  return snmp_send_GetRequest(remote_transport,
                              transport_handle,
                              oid,
                              value,
                              &int_size);
}

atlk_must_check  atlk_rc_t
snmp_send_uint32_SetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            uint32_t value)
{
  uint32_t returned_value;
  size_t int_size = sizeof(returned_value);
  atlk_rc_t rc;

  rc = snmp_send_SetRequest(remote_transport,
                            transport_handle,
                            oid,
                            OBJECT_TYPE_GAUGE,
                            &value,
                            sizeof(value),
                            &returned_value,
                            &int_size);
  if (atlk_error(rc)) {
    return rc;
  }
  if (value != returned_value) {
    TR_ERROR("Could not set value (%d). Retrieved %d",
             value,
             returned_value);
    return ATLK_E_PROTOCOL_ERROR;
  }
  return ATLK_OK;
}

atlk_must_check  atlk_rc_t
snmp_send_uint64_GetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            uint64_t *value)
{
  size_t int_size;

  int_size = sizeof(*value);

  return snmp_send_GetRequest(remote_transport,
                              transport_handle,
                              oid,
                              value,
                              &int_size);
}

atlk_must_check  atlk_rc_t
snmp_send_uint64_SetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            uint64_t value)
{
  uint64_t returned_value;
  size_t int_size = sizeof(returned_value);
  atlk_rc_t rc;

  rc = snmp_send_SetRequest(remote_transport,
                            transport_handle,
                            oid,
                            OBJECT_TYPE_UINT64,
                            &value,
                            sizeof(value),
                            &returned_value,
                            &int_size);
  if (atlk_error(rc)) {
    return rc;
  }
  if (value != returned_value) {
    TR_ERROR("Could not set value (%" PRIu64 "). Retrieved %" PRIu64,
             value,
             returned_value);
    return ATLK_E_PROTOCOL_ERROR;
  }
  return ATLK_OK;
}

atlk_must_check  atlk_rc_t
snmp_send_addr_ipv4_SetRequest(remote_transport_t *remote_transport,
                               int transport_handle,
                               struct oid *oid,
                               uint32_t value)
{
  uint32_t returned_value;
  size_t int_size = sizeof(returned_value);
  atlk_rc_t rc;

  rc = snmp_send_SetRequest(remote_transport,
                            transport_handle,
                            oid,
                            OBJECT_TYPE_INETADDRESSIPV4,
                            &value,
                            sizeof(value),
                            &returned_value,
                            &int_size);
  if (atlk_error(rc)) {
    return rc;
  }
  if (value != returned_value) {
    TR_ERROR("Could not set value (%d). Retrieved %d",
             value,
             returned_value);
    return ATLK_E_PROTOCOL_ERROR;
  }
  return ATLK_OK;
}

atlk_must_check  atlk_rc_t
snmp_send_counter_GetRequest(remote_transport_t *remote_transport,
                             int transport_handle,
                             struct oid *oid,
                             uint32_t *value)
{
  size_t int_size;

  int_size = sizeof(*value);

  return snmp_send_GetRequest(remote_transport,
                              transport_handle,
                              oid,
                              value,
                              &int_size);
}

atlk_must_check  atlk_rc_t
snmp_send_counter_SetRequest(remote_transport_t *remote_transport,
                             int transport_handle,
                             struct oid *oid,
                             uint32_t value)
{
  uint32_t returned_value;
  size_t int_size = sizeof(returned_value);
  atlk_rc_t rc;

  rc = snmp_send_SetRequest(remote_transport,
                            transport_handle,
                            oid,
                            OBJECT_TYPE_INT32,
                            &value,
                            sizeof(value),
                            &returned_value,
                            &int_size);
  if (atlk_error(rc)) {
    return rc;
  }
  if (value != returned_value) {
    TR_ERROR("Could not set value (%d). Retrieved %d",
             value,
             returned_value);
    return ATLK_E_PROTOCOL_ERROR;
  }
  return ATLK_OK;
}

atlk_must_check atlk_rc_t
snmp_send_gauge_GetRequest(remote_transport_t *remote_transport,
                           int transport_handle,
                           struct oid *oid,
                           uint32_t *value)
{
  size_t int_size;

  int_size = sizeof(*value);

  return snmp_send_GetRequest(remote_transport,
                              transport_handle,
                              oid,
                              value,
                              &int_size);
}

atlk_must_check atlk_rc_t
snmp_send_gauge_SetRequest(remote_transport_t *remote_transport,
                           int transport_handle,
                           struct oid *oid,
                            uint32_t value)
{
  uint32_t returned_value;
  size_t int_size = sizeof(returned_value);
  atlk_rc_t rc;

  rc = snmp_send_SetRequest(remote_transport,
                            transport_handle,
                            oid,
                            OBJECT_TYPE_GAUGE,
                            &value,
                            sizeof(value),
                            &returned_value,
                            &int_size);
  if (atlk_error(rc)) {
    return rc;
  }

  if (value != returned_value) {
    TR_ERROR("Could not set value (%d). Retrieved %d",
             value,
             returned_value);
    return ATLK_E_PROTOCOL_ERROR;
  }

  return ATLK_OK;
}

atlk_must_check atlk_rc_t
snmp_send_mac_GetRequest(remote_transport_t *remote_transport,
                         int transport_handle,
                         struct oid *oid,
                         eui48_t *value)
{
  atlk_rc_t rc;
  size_t mac_size = EUI48_LEN;

  rc = snmp_send_GetRequest(remote_transport,
                            transport_handle,
                            oid,
                            value->octets,
                            &mac_size);
  if (atlk_error(rc)) {
    return rc;
  }

  if (atlk_unlikely(mac_size != EUI48_LEN)) {
    TR_ERROR("Wrong MAC address size %zu", mac_size);
    return ATLK_E_INVALID_ARG;
  }

  return ATLK_OK;
}

atlk_must_check atlk_rc_t
snmp_send_mac_SetRequest(remote_transport_t *remote_transport,
                         int transport_handle,
                         struct oid *oid,
                         eui48_t value)
{
  eui48_t returned_value;
  size_t eui_size = EUI48_LEN;
  atlk_rc_t rc;

  rc = snmp_send_SetRequest(remote_transport,
                            transport_handle,
                            oid,
                            OBJECT_TYPE_OCTET_STR,
                            &value,
                            EUI48_LEN,
                            &returned_value,
                            &eui_size);
  if (atlk_error(rc)) {
    return rc;
  }

  if (atlk_unlikely(eui_size != EUI48_LEN)) {
    TR_ERROR("Wrong MAC address size received %zu", eui_size);
    return ATLK_E_PROTOCOL_ERROR;
  }

  if (memcmp(value.octets, returned_value.octets, EUI48_LEN)) {
    TR_ERROR("Could not set MAC address");
    return ATLK_E_PROTOCOL_ERROR;
  }

  return ATLK_OK;
}

atlk_must_check atlk_rc_t
snmp_send_string_GetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            char *value,
                            size_t *size)
{
  atlk_rc_t rc;
  size_t str_size;

  str_size = *size;

  rc = snmp_send_GetRequest(remote_transport,
                            transport_handle,
                            oid,
                            value,
                            &str_size);
  if (atlk_error(rc)) {
    return rc;
  }

  *size = str_size;
  return ATLK_OK;
}

atlk_rc_t atlk_must_check
snmp_send_string_SetRequest(remote_transport_t *remote_transport,
                            int transport_handle,
                            struct oid *oid,
                            const char *value,
                            size_t size)
{
  char returned_value[SNMP_MAX_PDU];
  size_t returned_size = sizeof(returned_value);
  atlk_rc_t rc;

  rc = snmp_send_SetRequest(remote_transport,
                            transport_handle,
                            oid,
                            OBJECT_TYPE_OCTET_STR,
                            value,
                            size,
                            returned_value,
                            &returned_size);
  if (atlk_error(rc)) {
    return rc;
  }

  if (atlk_unlikely(returned_size != size)) {
    TR_ERROR("Wrong LUT size received %zu", returned_size);
    return ATLK_E_PROTOCOL_ERROR;
  }

  if (memcmp(value, returned_value, returned_size)) {
    TR_ERROR("Could not set LUT");
    return ATLK_E_PROTOCOL_ERROR;
  }
  return ATLK_OK;
}
