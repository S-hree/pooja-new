/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#if defined __CRATON_NO_ARC

#include <craton/user.h>

/*
  CRATON User Abort Handler Example

  This example demonstrates basic usage of craton_user_abort_handler
  to override default exception handling on ARM CPU.

  craton_user_abort_handler can also be defined for each of the
  ARC CPUs

  The example will create an exception by writing to illegal address. The
  user-specific exception handler will be called upon exception.
*/

/* Illegal address for writing */
#define EXAMPLE_ILLEGAL_ADDRESS 0x0

/* Helper function to display informative string */
char *
example_exception_reason_to_string(exception_arm_reason_t reason)
{
  switch(reason) {
  case EXC_ARM_REASON_BACKGROUND:
    return "Background";
  case EXC_ARM_REASON_ALIGNMENT:
    return "Bad alignment";
  case EXC_ARM_REASON_DEBUG_EVENT:
    return "Debug event";
  case EXC_ARM_REASON_SYNC_EXTERNAL:
    return "Synchronous external event";
  case EXC_ARM_REASON_ASYNC_EXTERNAL:
    return "Asynchronous external event";
  case EXC_ARM_REASON_PERMISSION:
    return "Permission denied";
  case EXC_ARM_REASON_ASYNC_ECC:
    return "Asynchronous ECC";
  case EXC_ARM_REASON_SYNC_ECC:
    return "Synchronous ECC";
  case EXC_ARM_REASON_ABNORMAL_EXIT:
    return "Abnormal exit";
  default:
    return "Unknown reason";
  }
}

/* User-specific implementation for abort handling */
void
craton_user_abort_handler(const exception_info_t *info)
{
  printf("Craton user abort handler called.\n");
  printf("Exception reason: %s\n",
         example_exception_reason_to_string(info->reason));
}

void craton_user_init(void)
{
  printf("Application will cause an exception within 5 seconds...\n");
  usleep(5000000);

  /* Create exception by writing to illegal address */
  *(int *)EXAMPLE_ILLEGAL_ADDRESS = 1;

  return;
}

#else /* __CRATON_NO_ARC */

void craton_user_init(void)
{
}

#endif  /* __CRATON_NO_ARC  */
