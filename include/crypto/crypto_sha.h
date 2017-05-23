/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _CRYPTO_CRYPTO_SHA
#define _CRYPTO_CRYPTO_SHA

#include <tomcrypt.h>

#include <atlk/sdk.h>
#include <atlk/sha.h>
#include <atlk/sha_sw.h>

#include <platform/common.h>

/* LibTomCrypt name of SHA algorithm */
atlk_inline const char *
sha_algorithm_name(sha_algorithm_t algorithm)
{
  switch (algorithm) {
    case SHA_224:
      return "sha224";
    case SHA_256:
      return "sha256";
    case SHA_384:
      return "sha384";
  }
  BUG();
}

typedef struct {
  hash_state state;
  int hash;
} sha_sw_state_t;

#define SHA_SW_STATE_INIT { \
  .state = { { 0 } },       \
  .hash = -1,               \
}

atlk_rc_t atlk_must_check
sha_sw_sha224_init(sha_sw_state_t *state);

atlk_rc_t atlk_must_check
sha_sw_sha256_init(sha_sw_state_t *state);

atlk_rc_t atlk_must_check
sha_sw_sha384_init(sha_sw_state_t *state);

atlk_rc_t atlk_must_check
sha_sw_init(sha_algorithm_t algorithm,
            sha_sw_state_t *state);

atlk_rc_t atlk_must_check
sha_sw_process(sha_sw_state_t *state,
               const void *data_ptr,
               size_t data_size);

atlk_rc_t atlk_must_check
sha_sw_finalize(sha_sw_state_t *state,
                sha_digest_t *digest);

#endif /* _CRYPTO_CRYPTO_SHA */
