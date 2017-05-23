/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <unistd.h>

#include "unit.h"

/**
  CRATON ThreadX C++11 Example

  This example demonstrates a simple "Hello world" application written in
  C++11 running on top of a CRATON processor with ThreadX RTOS.
*/

#ifdef _DO_NOTHING

void craton_user_init(void)
{
}

#else /* _DO_NOTHING */

void craton_user_init(void)
{
#ifdef _INSERT_DELAY
  usleep(2000000);
#endif

  cxx_unit_test();
}

#endif /* _DO_NOTHING */
