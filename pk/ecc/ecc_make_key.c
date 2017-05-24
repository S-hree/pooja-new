/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */

/* Implements ECC over Z/pZ for curve y^2 = x^3 - 3x + b
 *
 * All curves taken from NIST recommendation paper of July 1999
 * Available at http://csrc.nist.gov/cryptval/dss.htm
 */
#include "tomcrypt.h"

/**
  @file ecc_make_key.c
  ECC Crypto, Tom St Denis
*/  

#ifdef LTC_MECC

/* Number of extra bytes for randomization of ECC private key */
#define LTC_ECC_PRIVATE_KEY_RAND_BYTES 8

int ecc_make_private_key(prng_state *prng, int wprng, void *k, const ltc_ecc_set_type *dp)
{
   int            err;
   void          *order;
   unsigned char *buf;
   int            keysize;

   LTC_ARGCHK(ltc_mp.name != NULL);
   LTC_ARGCHK(dp          != NULL);

   /* good prng? */
   if ((err = prng_is_valid(wprng)) != CRYPT_OK) {
      return err;
   }

   keysize  = dp->size + LTC_ECC_PRIVATE_KEY_RAND_BYTES;

   /* allocate ram */
   buf  = XMALLOC(ECC_MAXSIZE + LTC_ECC_PRIVATE_KEY_RAND_BYTES);
   if (buf == NULL) {
      return CRYPT_MEM;
   }

   /* make up random string */
   if (prng_descriptor[wprng].read(buf, (unsigned long)keysize, prng) != (unsigned long)keysize) {
      err = CRYPT_ERROR_READPRNG;
      goto ERR_BUF;
   }

   /* setup the key variables */
   if ((err = mp_init_multi(&order, NULL)) != CRYPT_OK) {
      goto ERR_BUF;
   }

   /* read in the specs for this key */
   if ((err = mp_read_radix(order,   (char *)dp->order, 16)) != CRYPT_OK)                  { goto cleanup; }
   if ((err = mp_sub_d(order, 1, order)) != CRYPT_OK)                                      { goto cleanup; }
   if ((err = mp_read_unsigned_bin(k, (unsigned char *)buf, keysize)) != CRYPT_OK)         { goto cleanup; }
   if ((err = mp_mod(k, order, k)) != CRYPT_OK)                                            { goto cleanup; }
   /* 0 =< k < dp->order - 1 */
   if ((err = mp_add_d(k, 1, k)) != CRYPT_OK)                                              { goto cleanup; }
   /* 1 =< k < dp->order */

   /* free up ram */
   err = CRYPT_OK;
   goto cleanup;

cleanup:
   mp_clear_multi(order, NULL);
ERR_BUF:
#ifdef LTC_CLEAN_STACK
   zeromem(buf, ECC_MAXSIZE + LTC_ECC_PRIVATE_KEY_RAND_BYTES);
#endif
   XFREE(buf);
   return err;
}

/**
  Make a new ECC key 
  @param prng         An active PRNG state
  @param wprng        The index of the PRNG you wish to use
  @param keysize      The keysize for the new key (in octets from 20 to 65 bytes)
  @param key          [out] Destination of the newly created key
  @return CRYPT_OK if successful, upon error all allocated memory will be freed
*/
int ecc_make_key(prng_state *prng, int wprng, int keysize, ecc_key *key)
{
   int x, err;

   /* find key size */
   for (x = 0; (keysize > ltc_ecc_sets[x].size) && (ltc_ecc_sets[x].size != 0); x++);
   keysize = ltc_ecc_sets[x].size;

   if (keysize > ECC_MAXSIZE || ltc_ecc_sets[x].size == 0) {
      return CRYPT_INVALID_KEYSIZE;
   }
   err = ecc_make_key_ex(prng, wprng, key, &ltc_ecc_sets[x]);
   key->idx = x;
   return err;
}

int ecc_make_key_ex(prng_state *prng, int wprng, ecc_key *key, const ltc_ecc_set_type *dp)
{
   int            err;
   ecc_point     *base;
   void          *prime, *a;

   LTC_ARGCHK(key         != NULL);
   LTC_ARGCHK(ltc_mp.name != NULL);
   LTC_ARGCHK(dp          != NULL);

   key->idx = -1;
   key->dp  = dp;

   /* setup the key variables */
   if ((err = mp_init_multi(&key->pubkey.x, &key->pubkey.y, &key->pubkey.z, &key->k, &prime, &a, NULL)) != CRYPT_OK) {
      return err;
   }
   base = ltc_ecc_new_point();
   if (base == NULL) {
      err = CRYPT_MEM;
      goto errkey;
   }

   /* read in the specs for this key */
   if (dp->A) {
      if ((err = mp_read_radix(a,    (char *)key->dp->A, 16)) != CRYPT_OK)                      { goto errkey; } 
   }
   if ((err = mp_read_radix(prime,   (char *)key->dp->prime, 16)) != CRYPT_OK)                  { goto errkey; }
   if ((err = mp_read_radix(base->x, (char *)key->dp->Gx, 16)) != CRYPT_OK)                     { goto errkey; }
   if ((err = mp_read_radix(base->y, (char *)key->dp->Gy, 16)) != CRYPT_OK)                     { goto errkey; }
   if ((err = mp_set(base->z, 1)) != CRYPT_OK)                                                  { goto errkey; }

   /* make the private key */
   if ((err = ecc_make_private_key(prng, wprng, key->k, dp)) != CRYPT_OK)                       { goto errkey; }

   /* make the public key */
   if ((err = ltc_mp.ecc_ptmul(key->k, base, &key->pubkey, dp->A ? a : NULL, prime, 1)) != CRYPT_OK)              { goto errkey; }
   key->type = PK_PRIVATE;

   /* free up ram */
   err = CRYPT_OK;
   goto cleanup;
errkey:
   mp_clear_multi(key->pubkey.x, key->pubkey.y, key->pubkey.z, key->k, NULL);
cleanup:
   ltc_ecc_del_point(base);
   mp_clear_multi(prime, a, NULL);
   return err;
}

#endif
/* $Source$ */
/* $Revision$ */
/* $Date$ */

