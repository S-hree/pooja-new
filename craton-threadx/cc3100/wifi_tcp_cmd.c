/*
 * wifi_tcp_cmd.c
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

uint8_t wlan_tcp_main_stack[ATLK_SZ_4K];
TX_THREAD wlan_tcp_main_thread;

extern int wlan_tcp_main(int argc, char** argv);

extern void tcp_SimpleLinkSockEventHandler(SlSockEvent_t *);
extern void tcp_SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,SlHttpServerResponse_t *pSlHttpServerResponse);
extern void tcp_SimpleLinkWlanEventHandler(SlWlanEvent_t *pSlWlanEvent);
extern void tcp_SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pSlNetApp);
extern void tcp_SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pSlDeviceEvent);

extern unsigned int IP_ADDR;
extern int PORT_NUM;

static void wlan_tcp_main_entry(ULONG opaque) {
    (void) opaque;
    wlan_tcp_main(0, NULL);
}

static int parse_ip(char * input, unsigned int * output_ip ) {
    unsigned int parsed[4];
    if (4
            == sscanf(input, "%u.%u.%u.%u", &parsed[0], &parsed[1], &parsed[2],
                    &parsed[3])) {
        *output_ip = ((int) parsed[0]) << 24 | ((int) parsed[1]) << 16
                | ((int) parsed[2]) << 8 | ((int) parsed[3]);
        return SUCCESS;
    }
    return ERROR;
}

int wifi_tcp_cmd(cli_instance_t *cli, const char *command, char *argv[],
        int argc) {
    /* Unused parameters */
    (void) command;

    /*
     * example cli cmd:
     *
     *     wlan_tcp 192.168.43.1 5001 pAsSwOrD ssid_name
     *
     */
    if( argc > 0 ) {
        assert(! parse_ip(argv[0], &IP_ADDR) );
    }
    if( argc > 1 ) {
        sscanf(argv[1],"%d", &PORT_NUM);
    }
    if( argc > 2 ) { // if we have password, we assume running security SL_SEC_TYPE_WPA_WPA2.
        pal_Strcpy(PASSKEY, argv[2]);
        SEC_TYPE = SL_SEC_TYPE_WPA_WPA2;
    }
    if( argc > 3 ) {
        pal_Strcpy(SSID_NAME, argv[3]);
    }
    PASSKEY_LEN = pal_Strlen(PASSKEY);

    cli_print(cli,
            "Configured ssid: %s, Pass: %s, Security: %d, IP: (0x%X):%d",
            SSID_NAME, PASSKEY, SEC_TYPE, IP_ADDR, PORT_NUM);

    //register callbacks:
    simpleLinkSockEventHandler = &tcp_SimpleLinkSockEventHandler;
    simpleLinkHttpServerCallback = &tcp_SimpleLinkHttpServerCallback;
    simpleLinkWlanEventHandler = &tcp_SimpleLinkWlanEventHandler;
    simpleLinkNetAppEventHandler = &tcp_SimpleLinkNetAppEventHandler;
    simpleLinkGeneralEventHandler = &tcp_SimpleLinkGeneralEventHandler;

    int rc = tx_thread_create(&wlan_tcp_main_thread, "wlan_tcp_main",
            wlan_tcp_main_entry, 0, wlan_tcp_main_stack,
            sizeof(wlan_tcp_main_stack), 5, 5, TX_NO_TIME_SLICE, TX_AUTO_START);
    if (rc != TX_SUCCESS) {
        cli_print(cli, "Failed to create thread (rc=%d)", rc);
        return CLI_ERROR;
    }

    cli_print(cli, "tcp thread started");

    return CLI_OK;
}
