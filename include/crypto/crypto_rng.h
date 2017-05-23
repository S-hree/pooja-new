/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _CRYPTO_CRYPTO_RNG
#define _CRYPTO_CRYPTO_RNG

#include <tomcrypt.h>

const struct ltc_prng_descriptor *
rng_descriptor_get(void);

const char *
rng_name_get(void);

#endif /* _CRYPTO_CRYPTO_RNG */
