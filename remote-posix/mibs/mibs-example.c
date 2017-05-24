/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include <atlk/mib_service.h>
#include <atlk/remote.h>
#include <atlk/mib_remote.h>

#include <atlk/mibs/wlan-mib.h>
#include <atlk/mibs/snmpv2-mib.h>

/*
  Remote POSIX MIBs Example

  This example demonstrates basic usage of the remote MIB API for code
  running externally to CRATON processor with an OS supporting POSIX.

  The MIB API mirrors Autotalks proprietary MIBs as well as selected MIB
  attributes from standard MIBs.

  The example demonstrates how to set the frequency of interface 1 to 5880
  MHz using WLAN MIB API (which mirrors AUTOTALKS-WLAN-MIB.mib) and how to
  get the system description via SNMPv2 MIB API (which mirrors the standard
  SNMPv2-MIB.mib).
*/

/* Size of system description string in bytes used in this example */
#define SYS_DESCR_SIZE 300

int main(int argc, char *argv[])
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Remote transport object */
  remote_transport_t *transport = NULL;
  /* Remote IP transport configuration parameters */
  remote_ip_transport_config_t config = REMOTE_IP_TRANSPORT_CONFIG_INIT;
  /* Local IPv4 address */
  uint32_t local_ipv4_addr;
  /* Server IPv4 address */
  uint32_t server_ipv4_addr;
  /* System description string */
  char sys_descr[SYS_DESCR_SIZE];
  /* Size of description string in bytes */
  size_t sys_descr_size = sizeof(sys_descr);
  /* MIB service */
  mib_service_t *mib_service = NULL;

  if ((argc != 3) || ((server_ipv4_addr = inet_addr(argv[1])) == INADDR_NONE)) {
    fprintf(stderr, "Usage: %s SERVER-IP4-ADDR LOCAL-IF-NAME\n", argv[0]);
    return EXIT_FAILURE;
  }

  /* Get local IPv4 address */
  rc = remote_util_local_ipv4_address_get(argv[2], &local_ipv4_addr);
  if (atlk_error(rc)) {
    fprintf(stderr, "remote_util_local_ipv4_address_get: %s\n",
            atlk_rc_to_str(rc));
    goto exit;
  }

  /* Set remote IP transport configuration parameters */
  config.local_ipv4_address = local_ipv4_addr;
  config.remote_ipv4_address = server_ipv4_addr;
  config.max_rtt_ms = 1500;

  /* Create remote IP transport object */
  rc = remote_ip_transport_create(&config, &transport);
  if (atlk_error(rc)) {
    fprintf(stderr, "remote_ip_transport_create: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  /* Create remote MIB service */
  rc = mib_remote_service_create(transport, NULL, &mib_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_remote_service_create: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  /* Set frequency at interface 1 to 5880 MHz */
  rc = mib_set_wlanFrequency(mib_service, 1, 5880);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_wlanFrequency: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }
  printf("Frequency at interface 1 set to 5880 MHz.\n");

  /* Get system description */
  rc = mib_get_sysDescr(mib_service, sys_descr, &sys_descr_size);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_get_sysDescr: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }
  printf("System description: %s\n", sys_descr);

exit:
  /* Clean-up resources */
  mib_service_delete(mib_service);
  remote_transport_delete(transport);

  return atlk_error(rc);
}
