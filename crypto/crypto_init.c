#include <tommath.h>
#include <tomcrypt.h>

#include <atlk/sdk.h>

#include <platform/common.h>
#include <platform/osal.h>

#include <crypto/crypto_rng.h>

static osal_once_t crypto_once = OSAL_ONCE_INIT;

static void
crypto_init_once(void)
{
  const struct ltc_prng_descriptor *prng;
  int err;

  /* Initialize math provider */
  ltc_mp = ltm_desc;

  /* Register SHA-224 */
  err = register_hash(&sha224_desc);
  BUG_ON(err == -1);

  /* Register SHA-256 */
  err = register_hash(&sha256_desc);
	BUG_ON(err == -1);

  /* Register SHA-384 */
  err = register_hash(&sha384_desc);
	BUG_ON(err == -1);

  /* Register PRNG */
  prng = rng_descriptor_get();
  err = register_prng(prng);
  BUG_ON(err == -1);

  /* Register AES */
  err = register_cipher(&aes_desc);
  BUG_ON(err == -1);
}

void
crypto_init(void)
{
  osal_once_run(&crypto_once, crypto_init_once);
}

