/*
 * wifi_p2p_cmd.c
 *
 *  Created on: Sep 29, 2014
 *      Author: Ido Reis <ido.reis@tandemg.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include <libcli.h>

#include <craton/cli.h>

#include "wlan_cmd.h"
#include <cc3100/sl_common.h>

#define ATLK_SZ_4K      0x00010000

uint8_t wlan_p2p_main_stack[ATLK_SZ_4K];
TX_THREAD wlan_p2p_main_thread;

extern int wlan_p2p_main(int argc, char** argv);

extern void p2p_SimpleLinkSockEventHandler(SlSockEvent_t *);
extern void p2p_SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,SlHttpServerResponse_t *pSlHttpServerResponse);
extern void p2p_SimpleLinkWlanEventHandler(SlWlanEvent_t *pSlWlanEvent);
extern void p2p_SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pSlNetApp);
extern void p2p_SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pSlDeviceEvent);

static void wlan_p2p_main_entry(ULONG opaque) {
	(void) opaque;
	wlan_p2p_main(0, NULL);
}

int wifi_p2p_cmd(cli_instance_t *cli, const char *command, char *argv[],
		int argc) {
	/* Unused parameters */
	(void) command;
  (void) argv;
  (void) argc;

    //register callbacks:
    simpleLinkSockEventHandler = &p2p_SimpleLinkSockEventHandler;
    simpleLinkHttpServerCallback = &p2p_SimpleLinkHttpServerCallback;
    simpleLinkWlanEventHandler = &p2p_SimpleLinkWlanEventHandler;
    simpleLinkNetAppEventHandler = &p2p_SimpleLinkNetAppEventHandler;
    simpleLinkGeneralEventHandler = &p2p_SimpleLinkGeneralEventHandler;

	int rc = tx_thread_create(&wlan_p2p_main_thread, "wlan_p2p_main",
			wlan_p2p_main_entry, 0, wlan_p2p_main_stack,
			sizeof(wlan_p2p_main_stack), 5, 5, TX_NO_TIME_SLICE,
			TX_AUTO_START);
	if (rc != TX_SUCCESS) {
		cli_print(cli, "Failed to create thread (rc=%d)", rc);
		return CLI_ERROR;
	}

	cli_print(cli, "p2p thread started");

	return CLI_OK;
}
