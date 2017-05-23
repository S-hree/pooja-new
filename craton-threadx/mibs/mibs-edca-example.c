/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/mib_service.h>

#include <atlk/mibs/wlan-mib.h>

/*
  CRATON ThreadX EDCA Table MIB Example 

  This example demonstrates basic usage of the MIB API for code running on
  top of CRATON processor with ThreadX RTOS.

  @todo: Currently v2x_send_params_t.user_priority is not supported. All
  frames are sent at access category AC_VO. Therefore, changing EDCA parameters
  of other access categories will have no effect.
*/

/* Access category: Best effort */
#define AC_BE 0

/* Access category: Background */
#define AC_BK 1

/* Access category: Video */
#define AC_VI 2

/* Access category: Voice */
#define AC_VO 3

/* Create EDCA table index from interface index and access category */
#define EDCA_INDEX(if_index, ac) ((if_index) * 4 + (ac) - 3)

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* MIB service */
  mib_service_t *service = NULL;
  /* EDCA CWmax value */
  uint32_t value;

  /* Get default MIB service instance */
  rc = mib_default_service_get(&service);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set CWmin of access category AC_BE at interface 1 to 15 */
  rc = mib_set_wlanEdcaCWmin(service, EDCA_INDEX(1, AC_BE), 15);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_wlanEdcaCWmin: %s\n", atlk_rc_to_str(rc));
    goto error;
  }
  printf("CWmin of access category AC_BE at interface 1 set to 15.\n");

  /* Get CWmax of access category AC_VO at interface 2 */
  rc = mib_get_wlanEdcaCWmax(service, EDCA_INDEX(2, AC_VO), &value);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_get_wlanEdcaCWmax: %s\n", atlk_rc_to_str(rc));
    goto error;
  }
  printf("CWmax of access category AC_VO at interface 2 is %lu\n", value);

  return;

error:
  mib_service_delete(service);
}
