/* Copyright (C) 2014 Autotalks Ltd. */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include <libcli.h>

#include <craton/cli.h>
#include <craton/syslog.h>
#include <cc3100/simplelink.h>

#include "wlan_cmd.h"
#include <cc3100/sl_common.h>

unsigned const int WORKQUEUE_PRIORITY = 1;
unsigned const  int WORKQUEUE_PREEMPT_THRESHOLD = 1;
unsigned const long WORKQUEUE_TIME_SLICE = TX_NO_TIME_SLICE;

unsigned const int SL_TASK_PRIORITY = 2;
unsigned const int SL_TASK_PREEMPT_THRESHOLD = 2;
unsigned const long SL_TASK_TIME_SLICE = TX_NO_TIME_SLICE;

#ifndef SSID_NAME
/* Configuration of the device when it comes up in Station-mode */
char SSID_NAME[64] = {"autotalks_test_ap"};
int SEC_TYPE = SL_SEC_TYPE_OPEN;
char PASSKEY[64] = {""};
int PASSKEY_LEN = 0;

/* Configuration of the device when it comes up in AP-mode */
char SSID_AP_MODE[64] = {"autotalks_ap"};
char PASSWORD_AP_MODE[64] = {""};
int SEC_TYPE_AP_MODE = SL_SEC_TYPE_OPEN;

unsigned int IP_ADDR = 0xC0A806B4; //192.168.6.180
int PORT_NUM = 5001; //iPerf default

#endif

/*
 * GLOBAL VARIABLES -- Start
 */
_u32  g_Status = 0;
_u32  g_PingPacketsRecv = 0;
_u32  g_GatewayIP = 0;
_u32  g_StationIP = 0;

simpleLinkSockEventHandler_t simpleLinkSockEventHandler;
simpleLinkWlanEventHandler_t simpleLinkWlanEventHandler;
simpleLinkHttpServerCallback_t simpleLinkHttpServerCallback;
simpleLinkNetAppEventHandler_t simpleLinkNetAppEventHandler;
simpleLinkGeneralEventHandler_t simpleLinkGeneralEventHandler;

void SimpleLinkSockEventHandler(SlSockEvent_t *pSock) {
  if (simpleLinkSockEventHandler)  {
    simpleLinkSockEventHandler(pSock);
  }
}

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,SlHttpServerResponse_t *pSlHttpServerResponse) {
  if (simpleLinkHttpServerCallback) {
    simpleLinkHttpServerCallback(pHttpEvent, pSlHttpServerResponse);
  }
}

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pSlWlanEvent) {
  if (simpleLinkWlanEventHandler) {
    simpleLinkWlanEventHandler(pSlWlanEvent);
  }
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pSlNetApp) {
  if (simpleLinkNetAppEventHandler) {
    simpleLinkNetAppEventHandler(pSlNetApp);
  }
}

void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pSlDeviceEvent) {
  if (simpleLinkGeneralEventHandler) {
    simpleLinkGeneralEventHandler(pSlDeviceEvent);
  }
}

void CLI_Configure(void) {}

int CLI_Write(unsigned char *inBuff) {
  syslog(LOG_INFO, "%s", inBuff);
  return 0;
}

typedef struct unknown_dummy_t dummy_t;
extern dummy_t* g_pCB;
extern dummy_t g_StatMem;

void craton_user_init(void) {
  simpleLinkGeneralEventHandler = NULL;
  simpleLinkNetAppEventHandler = NULL;
  simpleLinkWlanEventHandler = NULL;
  simpleLinkHttpServerCallback = NULL;
  simpleLinkSockEventHandler = NULL;


  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* CLI instance */
  cli_instance_t *cli = NULL;
  /* CLI command handle */
  struct cli_command *command = NULL;

  /* Get CRATON UART CLI instance */
  rc = cli_instance_get(&cli, CLI_INSTANCE_TYPE_UART);
  if (atlk_error(rc)) {
    fprintf(stderr, "cli_instance_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }
  assert(cli != NULL);

  /* Register example command */
  command = cli_register_command(cli, NULL, "wifi_udp", wifi_udp_cmd,
            PRIVILEGE_UNPRIVILEGED, MODE_ANY, "wifi udp example");
  if (command == NULL) {
    fprintf(stderr, "cli_register_command returned NULL\n");
    goto error;
  }
  /* Register example command */
  command = cli_register_command(cli, NULL, "wifi_tcp", wifi_tcp_cmd,
            PRIVILEGE_UNPRIVILEGED, MODE_ANY, "wifi tcp example");
  if (command == NULL) {
    fprintf(stderr, "cli_register_command returned NULL\n");
    goto error;
  }
  /* Register example command */
  command = cli_register_command(cli, NULL, "wifi_http_srv", wifi_http_srv_cmd,
            PRIVILEGE_UNPRIVILEGED, MODE_ANY, "wifi http server example");
  if (command == NULL) {
    fprintf(stderr, "cli_register_command returned NULL\n");
    goto error;
  }
  /* Register example command */
  command = cli_register_command(cli, NULL, "wifi_p2p", wifi_p2p_cmd,
            PRIVILEGE_UNPRIVILEGED, MODE_ANY, "wifi p2p example");
  if (command == NULL) {
    fprintf(stderr, "cli_register_command returned NULL\n");
    goto error;
  }
  /* Register example command */
  command = cli_register_command(cli, NULL, "wifi_ap", wifi_ap_cmd,
            PRIVILEGE_UNPRIVILEGED, MODE_ANY, "wifi ap example");
  if (command == NULL) {
    fprintf(stderr, "cli_register_command returned NULL\n");
    goto error;
  }

  return;

  error: abort();
}
