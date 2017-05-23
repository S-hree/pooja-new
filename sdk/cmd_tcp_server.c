   /* This is a small demo of the high-performance NetX Duo TCP/IP stack.
      This program demonstrates ICMPv6 protocols Neighbor Discovery and
      Stateless Address Configuration for IPv6, ARP for IPv4, and
      TCP packet sending and receiving with a simulated Ethernet driver.  */

#include <assert.h>
#include <stdio.h>

#include <tx_api.h>
#include <nx_api.h>

#include <craton/net.h>

#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <tx_posix.h>
#include <craton/fs.h>
#include <unistd.h>
#include <errno.h>
#include <libcli.h>
#include <craton/cli.h>
#include <atlk/v2x_service.h>
#include <atlk/mib_service.h>
#include <atlk/mibs/wlan-mib.h>

#define FW_UPDATE_SERVER_PORT 6666
#define FW_UPDATE_DEVICE "D:/"  /* Device used for firmware image */
#define FW_UPDATE_IMAGE_FILE_NAME "wave_itri.img"
#define FW_UPDATE_MAX_BLOCK_SIZE 512
#define FW_UPDATE_WINDOW_SIZE 2048
#define FW_UPDATE_CHANNEL 5920
#define FW_UPDATE_INTERFACE 2

static TX_THREAD tcp_server_thread;
static NX_IP *untrusted_instance = NULL;
static NX_TCP_SOCKET  server_socket;
static uint8_t tcp_server_thread_stack[0x1000];
static int fw_fd ;
static UCHAR buffer_packet[1440];
static UCHAR eof_string[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
static cli_instance_t *tcp_server_cli;
static mib_service_t *service = NULL;
/* NetX packet pool pointer */
NX_PACKET_POOL *packet_pool = NULL;

void tcp_server_thread_entry(ULONG thread_input);
void tcp_server_thread_connect_received(NX_TCP_SOCKET *server_socket, UINT port);
void tcp_server_thread_disconnect_received(NX_TCP_SOCKET *server_socket);

#define EXAMPLE_SERVER_MESSAGE "ACK"

int cmd_tcp_server(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	(void)cli;
	(void)command;
	(void)argc;
	(void)argv;	
	UINT status;
	atlk_rc_t rc;
	NXD_ADDRESS ip_address;
	int ret_flag = 0;
	int ret;
	
	tcp_server_cli = cli;
	
	
  
  	ret = mib_default_service_get(&service);
  	if (ret != ATLK_OK) 
  	{
		cli_print(tcp_server_cli, "mib_default_service_get: %d\n", ret);
		ret_flag = 1;
  	}
  	
  	if (ret_flag == 0)
  	{
	 	ret = mib_set_wlanFrequency(service, FW_UPDATE_INTERFACE, FW_UPDATE_CHANNEL); 
	 	if (ret != ATLK_OK) 
  		{	
			cli_print(tcp_server_cli, "mib_set_wlanFrequency: %d\n", ret);
			ret_flag = 1;
  		}	
  		else
  		{
	  		cli_print(tcp_server_cli, "use channel %d at interface %d\n", FW_UPDATE_CHANNEL, FW_UPDATE_INTERFACE);
  		}
  	}

	/* Get `untrusted` IP instance */
	if (ret_flag == 0)
	{
		rc = net_ip_untrusted_instance_get(&untrusted_instance);
		if(rc != ATLK_OK)
		{
			cli_print(tcp_server_cli, "net_ip_untrusted_instance_get fail\n");
			ret_flag = 1;
		}
	}
	
	/* IPv6 always enable, I do not need to enable again */

	if (ret_flag == 0)
	{
		// Enable IPv6
		status = nxd_ipv6_enable(untrusted_instance);
		if ((status != NX_SUCCESS) && (status != NX_ALREADY_ENABLED))
		{
			cli_print(tcp_server_cli, "nxd_ipv6_enable fail %x\n", status);
			ret_flag = 1;
		}
	}
	
	if (ret_flag == 0)
	{
		/* Enable ICMP */
		status = nxd_icmp_enable(untrusted_instance);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_server_cli, "nxd_icmp_enable fail %x\n", status);
			ret_flag = 1;
		}
	}

	if (ret_flag == 0)
	{
		ip_address.nxd_ip_version = NX_IP_VERSION_V6;
		ip_address.nxd_ip_address.v6[0] = 0x20010000;
		ip_address.nxd_ip_address.v6[1] = 0;
		ip_address.nxd_ip_address.v6[2] = 0;
		ip_address.nxd_ip_address.v6[3] = 6;
		status = nxd_ipv6_global_address_set(untrusted_instance, &ip_address, 64);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_server_cli, "nxd_ipv6_global_address_set fail\n");
			ret_flag = 1;
		}
	}

	if (ret_flag == 0)
	{
		/* Create the main thread.  */
		status = tx_thread_create(&tcp_server_thread,
									"tcp server thread",
									tcp_server_thread_entry,
									0,
									tcp_server_thread_stack,
									sizeof(tcp_server_thread_stack),
									40, 40,
									TX_NO_TIME_SLICE,
									TX_AUTO_START);
		if(status != TX_SUCCESS)
		{
			cli_print(tcp_server_cli, "tx_thread_create fail\n");
			ret_flag = 1;
		}
	}
}

void tcp_server_thread_entry(ULONG thread_input)
{
	(void)thread_input;
	UINT            status;
	NX_PACKET       *packet_ptr;
	ULONG           actual_status;
	int ret;
	ULONG eof_string_len = sizeof(eof_string);  
	ULONG length, copied;
	UCHAR *buffer;
	UINT  count = 0, loop_exit = 0;
	int ret_flag = 0;
	UINT nrc;
	NX_PACKET *tx_packet;

	cli_print(tcp_server_cli, "\n***Please you don't start remote tcp_client now, waiting IP thread ready (It need 20 secs)... ***\n");
	tx_thread_sleep(15000);

	/* Wait 5 seconds for the IP thread to finish its initilization and
		for the IPv6 stack to finish DAD process. */
	tx_thread_sleep(5000);

	cli_print(tcp_server_cli, "\n***Starting TCP Server***\n");
	cli_print(tcp_server_cli, "\n***You can start remote tcp_client now***\n");
  
	ret = fs_thread_enable();	 
	if(ret == -1)
	{
		cli_print(tcp_server_cli, "fs_thread_enable fail \n");
		ret_flag = 1;
	}
  
	if (ret_flag == 0)
	{ 
		/* Change directory to root of device */
		ret = chdir(FW_UPDATE_DEVICE);
  		if (ret == -1) 
  		{
	  		cli_print(tcp_server_cli, "chdir fail \n");
			ret_flag = 1;
    	}
	}
    
	if (ret_flag == 0)
	{ 
    	fw_fd = open(FW_UPDATE_IMAGE_FILE_NAME, O_APPEND | O_RDWR);
	    			
		if (fw_fd == -1)
		{
	  		cli_print(tcp_server_cli, "open %s fail \n", FW_UPDATE_IMAGE_FILE_NAME);
			ret_flag = 1;
    	}
	}
  

	if (ret_flag == 0)
	{ 
		/* Ensure the IP instance has been initialized.  */
		status = nx_ip_status_check(untrusted_instance,
									NX_IP_INITIALIZE_DONE,
									&actual_status,
									100);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_server_cli, "nx_ip_status_check fail\n");
			ret_flag = 1;
		}
	}
	
	if (ret_flag == 0)
	{
		packet_pool = untrusted_instance->nx_ip_default_packet_pool;
		if (packet_pool == NULL)
		{
			cli_print(tcp_server_cli, "packet_pool NULL\n");
			ret_flag = 1;
		}
	}

	if (ret_flag == 0)
	{ 
		/* Create a socket.  */
		status =  nx_tcp_socket_create(untrusted_instance,
										&server_socket,
										"Server Socket",
										NX_IP_NORMAL,
										NX_FRAGMENT_OKAY,
										NX_IP_TIME_TO_LIVE,                                 
										FW_UPDATE_WINDOW_SIZE,
										NX_NULL,
										tcp_server_thread_disconnect_received);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_server_cli, "nx_tcp_socket_create fail\n");
			ret_flag = 1;
		}
	}

	if (ret_flag == 0)
	{ 
		/* Setup this thread to listen.  */
		status = nx_tcp_server_socket_listen(untrusted_instance,
											FW_UPDATE_SERVER_PORT,
											&server_socket,
											5,
											tcp_server_thread_connect_received);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_server_cli, "nx_tcp_server_socket_listen fail\n");
			ret_flag = 1;
		}
	}

	if (ret_flag == 0)
	{ 
		/* Accept a client socket connection.  */
		status = nx_tcp_server_socket_accept(&server_socket, NX_WAIT_FOREVER);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_server_cli, "nx_tcp_server_socket_accept fail\n");
			ret_flag = 1;
		}
		else
		{
			cli_print(tcp_server_cli, "Someone was connected\n");
		}
	}

	if (ret_flag == 0)
	{ 
		/* Loop to create and establish server connections.  */
		do {
			/* Receive a TCP message from the socket.  */
			status = nx_tcp_socket_receive(&server_socket,
										&packet_ptr,
										NX_WAIT_FOREVER);
    		if (status != NX_SUCCESS)
			{
				cli_print(tcp_server_cli, "nx_tcp_socket_receive fail\n");
				ret_flag = 1;
			}

    
    
			if (ret_flag == 0)
			{ 
    			status = nx_packet_length_get(packet_ptr, &length);
    			if (status != NX_SUCCESS) 
    			{
            		cli_print(tcp_server_cli, "nx_packet_length_get get error");
            		ret_flag = 1;
    			}
			}
    
			if (ret_flag == 0)
			{ 
    			buffer = buffer_packet;
    			(void)memset(buffer, 0, sizeof(buffer_packet));
    
     			status = nx_packet_data_extract_offset(packet_ptr,
     													0, /* offset */
     													(void *)buffer,
     													length,
     													&copied);
    			if (status != NX_SUCCESS) 
    			{
					cli_print(tcp_server_cli, "nx_packet_data_extract_offset error");
					ret_flag = 1;
    			}
			}	
    
			if (ret_flag == 0)
			{ 
    			if (length != copied)
    			{
        		    cli_print(tcp_server_cli, "length != copied error");
					ret_flag = 1;
    			}
    			else
    			{
	    			//cli_print(tcp_server_cli, "Rx %d, num %d\n", (int)copied, count);
	    			if ((count % 100) == 0)
	    				cli_print(tcp_server_cli, "Rx %d", count);
    				count++;	
				}
			}
    
			if (ret_flag == 0)
			{ 
    			if ((copied == eof_string_len) &&
    				(strcmp((const char *)buffer, (const char *)eof_string) == 0))
    			{	/* get End of File notify */    
	    			loop_exit = 1;	    			
				}
				else
				{
					ret = write(fw_fd, (void *)buffer, copied);	
    				if (ret == -1)
					{							
           				cli_print(tcp_server_cli, "write error");
           				ret_flag = 1;
   					}	
   					
   					if (ret_flag == 0)
					{ 
						/*
						status = nx_tcp_socket_send(&server_socket, packet_ptr, 2000);
    					if (status != NX_SUCCESS)
						{
							cli_print(tcp_server_cli, "2: nx_tcp_socket_send fail\n");
							ret_flag = 1;
						}
						*/
						nx_packet_release(packet_ptr);
						
						/* Allocate a packet.  */
    					nrc = nx_packet_allocate(packet_pool,
                             					&tx_packet,
                             					NX_TCP_PACKET,
                             					NX_WAIT_FOREVER);
                             
    					if (nrc != NX_SUCCESS)
    					{
	    					cli_print(tcp_server_cli, "nx_packet_allocate fail\n");
							ret_flag = 1;
    					}

    					if (ret_flag == 0)
    					{
    						/* Write ABCs into the packet pay load!  */
    						nx_packet_data_append(tx_packet,
                          						EXAMPLE_SERVER_MESSAGE,
                          						sizeof(EXAMPLE_SERVER_MESSAGE),
                          						packet_pool,
                          						TX_WAIT_FOREVER);

    						nrc = nx_packet_length_get(tx_packet, &length);
    						if ((nrc) || (length != sizeof(EXAMPLE_SERVER_MESSAGE))) 
    						{
      							cli_print(tcp_server_cli, "nx_packet_length_get fail 2\n");
								ret_flag = 1;
    						}
						}
						
						if (ret_flag == 0)
						{
    						nrc = nx_tcp_socket_send(&server_socket,
                             							tx_packet,
                             							NX_WAIT_FOREVER);
    						if (nrc != NX_SUCCESS)
    						{
      							cli_print(tcp_server_cli, "nx_tcp_socket_send fail 2\n");
								ret_flag = 1;
    						}
						}
						
						/*
						else
						{
   	 						cli_print(tcp_server_cli, "Tx\n");
	 					} 
	 					*/ 
 					}   					
				}	
			}
  		}while((loop_exit == 0) && (ret_flag == 0));
	}
	
	if (ret_flag == 0)
	{ 
  
		ret = close(fw_fd);
  
		if(ret == -1)
		{
	  		cli_print(tcp_server_cli, "close file error %d, errno %d\n", ret, errno);
		}
	}
  
}

void  tcp_server_thread_connect_received(NX_TCP_SOCKET *socket_ptr, UINT port)
{
	(void)socket_ptr;
	(void)port;
	cli_print(tcp_server_cli, "Connection received\n");
}


void  tcp_server_thread_disconnect_received(NX_TCP_SOCKET *socket)
{
	(void)socket;
  
	cli_print(tcp_server_cli, "Disconnection received\n");
  
	tx_thread_terminate(&tcp_server_thread);
	tx_thread_delete(&tcp_server_thread);    
  
	nx_tcp_server_socket_unaccept(&server_socket);
	/* We are now done so unlisten on server port . */
	nx_tcp_server_socket_unlisten(untrusted_instance, FW_UPDATE_SERVER_PORT);
	/* Delete the server socket. */
	nx_tcp_socket_delete(&server_socket);
  
	cli_print(tcp_server_cli, "Finish upload\n");
	
  	
}
