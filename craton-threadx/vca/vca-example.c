/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <atlk/sdk.h>
#include <atlk/vca.h>
#include <atlk/vcad.h>

#include <craton/bootparam.h>

/*
  CRATON ThreadX VCA Example

  This example demonstrates how to build with Autotalks sdk and VCA.
*/

#ifdef __CRATON_ARM
/* VCA sub-system */
static vca_srv_config_t vca_srv_config = VCA_SRV_CONFIG_INIT;

atlk_rc_t
vca_init(void)
{
  atlk_rc_t rc;

  rc = vca_srv_bootparam_read(&vca_srv_config);
  if (atlk_error(rc)) {
    printf("Failed to read VCA boot parameters\n");
    return rc;
  }

  vca_srv_config.tcp_server_enabled = 1;
  vca_srv_config.socket_enable[0] = 1;
  vca_srv_config.socket_enable[1] = 1;

  rc = vca_srv_init(&vca_srv_config);
  if (atlk_error(rc)) {
    printf("vca_srv_init failed: %d\n", rc);
    return rc;
  }

  rc = vca_mng_init();
  if (atlk_error(rc)) {
    printf("connect_vca_mibs failed: %d\n", rc);
    return rc;
  }

  return ATLK_OK;
}
#endif

void craton_user_init(void)
{
#ifdef __CRATON_ARM
  atlk_rc_t rc;

  printf("Initializing VCA service ... ");
  rc = vca_init();
  if (atlk_error(rc)) {
    printf("vca_init Failed: %d\n", rc);
    return;
  }
  printf("OK\n");
#endif
}
