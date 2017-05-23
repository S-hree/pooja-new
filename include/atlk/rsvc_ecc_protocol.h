/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _REMOTE_ECC_PROTOCOL_H
#define _REMOTE_ECC_PROTOCOL_H

#include <string.h>

#include <atlk/ecc.h>
#include <atlk/ecdsa.h>
#include <atlk/sha.h>
#include <atlk/ecc_service.h>
#include <atlk/rsvc_protocol.h>

/* Constant local port number at the target side */
#define ECC_RSVCD_SERVICE_PORT_NUM 62000

typedef enum {
  /* Reserved messages */
  RSVC_ECC_SERVICE_RESERVED_0        = 0x00,
  RSVC_ECC_SERVICE_RESERVED_1        = 0x01,

  /* ECC request and response */
  RSVC_ECC_SERVICE_REQUEST           = 0x02,
  RSVC_ECC_SERVICE_RESPONSE          = 0x03,

} rsvc_ecc_msg_id_t;

/** ecc_curve_t */
typedef dx_uint8_t rsvc_ecc_curve_t;

/** ecc_scalar_t */
typedef dx_struct {
  dx_uint32le_t value[ECC_SCALAR_NUM_OF_UINT32];
} rsvc_ecc_scalar_t;

/** ecc_point_t */
typedef dx_struct {
  dx_uint8_t point_type;
  rsvc_ecc_scalar_t x_coordinate;
  rsvc_ecc_scalar_t y_coordinate;
} rsvc_ecc_point_t;

/** sha_digest_t */
typedef dx_struct {
  uint8_t value[SHA_DIGEST_MAX_SIZE];
  dx_uint8_t value_size;
} rsvc_sha_digest_t;

/** ecc_signature_t */
typedef dx_struct {
  rsvc_ecc_scalar_t r_scalar;
  rsvc_ecc_scalar_t s_scalar;
} rsvc_ecdsa_signature_t;

/** ecc_fast_verification_signature_t */
typedef dx_struct {
  rsvc_ecc_point_t R_point;
  rsvc_ecc_scalar_t s_scalar;
} rsvc_ecdsa_fast_verification_signature_t;

/** ecc_request_id_t */
typedef dx_uint32le_t rsvc_ecc_request_id_t;

/** ecc_request_type_t */
typedef dx_uint8_t rsvc_ecc_request_type_t;

/** ecc_request_context_t */
typedef dx_struct {
  rsvc_ecc_request_id_t request_id;
  rsvc_ecc_request_type_t request_type;
  rsvc_ecc_curve_t curve;
} rsvc_ecc_request_context_t;

/** ecc_sign_params_t */
typedef dx_struct {
  rsvc_ecc_scalar_t private_key;
  rsvc_sha_digest_t digest;
} rsvc_ecc_sign_params_t;

/** ecc_verify_params_t */
typedef dx_struct {
  rsvc_ecc_point_t public_key;
  rsvc_sha_digest_t digest;
  rsvc_ecdsa_signature_t signature;
} rsvc_ecc_verify_params_t;

/** ecc_pma_params_t */
typedef dx_struct {
  rsvc_ecc_point_t point;
  rsvc_ecc_scalar_t multiplier;
  rsvc_ecc_point_t addend;
} rsvc_ecc_pma_params_t;

/** ecc_request_t */
typedef dx_struct {
  rsvc_ecc_request_context_t context;

  dx_union {
    rsvc_ecc_verify_params_t verify_params;
    rsvc_ecc_sign_params_t sign_params;
    rsvc_ecc_pma_params_t pma_params;
  } params;
} rsvc_ecc_request_t;

/** ecc_response_t */
typedef dx_struct {
  rsvc_ecc_request_context_t context;
  dx_uint8_t rc;
  dx_union {
    rsvc_ecdsa_fast_verification_signature_t sign_result;
    rsvc_ecc_point_t pma_result;
  } result;
} rsvc_ecc_response_t;

/** ECC request message */
typedef dx_struct {
  dx_rsvc_msg_header_t header;
  rsvc_ecc_request_t request;
} rsvc_ecc_request_msg_t;

/** ECC response message */
typedef dx_struct {
  dx_rsvc_msg_header_t header;
  rsvc_ecc_response_t response;
} rsvc_ecc_response_msg_t;

/**
   Get the over-the-air size in bytes of an ECC request;
   Returns 0 on invalid request.
*/
atlk_inline size_t
rsvc_ecc_request_size(ecc_request_type_t request_type)
{
  rsvc_ecc_request_t request;
  size_t size = 0;

  size += sizeof(request.context);

  switch (request_type) {
    case ECC_REQUEST_TYPE_SIGN:
      size += sizeof(request.params.sign_params);
      break;
    case ECC_REQUEST_TYPE_VERIFY:
      size += sizeof(request.params.verify_params);
      break;
    case ECC_REQUEST_TYPE_PMA:
      size += sizeof(request.params.pma_params);
      break;
    default:
      return 0;
  }

  return size;
}

/**
   Get the over-the-air size in bytes of an ECC response;
   Returns 0 on invalid response.
*/
atlk_inline size_t
rsvc_ecc_response_size(ecc_request_type_t request_type)
{
  rsvc_ecc_response_t response;
  size_t size = 0;

  size += sizeof(response.context);
  size += sizeof(response.rc);

  switch (request_type) {
    case ECC_REQUEST_TYPE_SIGN:
      size += sizeof(response.result.sign_result);
      break;
    case ECC_REQUEST_TYPE_VERIFY:
      break;
    case ECC_REQUEST_TYPE_PMA:
      size += sizeof(response.result.pma_result);
      break;
    default:
      return 0;
  }

  return size;
}

/**
   Encode SHA digest.

   @param[out] out Encoded SHA digest
   @param[in] in SHA digest
*/
atlk_inline void
rsvc_sha_digest_encode(rsvc_sha_digest_t *out,
                       const sha_digest_t *in)
{
  memcpy(out->value, in->value, sizeof(out->value));
  dx_uint8_set(&out->value_size, in->value_size);
}

/**
   Decode SHA digest.

   @param[out] out Decoded SHA digest
   @param[in] in Encoded SHA digest
*/
atlk_inline void
rsvc_sha_digest_decode(sha_digest_t *out,
                       const rsvc_sha_digest_t *in)
{
  uint8_t value_size;
  dx_uint8_get(&in->value_size, &value_size);
  out->value_size = value_size;
  memcpy(out->value, in->value, sizeof(out->value));
}

/**
   Encode ECC scalar.

   @param[out] out Encoded ECC scalar
   @param[in] in ECC scalar
*/
atlk_inline void
rsvc_ecc_scalar_encode(rsvc_ecc_scalar_t *out,
                       const ecc_scalar_t *in)
{
  int i;

  for (i = 0; i < ECC_SCALAR_NUM_OF_UINT32; i++) {
    dx_uint32le_set(&out->value[i], in->value[i]);
  }
}

/**
   Decode ECC scalar.

   @param[out] out Decoded ECC scalar
   @param[in] in Encoded ECC scalar
*/
atlk_inline void
rsvc_ecc_scalar_decode(ecc_scalar_t *out,
                       const rsvc_ecc_scalar_t *in)
{
  int i;

  for (i = 0; i < ECC_SCALAR_NUM_OF_UINT32; i++) {
    dx_uint32le_get(&in->value[i], &out->value[i]);
  }
}

/**
   Encode ECC point.

   @param[out] out Encoded ECC point
   @param[in] in ECC point
*/
atlk_inline void
rsvc_ecc_point_encode(rsvc_ecc_point_t *out,
                      const ecc_point_t *in)
{
  dx_uint8_set(&out->point_type, in->point_type);
  rsvc_ecc_scalar_encode(&out->x_coordinate, &in->x_coordinate);
  rsvc_ecc_scalar_encode(&out->y_coordinate, &in->y_coordinate);
}

/**
   Decode ECC point.

   @param[out] out Decoded ECC point
   @param[in] in Encoded ECC point
*/
atlk_inline void
rsvc_ecc_point_decode(ecc_point_t *out,
                      const rsvc_ecc_point_t *in)
{
  uint8_t point_type = 0;
  dx_uint8_get(&in->point_type, &point_type);
  out->point_type = point_type;
  rsvc_ecc_scalar_decode(&out->x_coordinate, &in->x_coordinate);
  rsvc_ecc_scalar_decode(&out->y_coordinate, &in->y_coordinate);
}

/**
   Decode ECC request context.

   @param[out] out Decoded ECC request context
   @param[in] in Encoded ECC request context
*/
atlk_inline void
rsvc_ecc_request_context_decode(ecc_request_context_t *out,
                                const rsvc_ecc_request_context_t *in)
{
  uint8_t request_type, curve;
  dx_uint32le_get(&in->request_id, &out->request_id);
  dx_uint8_get(&in->request_type, &request_type);
  out->request_type = request_type;
  dx_uint8_get(&in->curve, &curve);
  out->curve = curve;
}

/**
   Encode ECC request

   @param[out] out Encoded ECC request
   @param[in] in ECC request
*/
atlk_inline atlk_rc_t atlk_must_check
rsvc_ecc_request_encode(rsvc_ecc_request_t *out,
                        const ecc_request_t *in)
{
  const ecc_request_context_t *in_context;
  rsvc_ecc_request_context_t *out_context;

  /* Validate arguments */
  if (!in || !out) {
    return ATLK_E_INVALID_ARG;
  }

  /* Encode context */
  in_context = &in->context;
  out_context = &out->context;
  dx_uint32le_set(&out_context->request_id, in_context->request_id);
  dx_uint8_set(&out_context->request_type, in_context->request_type);
  dx_uint8_set(&out_context->curve, in_context->curve);

  /* Encode request params */
  switch (in_context->request_type) {
    case ECC_REQUEST_TYPE_VERIFY: {
      const ecc_verify_params_t *in_params = &in->params.verify_params;
      const ecc_signature_t *in_signature = &in_params->signature;
      rsvc_ecc_verify_params_t *out_params = &out->params.verify_params;
      rsvc_ecdsa_signature_t *out_signature = &out_params->signature;
      rsvc_ecc_point_encode(&out_params->public_key, &in_params->public_key);
      rsvc_sha_digest_encode(&out_params->digest, &in_params->digest);
      rsvc_ecc_scalar_encode(&out_signature->r_scalar, &in_signature->r_scalar);
      rsvc_ecc_scalar_encode(&out_signature->s_scalar, &in_signature->s_scalar);
      break;
    }
    case ECC_REQUEST_TYPE_SIGN: {
      const ecc_sign_params_t *in_params = &in->params.sign_params;
      rsvc_ecc_sign_params_t *out_params = &out->params.sign_params;
      rsvc_ecc_scalar_encode(&out_params->private_key, &in_params->private_key);
      rsvc_sha_digest_encode(&out_params->digest, &in_params->digest);
      break;
    }
    case ECC_REQUEST_TYPE_PMA: {
      const ecc_pma_params_t *in_params = &in->params.pma_params;
      rsvc_ecc_pma_params_t *out_params = &out->params.pma_params;
      rsvc_ecc_point_encode(&out_params->point, &in_params->point);
      rsvc_ecc_scalar_encode(&out_params->multiplier, &in_params->multiplier);
      rsvc_ecc_point_encode(&out_params->addend, &in_params->addend);
      break;
    }
    default:
      return ATLK_E_INVALID_ARG;
  }

  return ATLK_OK;
}

/**
   Decode ECC request

   @param[out] out Decoded ECC request
   @param[in] in Encoded ECC request
*/
atlk_inline atlk_rc_t atlk_must_check
rsvc_ecc_request_decode(ecc_request_t *out,
                        const rsvc_ecc_request_t *in)
{
  const rsvc_ecc_request_context_t *in_context;
  ecc_request_context_t *out_context;

  /* Validate arguments */
  if (!in || !out) {
    return ATLK_E_INVALID_ARG;
  }

  /* Decode context */
  in_context = &in->context;
  out_context = &out->context;
  rsvc_ecc_request_context_decode(out_context, in_context);

  /* Decode request params */
  switch (out_context->request_type) {
    case ECC_REQUEST_TYPE_VERIFY: {
      const rsvc_ecc_verify_params_t *in_params = &in->params.verify_params;
      const rsvc_ecdsa_signature_t *in_signature = &in_params->signature;
      ecc_verify_params_t *out_params = &out->params.verify_params;
      ecc_signature_t *out_signature = &out_params->signature;
      rsvc_ecc_point_decode(&out_params->public_key, &in_params->public_key);
      rsvc_sha_digest_decode(&out_params->digest, &in_params->digest);
      rsvc_ecc_scalar_decode(&out_signature->r_scalar, &in_signature->r_scalar);
      rsvc_ecc_scalar_decode(&out_signature->s_scalar, &in_signature->s_scalar);
      break;
    }
    case ECC_REQUEST_TYPE_SIGN: {
      const rsvc_ecc_sign_params_t *in_params = &in->params.sign_params;
      ecc_sign_params_t *out_params = &out->params.sign_params;
      rsvc_ecc_scalar_decode(&out_params->private_key, &in_params->private_key);
      rsvc_sha_digest_decode(&out_params->digest, &in_params->digest);
      break;
    }
    case ECC_REQUEST_TYPE_PMA: {
      const rsvc_ecc_pma_params_t *in_params = &in->params.pma_params;
      ecc_pma_params_t *out_params = &out->params.pma_params;
      rsvc_ecc_point_decode(&out_params->point, &in_params->point);
      rsvc_ecc_scalar_decode(&out_params->multiplier, &in_params->multiplier);
      rsvc_ecc_point_decode(&out_params->addend, &in_params->addend);
      break;
    }
    default:
      return ATLK_E_PROTOCOL_ERROR;
  }

  return ATLK_OK;
}

/**
   Encode ECC response

   @param[out] out Encoded ECC response
   @param[in] in ECC response
*/
atlk_inline atlk_rc_t atlk_must_check
rsvc_ecc_response_encode(rsvc_ecc_response_t *out,
                         const ecc_response_t *in)
{
  const ecc_request_context_t *in_context;
  rsvc_ecc_request_context_t *out_context;

  /* Validate arguments */
  if (!in || !out) {
    return ATLK_E_INVALID_ARG;
  }

  /* Encode context */
  in_context = &in->context;
  out_context = &out->context;
  dx_uint32le_set(&out_context->request_id, in_context->request_id);
  dx_uint8_set(&out_context->request_type, in_context->request_type);
  dx_uint8_set(&out_context->curve, in_context->curve);

  /* Encode return code */
  dx_uint8_set(&out->rc, in->rc);

  /* Encode response result */
  switch (in_context->request_type) {
    case ECC_REQUEST_TYPE_VERIFY: {
      break;
    }
    case ECC_REQUEST_TYPE_SIGN: {
      const ecc_fast_verification_signature_t *in_signature = \
        &in->result.sign_result;
      rsvc_ecdsa_fast_verification_signature_t *out_signature = \
        &out->result.sign_result;
      rsvc_ecc_point_encode(&out_signature->R_point, &in_signature->R_point);
      rsvc_ecc_scalar_encode(&out_signature->s_scalar, &in_signature->s_scalar);
      break;
    }
    case ECC_REQUEST_TYPE_PMA: {
      rsvc_ecc_point_encode(&out->result.pma_result,
                            &in->result.pma_result);
      break;
    }
    default:
      return ATLK_E_INVALID_ARG;
  }

  return ATLK_OK;
}

/**
   Decode ECC response

   @param[out] out Decoded ECC response
   @param[in] in Encoded ECC response
*/
atlk_inline atlk_rc_t atlk_must_check
rsvc_ecc_response_decode(ecc_response_t *out,
                         const rsvc_ecc_response_t *in)
{
  const rsvc_ecc_request_context_t *in_context;
  ecc_request_context_t *out_context;
  uint8_t rc;

  /* Validate arguments */
  if (!in || !out) {
    return ATLK_E_INVALID_ARG;
  }

  /* Decode context */
  in_context = &in->context;
  out_context = &out->context;
  rsvc_ecc_request_context_decode(out_context, in_context);

  /* Decode return code */
  dx_uint8_get(&in->rc, &rc);
  out->rc = rc;

  /* Decode response result */
  switch (out_context->request_type) {
    case ECC_REQUEST_TYPE_VERIFY: {
      break;
    }
    case ECC_REQUEST_TYPE_SIGN: {
      const rsvc_ecdsa_fast_verification_signature_t *in_signature = \
        &in->result.sign_result;
      ecc_fast_verification_signature_t *out_signature = \
        &out->result.sign_result;
      rsvc_ecc_point_decode(&out_signature->R_point, &in_signature->R_point);
      rsvc_ecc_scalar_decode(&out_signature->s_scalar, &in_signature->s_scalar);
      break;
    }
    case ECC_REQUEST_TYPE_PMA: {
      rsvc_ecc_point_decode(&out->result.pma_result, &in->result.pma_result);
      break;
    }
    default:
      return ATLK_E_PROTOCOL_ERROR;
  }

  return ATLK_OK;
}

#endif /* _REMOTE_ECC_PROTOCOL_H */
