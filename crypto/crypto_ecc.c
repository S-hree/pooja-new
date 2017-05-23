#include <unistd.h>
#include <tommath.h>
#include <tomcrypt.h>

#include <atlk/sdk.h>
#include <atlk/sizes.h>
#include <atlk/ecc.h>
#include <atlk/ecc_math.h>
#include <atlk/ecdsa.h>
#include <atlk/sha.h>

#include <platform/common.h>

#include <crypto/ecc_curves.h>
#include <crypto/crypto_fmt.h>
#include <crypto/crypto_init.h>
#include <crypto/crypto_rng.h>
#include <crypto/crypto_ecc.h>
#include <crypto/crypto_kdf.h>
#include <crypto/crypto_error.h>

/*
 * TODO:
 *
 * At least some of the static functions in this file are partially validating
 * arguments which is unnecessary since the API functions that call them
 * already do that.
 */

/*
 * Brainpool elliptic curves domain parameters (not part of LibTomCrypt)
 *
 * References:
 *   1. RFC 5639:
 *      "Elliptic Curve Cryptography (ECC) Brainpool Standard
 *      Curves and Curve Generation"
 *   2. RFC 6932:
 *      "Brainpool Elliptic Curves
 *      for the Internet Key Exchange (IKE) Group Description Registry"
 *   3. RFC 7027:
 *      "Elliptic Curve Cryptography (ECC) Brainpool Curves
 *      for Transport Layer Security (TLS)"
 */
static const ltc_ecc_set_type brainpoolP256t1 = {
  .size  = 32,
  .name  = "brainpoolP256t1",
  .prime = "A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377",
  .A     = NULL,
  .B     = "662C61C430D84EA4FE66A7733D0B76B7BF93EBC4AF2F49256AE58101FEE92B04",
  .order = "A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7",
  .Gx    = "A3E8EB3CC1CFE7B7732213B23A656149AFA142C47AAFBC2B79A191562E1305F4",
  .Gy    = "2D996C823439C56D7F7B22E14644417E69BCB6DE39D027001DABE8F35B25C9BE"
};

static const ltc_ecc_set_type brainpoolP256r1 = {
  .size  = 32,
  .name  = "brainpoolP256r1",
  .prime = "A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377",
  .A     = "7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9",
  .B     = "26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6",
  .order = "A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7",
  .Gx    = "8BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262",
  .Gy    = "547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997"
};

static const ltc_ecc_set_type brainpoolP384t1 = {
  .size  = 48,
  .name  = "brainpoolP384t1",
  .prime = "8CB91E82A3386D280F5D6F7E50E641DF152F7109ED5456B412B1DA197FB71123ACD3A729901D1A71874700133107EC53",
  .A     = NULL,
  .B     = "7F519EADA7BDA81BD826DBA647910F8C4B9346ED8CCDC64E4B1ABD11756DCE1D2074AA263B88805CED70355A33B471EE",
  .order = "8CB91E82A3386D280F5D6F7E50E641DF152F7109ED5456B31F166E6CAC0425A7CF3AB6AF6B7FC3103B883202E9046565",
  .Gx    = "18DE98B02DB9A306F2AFCD7235F72A819B80AB12EBD653172476FECD462AABFFC4FF191B946A5F54D8D0AA2F418808CC",
  .Gy    = "25AB056962D30651A114AFD2755AD336747F93475B7A1FCA3B88F2B6A208CCFE469408584DC2B2912675BF5B9E582928"
};

static const ltc_ecc_set_type brainpoolP384r1 = {
  .size  = 48,
  .name  = "brainpoolP384r1",
  .prime = "8CB91E82A3386D280F5D6F7E50E641DF152F7109ED5456B412B1DA197FB71123ACD3A729901D1A71874700133107EC53",
  .A     = "7BC382C63D8C150C3C72080ACE05AFA0C2BEA28E4FB22787139165EFBA91F90F8AA5814A503AD4EB04A8C7DD22CE2826",
  .B     = "04A8C7DD22CE28268B39B55416F0447C2FB77DE107DCD2A62E880EA53EEB62D57CB4390295DBC9943AB78696FA504C11",
  .order = "8CB91E82A3386D280F5D6F7E50E641DF152F7109ED5456B31F166E6CAC0425A7CF3AB6AF6B7FC3103B883202E9046565",
  .Gx    = "1D1C64F068CF45FFA2A63A81B7C13F6B8847A3E77EF14FE3DB7FCAFE0CBD10E8E826E03436D646AAEF87B2E247D4AF1E",
  .Gy    = "8ABE1D7520F9C2A45CB1EB8E95CFD55262B70B29FEEC5864E19C054FF99129280E4646217791811142820341263C5315"
};

/* Convert LibTomMath error to atlk_rc_t */
static atlk_rc_t atlk_must_check
mp_error_to_rc(int error)
{
  switch (error) {
    case MP_OKAY:
      return ATLK_OK;
    case MP_MEM:
      return ATLK_E_OUT_OF_MEMORY;
    case MP_VAL:
      return ATLK_E_INVALID_ARG;
    default:
      return ATLK_E_UNSPECIFIED;
  }
}

/**
   Convert ECC scalar to multiple precision integer

   @param[in] in ECC scalar 
   @param[out] out Multiple precision integer (must be already initialized!)

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
static atlk_rc_t
ecc_scalar_to_mp_int(const ecc_scalar_t *in, mp_int *out)
{
  uint8_t buf[sizeof(ecc_scalar_t)];
  uint8_t *pos;
  ssize_t i;
  int err;

  /* Validate arguments */
  if (!in || !out) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Initialize local buffer */
  memset(buf, 0, sizeof(buf));
  pos = buf;

  /* Store ECC scalar in unsigned big endian format */
  for (i = ECC_SCALAR_NUM_OF_UINT32 - 1; i >= 0; --i) {
    uint32_t elem = in->value[i];
    *(pos++) = (uint8_t)(elem >> 24);
    *(pos++) = (uint8_t)((elem & 0xff0000) >> 16);
    *(pos++) = (uint8_t)((elem & 0xff00) >> 8);
    *(pos++) = (uint8_t)(elem & 0xff);
  }

  /* Load multiple precision integer */
  err = mp_read_unsigned_bin(out, buf, sizeof(buf));
  if (err != MP_OKAY) {
    TR_ERROR("Failed to load multiple precision integer: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  return ATLK_OK;
}

/**
   Convert multiple precision integer to ECC scalar

   @param[in] in Multiple precision integer 
   @param[out] out ECC scalar

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
static atlk_rc_t
mp_int_to_ecc_scalar(mp_int *in, ecc_scalar_t *out)
{
  uint8_t buf[sizeof(ecc_scalar_t)];
  unsigned long buf_size;
  uint8_t *pos;
  ssize_t i, j;
  int err;

  /* Validate arguments */
  if (!in || !out) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Initialize local buffer */
  memset(buf, 0, sizeof(buf));
  buf_size = sizeof(buf);
  pos = buf;

  /* Store multiple precision integer in unsigned big endian format */
  err = mp_to_unsigned_bin_n(in, buf, &buf_size);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to store multiple precision integer: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  /* Convert to little endian format */
  for (i = 0, j = buf_size - 1; i < j; i++, j--) {
    uint8_t tmp = buf[i];
    buf[i] = buf[j];
    buf[j] = tmp;
  }

  /* Initialize ECC scalar */
  *out = (ecc_scalar_t)ECC_SCALAR_INIT;

  /* Load ECC scalar */
  for (i = 0; i < ECC_SCALAR_NUM_OF_UINT32; i++) {
    uint32_t elem = (uint32_t)*(pos++);
    elem |= (uint32_t)*(pos++) << 8;
    elem |= (uint32_t)*(pos++) << 16;
    elem |= (uint32_t)*(pos++) << 24;
    out->value[i] = elem;
  }

  return ATLK_OK;
}

atlk_rc_t
ecc_point_compress(const ecc_point_t *uncompressed, ecc_point_t *compressed)
{
  /* Validate arguments */
  if (!uncompressed || !compressed) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  compressed->x_coordinate = uncompressed->x_coordinate;

  if (uncompressed->point_type != ECC_POINT_UNCOMPRESSED) {
    compressed->point_type = uncompressed->point_type;
  }
  else if (uncompressed->y_coordinate.value[0] & 0x1) {
    compressed->point_type = ECC_POINT_COMPRESSED_LSB_Y_1;
  }
  else {
    compressed->point_type = ECC_POINT_COMPRESSED_LSB_Y_0;
  }

  return ATLK_OK;
}

/*
 * Find a quadratic residue (mod p) of 'a'. p must be an odd prime.
 *
 * Solve the congruence of the form:
 *   x^2 = a (mod p)
 * And returns x. Note that p - x is also a root.
 *
 * 0 is returned if no square root exists for these a and p.
 *
 * The Tonelli-Shanks algorithm is used (except for some simple cases
 * in which the solution is known from an identity). This algorithm runs in
 * polynomial time (unless the generalized Riemann hypothesis is false).
 *
 * References:
 *   - Wikipedia articles: Quadratic residue, Legendre symbol, Jacobi symbol,
 *     Euler's criterion
 *
 *   - "Computing modular square roots in Python" from "Eli Bendersky's website",
 *     http://eli.thegreenplace.net/2009/03/07/computing-modular-square-roots-in-python/
 *
 *   - "Square Roots from 1;24,51,10 to Dan Shanks",
 *     College Mathematics Journal 30 (March 1999), 82-95.
 */
static int
mp_sqrtmod(mp_int *a, mp_int *prime, mp_int *o)
{
  mp_int t, t1, t2, e, s, n, x, b, g, r, gs, m;
  int jacobi;
  int err = MP_OKAY;

  /* Calculate Jacobi symbol a|p (which in our case is just Legendre symbol) */
  err = mp_jacobi(a, prime, &jacobi);
  if (err != MP_OKAY) {
    return err;
  }

  /* Check Euler's criterion */
  if (jacobi != 1) {
    return MP_VAL;
  }

  /* Initialize multiple precision integers */
  err = mp_init_multi(&t, &t1, &t2, &e, &s, &n, &x, &b, &g, &r, &gs, &m, NULL);
  if (err != MP_OKAY) {
    return err;
  }

  /* If p = 3 (mod 4) then x = a^((p + 1)/4) (mod p) */

  /* t1 = 4 */
  mp_set(&t1, 4);

  /* t1 = p (mod 4) */
  err = mp_mod(prime, &t1, &t1);            if (err != MP_OKAY) { goto out; }

  /* If p == 3 (mod 4) */
  if (mp_cmp_d(&t1, 3) == MP_EQ) {
    /* t1 = 1 */
    mp_set(&t1, 1);

    /* t1 = p + 1 */
    err = mp_add(prime, &t1, &t1);          if (err != MP_OKAY) { goto out; }

    /* t1 = (p + 1)/2 */
    err = mp_div_2(&t1, &t1);               if (err != MP_OKAY) { goto out; }

    /* t1 = (p + 1)/4 */
    err = mp_div_2(&t1, &t1);               if (err != MP_OKAY) { goto out; }

    /* t1 = a^((p + 1)/4) (mod p) */
    err = mp_exptmod(a, &t1, prime, o);     if (err != MP_OKAY) { goto out; }

    goto out;
  }

  /*
   * Partition p-1 to s * 2^e for an odd s (i.e.
   * reduce all the powers of 2 from p-1)
   */

  /* s = p - 1 */
  err = mp_sub_d(prime, 1, &s);             if (err != MP_OKAY) { goto out; }

  /* e = 0 */
  mp_set(&e, 0);

  /* t2 = s mod 2 */
  err = mp_mod_2d(&s, 1, &t2);              if (err != MP_OKAY) { goto out; }

  /* While s is even */
  while (mp_cmp_d(&t2, 0) == MP_EQ) {
    /* s /= 2 */
    err = mp_div_2(&s, &s);                 if (err != MP_OKAY) { goto out; }

    /* e += 1*/
    err = mp_add_d(&e, 1, &e);              if (err != MP_OKAY) { goto out; }

    /* t2 = s mod 2 */
    err = mp_mod_2d(&s, 1, &t2);
  }

  /*
   * Find some 'n' with a Legendre symbol n|p = -1.
   * Shouldn't take long.
   */

  /* n = 1 */
  mp_set(&n, 1);

  do {
    /* n += 1 */
    err = mp_add_d(&n, 1, &n);              if (err != MP_OKAY) { goto out; }

    /* Calculate Jacobi symbol n|p */
    err = mp_jacobi(&n, prime, &jacobi);    if (err != MP_OKAY) { goto out; }

  } while (jacobi != -1);

  /*
   * x is a guess of the square root that gets better with each iteration.
   * b is the "fudge factor" - by how much we're off with the guess.
   * The invariant x^2 = ab (mod p) is maintained throughout the loop.
   * g is used for successive powers of n to update both a and b.
   * r is the exponent - decreases with each update.
   */

  /* x = a^((s + 1)/2) (mod p) */
  err = mp_add_d(&s, 1, &t2);               if (err != MP_OKAY) { goto out; }
  err = mp_div_2(&t2, &t2);                 if (err != MP_OKAY) { goto out; }
  err = mp_exptmod(a, &t2, prime, &x);      if (err != MP_OKAY) { goto out; }

  /* b = a^s (mod p) */
  err = mp_exptmod(a, &s, prime, &b);       if (err != MP_OKAY) { goto out; }

  /* g = n^s (mod p) */
  err = mp_exptmod(&n, &s, prime, &g);      if (err != MP_OKAY) { goto out; }

  /* r = e */
  err = mp_copy(&e, &r);                    if (err != MP_OKAY) { goto out; }

  while (1) {
    /* t = b */
    err = mp_copy(&b, &t);                  if (err != MP_OKAY) { goto out; }

    /* m = 0 */
    mp_set(&m, 0);

    /* t2 = 2 */
    mp_set(&t2, 2);

    /* While m <= r */
    while (mp_cmp(&m, &r) != MP_EQ) {
      /* If t == 1 */
      if (mp_cmp_d(&t, 1) == MP_EQ) {
        break;
      }

      /* t = t^2 (mod p) */
      err = mp_exptmod(&t, &t2, prime, &t); if (err != MP_OKAY) { goto out; }

      /* m += 1 */
      err = mp_add_d(&m, 1, &m);            if (err != MP_OKAY) { goto out; }
    }

    /* If m == 0 return x */
    if (mp_cmp_d(&m, 0) == MP_EQ) {
      break;
    }

    /* gs = g ^ (2 ^ (r - m - 1)) (mod p) */
    err = mp_sub(&r, &m, &t1);              if (err != MP_OKAY) { goto out; }
    err = mp_sub_d(&t1, 1, &t1);            if (err != MP_OKAY) { goto out; }

    /* 
     * We need to calculate 2^(r - m - 1) but LibTomMath does not have a
     * generic exponentiation function.
     * Fortunately, we know that
     *                   r-m-1 < r <= e
     * And
     *                   p-1 = s*2^e
     * Which implies
     *                   2^e <= p-1 < p
     * Therefore
     *                   2^(r-m-1) < 2^e < p
     * So
     *                   2^(r-m-1) = 2^(r-m-1) (mod p)
     */
    err = mp_exptmod(&t2, &t1, prime, &t1); if (err != MP_OKAY) { goto out; }
    err = mp_exptmod(&g, &t1, prime, &gs);  if (err != MP_OKAY) { goto out; }

    /* g = gs^2 (mod p) */
    err = mp_mulmod(&gs, &gs, prime, &g);   if (err != MP_OKAY) { goto out; }

    /* x = x * gs (mod p) */
    err = mp_mulmod(&gs, &x, prime, &x);    if (err != MP_OKAY) { goto out; }

    /* b = b * g (mod p) */
    err = mp_mulmod(&g, &b, prime, &b);     if (err != MP_OKAY) { goto out; }

    /* r = m */
    err = mp_copy(&m, &r);                  if (err != MP_OKAY) { goto out; }
  }

  err = mp_copy(&x, o);                     if (err != MP_OKAY) { goto out; }

out:
  /* Free multiple precision integers */
  mp_clear_multi(&t, &t1, &t2, &e, &s, &n, &x, &b, &g, &r, &gs, &m, NULL);

  return err;
}

static const ltc_ecc_set_type *
ecc_curve_dp(ecc_curve_t curve)
{
  int i;

  if (!ecc_curve_valid(curve)) {
    return NULL;
  }

  switch (curve) {
    /* First handle Brainpool curves which are not part of LibTomCrypt */
    case ECC_CURVE_BRAINPOOL_P256t1:
      return &brainpoolP256t1;
    case ECC_CURVE_BRAINPOOL_P256r1:
      return &brainpoolP256r1;
    case ECC_CURVE_BRAINPOOL_P384t1:
      return &brainpoolP384t1;
    case ECC_CURVE_BRAINPOOL_P384r1:
      return &brainpoolP384r1;
    default:
      /* NIST curves are part of LibTomCrypt and ordered by size */
      for (i = 0; ltc_ecc_sets[i].size != 0; i++) {
        if (ltc_ecc_sets[i].size == ecc_curve_size(curve)) {
          return &ltc_ecc_sets[i];
        }
      }
  }

  /* 
   * Curve was not found.
   * Shouldn't happen since we verified the curve is valid.
   */
  BUG();
}

atlk_rc_t
ecc_point_decompress(ecc_curve_t curve, const ecc_point_t *compressed,
  ecc_point_t *decompressed)
{
  mp_int prime, a, b, t1, t2, t3;
  const ltc_ecc_set_type *dp;
  int lsb;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!compressed || !decompressed) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* If point is already uncompressed, copy it's value as-is */
  if (compressed->point_type == ECC_POINT_UNCOMPRESSED) {
    *decompressed = *compressed;
    return ATLK_OK;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find the curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Initialize multiple precision integers */
  err = mp_init_multi(&prime, &a, &b, &t1, &t2, &t3, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  /* Read 'p' domain parameter */
  err = mp_read_radix(&prime, dp->prime, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read 'p' domain parameter for curve: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Read 'a' coefficient domain parameter */
  if (dp->A) {
    err = mp_read_radix(&a, dp->A, 16);
    if (err != MP_OKAY) {
      TR_ERROR("Failed to read 'a' coefficient domain parameter for curve: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto out;
    }
  }
  else {
    /* A = -3 (mod p) */
    mp_set(&a, 3);
    err = mp_neg(&a, &a);
    if (err != MP_OKAY) {
      TR_ERROR("Failed to set 'a' coefficient domain parameter for curve: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto out;
    }
  }

  /* Read 'b' coefficient domain parameter */
  err = mp_read_radix(&b, dp->B, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read 'b' coefficient domain parameter for curve: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Supported elliptic curve equation: y^2 = x^3 + a*x + b (mod p) */

  /* t1 = x */
  rc = ecc_scalar_to_mp_int(&compressed->x_coordinate, &t1); 
  if (atlk_error(rc)) {
    goto out;
  }

  /* t2 = 3 */
  mp_set(&t2, 3);

  /* t3 = x^3 (mod p) */
  err = mp_exptmod(&t1, &t2, &prime, &t3);
  if (err != MP_OKAY) {
    TR_ERROR("Modular exponentiation failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* t1 = a*x (mod p) */
  err = mp_mulmod(&t1, &a, &prime, &t1);
  if (err != MP_OKAY) {
    TR_ERROR("Modular multiplication failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* t1 = x^3 + a*x (mod p) */
  err = mp_addmod(&t3, &t1, &prime, &t1);
  if (err != MP_OKAY) {
    TR_ERROR("Modular subtraction failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* t1 = x^3 + a*x + b (mod p) */
  err = mp_addmod(&t1, &b, &prime, &t1);
  if (err != MP_OKAY) {
    TR_ERROR("Modular addition failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* t2^2 = x^3 + a*x + b (mod p) */
  err = mp_sqrtmod(&t1, &prime, &t2);
  if (err != MP_OKAY) {
    TR_ERROR("Modular square root failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* 
   * There are two solutions, in order to find out which one to use
   * we need to check the lsb.
   */
  if (compressed->point_type == ECC_POINT_COMPRESSED_LSB_Y_0) {
    lsb = 0;
  }
  else {
    lsb = 1;
  }

  /* t1 = t2 mod 2 */
  err = mp_mod_2d(&t2, 1, &t1);
  if (err != MP_OKAY) {
    TR_ERROR("Modular devision failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  if (mp_cmp_d(&t1, lsb) != MP_EQ) {
    /* t2 = p - t2 */
    err = mp_sub(&prime, &t2, &t2);
    if (err != MP_OKAY) {
      TR_ERROR("Modular subtraction failed: %s", mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto out;
    }
  }

  /* Copy the X coordinate */
  decompressed->point_type = ECC_POINT_UNCOMPRESSED;
  decompressed->x_coordinate = compressed->x_coordinate;

  /* Convert the MP integer Y coordinate to ECC scalar */
  rc = mp_int_to_ecc_scalar(&t2, &decompressed->y_coordinate);
  if (atlk_error(rc)) {
    goto out;
  }

out:
  /* Free multiple precision integers */
  mp_clear_multi(&prime, &a, &b, &t1, &t2, &t3, NULL);

  return rc;
}

static atlk_rc_t
ecc_point_to_ltc(const ecc_point_t *in, ecc_point *out)
{
  atlk_rc_t rc;

  /* Validate arguments */
  if (!in || !out) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (in->point_type != ECC_POINT_UNCOMPRESSED) {
    TR_ERROR_NO_ARGS("Currently only uncompressed ECC points are supported");
    return ATLK_E_UNSUPPORTED;
  }

  /* Convert ECC scalars to MP integers */
  rc = ecc_scalar_to_mp_int(&in->x_coordinate, out->x);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = ecc_scalar_to_mp_int(&in->y_coordinate, out->y);
  if (atlk_error(rc)) {
    return rc;
  }

  mp_set(out->z, 1);

  return ATLK_OK;
}

static atlk_rc_t
ltc_to_ecc_point(const ecc_point *in, ecc_point_t *out)
{
  atlk_rc_t rc;

  /* Validate arguments */
  if (!in || !out) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Make sure the ECC point is in affine coordinates */
  if (mp_cmp_d(in->z, 1) != MP_EQ) {
    TR_ERROR_NO_ARGS("ECC point must be in affine coordinates");
    return ATLK_E_INVALID_ARG;
  }

  /* Initialize ECC point */
  *out = (ecc_point_t)ECC_POINT_INIT;

  out->point_type = ECC_POINT_UNCOMPRESSED;

  /* Convert MP integers to ECC scalars */
  rc = mp_int_to_ecc_scalar(in->x, &out->x_coordinate);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = mp_int_to_ecc_scalar(in->y, &out->y_coordinate);
  if (atlk_error(rc)) {
    return rc;
  }

  return ATLK_OK;
}

int
ecc_point_valid(ecc_curve_t curve, const ecc_point_t *point)
{
  mp_int x, y, z;
  const ltc_ecc_set_type *dp;
  ecc_key key;
  int result = 0;
  int err;
  atlk_rc_t rc;

  /* Validate arguments */
  if (!point) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return 0;
  }

  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return 0;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* If the point is compressed, just try to decompress it */
  if (point->point_type != ECC_POINT_UNCOMPRESSED) {
    ecc_point_t decompressed = ECC_POINT_INIT;

    /* Decompress point */
    rc = ecc_point_decompress(curve, point, &decompressed);
    if (atlk_error(rc)) {
      return 0;
    }

    return 1;
  }

  /* Initialize MP integers */
  err = mp_init_multi(&x, &y, &z, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return 0;
  }

  rc = ecc_scalar_to_mp_int(&point->x_coordinate, &x);
  if (atlk_error(rc)) {
    goto out;
  }

  rc = ecc_scalar_to_mp_int(&point->y_coordinate, &y);
  if (atlk_error(rc)) {
    goto out;
  }

  mp_set(&z, 1);

  /* Find the curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Set ECC key */
  key.type = PK_PUBLIC;
  key.idx = -1;
  key.dp = dp;
  key.pubkey.x = &x;
  key.pubkey.y = &y;
  key.pubkey.z = &z;
  key.k = NULL;

  err = is_point(&key);
  if (err != CRYPT_OK) {
    goto out;
  }

  result = 1;

out:
  mp_clear_multi(&x, &y, &z, NULL);

  return result;
}

/* Make sure the private key < curve order and private_key > 0 */
int
ecc_private_key_valid(ecc_curve_t curve, const ecc_scalar_t *private_key)
{
  const ltc_ecc_set_type *dp;
  mp_int order, k;
  int result = 0;
  int err;
  atlk_rc_t rc;

  /* Validate arguments */
  if (!private_key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return 0;
  }

  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return 0;
  }

  /* Initialize MP integers */
  err = mp_init_multi(&order, &k, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return 0;
  }

  /* Convert private key to MP integer */
  rc = ecc_scalar_to_mp_int(private_key, &k);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Find curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Read 'n' domain parameter */
  err = mp_read_radix(&order, dp->order, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read 'n' domain parameter for curve: %s",
      mp_error_to_string(err));
    goto out;
  }

  /* Private key should be smaller than the order of the base point */
  if (mp_cmp(&k, &order) != MP_LT) {
    TR_ERROR_NO_ARGS("Private key is not smaller than the order of the curve base point");
    goto out;
  }

  /* Private key should not be zero */
  if (mp_cmp_d(&k, 0) == MP_EQ) {
    TR_ERROR_NO_ARGS("Private key cannot be zero");
    goto out;
  }

  result = 1;

out:
  mp_clear_multi(&order, &k, NULL);

  return result;
}

int
ecc_key_pair_valid(ecc_curve_t curve,
                   const ecc_scalar_t *private_key,
                   const ecc_point_t *public_key)
{
  ecc_point_t point = ECC_POINT_INIT;
  ecc_point_t uncompressed = ECC_POINT_INIT;
  atlk_rc_t rc;

  /* Validate arguments */
  if (!private_key || !public_key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return 0;
  }

  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return 0;
  }

  /* Make sure private key is valid */
  if (!ecc_private_key_valid(curve, private_key)) {
    TR_ERROR_NO_ARGS("Invalid private key");
    return 0;
  }

  /* Decompress public key (if needed) */
  rc = ecc_point_decompress(curve, public_key, &uncompressed);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to decompress public key (rc=%d)", rc);
    return 0;
  }

  /* Derive public key */
  rc = ecc_derive_public_key(curve, private_key, &point);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to derive public key (rc=%d)", rc);
    return 0;
  }

  /* Compare uncompressed public key with the derived public key */
  if (memcmp(&uncompressed, &point, sizeof(uncompressed)) != 0) {
    return 0;
  }

  return 1;
}

atlk_rc_t
ecc_point_add(ecc_curve_t curve,
              const ecc_point_t *P,
              const ecc_point_t *R,
              ecc_point_t *Q)
{
  ecc_point_t P_decompressed = ECC_POINT_INIT;
  ecc_point_t R_decompressed = ECC_POINT_INIT;
  const ltc_ecc_set_type *dp;
  ecc_point *PP = NULL;
  ecc_point *RR = NULL;
  ecc_point *QQ = NULL;
  mp_int prime, a, mu;
  mp_digit mp;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!P || !R || !Q) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  if (!ecc_point_valid(curve, P) || !ecc_point_valid(curve, R)) {
    TR_ERROR("Invalid ECC points for curve %s", ecc_curve_name(curve));
    return ATLK_E_INVALID_ARG;
  }

  /* Allocate ECC points */
  PP = ltc_ecc_new_point();
  RR = ltc_ecc_new_point();
  QQ = ltc_ecc_new_point();
  if (!PP || !RR || !QQ) {
    TR_ERROR_NO_ARGS("Failed to allocate ECC points");
    rc = ATLK_E_OUT_OF_MEMORY;
    goto out;
  }

  /* Initialize MP integers */
  err = mp_init_multi(&prime, &a, &mu, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Decompress ECC points */
  rc = ecc_point_decompress(curve, P, &P_decompressed);
  if (atlk_error(rc)) {
    goto clear;
  }

  rc = ecc_point_decompress(curve, R, &R_decompressed);
  if (atlk_error(rc)) {
    goto clear;
  }

  /* Convert ECC points */
  rc = ecc_point_to_ltc(&P_decompressed, PP);
  if (atlk_error(rc)) {
    goto clear;
  }

  rc = ecc_point_to_ltc(&R_decompressed, RR);
  if (atlk_error(rc)) {
    goto clear;
  }

  /* Find curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Read 'p' domain parameter */
  err = mp_read_radix(&prime, dp->prime, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read 'p' domain parameter for curve: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto clear;
  }

  /* Read 'a' coefficient domain parameter (if a != -3 mod p) */
  if (dp->A) {
    err = mp_read_radix(&a, dp->A, 16);
    if (err != MP_OKAY) {
      TR_ERROR("Failed to read 'a' coefficient domain parameter for curve: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto clear;
    }
  }

  /* Now everything is ready to actually do the computations */

  err = mp_montgomery_setup(&prime, &mp);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to setup montgomery reduction: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto clear;
  }

  err = mp_montgomery_calc_normalization(&mu, &prime);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to normalize montgomery reduction: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto clear;
  }

  if (mp_cmp_d(&mu, 1) != LTC_MP_EQ) {
    err = mp_mulmod(PP->x, &mu, &prime, PP->x);
    if (err != MP_OKAY) {
      TR_ERROR("Modular multiplication failed for P.x: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto clear;
    }

    err = mp_mulmod(PP->y, &mu, &prime, PP->y);
    if (err != MP_OKAY) {
      TR_ERROR("Modular multiplication failed for P.y: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto clear;
    }

    err = mp_mulmod(PP->z, &mu, &prime, PP->z);
    if (err != MP_OKAY) {
      TR_ERROR("Modular multiplication failed for P.z: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto clear;
    }

    err = mp_mulmod(RR->x, &mu, &prime, RR->x);
    if (err != MP_OKAY) {
      TR_ERROR("Modular multiplication failed for R.x: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto clear;
    }

    err = mp_mulmod(RR->y, &mu, &prime, RR->y);
    if (err != MP_OKAY) {
      TR_ERROR("Modular multiplication failed for R.y: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto clear;
    }

    err = mp_mulmod(RR->z, &mu, &prime, RR->z);
    if (err != MP_OKAY) {
      TR_ERROR("Modular multiplication failed for R.z: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto clear;
    }
  }

  err = ltc_mp.ecc_ptadd(PP, RR, QQ, dp->A ? &a : NULL, &prime, &mp);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to add ECC point: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto clear;
  }

  err = ltc_mp.ecc_map(QQ, &prime, &mp);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to map ECC point: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto clear;
  }

  /* Convert ECC point */
  rc = ltc_to_ecc_point(QQ, Q);
  if (atlk_error(rc)) {
    goto clear;
  }

clear:
  /* Free multiple precision integers */
  mp_clear_multi(&prime, &a, &mu, NULL);

out:
  /* Free ECC points */
  ltc_ecc_del_point(PP);
  ltc_ecc_del_point(RR);
  ltc_ecc_del_point(QQ);

  return rc;
}

atlk_rc_t
ecc_point_multiply_add(ecc_curve_t curve,
                       const ecc_point_t *P,
                       const ecc_scalar_t *e,
                       const ecc_point_t *R,
                       ecc_point_t *Q)
{
  ecc_point_t P_decompressed = ECC_POINT_INIT;
  ecc_point_t R_decompressed = ECC_POINT_INIT;
  const ltc_ecc_set_type *dp;
  ecc_point *PP = NULL;
  ecc_point *RR = NULL;
  ecc_point *QQ = NULL;
  mp_int ee, prime, a, one;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!P || !e || !R || !Q) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  if (!ecc_point_valid(curve, P) || !ecc_point_valid(curve, R)) {
    TR_ERROR("Invalid ECC points for curve %s", ecc_curve_name(curve));
    return ATLK_E_INVALID_ARG;
  }

  /* Allocate ECC points */
  PP = ltc_ecc_new_point();
  RR = ltc_ecc_new_point();
  QQ = ltc_ecc_new_point();
  if (!PP || !RR || !QQ) {
    TR_ERROR_NO_ARGS("Failed to allocate ECC points");
    rc = ATLK_E_OUT_OF_MEMORY;
    goto out;
  }

  /* Initialize MP integers */
  err = mp_init_multi(&ee, &prime, &a, &one, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Decompress ECC points */
  rc = ecc_point_decompress(curve, P, &P_decompressed);
  if (atlk_error(rc)) {
    goto clear;
  }

  rc = ecc_point_decompress(curve, R, &R_decompressed);
  if (atlk_error(rc)) {
    goto clear;
  }

  /* Convert ECC points */
  rc = ecc_point_to_ltc(&P_decompressed, PP);
  if (atlk_error(rc)) {
    goto clear;
  }

  rc = ecc_point_to_ltc(&R_decompressed, RR);
  if (atlk_error(rc)) {
    goto clear;
  }

  /* Convert ECC scalar to MP integer */
  rc = ecc_scalar_to_mp_int(e, &ee);
  if (atlk_error(rc)) {
    goto clear;
  }

  /* Find curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Read 'p' domain parameter */
  err = mp_read_radix(&prime, dp->prime, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read 'p' domain parameter for curve: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto clear;
  }

  /* Read 'a' coefficient domain parameter (if a != -3 mod p) */
  if (dp->A) {
    err = mp_read_radix(&a, dp->A, 16);
    if (err != MP_OKAY) {
      TR_ERROR("Failed to read 'a' coefficient domain parameter for curve: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto clear;
    }
  }

  /* Now everything is ready to actually do the computations */

  mp_set(&one, 1);

  err = ltc_mp.ecc_mul2add(PP, &ee, RR, &one, QQ, dp->A ? &a : NULL, &prime);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to multiply-add ECC point: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto clear;
  }

  /* Convert ECC point */
  rc = ltc_to_ecc_point(QQ, Q);
  if (atlk_error(rc)) {
    goto clear;
  }

clear:
  /* Free multiple precision integers */
  mp_clear_multi(&ee, &prime, &a, &one, NULL);

out:
  /* Free ECC points */
  ltc_ecc_del_point(PP);
  ltc_ecc_del_point(RR);
  ltc_ecc_del_point(QQ);

  return rc;
}

atlk_rc_t
ecc_private_key_multiply_add(ecc_curve_t curve,
                             const ecc_scalar_t *private_key,
                             const ecc_scalar_t *addend,
                             const ecc_scalar_t *multiplier,
                             ecc_scalar_t *result)
{
  const ltc_ecc_set_type *dp;
  mp_int k, a, m ,r, n;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!private_key || !addend || !multiplier || !result) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  if (!ecc_private_key_valid(curve, private_key)) {
    TR_ERROR("Invalid private key for curve %s", ecc_curve_name(curve));
    return ATLK_E_INVALID_ARG;
  }

  /* Find the curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Initialize multiple precision integers */
  err = mp_init_multi(&k, &a, &m ,&r, &n, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  /* Read 'n' domain parameter */
  err = mp_read_radix(&n, dp->order, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read 'n' domain parameter for curve: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Convert ECC scalars to MP integers */
  rc = ecc_scalar_to_mp_int(private_key, &k);
  if (atlk_error(rc)) {
    goto out;
  }

  rc = ecc_scalar_to_mp_int(addend, &a);
  if (atlk_error(rc)) {
    goto out;
  }

  rc = ecc_scalar_to_mp_int(multiplier, &m);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Make sure the key multiplier is not a multiple of the curve order */
  err = mp_mod(&m, &n, &r);
  if (err != MP_OKAY) {
    TR_ERROR("Modular devision failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  if (mp_cmp_d(&r, 0) != MP_GT) {
    TR_ERROR_NO_ARGS("Key multiplier cannot be a multiple of curve order");
    rc = ATLK_E_UNSPECIFIED;
    goto out;
  }

  /* We want to calculate: r = a + (k * m) mod n */

  /* r = k * m (mod n) */
  err = mp_mulmod(&k, &m, &n, &r);
  if (err != MP_OKAY) {
    TR_ERROR("Modular multiplication failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* r = r + a = (k * m) + a (mod n) */
  err = mp_addmod(&r, &a, &n, &r);
  if (err != MP_OKAY) {
    TR_ERROR("Modular addition failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Convert MP integer to ECC scalar */
  rc = mp_int_to_ecc_scalar(&r, result);
  if (atlk_error(rc)) {
    goto out;
  }

out:
  /* Free multiple precision integers */
  mp_clear_multi(&k, &a, &m ,&r, &n, NULL);

  return rc;
}

atlk_rc_t
ecdsa_signature_convert(ecc_curve_t curve,
                        const ecc_fast_verification_signature_t *fv_signature,
                        ecc_signature_t *signature)
{
#if ATLK_SDK_VER < ATLK_SDK_VER_TUPLE(5, 0, 0)
  const ltc_ecc_set_type *dp;
  const ecc_point_t *R;
  const ecc_scalar_t *s;
  mp_int order, r;
  int err;
#endif
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!fv_signature || !signature) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

#if ATLK_SDK_VER >= ATLK_SDK_VER_TUPLE(5, 0, 0)
  signature->r_scalar = fv_signature->r_scalar;
  signature->s_scalar = fv_signature->s_scalar;
#else
  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Initialize multiple precision integers */
  err = mp_init_multi(&order, &r, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  R = &fv_signature->R_point;
  s = &fv_signature->s_scalar;

  /* Find the curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Read 'n' domain parameter */
  err = mp_read_radix(&order, dp->order, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read 'n' domain parameter for curve: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Convert X coordinate of R to MP integer */
  rc = ecc_scalar_to_mp_int(&R->x_coordinate, &r);
  if (atlk_error(rc)) {
    goto out;
  }

  /* r = r mod n */
  err = mp_mod(&r, &order, &r);
  if (err != MP_OKAY) {
    TR_ERROR("Modular devision failed: %s", mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Convert 'r' to ECC scalar */
  rc = mp_int_to_ecc_scalar(&r, &signature->r_scalar);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Copy 's' as-is */
  signature->s_scalar = *s;

out:
  /* Free multiple precision integers */
  mp_clear_multi(&order, &r, NULL);
#endif

  return rc;
}

atlk_rc_t
ecc_generate_private_key(ecc_curve_t curve,
                         ecc_scalar_t *private_key)
{
  const ltc_ecc_set_type *dp;
  mp_int k;
  prng_state prng;
  int prng_index;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!private_key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find PRNG */
  prng_index = find_prng(rng_name_get());
  BUG_ON(prng_index == -1);

  /* Find curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Initialize multiple precision integers */
  err = mp_init_multi(&k, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  /* Generate an ECC private key */
  err = ecc_make_private_key(&prng, prng_index, &k, dp);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to generate ECC private key: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto out;
  }

  /* Read private key */
  rc = mp_int_to_ecc_scalar(&k, private_key);
  if (atlk_error(rc)) {
    goto out;
  }

out:
  /* Free multiple precision integers */
  mp_clear_multi(&k, NULL);

  /* TODO: Terminate the PRNG (?) */

  return rc;
}

atlk_rc_t
ecc_generate_key_pair(ecc_curve_t curve,
                      ecc_scalar_t *private_key,
                      ecc_point_t *public_key)
{
  const ltc_ecc_set_type *dp;
  ecc_key key;
  prng_state prng;
  int prng_index;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!private_key || !public_key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Find PRNG */
  prng_index = find_prng(rng_name_get());
  BUG_ON(prng_index == -1);

  /* Find curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Generate an ECC key */
  err = ecc_make_key_ex(&prng, prng_index, &key, dp);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to generate ECC key: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto out;
  }

  /* Read private key */
  rc = mp_int_to_ecc_scalar(key.k, private_key);
  if (atlk_error(rc)) {
    goto cleanup;
  }

  /* Read public key */
  rc = ltc_to_ecc_point(&key.pubkey, public_key);
  if (atlk_error(rc)) {
    goto cleanup;
  }

cleanup:
  /* Free the ECC key */
  ecc_free(&key);

out:
  /* TODO: Terminate the PRNG (?) */

  return rc;
}

atlk_rc_t
ecc_derive_public_key(ecc_curve_t curve,
                      const ecc_scalar_t *private_key,
                      ecc_point_t *public_key)
{
  const ltc_ecc_set_type *dp;
  ecc_point *G = NULL;
  ecc_point *R = NULL;
  mp_int prime, a, k;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!private_key || !public_key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Check validity of private key */
  if (!ecc_private_key_valid(curve, private_key)) {
    TR_ERROR("Invalid private key for curve %s", ecc_curve_name(curve));
    return ATLK_E_INVALID_ARG;
  }

  /* Allocate ECC points */
  G = ltc_ecc_new_point();
  if (!G) {
    TR_ERROR("Failed to allocate ECC point");
    rc = ATLK_E_OUT_OF_MEMORY;
    goto out;
  }

  R = ltc_ecc_new_point();
  if (!R) {
    TR_ERROR("Failed to allocate ECC point");
    rc = ATLK_E_OUT_OF_MEMORY;
    goto out;
  }

  /* Initialize MP integers */
  err = mp_init_multi(&prime, &a, &k, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Find the curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Convert ECC private key to MP integer */
  rc = ecc_scalar_to_mp_int(private_key, &k);
  if (atlk_error(rc)) {
    goto clear;
  }

  /* Read the prime modulus for this curve */
  err = mp_read_radix(&prime, dp->prime, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read prime modulus: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto clear;
  }

  /* Read 'a' coefficient domain parameter (if a != -3 mod p) */
  if (dp->A) {
    err = mp_read_radix(&a, dp->A, 16);
    if (err != MP_OKAY) {
      TR_ERROR("Failed to read 'a' coefficient domain parameter for curve: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto clear;
    }
  }

  /* Read the curve generator element */
  err = mp_read_radix(G->x, dp->Gx, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read generator X coordinate: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto clear;
  }

  err = mp_read_radix(G->y, dp->Gy, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read generator Y coordinate: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto clear;
  }

  mp_set(G->z, 1);

  /* Make the public key */
  err = ltc_mp.ecc_ptmul(&k, G, R, dp->A ? &a : NULL, &prime, 1);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to multiply ECC point: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto clear;
  }

  /* Convert public key to ECC point */
  rc = ltc_to_ecc_point(R, public_key);
  if (atlk_error(rc)) {
    goto clear;
  }

clear:
  mp_clear_multi(&prime, &a, &k, NULL);

out:
  ltc_ecc_del_point(G);
  ltc_ecc_del_point(R);

  return rc;
}

#define CRYPTO_ECC_MAX_BUFFER_SIZE ATLK_SZ_256

static atlk_rc_t
ecc_key_import(ecc_curve_t curve,
               const ecc_scalar_t *private_key,
               const ecc_point_t *public_key,
               ecc_key *key)
{
  const ltc_ecc_set_type *dp;
  mp_int k, Qx, Qy;
  unsigned char flags[1];
  unsigned long key_size;
  uint8_t der_key[CRYPTO_ECC_MAX_BUFFER_SIZE];
  unsigned long der_key_size = sizeof(der_key);
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!public_key || !key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (public_key->point_type != ECC_POINT_UNCOMPRESSED) {
    TR_ERROR("Unsupported ECC point type %d", public_key->point_type);
    return ATLK_E_UNSUPPORTED;
  }

  /* We assume the private and public keys are valid */

  /* Initialize MP integers */
  err = mp_init_multi(&k, &Qx, &Qy, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  /* Convert public key coordinates to MP integers */
  rc = ecc_scalar_to_mp_int(&public_key->x_coordinate, &Qx);
  if (atlk_error(rc)) {
    goto out;
  }

  rc = ecc_scalar_to_mp_int(&public_key->y_coordinate, &Qy);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Find elliptic curve key size */
  key_size = ecc_curve_size(curve);

  /* Find the curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  if (private_key) {
    /* Private key */
    flags[0] = 1;

    /* Convert private key to MP integer */
    rc = ecc_scalar_to_mp_int(private_key, &k);
    if (atlk_error(rc)) {
      goto out;
    }

    /* Encode private & public key to DER format */
    err = der_encode_sequence_multi(der_key, &der_key_size,
                                    LTC_ASN1_BIT_STRING,    1UL, flags,
                                    LTC_ASN1_SHORT_INTEGER, 1UL, &key_size,
                                    LTC_ASN1_INTEGER,       1UL, &Qx,
                                    LTC_ASN1_INTEGER,       1UL, &Qy,
                                    LTC_ASN1_INTEGER,       1UL, &k,
                                    LTC_ASN1_EOL,           0UL, NULL);
    if (err != CRYPT_OK) {
      TR_ERROR("Failed to encode DER private key: %s", error_to_string(err));
      rc = crypto_error_to_rc(err);
      goto out;
    }
  }
  else {
    /* Public key */
    flags[0] = 0;

    /* Encode public key to DER format */
    err = der_encode_sequence_multi(der_key, &der_key_size,
                                    LTC_ASN1_BIT_STRING,    1UL, flags,
                                    LTC_ASN1_SHORT_INTEGER, 1UL, &key_size,
                                    LTC_ASN1_INTEGER,       1UL, &Qx,
                                    LTC_ASN1_INTEGER,       1UL, &Qy,
                                    LTC_ASN1_EOL,           0UL, NULL);
    if (err != CRYPT_OK) {
      TR_ERROR("Failed to encode DER public key: %s", error_to_string(err));
      rc = crypto_error_to_rc(err);
      goto out;
    }
  }

  /* Import ECC key from DER format */
  err = ecc_import_ex(der_key, der_key_size, key, dp);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to import ECC key: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto out;
  }

out:
  mp_clear_multi(&k, &Qx, &Qy, NULL);

  return rc;
}

atlk_rc_t
ecdsa_digest_sign(ecc_curve_t curve,
                  const ecc_scalar_t *private_key,
                  const sha_digest_t *digest,
                  ecc_fast_verification_signature_t *fv_signature)
{
  uint8_t der_signature[CRYPTO_ECC_MAX_BUFFER_SIZE];
  unsigned long der_signature_size = sizeof(der_signature);
  prng_state prng;
  ecc_key key;
  mp_int Rx, Ry, r, s;
  ecc_point_t public_key = ECC_POINT_INIT;
  int prng_index;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!private_key || !digest || !fv_signature) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Initialize MP integers */
  err = mp_init_multi(&Rx, &Ry, &r, &s, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  /* Derive public key */
  rc = ecc_derive_public_key(curve, private_key, &public_key);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Import ECC key */
  rc = ecc_key_import(curve, private_key, &public_key, &key);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Find PRNG */
  prng_index = find_prng(rng_name_get());
  BUG_ON(prng_index == -1);

  /* ECDSA sign for fast verification */
  err = ecc_sign_hash_fast(digest->value, digest->value_size,
    der_signature, &der_signature_size, &prng, prng_index, &key);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to sign hash: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto out;
  }

  /* Decode signature from DER format */
  err = der_decode_sequence_multi(der_signature, der_signature_size,
    LTC_ASN1_INTEGER, 1UL, &Rx,
    LTC_ASN1_INTEGER, 1UL, &Ry,
    LTC_ASN1_INTEGER, 1UL, &r,
    LTC_ASN1_INTEGER, 1UL, &s,
    LTC_ASN1_EOL, 0UL, NULL);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to decode DER signature: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto out;
  }

  /* Initialize signature */
  *fv_signature = (ecc_fast_verification_signature_t)
    ECC_FAST_VERIFICATION_SIGNATURE_INIT;

  /* Convert the MP integers to signature */
  fv_signature->R_point.point_type = ECC_POINT_UNCOMPRESSED;

  rc = mp_int_to_ecc_scalar(&Rx, &fv_signature->R_point.x_coordinate);
  if (atlk_error(rc)) {
    goto out;
  }

  rc = mp_int_to_ecc_scalar(&Ry, &fv_signature->R_point.y_coordinate);
  if (atlk_error(rc)) {
    goto out;
  }

#if ATLK_SDK_VER >= ATLK_SDK_VER_TUPLE(5, 0, 0)
  rc = mp_int_to_ecc_scalar(&r, &fv_signature->r_scalar);
  if (atlk_error(rc)) {
    goto out;
  }
#endif

  rc = mp_int_to_ecc_scalar(&s, &fv_signature->s_scalar);
  if (atlk_error(rc)) {
    goto out;
  }

  /* TODO: Terminate the PRNG (?) */

out:
  mp_clear_multi(&Rx, &Ry, &r, &s, NULL);

  return rc;
}

atlk_rc_t
ecdsa_digest_verify(ecc_curve_t curve,
                    const ecc_point_t *public_key,
                    const sha_digest_t *digest,
                    const ecc_signature_t *signature,
                    ecc_rc_t *result)
{
  ecc_point_t decompressed = ECC_POINT_INIT;
  uint8_t der_signature[CRYPTO_ECC_MAX_BUFFER_SIZE];
  unsigned long der_signature_size = sizeof(der_signature);
  mp_int r, s;
  ecc_key key;
  int stat;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!public_key || !digest || !signature || !result) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Decompress public key */
  rc = ecc_point_decompress(curve, public_key, &decompressed);
  if (atlk_error(rc)) {
    return rc;
  }

  if ((public_key->point_type == ECC_POINT_UNCOMPRESSED) &&
      !ecc_point_valid(curve, public_key)) {
    TR_ERROR("Invalid public key for curve %s", ecc_curve_name(curve));
    return ATLK_E_INVALID_ARG;
  }

  err = mp_init_multi(&r, &s, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  rc = ecc_scalar_to_mp_int(&signature->r_scalar, &r);
  if (atlk_error(rc)) {
    goto out;
  }

  rc = ecc_scalar_to_mp_int(&signature->s_scalar, &s);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Encode DER signature */
  err = der_encode_sequence_multi(der_signature, &der_signature_size,
                                  LTC_ASN1_INTEGER, 1UL, &r,
                                  LTC_ASN1_INTEGER, 1UL, &s,
                                  LTC_ASN1_EOL, 0UL, NULL);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to encode DER signature: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto out;
  }

  /* Import ECC key */
  rc = ecc_key_import(curve, NULL, &decompressed, &key);
  if (atlk_error(rc)) {
    goto out;
  }

  /* ECDSA verify */
  err = ecc_verify_hash(der_signature, der_signature_size,
                        digest->value, digest->value_size,
                        &stat, &key);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to verify hash: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto out;
  }

  *result = stat ? ECC_OK : ECC_E_VERIFY_FAILED;

out:
  mp_clear_multi(&r, &s, NULL);

  return rc;
}

atlk_rc_t
ecies_key_derive(ecc_curve_t curve,
                 const ecc_scalar_t *private_key,
                 const ecc_point_t *peer_public_key,
                 void *key,
                 size_t key_size,
                 const void *kdf_param,
                 size_t kdf_param_size)
{
  ecc_point_t decompressed = ECC_POINT_INIT;
  mp_int k, prime, a;
  ecc_point *public_key = NULL;
  ecc_point *result = NULL;
  const ltc_ecc_set_type *dp;
  ecc_scalar_t secret = ECC_SCALAR_INIT;
  size_t secret_u32_len;
  size_t secret_len;
  uint8_t shared_secret[sizeof(secret)] = { 0 };
  sha_algorithm_t sha_algorithm;
  int err;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!private_key || !peer_public_key || !key_size || !key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Verify elliptic curve is over 224-bit */
  if (ecc_curve_size(curve) <= ECC_CURVE_P224_KEY_SIZE) {
    TR_ERROR("Unsupported elliptic curve %d", curve);
    return ATLK_E_UNSUPPORTED;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Check validity of private key */
  if (!ecc_private_key_valid(curve, private_key)) {
    TR_ERROR("Invalid private key for curve %s", ecc_curve_name(curve));
    return ATLK_E_INVALID_ARG;
  }

  /* Check validity of peer public key */
  if (!ecc_point_valid(curve, peer_public_key)) {
    TR_ERROR("Invalid peer public key for curve %s", ecc_curve_name(curve));
    return ATLK_E_INVALID_ARG;
  }

  /* Initialize MP integers */
  err = mp_init_multi(&k, &prime, &a, NULL);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to initialize multiple precision integers: %s",
      mp_error_to_string(err));
    return mp_error_to_rc(err);
  }

  /* Allocate ECC points */
  public_key = ltc_ecc_new_point();
  result = ltc_ecc_new_point();
  if (!public_key || !result) {
    TR_ERROR_NO_ARGS("Failed to allocate ECC points");
    rc = ATLK_E_OUT_OF_MEMORY;
    goto out;
  }

  /* Convert private key to MP integer */
  rc = ecc_scalar_to_mp_int(private_key, &k);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Decompress public key if needed */
  rc = ecc_point_decompress(curve, peer_public_key, &decompressed);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Convert public key to ECC point */
  rc = ecc_point_to_ltc(&decompressed, public_key);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Find curve domain parameters */
  dp = ecc_curve_dp(curve);
  BUG_ON(!dp);

  /* Read 'p' domain parameter */
  err = mp_read_radix(&prime, dp->prime, 16);
  if (err != MP_OKAY) {
    TR_ERROR("Failed to read 'p' domain parameter for curve: %s",
      mp_error_to_string(err));
    rc = mp_error_to_rc(err);
    goto out;
  }

  /* Read 'a' coefficient domain parameter (if a != -3 mod p) */
  if (dp->A) {
    err = mp_read_radix(&a, dp->A, 16);
    if (err != MP_OKAY) {
      TR_ERROR("Failed to read 'a' coefficient domain parameter for curve: %s",
        mp_error_to_string(err));
      rc = mp_error_to_rc(err);
      goto out;
    }
  }

  /* Multiply point */
  err = ltc_mp.ecc_ptmul(&k, public_key, result, dp->A ? &a : NULL, &prime, 1);
  if (err != CRYPT_OK) {
    TR_ERROR("Failed to multiply ECC point: %s", error_to_string(err));
    rc = crypto_error_to_rc(err);
    goto out;
  }

  /* TODO: check result != O (?) */

  /* Store the X coordinate as the shared secret */
  rc = mp_int_to_ecc_scalar(result->x, &secret);
  if (atlk_error(rc)) {
    goto out;
  }

  /* Convert the shared secret value to an octet string */
  secret_u32_len = ecc_curve_to_u32_len(curve);
  secret_len = secret_u32_len * sizeof(uint32_t);
  fmt_i2osp(secret.value, shared_secret, secret_u32_len);

  /*
   * Currently we have a 1-to-1 mapping between
   * elliptic curve size and SHA algorithm.
   */
  switch (ecc_curve_size(curve)) {
    case ECC_CURVE_P256_KEY_SIZE:
      sha_algorithm = SHA_256;
      break;
    case ECC_CURVE_P384_KEY_SIZE:
      sha_algorithm = SHA_384;
      break;
    default:
      BUG();
  }

  /* Derive an ECIES secret key */
  rc = kdf2_compute(sha_algorithm, shared_secret, secret_len,
                    kdf_param, kdf_param_size,
                    key, key_size);
  if (atlk_error(rc)) {
    goto out;
  }

out:
  /* Free ECC points */
  ltc_ecc_del_point(public_key);
  ltc_ecc_del_point(result);

  /* Clear MP integers */
  mp_clear_multi(&k, &prime, &a, NULL);

  return rc;
}

atlk_rc_t
ecies_key_create(ecc_curve_t curve,
              const ecc_point_t *peer_public_key,
              ecc_point_t *public_key,
              void *key,
              size_t key_size,
              const void *kdf_param,
              size_t kdf_param_size)
{
  ecc_scalar_t private_key = ECC_SCALAR_INIT;
  atlk_rc_t rc = ATLK_OK;

  /* Validate arguments */
  if (!ecc_curve_valid(curve)) {
    TR_ERROR("Invalid elliptic curve %d", curve);
    return ATLK_E_INVALID_ARG;
  }

  if (!peer_public_key || !public_key || !key_size || !key) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  /* Verify elliptic curve is over 224-bit */
  if (ecc_curve_size(curve) <= ECC_CURVE_P224_KEY_SIZE) {
    TR_ERROR("Unsupported elliptic curve %d", curve);
    return ATLK_E_UNSUPPORTED;
  }

  /* Lazy initialize the cryptographic library */
  crypto_init();

  /* Check validity of peer public key */
  if (!ecc_point_valid(curve, peer_public_key)) {
    TR_ERROR("Invalid peer public key for curve %s", ecc_curve_name(curve));
    return ATLK_E_INVALID_ARG;
  }

  /* Generate ephemeral key pair */
  rc = ecc_generate_key_pair(curve, &private_key, public_key);
  if (atlk_error(rc)) {
    return rc;
  }

  /* Derive ECIES shared secret key */
  rc = ecies_key_derive(curve, &private_key, peer_public_key, key, key_size,
                        kdf_param, kdf_param_size);
  if (atlk_error(rc)) {
    return rc;
  }

  return rc;
}

