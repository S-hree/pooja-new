/* Copyright (C) 2016 Autotalks Ltd. */
#ifndef _CRYPTO_CRYPTO_ERROR
#define _CRYPTO_CRYPTO_ERROR

#include <atlk/sdk.h>

/* Convert LibTomCrypt error to atlk_rc_t */
atlk_rc_t atlk_must_check
crypto_error_to_rc(int error);

#endif /* _CRYPTO_CRYPTO_ERROR */
