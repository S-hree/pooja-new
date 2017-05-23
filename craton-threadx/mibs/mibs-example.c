/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/mib_service.h>

#include <atlk/mibs/wlan-mib.h>
#include <atlk/mibs/snmpv2-mib.h>

/* 
  CRATON ThreadX MIBs Example 
  
  This example demonstrates basic usage of the MIB API for code running on
  top of CRATON processor with ThreadX RTOS.

  The MIB API mirrors Autotalks proprietary MIBs as well as selected MIB
  attributes from standard MIBs.
  
  The example demonstrates how to set the frequency of interface 1 to 5880
  MHz using WLAN MIB API (which mirrors AUTOTALKS-WLAN-MIB.mib) and how to
  get the system description via SNMPv2 MIB API (which mirrors the standard
  SNMPv2-MIB.mib).
*/

/* Size of system description string in bytes used in this example */
#define EXAMPLE_SYS_DESCR_SIZE 100

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* System description string */
  char sys_descr[EXAMPLE_SYS_DESCR_SIZE];
  /* Size of description string in bytes */
  size_t sys_descr_size = sizeof(sys_descr);
  /* MIB service */
  mib_service_t *service = NULL;

  /* Get default MIB service instance */
  rc = mib_default_service_get(&service);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  /* Set frequency at interface 1 to 5880 MHz */
  rc = mib_set_wlanFrequency(service, 1, 5880);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_wlanFrequency: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }
  printf("Frequency at interface 1 set to 5880 MHz.\n");

  /* Get system description */
  rc = mib_get_sysDescr(service, sys_descr, &sys_descr_size);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_get_sysDescr: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }
  printf("System description: %s\n", sys_descr);

exit:
  /* Clean-up resources */
  mib_service_delete(service);

  return;
}
