#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __CRATON__
#include <craton/rng_hw.h>
#endif

#include <tommath.h>
#include <tomcrypt.h>

#include <atlk/sdk.h>
#include <atlk/rng.h>

#include <platform/common.h>

#include <crypto/crypto_rng.h>

atlk_rc_t
rng_data_get(void *ptr, size_t size)
{
  uint8_t *dest = ptr;

  /* Validate arguments */
  if (!ptr || !size) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

#ifndef __CRATON__
  if (rng_get_bytes(dest, size, NULL) != size) {
    TR_ERROR("Failed to get %zu random bytes", size);
    return ATLK_E_IO_ERROR;
  }
#else /* __CRATON__ */
  while (size) {
    size_t count;
    uint32_t random;

    atlk_rc_t rc;
    rc = rng_hw_get(&random, 1);
    if (atlk_error(rc)) {
      return rc;
    }

    count = min(size, sizeof(random));
    memcpy(dest, &random, count);
    dest += count;
    size -= count;
  }
#endif /* __CRATON__ */

  return ATLK_OK;
}

static int 
rng_start(prng_state *prng)
{
  return CRYPT_OK;
}

static int
rng_add_entropy(const unsigned char *in, unsigned long inlen, prng_state *prng)
{
  return CRYPT_OK;
}

static int 
rng_ready(prng_state *prng)
{
  return CRYPT_OK;
}

/** Read from the PRNG
    @param out     [out] Where to store the data
    @param outlen  Length of data desired (octets)
    @param prng    The PRNG state to read from
    @return Number of octets read
*/
static unsigned long
rng_read(unsigned char *out, unsigned long outlen, prng_state *prng)
{
  atlk_rc_t rc;

  if (!out || !outlen) {
    return 0;
  }

  rc = rng_data_get(out, outlen);
  if (atlk_error(rc)) {
    return 0;
  }

  return outlen;
}

static int
rng_done(prng_state *prng)
{
  return CRYPT_OK;
}


static int
rng_export(unsigned char *out, unsigned long *outlen, prng_state *prng)
{
  if (outlen) {
    *outlen = 0;
  }

  return CRYPT_OK;
}

static int
rng_import(const unsigned char *in, unsigned long inlen, prng_state *prng)
{
  return CRYPT_OK;
}

static int
rng_test(void)
{
  return CRYPT_OK;
}

/* Autotalks secure RNG descriptor */
static const struct ltc_prng_descriptor rng_descriptor = {
  "atlk", 0,
  rng_start,
  rng_add_entropy,
  rng_ready,
  rng_read,
  rng_done,
  rng_export,
  rng_import,
  rng_test
};

const struct ltc_prng_descriptor *
rng_descriptor_get(void)
{
#if defined(__CRATON__)
  return &rng_descriptor;
#else
  return &sprng_desc;
#endif /* __CRATON__ */
}

const char *
rng_name_get(void)
{
  const struct ltc_prng_descriptor *prng = rng_descriptor_get();
  return prng->name;
}

