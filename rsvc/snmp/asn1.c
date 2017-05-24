#include <string.h>

#include <platform/common.h>
#include <atlk/memio.h>

#include "ber.h"
#include "asn1.h"

/* Allocate memory and return pointer to memory start */
static inline uint8_t *
memio_allocate(struct memio *memio, size_t len)
{
  atlk_rc_t rc;
  uint8_t *p = memio->next;

  rc = memio_reserve(memio, len);
  if (atlk_error(rc)) {
    TR_ERROR("No space available for %zu bytes", len);
    return NULL;
  }

  return p;
}

/* Field size can be up to 32767 bytes (more than we need) */
atlk_must_check atlk_rc_t
asn1_fill_field_size(uint8_t *stream, size_t size)
{
  uint8_t *p = stream;
  /* We can have 2 bytes for sequence size */
  if (size > 0xFFFF) {
    TR_ERROR("Unsupported size for field (%zu)", size);
    return ATLK_E_UNSUPPORTED;
  }
  /* Reserve two bytes for length */
  p[0] = BER_MORE_BIT | 0x2;
  p[1] = 0xFF & (size >> 8);
  p[2] = 0xFF & size;

  return ATLK_OK;
}

/* Field size can be up to 32767 bytes (more than we need) */
atlk_must_check atlk_rc_t
asn1_encode_field_size(struct memio *memio, size_t size)
{
  uint8_t *p =  memio_allocate(memio, 3);

  if (!p) {
    TR_ERROR("Out of buffer space");
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  return asn1_fill_field_size(p, size);
}

/* Extract field size (in byte) from BER Encoded buffer */
static atlk_rc_t
asn1_decode_field_size(struct memio *memio, size_t *size)
{
  size_t length_size;
  const uint8_t *p = memio_allocate(memio, 1);

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }
  /* If length is one byte */
  if (((*p) & 0x80) == 0) {
    *size = *p;
    return ATLK_OK;
  }

  *size = 0;
  length_size = (*p) & 0x7F;
  if (length_size > sizeof(int32_t)) {
    TR_ERROR("Invalid size for integer");
    return ATLK_E_PROTOCOL_ERROR;
  }

  while (length_size--) {
    p = memio_allocate(memio, 1);
    if (!p) {
      return ATLK_E_BUFFER_TOO_SMALL;
    }

    *size = ((*size) << 8) | (*p);
  }

  return ATLK_OK;
}

atlk_must_check atlk_rc_t
asn1_encode_termination(struct memio *memio)
{
  uint8_t *p = memio_allocate(memio, 2);

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  p[0] = BER_NULL;
  p[1] = 0;
  return ATLK_OK;
}

atlk_must_check atlk_rc_t
asn1_encode_sequence(struct memio *memio, uint8_t **seq_size_ptr)
{
  uint8_t *p = memio_allocate(memio, 1);

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  p[0] = BER_SEQUENCE;
  p++;
  *seq_size_ptr = p;
  /* Placeholder for length */
  return asn1_encode_field_size(memio, 0);
}

atlk_must_check atlk_rc_t
asn1_decode_sequence(struct memio *memio, size_t *seq_len)
{
  const uint8_t *p = memio_allocate(memio, 1);

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  if (p[0] != BER_SEQUENCE) {
    TR_ERROR("Expected sequence sentinel %02X, got %02X", BER_SEQUENCE, *p);
    return ATLK_E_PROTOCOL_ERROR;
  }

  return asn1_decode_field_size(memio, seq_len);
}

static atlk_rc_t
asn1_decode_int_type(struct memio *memio, uint32_t *value, uint8_t int_type)
{
  const uint8_t *p = memio_allocate(memio, 1);
  size_t int_size;
  atlk_rc_t rc;

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  if (p[0] != int_type) {
    TR_ERROR("Expected integer sentinel %02X, got %02X", int_type, *p);
    return ATLK_E_PROTOCOL_ERROR;
  }

  rc = asn1_decode_field_size(memio, &int_size);
  if (atlk_error(rc)) {
    return rc;
  }

  *value = 0;
  while (int_size--) {
    p = memio_allocate(memio, 1);
    if (!p) {
      return ATLK_E_BUFFER_TOO_SMALL;
    }

    *value = (*value) << 8 | (p[0]);
  }

  return ATLK_OK;
}

atlk_must_check atlk_rc_t
asn1_decode_inetaddressipv4(struct memio *memio, int32_t *value)
{
  return asn1_decode_int_type(memio, (uint32_t *)value, BER_INETADDRESSIPV4);
}

atlk_must_check atlk_rc_t
asn1_decode_integer(struct memio *memio, int32_t *value)
{
  return asn1_decode_int_type(memio, (uint32_t *)value, BER_INTEGER);
}

atlk_rc_t
asn1_decode_gauge(struct memio *memio, uint32_t *value)
{
  return asn1_decode_int_type(memio, value, BER_GAUGE);
}

atlk_must_check atlk_rc_t
asn1_decode_counter_type(struct memio *memio, uint64_t *value, uint8_t int_type)
{
  const uint8_t *p = memio_allocate(memio, 1);
  size_t int_size;
  atlk_rc_t rc;

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  if (p[0] != int_type) {
    TR_ERROR("Expected COUNTER sentinel %02X, got %02X", int_type, p[0]);
    return ATLK_E_PROTOCOL_ERROR;
  }

  rc = asn1_decode_field_size(memio, &int_size);
  if (atlk_error(rc)) {
    return rc;
  }

  *value = 0;
  while (int_size--) {
    p = memio_allocate(memio, 1);
    if (!p) {
      return ATLK_E_BUFFER_TOO_SMALL;
    }

    *value = (*value) << 8 | (*p);
  }

  return ATLK_OK;
}

atlk_rc_t
asn1_decode_counter(struct memio *memio, uint32_t *value)
{
  return asn1_decode_counter_type(memio, (uint64_t *)value, BER_COUNTER);
}

atlk_rc_t
asn1_decode_counter64(struct memio *memio, uint64_t *value)
{
  return asn1_decode_counter_type(memio, value, BER_COUNTER64);
}

atlk_must_check atlk_rc_t
asn1_encode_snmp_GetRequest(struct memio *memio, uint8_t **req_size_ptr)
{
  uint8_t *p = memio_allocate(memio, 1);

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  p[0] = BER_GET_REQUEST;
  *req_size_ptr = &p[1];
  /* Placeholder for length */
  return asn1_encode_field_size(memio, 0);
}

atlk_must_check atlk_rc_t
asn1_encode_snmp_SetRequest(struct memio *memio, uint8_t **req_size_ptr)
{
  uint8_t *p = memio_allocate(memio, 1);

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  p[0] = BER_SET_REQUEST;
  *req_size_ptr = &p[1];
  /* Placeholder for length */
  return asn1_encode_field_size(memio, 0);
}

atlk_must_check atlk_rc_t
asn1_decode_snmp_GetResponse(struct memio *memio, size_t *resp_len)
{
  const uint8_t *p = memio_allocate(memio, 1);
  size_t response_size;
  atlk_rc_t rc;

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  if (p[0] != BER_GET_RESPONSE) {
    TR_ERROR("Expected integer sentinel %02X, got %02X", BER_GET_RESPONSE, p[0]);
    return ATLK_E_PROTOCOL_ERROR;
  }

  rc = asn1_decode_field_size(memio, &response_size);
  if (atlk_error(rc)) {
    return rc;
  }

  *resp_len = response_size;
  return ATLK_OK;
}

atlk_must_check atlk_rc_t
asn1_encode_oid(struct memio *memio, const struct oid *oid)
{
  uint8_t *p = memio_allocate(memio, 3);
  uint8_t *oid_len_ptr;
  uint32_t index;
  uint32_t val;
  uint8_t *start_of_mem;

  if (oid->count < 2) {
    TR_ERROR("Too short OID (%zu)", oid->count);
    return ATLK_E_INVALID_ARG;
  }

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  start_of_mem = p;
  p[0] = BER_OBJECT_IDENTIFIER;
  /* Placeholder for length. Assumption: Total length doesn't exceed 128 bytes */
  oid_len_ptr = &p[1];
  p[1] = 0;
  /* Special encoding for first two numbers */
  p[2] = 40 * oid->element[0] + oid->element[1];
  /* Start index from 2 since we handled first 2 numbers above */
  for (index = 2; index < oid->count; index++) {
    val = oid->element[index];
    if (val < 0x80) {
      /* Fits in 1 octet */
      p = memio_allocate(memio, 1);
      if (!p) {
        return ATLK_E_BUFFER_TOO_SMALL;
      }
      p[0] = val;
      continue;
    }

    if (oid->element[index] < 0x4000) {
      /* Fits in 2 octets */
      p = memio_allocate(memio, 2);
      if (!p) {
        return ATLK_E_BUFFER_TOO_SMALL;
      }
      p[0] = (val >> 7) | BER_MORE_BIT;
      p[1] = val & 0x7F;
      continue;
    }
    /* Fits in 3 octets */
    p = memio_allocate(memio, 3);
    if (!p) {
      return ATLK_E_BUFFER_TOO_SMALL;
    }
    p[0] = (val >> 14) | BER_MORE_BIT;
    p[1] = ((val >> 7) & 0x7F) | BER_MORE_BIT;
    p[2] = val & 0x7F;
    continue;
  }
  /* Update OID length */
  *oid_len_ptr = (uint8_t*)memio->next - start_of_mem - 2;
  return ATLK_OK;
}

atlk_must_check atlk_rc_t
asn1_decode_oid(struct memio *memio, struct oid *oid)
{
  const uint8_t *p = memio_allocate(memio, 1);
  size_t oid_size;
  int i;
  atlk_rc_t rc;

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  if (p[0] != BER_OBJECT_IDENTIFIER) {
    TR_ERROR("Expected OID sentinel %02X, got %02X", BER_OBJECT_IDENTIFIER, *p);
    return ATLK_E_PROTOCOL_ERROR;
  }

  rc = asn1_decode_field_size(memio, &oid_size);
  if (atlk_error(rc)) {
    return rc;
  }

  if (oid_size < 1) {
    TR_ERROR("Wrong OID size (%zu)", oid_size);
    return ATLK_E_INVALID_ARG;
  }

  p = memio_allocate(memio, oid_size);
  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  oid->count = 0;
  memset(oid->element, 0x00, sizeof(oid->element));
  /* Special decode for first two numbers */
  oid->element[oid->count] = p[0] / 40;
  oid->count++;
  oid->element[oid->count] = p[0] % 40;
  oid->count++;

  for (i = 1; i < oid_size; i++) {
    oid->element[oid->count] <<= 7;
    if (p[i] & BER_MORE_BIT) {
      /* In case we have more bytes to take into account */
      oid->element[oid->count] |= (p[i] & 0x7F);
    }
    else {
      oid->element[oid->count++] |= p[i];
      if (oid->count >= OID_MAX_LENGTH) {
        TR_ERROR("OID exceeded OID length (%d)", OID_MAX_LENGTH);
        return ATLK_E_UNSUPPORTED;
      }
    }
  }

  return ATLK_OK;
}

atlk_must_check atlk_rc_t
asn1_encode_int_type(struct memio *memio, int64_t value, uint8_t type)
{
  uint8_t *p = memio_allocate(memio, 1);
  uint64_t u_number;
  /* Divisor for integer upper limits */
  uint32_t shift = 0;
  /* This is needed for the following calculations */
  u_number = (uint64_t)value;

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  p[0] = type;

  /* In case of gauge/counter type, MSb must be zero. Therefore, the following
     upper limits shall be shifted right (divided by 2) */
  if (type == BER_GAUGE || type == BER_COUNTER) {
    shift = 1;
  }

  if (u_number < (0x100 >> shift)) {
    p = memio_allocate(memio, 2);
    if (!p) {
      return ATLK_E_BUFFER_TOO_SMALL;
    }
    p[0] = 1;
    p[1] = u_number;
    return ATLK_OK;
  }

  if (u_number < (0x10000 >> shift)) {
    p = memio_allocate(memio, 3);
    if (!p) {
      return ATLK_E_BUFFER_TOO_SMALL;
    }
    p[0] = 2;
    p[1] = 0xFF & (u_number >> 8);
    p[2] = 0xFF & u_number;
    return ATLK_OK;
  }

  if (u_number < (0x1000000 >> shift)) {
    p = memio_allocate(memio, 4);
    if (!p) {
      return ATLK_E_BUFFER_TOO_SMALL;
    }
    p[0] = 3;
    p[1] = 0xFF & (u_number >> 16);
    p[2] = 0xFF & (u_number >> 8);
    p[3] = 0xFF & u_number;
    return ATLK_OK;
  }

  if (u_number < (0x100000000UL >> shift)) {
    p = memio_allocate(memio, 5);
    if (!p) {
      return ATLK_E_BUFFER_TOO_SMALL;
    }

    p[0] = 4;
    p[1] = 0xFF & (u_number >> 24);
    p[2] = 0xFF & (u_number >> 16);
    p[3] = 0xFF & (u_number >> 8);
    p[4] = 0xFF & u_number;
    return ATLK_OK;
  }

  /* This case is only for gauge/counter/counter64 values */
  p = memio_allocate(memio, 10);
  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  p[0] = 9;
  p[1] = 0;
  p[2] = 0xFF & (u_number >> 54);
  p[3] = 0xFF & (u_number >> 48);
  p[4] = 0xFF & (u_number >> 40);
  p[5] = 0xFF & (u_number >> 32);
  p[6] = 0xFF & (u_number >> 24);
  p[7] = 0xFF & (u_number >> 16);
  p[8] = 0xFF & (u_number >> 8);
  p[9] = 0xFF & u_number;
  return ATLK_OK;
}

atlk_must_check atlk_rc_t
asn1_encode_inetaddressipv4(struct memio *memio, uint32_t value)
{
  return asn1_encode_int_type(memio, value, BER_INETADDRESSIPV4);
}

atlk_must_check atlk_rc_t
asn1_encode_integer(struct memio *memio, int32_t value)
{
  return asn1_encode_int_type(memio, value, BER_INTEGER);
}

atlk_must_check atlk_rc_t
asn1_encode_gauge(struct memio *memio, uint32_t value)
{
  return asn1_encode_int_type(memio, value, BER_GAUGE);
}

atlk_must_check atlk_rc_t
asn1_encode_counter64(struct memio *memio, uint64_t value)
{
  return asn1_encode_int_type(memio, value, BER_COUNTER64);
}

atlk_must_check atlk_rc_t
asn1_encode_octet_string(struct memio *memio,
                         const uint8_t *octet,
                         size_t len)
{
  uint8_t *p;
  atlk_rc_t rc;

  if (!octet) {
    TR_ERROR("Invalid input parameters");
    return ATLK_E_INVALID_ARG;
  }

  if (len < 1) {
    TR_ERROR("Unsupported octet string length %d", (int)len);
    return ATLK_E_INVALID_ARG;
  }

  p = memio_allocate(memio, 1);
  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }
  /* Copy octets */
  p[0] = BER_OCTET_STRING;
  rc = asn1_encode_field_size(memio, len);
  if (atlk_error(rc)) {
    return rc;
  }

  p = memio_allocate(memio, len);
  memcpy(p, octet, len);

  return ATLK_OK;
}

atlk_rc_t
asn1_decode_octet_string(struct memio *memio,
                         char **octet_ptr,
                         size_t *octet_len)
{
  const uint8_t *p = memio_allocate(memio, 1);
  atlk_rc_t rc;

  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  if (p[0] != BER_OCTET_STRING) {
    TR_ERROR("Expected octet string sentinel %02X, got %02X",
             BER_OCTET_STRING,
             *p);
    return ATLK_E_PROTOCOL_ERROR;
  }

  rc  = asn1_decode_field_size(memio, octet_len);
  if (atlk_error(rc)) {
    return rc;
  }

  p = memio_allocate(memio, *octet_len);
  if (!p) {
    return ATLK_E_BUFFER_TOO_SMALL;
  }

  *octet_ptr = (char *)p;
  return ATLK_OK;
}

atlk_must_check atlk_rc_t
asn1_decode_variable(struct memio *memio,
                     uint8_t *var_ptr,
                     size_t *var_size)
{
  const uint8_t *p = memio->next;
  int32_t int_value;
  uint32_t counter;
  uint64_t counter64;
  char *p_str;
  size_t size;
  atlk_rc_t rc;

  if (!memio || !var_ptr || !var_size) {
    TR_ERROR("One or more parameters are NULL");
    return ATLK_E_INVALID_ARG;
  }

  switch(*p) {
  case BER_INETADDRESSIPV4:
    if (*var_size < sizeof(int_value)) {
      TR_ERROR("Insufficient space for integer");
      return ATLK_E_BUFFER_TOO_SMALL;
    }

    rc = asn1_decode_inetaddressipv4(memio, &int_value);
    if (atlk_error(rc)) {
      return rc;
    }

    memcpy(var_ptr, &int_value, sizeof(int_value));
    *var_size = sizeof(int_value);
    break;
  case BER_INTEGER:
    if (*var_size < sizeof(int_value)) {
      TR_ERROR("Insufficient space for integer");
      return ATLK_E_BUFFER_TOO_SMALL;
    }

    rc = asn1_decode_integer(memio, &int_value);
    if (atlk_error(rc)) {
      return rc;
    }

    memcpy(var_ptr, &int_value, sizeof(int_value));
    *var_size = sizeof(int_value);
    break;
  case BER_COUNTER:
  case BER_GAUGE:
    if (*var_size < sizeof(counter)) {
      TR_ERROR("Insufficient space for counter");
      return ATLK_E_BUFFER_TOO_SMALL;
    }
    if (*p == BER_COUNTER) {
      rc = asn1_decode_counter(memio, &counter);
    }
    else {
      rc = asn1_decode_gauge(memio, &counter);
    }
    if (atlk_error(rc)) {
      return rc;
    }

    memcpy(var_ptr, &counter, sizeof(counter));
    *var_size = sizeof(int_value);
    break;

  case BER_COUNTER64:
    if (*var_size < sizeof(counter64)) {
      TR_ERROR("Insufficient space for counter");
      return ATLK_E_BUFFER_TOO_SMALL;
    }

    rc = asn1_decode_counter64(memio, &counter64);

    if (atlk_error(rc)) {
      return rc;
    }

    memcpy(var_ptr, &counter64, sizeof(counter64));
    *var_size = sizeof(counter64);
    break;

  case BER_OCTET_STRING:
    rc = asn1_decode_octet_string(memio, &p_str, &size);
    if (atlk_error(rc)) {
      return rc;
    }

    if (*var_size < size) {
      TR_ERROR("Insufficient space for octet string");
      return ATLK_E_BUFFER_TOO_SMALL;
    }

    memcpy(var_ptr, p_str, size);
    *var_size = size;
    break;
  default:
    TR_ERROR("Unknown type 0x%02X", *p);
    return ATLK_E_PROTOCOL_ERROR;
  }

  return ATLK_OK;
}
