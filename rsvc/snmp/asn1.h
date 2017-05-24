#ifndef _LIBSNMP_ASN1_H
#define _LIBSNMP_ASN1_H

#include <stdint.h>

#include <atlk/memio.h>

#include "oid.h"

/**
 Encode field size
 @param[in,out] memio memio pointing to destination buffer
 @param[in] size Field size
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_field_size(struct memio *memio, size_t size);

/**
 Fill previousely allocated buffer with field size
 @param[out] stream of destination buffer
 @param[in] size Field size
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_fill_field_size(uint8_t *stream, size_t size);
/**
 Encode NULL to terminate ASN.1 sequence
 @param[in,out] memio memio pointing to destination buffer
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_termination(struct memio *memio);

/**
 Encode ASN.1 sequence
 @param[in,out] memio memio pointing to destination buffer
 @param[out] seq_size_ptr Pointer to sequence size to be filled
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_sequence(struct memio *memio, uint8_t **seq_size_ptr);

/**
 Decode ASN.1 sequence
 @param[in, out] memio memio pointing to BER encoded sequence
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_decode_sequence(struct memio *memio, size_t *seq_len);

/**
 Encode SNMP get request
 @param[in,out] memio memio pointing to destination buffer
 @param[out] req_size_ptr Pointer to request size
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_snmp_GetRequest(struct memio *memio, uint8_t **req_size_ptr);

/**
 Encode SNMP set request
 @param[in,out] memio memio pointing to destination buffer
 @param[out] req_size_ptr Pointer to request size pointer
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_snmp_SetRequest(struct memio *memio, uint8_t **req_size_ptr);

/**
 Decode SNMP get response
 @param[in, out] memio memio pointing to BER encoded response
 @param[out] resp_len Length of respond
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_decode_snmp_GetResponse(struct memio *memio, size_t *resp_len);

/**
 Encode object identifier
 @param[in,out] memio memio pointing to destination buffer
 @param[in] oid String representation of the OID
 @param[in] oid_len OID length
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_oid(struct memio *memio, const struct oid *oid);

/**
 Decode object identifier
 @param[in, out] memio memio pointing to BER encoded OID
 @param[out] oid Pointer to OID structure
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_decode_oid(struct memio *memio, struct oid *oid);

/**
 Encode inetaddressipv4 value
 @param[in,out] memio memio pointing to destination buffer
 @param[in] number unsigned integer number
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_inetaddressipv4(struct memio *memio, uint32_t value);

/**
 Encode integer value
 @param[in,out] memio memio pointing to destination buffer
 @param[in] number Integer number
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_integer(struct memio *memio, int32_t number);

/**
 Encode gauge value
 @param[in,out] memio memio pointing to destination buffer
 @param[in] number Gauge number
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_gauge(struct memio *memio, uint32_t number);

/**
 Encode gauge value
 @param[in,out] memio memio pointing to destination buffer
 @param[in] number counter64 number
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_counter64(struct memio *memio, uint64_t number);

/**
 Decode InetAddressIPv4 value
 @param[in, out] memio memio pointing to BER encoded InetAddressIPv4
 @param[out] value Value of integer
 @return Pointer after integer, NULL upon error
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_decode_inetaddressipv4(struct memio *memio, int32_t *value);

/**
 Decode integer value
 @param[in, out] memio memio pointing to BER encoded integer
 @param[out] value Value of integer
 @return Pointer after integer, NULL upon error
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_decode_integer(struct memio *memio, int32_t *value);

/**
 Decode Gauge value
 @param[in, out] memio memio pointing to BER encoded integer
 @param[out] value Value of gauge
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_decode_gauge(struct memio *memio, uint32_t *value);

/**
 Decode counter value
 @param[in, out] memio memio pointing to BER encoded counter
 @param[out] value Value of counter
 @return Pointer after counter, NULL upon error
 */
atlk_must_check atlk_rc_t
asn1_decode_counter(struct memio *memio, uint32_t *value);

/**
 Decode counter64 value
 @param[in, out] memio memio pointing to BER encoded counter
 @param[out] value Value of counter64
 @return Pointer after counter, NULL upon error
 */
atlk_must_check atlk_rc_t
asn1_decode_counter64(struct memio *memio, uint64_t *value);

/**
 Encode octet string into stream
 @param[in,out] memio memio pointing to destination buffer
 @param[in] octet Octet string
 @param[in] len Octet string length
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_encode_octet_string(struct memio *memio,
                         const uint8_t *octet,
                         size_t len);

/**
 Decode octet string from stream
 @param[in, out] memio memio pointing to BER encoded octet string
 @param[out] octet_ptr Pointer to octet string
 @param[out] octet_len Length of octet string
 @return Pointer after octet string, of NULL upon error
 */
atlk_must_check atlk_rc_t
asn1_decode_octet_string(struct memio *memio,
                         char **octet_ptr,
                         size_t *octet_len);

/**
 Decode variable based on its type
 @param[in, out] memio memio pointing to BER encoded variable
 @param[out] var_ptr Pointer to variable
 @param[in, out] var_size, variable size
 @retval ::ATLK_OK if succeeded
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
asn1_decode_variable(struct memio *memio,
                     uint8_t *var_ptr,
                     size_t *var_size);

#endif /* _LIBSNMP_ASN1_H */
