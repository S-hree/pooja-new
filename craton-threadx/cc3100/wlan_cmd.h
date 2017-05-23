/*
 * wlan_cmd.h
 *
 *  Created on: Sep 29, 2014
 *      Author: Ido Reis <ido.reis@tandemg.com>
 */

#ifndef WLAN_CMD_H_
#define WLAN_CMD_H_

#include <cc3100/simplelink.h>
#include <craton/cli.h>

#define WLAN_CMD_INIT_VAL 0xFFFFFFFF

#define WLAN_CMD_MAX_TX_POWER   15
#define WLAN_CMD_MIN_TX_CHANNEL 1
#define WLAN_CMD_MAX_TX_CHANNEL 13
#define WLAN_CMD_CW_MIN         (-25)
#define WLAN_CMD_CW_MAX         (25)

typedef struct wlan_config_params {
  _u32 tx_power;
  _u32 tx_channel;
  _i32 cw_offset;
  SlRateIndex_e rate;
  _u8 preamble;
}wlan_config_params_t;
#define WLAN_CONFIG_PARAMS_INIT {1, 11, 0, RATE_11M, 1}

struct wlan_cmd_demo_params {
  _u32 num_of_packets;
  _u32 packet_size;
  _u32 burst_size;
  _u32 burst_delay_ms;
  wlan_config_params_t wlan_config;
};

typedef struct wlan_statistics {
  _u32                  rx_byte_count;
  SlGetRxStatResponse_t rxStatResp;
}wlan_statistics_t;

#define WLAN_CMD_DAMO_PARAMS_INIT {0,0,0,0,WLAN_CONFIG_PARAMS_INIT}

extern int wifi_udp_cmd(cli_instance_t *cli, const char *command, char *argv[],
                        int argc);
extern int wifi_tcp_cmd(cli_instance_t *cli, const char *command, char *argv[],
                        int argc);
extern int wifi_http_srv_cmd(cli_instance_t *cli,
                             const char *command,
                             char *argv[],
                             int argc);
extern int wifi_ap_cmd(cli_instance_t *cli, const char *command, char *argv[],
                       int argc);
extern int wifi_p2p_cmd(cli_instance_t *cli, const char *command, char *argv[],
    int argc);

#endif /* WLAN_CMD_H_ */
