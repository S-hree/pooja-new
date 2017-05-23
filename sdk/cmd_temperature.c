#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <tx_api.h>
#include <tx_posix.h>
#include <nx_api.h>
#include <libcli.h>
#include <craton/cli.h>
#include <craton/net.h>
#include <craton/fs.h>

#include <linuxapi.h>

#include <nxd_bsd.h>



#include <wave_lib.h>

#include <dot3_common.h>
#include <dot3_api.h>
//#include "cmd_oslink.h"
#include <porting_apis.h>

#include <atlk/sdk.h>
#include <atlk/can.h>
#include <atlk/can_service.h>
#include <craton/sha_hw.h>
#include <craton/slx97_host.h>
#include <craton/bootparam.h>
#include <sys/time.h>
#include <nxd_bsd.h>
#include <tx_posix.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <porting_apis.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <atlk/nav_service.h>
#include <gps.h>


#define PSID_NUMBER 22
/* CAN ID used in this example */
#define EXAMPLE_CAN_ID 0x50UL

/* CAN device ID used in this example */
#define EXAMPLE_CAN_DEVICE_ID 0

/* Thread priorities */
#define CAN_SEND_THREAD_PRIORITY 40
#define CAN_RECEIVE_THREAD_PRIORITY 41

/* Port number used in this example */
#define UDP_PORT_NUMBER 2015

/* Maximum message size in octets */
#define UDP_MAX_MSG_SIZE 100

/* CAN send thread */
static TX_THREAD can_send_thread;
static uint8_t can_send_thread_stack[5*0x1000];
static void can_send_thread_entry(ULONG input);

/* CAN receive thread */
static TX_THREAD can_receive_thread;
static uint8_t can_receive_thread_stack[5*0x1000];
static void can_receive_thread_entry(ULONG input);

/* DSRC thread */
static TX_THREAD wsmp_recv_thread;
static unsigned char wsmp_recv_stack[5*1024];

/* UDP Rx thread */
static TX_THREAD udp_recv_thread;
static unsigned char udp_recv_stack[5*1024];

/* CAN service */
static can_service_t *can_service = NULL;

/* CAN socket */
static can_socket_t *can_socket = NULL;

/* CAN ID filter array */
static const can_id_filter_t filter_array[] = {
  { .can_id = 0, .can_id_mask = 0 },
};

static wme_handle_t handle;
static int handle_ok_flag = 0;

static int udp_fd_ok_flag = 0;
static char server_ip[20];
static struct sockaddr_in servaddr;


static int fd = -1;
static cli_instance_t *temp_cli;

void gps_handle_show(void)
{	
	struct gps_info gps;
	gps_ret_t ret;

	(void) memset(&gps, 0, sizeof(gps));	
	ret = get_gps_info(&gps);

	if(ret == GPS_OK)
	{
		/* Print reported */
		cli_print(temp_cli, "GPSINFO:");
		cli_print(temp_cli, "    latitude %.7lf, longitude %.7lf", gps.latitude, gps.longitude);
		cli_print(temp_cli, "    altitude %.7lf m", gps.altitude);
		cli_print(temp_cli, "    ground speed %.7lf m/s", gps.groundspeed);
		cli_print(temp_cli, "    angle %.7lf", gps.angle);
	} 
	else 
	{
		cli_print(temp_cli, "GPSINFO: GPS error!!");
	}
	
}

int form_hash_str(char* tbhash_str, unsigned int* size)
{
	int ret = -1;
	atlk_rc_t rc = ATLK_OK;
	char fw_str[32];
	slx97_chip_info_t chip_info;
	

	(void)memset(&chip_info, 0, sizeof(chip_info));
	(void)memset(fw_str, 0, sizeof(fw_str));
	(void)memset(tbhash_str, 0, *size);
	rc = slx97_chip_info_get(&chip_info);
	if (atlk_error(rc)) 
	{
	  	(void)cli_print(temp_cli,"get slx97_chip_info_get failed: %s\n", atlk_rc_to_str(rc));
	}
	else
	{
		ret = 0;
	}
	
	return ret;
}

/* Release allocated resources */
static void example_cleanup(void)
{
	(void)can_socket_delete(can_socket);
	(void)can_service_delete(can_service);
}

void can_init(void)
{
	/* ThreadX return value */
	ULONG trv = TX_SUCCESS;
	/* Autotalks return code */
	atlk_rc_t rc = ATLK_OK;
	/* CAN socket configuration */
	can_socket_config_t socket_config = CAN_SOCKET_CONFIG_INIT;
	int ret_flag = 0;

	/* Get default CAN service instance */
	rc = can_default_service_get(&can_service);
	if (atlk_error(rc)) 
	{
		cli_print(temp_cli,"can_default_service_get: %s\n", atlk_rc_to_str(rc));
    
		example_cleanup();
		ret_flag = 1;
	}

	if (ret_flag == 0)
	{
		/* Set socket configuration */
		socket_config.filter_array_ptr = filter_array;
		socket_config.filter_array_size = 1;
		socket_config.device_id = EXAMPLE_CAN_DEVICE_ID;

		/* Create CAN socket */
		rc = can_socket_create(can_service, &can_socket, &socket_config);
		if (atlk_error(rc)) 
		{
			cli_print(temp_cli,"can_socket_create: %s\n", atlk_rc_to_str(rc));
			example_cleanup();
			ret_flag = 1;
		}

		if (ret_flag == 0)
		{
			/* Create CAN send thread */
			trv = tx_thread_create(&can_send_thread, "can_send_thread",
                         can_send_thread_entry, 0,
                         can_send_thread_stack,
                         sizeof(can_send_thread_stack),
                         CAN_SEND_THREAD_PRIORITY,
                         CAN_SEND_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
			if (trv != TX_SUCCESS)
			{
				cli_print(temp_cli,"CAN send thread create error \n");
			}
  
			/* Create CAN receive thread */
			trv = tx_thread_create(&can_receive_thread, "can_receive_thread",
                         can_receive_thread_entry, 0,
                         can_receive_thread_stack,
                         sizeof(can_receive_thread_stack),
                         CAN_RECEIVE_THREAD_PRIORITY,
                         CAN_RECEIVE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
			if(trv != TX_SUCCESS)
			{
				cli_print(temp_cli,"CAN receive thread create error\n");
			}
		}
  		
	}
 
	return;


}

int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)     
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) 
	{
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	
	if (x->tv_usec - y->tv_usec > 1000000) 
	{
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    	y->tv_usec += 1000000 * nsec;
    	y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
     tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

static void can_send_thread_entry(ULONG input)
{
	/* Autotalks return code */
	atlk_rc_t rc = ATLK_OK;
	/* CAN message data to send */
	uint8_t data[] = { 1, 2, 3, 4, 5 };
	/* Not using input */
	(void)input;
	int count = 0, ret, tx_count = 0;
	char tbhash_str[128];
	unsigned int tbhash_str_len = sizeof(tbhash_str);
	struct timeval start_time, end_time, diff_time; /* System time */  
	char buf[64]; /* Buffer for datetime string */
	struct out_wsm out_wsm;	
	int len = sizeof(servaddr), n, udp_tx_count = 0;
  
	(void)gettimeofday(&start_time, NULL);
  
	while (1) 
	{
		/* Send CAN message with CAN ID 0x50 */
		rc = can_send(can_socket, data, sizeof(data), EXAMPLE_CAN_ID, NULL);
		if (rc == ATLK_E_OUT_OF_MEMORY) 
		{
			/* Sleeping for 1 milisecond to avoid a busy loop. The 'out of memory'
			error is expected when the TX queue is full.
			*/
			cli_print(temp_cli,"Error: CAN out of memory, CAN Tx queue full\n");
			
		}
		else if (atlk_error(rc)) 
		{
			cli_print(temp_cli,"Error: can_send: %s\n", atlk_rc_to_str(rc));
			
		}
		else
		{
    
			count++;
			/* Print sent CAN message */
			cli_print(temp_cli,"CAN Tx OK %d :\"%d,%d,%d,%d,%d\", ID 0x%lx\n", count,
						data[0], data[1], data[2], data[3], data[4], EXAMPLE_CAN_ID);
			data[0]++;
			if (data[0] == 0xF0)
			{
				data[0] = 0;
			}
		}
    

    
		/**** HSM check ****/
		if (form_hash_str(tbhash_str, &tbhash_str_len) != 0)
		{
			(void)cli_print(temp_cli,"Error : HSM failed\n");
		}
		else
		{
			(void)cli_print(temp_cli,"HSM OK\n");
		}
		
		/**** WSMP send *****/
		if (handle_ok_flag)
		{
			out_wsm.psid = PSID_NUMBER;	
			out_wsm.channel_number = 178;
			out_wsm.data_rate = 6;
			out_wsm.txpwr_level = 15;
			out_wsm.extensions.channel = 1;
			out_wsm.extensions.rate = 1;
			out_wsm.extensions.power = 1;
			out_wsm.wsm_type = EID_WSMP;
			(void)memcpy(out_wsm.dest_mac, "\xFF\xFF\xFF\xFF\xFF\xFF", 6); 
			(void)memcpy(out_wsm.data, buf, sizeof(buf)); 
			out_wsm.length = sizeof(buf);

			ret = wme_wsm_send(&handle, &out_wsm);
			if(ret != 0) 
			{
				cli_print(temp_cli,"Error : WSMP send fail = %d\n", ret);
		
			}
			else
			{
				++tx_count;
				cli_print(temp_cli,"WSMP send OK %d\n", tx_count);
			}		
		}
		
		/**** UDP send *****/
		if (udp_fd_ok_flag)
		{		
			n = sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *) &servaddr, len);
			if(n <= 0)
			{
				cli_print(temp_cli,"Error : UDP Tx fail\n");			
			}
			else
			{
				++udp_tx_count;
				cli_print(temp_cli,"UDP Tx OK %d\n", udp_tx_count);
			}
		}
		
		/***** show GPS infomation *****/
		gps_handle_show(); 
		
		/**** show spend time *****/
		/* Get system time */
		(void)gettimeofday(&end_time, NULL);
		(void)timeval_subtract(&diff_time, &end_time, &start_time);
		
		cli_print(temp_cli,"Spend time: %ld hour, %ld min, %ld sec  \n\n", 
					diff_time.tv_sec / (60 * 60), (diff_time.tv_sec / 60) % 60, diff_time.tv_sec % 60 );

    	/* Wait for 3000 milliseconds */
		(void)usleep(3000000);
	}
}

static void can_receive_thread_entry(ULONG input)
{
	/* Autotalks return code */
	atlk_rc_t rc = ATLK_OK;
	/* Received CAN message data */
	uint8_t data[CAN_DATA_SIZE_MAX];
	/* Received CAN message data size */
	size_t data_size = sizeof(data);
	/* Received CAN ID */
	can_id_t can_id;
	/* Not using input */
	(void)input;
	int can_rx_count = 0;

	while (1) 
	{
		/* Receive CAN message */
		rc = can_receive(can_socket, data, &data_size, &can_id,
                     &atlk_wait_forever);
		if (atlk_error(rc)) 
		{
			cli_print(temp_cli,"Error: can_receive fail : %s\n", atlk_rc_to_str(rc));
			
			continue;
    	}
		++can_rx_count;
		/* Print received CAN message */
		cli_print(temp_cli,"CAN Rx OK %d :\"%d,%d,%d,%d,%d\", ID 0x%lx\n", can_rx_count,
           		data[0], data[1], data[2], data[3], data[4], can_id);
	}


}

void wsmp_recv_handler(ULONG input)
{
	wme_handle_t handle = *(wme_handle_t *)input;
	//(void)input;
	int ret;
	int rx_count = 0;
	struct in_wsm in_wsm;
	
	cli_print(temp_cli,"WSMP Rx start\n");
	
	while(1) 
	{
		if (handle_ok_flag == 0)
		{
			cli_print(temp_cli,"WSMP handle no ready\n");
			(void)usleep(1000000);
			continue;
		}
		
		ret = wme_wsm_recv(&handle, &in_wsm, 0xFFFFFFFF);
		if(ret != 0) 
		{
			cli_print(temp_cli,"Error : WSMP Rx fail = %d\n", ret);
			continue;
		}
		else
		{
			++rx_count;
			cli_print(temp_cli,"WSMP Rx OK %d\n", rx_count);
		}
	}
	
	return;
}

void wsmp_init(void)
{
	int ret, ret_flag = 0;	
	struct wsm_info wsm_info;
	unsigned int psid;
	
	psid = PSID_NUMBER;
	
	
	/* DSRC init */
	ret = wme_init(&handle);
	if(ret != ACCEPT) 
	{
		cli_print(temp_cli,"Error :wme_init = %d\n", ret);
		ret_flag = 1;		
	}
	else
	{
		handle_ok_flag = 1;
	}	
	
	if (ret_flag == 0)
	{
		wsm_info.action = ADD;
		wsm_info.psid = psid;
		ret  = wme_wsm_service(&handle, &wsm_info);
		if(ret != 0) 
		{
			cli_print(temp_cli,"wme_wsm_service = %d\n", ret);
			(void)wme_exit(&handle);
			ret_flag = 1;
			
		}
		
		if (ret_flag == 0)
		{
			cli_print(temp_cli,"wme_init success, handle = %d\n", handle);
	
	
			/* wsmp thread init */
			ret = tx_thread_create(&wsmp_recv_thread, "wsmp_recv_thread", wsmp_recv_handler, (ULONG) &handle, wsmp_recv_stack, sizeof(wsmp_recv_stack), CAN_RECEIVE_THREAD_PRIORITY, CAN_RECEIVE_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);
			if(ret != TX_SUCCESS)
			{
				cli_print(temp_cli,"WSMP Rx thread create error \n");
			}
		}
	
	}
	
	
}

void udp_receive_thread_entry(ULONG input)
{
	(void)input;
	int rx_count = 0;
	
	
	while (1) 
	{
    	/* Buffer for received messages */
		char msg[UDP_MAX_MSG_SIZE];
		/* Length of received message */
		ssize_t len = 0;

		cli_print(temp_cli,"Receiving UDP packet...\n");
		len = recv(fd, msg, sizeof(msg), 0);
		if (len == -1) 
		{
			cli_print(temp_cli,"recv failed: %d\n", errno);
			continue;
		}
		++rx_count;
		/* Print received packet length */
		cli_print(temp_cli,"UDP Rx OK: %d \n", rx_count);
	}
		
}

void udp_init(void)
{
	/* Internet address family socket address */
	struct sockaddr_in sockaddr;
	/* Socket's file descriptor */
  
	/* POSIX return code */
	int rc = 0, ret_flag = 0, trv;

	cli_print(temp_cli,"NX-BSD UDP receive start.\n");

	/* Create a UDP socket */
	cli_print(temp_cli,"Creating UDP socket...\n");
	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd == -1) 
	{
		cli_print(temp_cli,"Error : socket failed: %d\n", errno);
    	ret_flag = 1;
	}
  
  

	if (ret_flag == 0)
	{
		/* Prepare socket address struct */
		(void)memset(&sockaddr, 0, sizeof(sockaddr));
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_port = htons(UDP_PORT_NUMBER);
		sockaddr.sin_addr.s_addr = INADDR_ANY;

		/* Bind the socket */
		cli_print(temp_cli,"Binding UDP socket to port %d...\n", UDP_PORT_NUMBER);
		rc = bind(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
		if (rc == -1) 
		{
			cli_print(temp_cli,"Error: bind failed: %d\n", errno);
    		ret_flag = 1;
		}
		else
		{
			/* init servaddr */
			(void) memset(&servaddr, 0, sizeof(servaddr));
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			servaddr.sin_port = htons(UDP_PORT_NUMBER);
	
			servaddr.sin_addr.s_addr = inet_addr(server_ip);
		  
			udp_fd_ok_flag = 1;
	
	
		}
	}
  
	if (ret_flag == 0)
	{

		/* udp rx thread init */
		trv = tx_thread_create(&udp_recv_thread, "udp_rcv_thread", udp_receive_thread_entry, 0, udp_recv_stack, sizeof(udp_recv_stack), CAN_RECEIVE_THREAD_PRIORITY, CAN_RECEIVE_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);
		if(trv != TX_SUCCESS)
		{
			cli_print(temp_cli,"UDP Rx thread create error\n");
		}
	}

  return;
}


int cmd_temperature(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	(void)cli;
	(void)command;
	(void)argc;
	(void)argv;
	
	temp_cli = cli;
	int ret_flag = 0;
	int ret = CLI_OK;
	
	if(argc != 1)
	{
		cli_print(temp_cli,"Usage : temperature <peer IP> \n");
		cli_print(temp_cli,"Example: temperature 192.168.10.50\n");
		ret_flag = 1;
		ret = -1;
	}
	
	if (ret_flag == 0)
	{
		(void)memset(server_ip, 0, sizeof(server_ip));
		(void)strcpy(server_ip, argv[0]);
	
	
	
		/* wsmp init */
		wsmp_init();
	
		/* udp init */
		udp_init();
	
		/* init CAN Tx thread */
		can_init();
	}
	
	return ret;
}