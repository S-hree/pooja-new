
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>
#include <nx_api.h>
#include <libcli.h>
#include <craton/cli.h>
#include <craton/net.h>
#include <libcli.h>
#include <craton/cli.h>

#include <linuxapi.h>
#include <wave_lib.h>
#include <porting_apis.h>

static int ip_is_initialed;
static NX_IP *ether_ip_instance;
static NX_IP *wlan_ip_instance;
static NX_PACKET_POOL cvt_packet_pool;
static NX_UDP_SOCKET ether_udp_inst;
static unsigned char cvt_mem[32000];

void _ip_init(void)
{
	atlk_rc_t rc = ATLK_OK;
	UINT status;
//	NXD_ADDRESS ip_address;
//	UINT address_index;

	if(! ip_is_initialed) {
		/* Get trusted IP instance */
		rc = net_ip_trusted_instance_get(&ether_ip_instance);
		assert(!atlk_error(rc));

		/* Get untrusted IP instance */
		rc = net_ip_untrusted_instance_get(&wlan_ip_instance);
		assert(!atlk_error(rc));

		status = nxd_ipv6_enable(wlan_ip_instance);
		syslog(LOG_DEBUG, "nxd_ipv6_enable ret=0x%X\n", status);
		assert(status == 0x00);

		status = nxd_icmp_enable(wlan_ip_instance);
		syslog(LOG_DEBUG, "nxd_icmp_enable ret=0x%X\n", status);
		assert(status == 0x00);

		status = nxd_ipv6_address_set(wlan_ip_instance, 0, NX_NULL, 10, NX_NULL);
		syslog(LOG_DEBUG, "nxd_ipv6_address_set 0 ret=0x%X\n", status);
		assert(status == 0x00);

		ip_is_initialed=1;
	}
}

void handle_eget(cli_instance_t *cli, char *argv[], int argc)
{
	(void) argv;
	(void) argc;
	UINT status;
	ULONG addr=0, mask=0;
	char addr_str[20], mask_str[20];

	if((strcmp(argv[argc - 1] , "?") == 0) || (argc != 1)){
		cli_print(cli, "(get Ethernet ip): ip eget");
		cli_print(cli, "Example: ip eget");
		// return
	} else {
		status = nx_ip_interface_address_get(ether_ip_instance, 0, &addr, &mask);
		syslog(LOG_DEBUG, "nx_ip_interface_address_get ret=0x%X\n", status);

		cli_print(cli, "Ethernet addr=%s, mask=%s",
			addr_to_string_v4(addr, addr_str, sizeof(addr_str)),
			addr_to_string_v4(mask, mask_str, sizeof(mask_str)));
	}
}

void handle_eset(cli_instance_t *cli, char *argv[], int argc)
{
	(void) argv;
	(void) argc;
	UINT status;
	ULONG addr=0, mask=0;

	if((strcmp(argv[argc - 1] , "?") == 0) || (argc != 3)){
		cli_print(cli, "(set Ethernet ip): ip eset <addr> <netmask>");
		cli_print(cli, "Example: ip eset 192.168.2.50 255.255.255.0");
	} else {
		addr = string_to_addr_v4(argv[1]);
		mask = string_to_addr_v4(argv[2]);

		if((addr == 0) || (mask == 0)){
			cli_print(cli, "addr/mask format error!");
		} else {
			status = nx_ip_interface_address_set(ether_ip_instance, 0, addr, mask);
			syslog(LOG_DEBUG, "nx_ip_interface_address_set ret=0x%X\n", status);
		}
	}
}

void handle_eping(cli_instance_t *cli, char *argv[], int argc)
{
	(void) argv;
	(void) argc;
	UINT status;
	ULONG ping_addr=0;
	char data[]="abcd";
	ULONG data_size=4;
	int counts=0, max=5;
	NX_PACKET *recv;
	char ip_str[64];

	if((strcmp(argv[argc - 1] , "?") == 0) || (argc != 2)){
		cli_print(cli, "(ping from Ethernet ip): ip eping <addr>");
		cli_print(cli, "Example: ip eping 192.168.10.30");
	} else {
		(void) memset(ip_str, 0, sizeof(ip_str));
		(void) strcpy(ip_str, argv[1]);
		ping_addr = string_to_addr_v4(argv[1]);

		while(counts < max){
			status = nx_icmp_ping(ether_ip_instance, ping_addr, data, data_size, &recv, 10);
			syslog(LOG_DEBUG, "nx_icmp_ping ret=0x%X\n", status);

			if(status == NX_NO_RESPONSE) {
				cli_print(cli, "ping to %s count=%d/%d, NO RESPONSE!!", ip_str, counts+1, max);
			} else if(status == NX_SUCCESS) {
				cli_print(cli, "ping to %s count=%d/%d, OK.", ip_str, counts+1, max);
			} else {
				cli_print(cli, "ping to %s count=%d/%d, err=0x%X", ip_str, counts+1, max, status);
			}
			counts++;
			(void) usleep(1000 * 1000);
		}
	}
}

void handle_wget(cli_instance_t *cli, char *argv[], int argc)
{
	(void) argv;
	(void) argc;
	UINT status;
	NXD_ADDRESS ip_address;
	UINT address_index;
	ULONG prefix_length;
	UINT interface_id;
	char addr_str[40];

	if((strcmp(argv[argc - 1] , "?") == 0) || (argc != 1)){
		cli_print(cli, "(get wlan ipv6): ip eget");
		cli_print(cli, "Example: ip eget");
	} else {
		for(address_index = 0 ; address_index < NX_MAX_IPV6_ADDRESSES ; address_index++){
			status = nxd_ipv6_address_get(wlan_ip_instance, address_index, &ip_address,	&prefix_length, &interface_id);
			syslog(LOG_DEBUG, "nxd_ipv6_address_get ret=0x%X\n", status);

			if(status == NX_SUCCESS){
				cli_print(cli, "wlan @%d addr=%s, prefix=%ld", interface_id,
					addr_to_string_v6(ip_address, addr_str, sizeof(addr_str)), prefix_length);
			}
		}
	}
}

void handle_wset(cli_instance_t *cli, char *argv[], int argc)
{
	(void) argv;
	(void) argc;
	NXD_ADDRESS set_ip;
	UINT status;
	ULONG prefix_length;
	char addr_str[40];

	if((strcmp(argv[argc - 1] , "?") == 0) || (argc != 3)){
		cli_print(cli, "(set wlan ipv6): ip wset <ipv6 addr> <prefix>");
		cli_print(cli, "Example: ip wset 3ffe:ffff:0:f101::1 64");
	} else {
		(void) string_to_addr_v6(argv[1], &set_ip);
		prefix_length = i_atoi(argv[2]);

		cli_print(cli, "set addr:%s, prefix=%ld", addr_to_string_v6(set_ip, addr_str, sizeof(addr_str)), prefix_length);

		status = nxd_ipv6_address_set(wlan_ip_instance, 0, &set_ip, prefix_length, NX_NULL);
		if(status == NX_SUCCESS){
			cli_print(cli, "set wlan @0 ipv6 addr success.");
		}else{
			cli_print(cli, "nxd_ipv6_address_set ret=0x%X", status);
		}
	}
}

void handle_wping(cli_instance_t *cli, char *argv[], int argc)
{
	(void) argv;
	(void) argc;
	UINT status;
	NXD_ADDRESS ping6_ip;
	char data[]="abcd";
	ULONG data_size=4;
	int counts=0, max=5;
	NX_PACKET *recv;
	char addr_str[40];
	ULONG prefix_length;

	if((strcmp(argv[argc - 1] , "?") == 0) || (argc != 3)){
		cli_print(cli, "(ping from wlan ipv6): ip wp	ing <IPv6 addr> <prefix>");
		cli_print(cli, "Example: ip wping 3ffe:ffff:0:f101::2 64");
	} else {
		(void) string_to_addr_v6(argv[1], &ping6_ip);
		prefix_length = i_atoi(argv[2]);
		cli_print(cli, "start ping to %s/%ld", addr_to_string_v6(ping6_ip, addr_str, sizeof(addr_str)), prefix_length);

		while(counts < max){
			status = nxd_icmp_ping(wlan_ip_instance, &ping6_ip, data, data_size, &recv, prefix_length * 100);
			syslog(LOG_DEBUG, "nxd_icmp_interface_ping ret=0x%X\n", status);

			if(status == NX_NO_RESPONSE) {
				cli_print(cli, "ping count=%d/%d, NO RESPONSE!!", counts+1, max);
			} else if(status == NX_SUCCESS) {
				cli_print(cli, "ping count=%d/%d, OK.", counts+1, max);
			} else {
				cli_print(cli, "ping count=%d/%d, err=0x%X", counts+1, max, status);
			}
			counts++;
			(void) usleep(1000 * 1000);
		}
	}
}

void handle_wtcp(cli_instance_t *cli, char *argv[], int argc)
{
	(void) argv;
	(void) argc;
	UINT status;
	NX_TCP_SOCKET client_socket;
	NXD_ADDRESS server_ip;
	UINT port;

	if((strcmp(argv[argc - 1] , "?") == 0) || (argc != 3)){
		cli_print(cli, "(tcp client test from wlan ipv6): ip wtcp <IPv6 addr> <port>");
		cli_print(cli, "Example: ip wtcp 3ffe:ffff:0:f101::2 12345");
	} else {
		(void) string_to_addr_v6(argv[1], &server_ip);
		port = i_atoi(argv[2]);

		status = nx_tcp_socket_create(wlan_ip_instance, &client_socket, "Client Socket",
					NX_IP_NORMAL, NX_FRAGMENT_OKAY, 0x80, 200, NX_NULL, NX_NULL);
		cli_print(cli, "nx_tcp_socket_create ret=0x%X", status);

		status = nx_tcp_client_socket_bind(&client_socket, NX_ANY_PORT, 7);
		cli_print(cli, "nx_tcp_client_socket_bind ret=0x%X", status);

		status = nxd_tcp_client_socket_connect(&client_socket, &server_ip, port, NX_WAIT_FOREVER);
		cli_print(cli, "nxd_tcp_client_socket_connect ret=0x%X", status);


		status = nx_tcp_client_socket_unbind(&client_socket);
		status = nx_tcp_socket_delete(&client_socket);
	}
}

void handle_cvt(cli_instance_t *cli, char *argv[], int argc)
{
	(void) argv;
	(void) argc;
	UINT status;
	//ULONG ping_addr=0;
	//char *data="abcd";
	//ULONG data_size=4;
	//int counts=0, max=5;
	//NX_PACKET *recv;
	NX_PACKET *cvt_send_packet;

	if((strcmp(argv[argc - 1] , "?") == 0) || (argc != 2)){
		cli_print(cli, "(ping from Ethernet ip): ip eping <addr>");
		cli_print(cli, "Example: ip eping 192.168.2.30");
	} else {

		status = nx_packet_pool_create(&cvt_packet_pool, NULL, 256, cvt_mem, 32000);
		cli_print(cli, "nx_packet_pool_create ret=0x%X", status);

		status = nx_packet_allocate(&cvt_packet_pool, &cvt_send_packet, NX_UDP_PACKET, NX_WAIT_FOREVER);
		cli_print(cli, "nx_packet_allocate ret=0x%X", status);

		status = nx_packet_data_append(cvt_send_packet, "abcde", 5, &cvt_packet_pool, NX_WAIT_FOREVER);
		cli_print(cli, "nx_packet_data_append ret=0x%X", status);

		status = nx_udp_socket_create(ether_ip_instance, &ether_udp_inst, NULL, NX_IP_NORMAL, NX_FRAGMENT_OKAY, 0x80, 30);
		cli_print(cli, "nx_udp_socket_create ret=0x%X", status);

		status = nx_udp_socket_bind(&ether_udp_inst, 100, NX_WAIT_FOREVER);
		cli_print(cli, "nx_udp_socket_bind ret=0x%X", status);

		status = nx_udp_socket_send(&ether_udp_inst, cvt_send_packet, IP_ADDRESS(140,96,109,80), 100);
		cli_print(cli, "nx_udp_socket_send ret=0x%X", status);

		(void) usleep(1000 * 1000);
	}
}

void ip_print_help(cli_instance_t *cli)
{
	cli_print(cli, "ip commands");
	cli_print(cli, "usage: ip <command>");
	cli_print(cli, "<command>: eget");
	cli_print(cli, "<command>: eset");
	cli_print(cli, "<command>: eping");
	cli_print(cli, "<command>: wget");
	cli_print(cli, "<command>: wset");
	cli_print(cli, "<command>: wping");
}

int cmd_ip(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	(void)cli;
	(void)command;

	if(strcmp(argv[0] , "eget") == 0) {
		handle_eget(cli, argv, argc);
	} else if(strcmp(argv[0] , "eset") == 0) {
		handle_eset(cli, argv, argc);
	} else if(strcmp(argv[0] , "eping") == 0) {
		handle_eping(cli, argv, argc);
	} else if(strcmp(argv[0] , "wget") == 0) {
		handle_wget(cli, argv, argc);
	} else if(strcmp(argv[0] , "wset") == 0) {
		handle_wset(cli, argv, argc);
	} else if(strcmp(argv[0] , "wping") == 0) {
		handle_wping(cli, argv, argc);
	} else if(strcmp(argv[0] , "wtcp") == 0) {
		handle_wtcp(cli, argv, argc);
	} else if(strcmp(argv[0] , "cvt") == 0) {
		handle_cvt(cli, argv, argc);
	} else {
		ip_print_help(cli);
	}
    return CLI_OK;

}

