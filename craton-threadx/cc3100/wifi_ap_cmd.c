/*
 * wifi_ap_cmd.c
 *
 *  Created on: Sep 29, 2014
 *      Author: Ido Reis <ido.reis@tandemg.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <tx_api.h>

#include <libcli.h>

#include <craton/cli.h>

#include "wlan_cmd.h"
#include <cc3100/sl_common.h>

#define ATLK_SZ_4K      0x00010000

uint8_t wlan_ap_main_stack[ATLK_SZ_4K];
TX_THREAD wlan_ap_main_thread;

extern int wlan_ap_main(int argc, char** argv);

extern void ap_SimpleLinkSockEventHandler(SlSockEvent_t *);
extern void ap_SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,SlHttpServerResponse_t *pSlHttpServerResponse);
extern void ap_SimpleLinkWlanEventHandler(SlWlanEvent_t *pSlWlanEvent);
extern void ap_SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pSlNetApp);
extern void ap_SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pSlDeviceEvent);

static void wlan_ap_main_entry(ULONG opaque) {
	(void) opaque;
	wlan_ap_main(0, NULL);
}


int wifi_ap_cmd(cli_instance_t *cli, const char *command, char *argv[],
		int argc) {
	/* Unused parameters */
	(void) command;

	cli_print(cli, "wlan_ap command, argc=%02d", argc);
	for (int i = 0; i < argc; i++) {
		cli_print(cli, "argv[%02d] = %s", i, argv[i]);
	}


	//register callbacks:
	simpleLinkSockEventHandler = &ap_SimpleLinkSockEventHandler;
	simpleLinkHttpServerCallback = &ap_SimpleLinkHttpServerCallback;
	simpleLinkWlanEventHandler = &ap_SimpleLinkWlanEventHandler;
	simpleLinkNetAppEventHandler = &ap_SimpleLinkNetAppEventHandler;
	simpleLinkGeneralEventHandler = &ap_SimpleLinkGeneralEventHandler;


	/*
	 * example cli cmd:
	 *
	 *     "wlan_ap AP_SSID PASSWORD"
	 *
	 */
	if( argc > 0 ) {
		pal_Strcpy(SSID_AP_MODE, argv[0]);
	}
	if( argc > 1 ) { // if we have password, we assume running security SL_SEC_TYPE_WPA_WPA2.
		pal_Strcpy(PASSWORD_AP_MODE, argv[1]); 
		SEC_TYPE_AP_MODE = SL_SEC_TYPE_WPA_WPA2;
	}
    PASSKEY_LEN = pal_Strlen(PASSKEY);

	cli_print(cli, "Configured ssid: %s, pass: %s, sec: %d", SSID_AP_MODE, PASSWORD_AP_MODE, SEC_TYPE_AP_MODE);

	int rc = tx_thread_create(&wlan_ap_main_thread, "wlan_ap_main",
			wlan_ap_main_entry, 0, wlan_ap_main_stack,
			sizeof(wlan_ap_main_stack), 5, 5, TX_NO_TIME_SLICE,
			TX_AUTO_START);
	if (rc != TX_SUCCESS) {
		cli_print(cli, "Failed to create thread (rc=%d)", rc);
		return CLI_ERROR;
	}

	cli_print(cli, "ap thread started");

	return CLI_OK;
}
