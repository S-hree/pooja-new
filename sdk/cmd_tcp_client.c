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
#include <libcli.h>
#include <craton/cli.h>
#include <atlk/v2x_service.h>
#include <atlk/mib_service.h>
#include <atlk/mibs/wlan-mib.h>

#define FW_UPDATE_SERVER_PORT 6666
#define FW_UPDATE_CLIENT_PORT 5555
#define FW_UPDATE_DEVICE "D:/"  /* Device used for firmware image */
#define FW_UPDATE_IMAGE_FILE_NAME "wave_itri.img"
#define FW_UPDATE_MAX_BLOCK_SIZE 512
#define FW_UPDATE_WINDOW_SIZE 2048
#define FW_UPDATE_CHANNEL 5920
#define FW_UPDATE_INTERFACE 2

static TX_THREAD tcp_client_thread;
static NX_IP *untrusted_instance = NULL;
static NX_TCP_SOCKET client_socket;
static uint8_t example_client_thread_stack[0x1000];
static UCHAR buffer_packet[1440];
static UCHAR eof_string[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ  ";
static cli_instance_t *tcp_client_cli;
static mib_service_t *service = NULL;

void tcp_client_thread_entry(ULONG thread_input);

/* Define what the initial system looks like.  */

int cmd_tcp_client(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	UINT status;
	atlk_rc_t rc;
	NXD_ADDRESS ip_address;
	int ret_flag = 0;
	int ret;
  
	tcp_client_cli = cli;
	
	ret = mib_default_service_get(&service);
  	if (ret != ATLK_OK) 
  	{
		cli_print(tcp_client_cli, "mib_default_service_get: %d\n", ret);
		ret_flag = 1;
  	}
  	
  	if (ret_flag == 0)
  	{
	 	ret = mib_set_wlanFrequency(service, FW_UPDATE_INTERFACE, FW_UPDATE_CHANNEL); 
	 	if (ret != ATLK_OK) 
  		{	
			cli_print(tcp_client_cli, "mib_set_wlanFrequency: %d\n", ret);
			ret_flag = 1;
  		}	
  		else
  		{
	  		cli_print(tcp_client_cli, "use channel %d at interface %d\n", FW_UPDATE_CHANNEL, FW_UPDATE_INTERFACE);
  		}
  	}

	/* Get `untrusted` IP instance */
	if (ret_flag == 0)
	{
		rc = net_ip_untrusted_instance_get(&untrusted_instance);
		if(rc != ATLK_OK)
		{
			cli_print(tcp_client_cli, "net_ip_untrusted_instance_get fail\n");
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
			cli_print(tcp_client_cli, "nxd_ipv6_enable fail\n");
			ret_flag = 1;
		}
	}
	

	if (ret_flag == 0)
	{
		/* Enable ICMP */
		status = nxd_icmp_enable(untrusted_instance);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_client_cli, "nxd_icmp_enable fail\n");
			ret_flag = 1;
		}
	}

	if (ret_flag == 0)
	{
		ip_address.nxd_ip_version = NX_IP_VERSION_V6;
		ip_address.nxd_ip_address.v6[0] = 0x20010000;
		ip_address.nxd_ip_address.v6[1] = 0;
		ip_address.nxd_ip_address.v6[2] = 0;
		ip_address.nxd_ip_address.v6[3] = 5;

		status = nxd_ipv6_global_address_set(untrusted_instance, &ip_address, 64);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_client_cli, "nxd_ipv6_global_address_set fail\n");
			ret_flag = 1;
		}
	}
	
	if (ret_flag == 0)
	{
		/* Create the main thread.  */
		status = tx_thread_create(&tcp_client_thread,
                            "thread 0",
                            tcp_client_thread_entry,
                            0,
                            example_client_thread_stack,
                            sizeof(example_client_thread_stack),
                            40, 40,
                            TX_NO_TIME_SLICE,
                            TX_AUTO_START);
		if (status != TX_SUCCESS)
		{
			cli_print(tcp_client_cli, "tx_thread_create fail\n");
			ret_flag = 1;
		}
	}
}


/* Define the test threads.  */

void tcp_client_thread_entry(ULONG thread_input)
{
	(void)thread_input;
	UINT            status;
	NX_PACKET      *my_packet, *received_packet;
	ULONG           length;
	NXD_ADDRESS     server_ipv6_address;
	NXD_ADDRESS     peer_address;
	ULONG           peer_port;
	NX_PACKET_POOL *packet_pool = NULL;
	int ret, fw_fd;  
	int size; 
	UINT count = 0;  
	UCHAR *buffer;
	int ret_flag = 0;
	
	cli_print(tcp_client_cli, "\n***Waiting IP thread ready (It need 10 secs)... ***\n");
	tx_thread_sleep(5000);
	/* Wait 5 seconds for the IP thread to finish its initilization and
     		for the IPv6 stack to finish DAD process. */
  	tx_thread_sleep(5000);
  
	ret = fs_thread_enable();	 
	if(ret == -1)
	{
		cli_print(tcp_client_cli, "fs_thread_enable fail \n");
		ret_flag = 1;
	}
  
  

	if (ret_flag == 0)
	{
  		

  		cli_print(tcp_client_cli, "\n***Starting TCP Client***\n");

  		server_ipv6_address.nxd_ip_version = NX_IP_VERSION_V6;
  		server_ipv6_address.nxd_ip_address.v6[0] = 0x20010000;
  		server_ipv6_address.nxd_ip_address.v6[1] = 0;
  		server_ipv6_address.nxd_ip_address.v6[2] = 0;
  		server_ipv6_address.nxd_ip_address.v6[3] = 6;

  		packet_pool = untrusted_instance->nx_ip_default_packet_pool;
		if(packet_pool == 0)
		{
			cli_print(tcp_client_cli, "packet_pool fail \n");
			ret_flag = 1;
		}
	}
	
	if (ret_flag == 0)
	{	
		/* Create a socket.  */
		status = nx_tcp_socket_create(untrusted_instance,
                                &client_socket,
                                "Client Socket",
                                NX_IP_NORMAL,
                                NX_FRAGMENT_OKAY,
                                NX_IP_TIME_TO_LIVE,
                                FW_UPDATE_WINDOW_SIZE,
                                NX_NULL, NX_NULL);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_client_cli, "nx_tcp_socket_create fail\n");
			ret_flag = 1;
		}
	}

	if (ret_flag == 0)
	{	
		status =  nx_tcp_client_socket_bind(&client_socket, FW_UPDATE_CLIENT_PORT, NX_WAIT_FOREVER);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_client_cli, "nx_tcp_socket_create fail\n");
			ret_flag = 1;
		}
	}
	
	
	if (ret_flag == 0)
	{
		/* Attempt to connect the socket.  */
		status = nxd_tcp_client_socket_connect(&client_socket,
                                         &server_ipv6_address, FW_UPDATE_SERVER_PORT,
                                         NX_WAIT_FOREVER);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_client_cli, "nxd_tcp_client_socket_connect fail\n");
			ret_flag = 1;
		}
		else
		{
			 cli_print(tcp_client_cli, "Connection established\n");
		}
	}

 
	if (ret_flag == 0)
	{
		status = nxd_tcp_socket_peer_info_get(&client_socket, &peer_address, &peer_port);
		if (status != NX_SUCCESS)
		{
			cli_print(tcp_client_cli, "nxd_tcp_socket_peer_info_get fail\n");
			ret_flag = 1;
		}
	}
  
  
	if (ret_flag == 0)
	{
		/* Change directory to root of device */
		ret = chdir(FW_UPDATE_DEVICE);
  		if (ret == -1) 
  		{
	 	 	cli_print(tcp_client_cli, "chdir fail \n");
			ret_flag = 1;
    	}
	}
    
	if (ret_flag == 0)
	{
    	fw_fd = open(FW_UPDATE_IMAGE_FILE_NAME, O_RDONLY );
	    			
		if (fw_fd == -1)
		{
	  		cli_print(tcp_client_cli, "open %s fail \n", FW_UPDATE_IMAGE_FILE_NAME);
			ret_flag = 1;
   		}
	}
    
    

	if (ret_flag == 0)
	{
		/* Loop to repeat things over and over again!  */
		while (1) 
		{
    		/* Allocate a packet.  */
    		status = nx_packet_allocate(packet_pool,
                                &my_packet,
                                NX_TCP_PACKET,
                                NX_WAIT_FOREVER);
    		if (status != NX_SUCCESS)
			{
				cli_print(tcp_client_cli, "nx_packet_allocate fail\n");
				ret_flag = 1;
			}
    
			if (ret_flag == 0)
			{
    			buffer = buffer_packet;
    			(void)memset(buffer, 0, sizeof(buffer_packet));
    
    			size = read(fw_fd, (void *)buffer, FW_UPDATE_MAX_BLOCK_SIZE);
    
    			if (size == -1)
    			{
	    			cli_print(tcp_client_cli, "read %s error\n", FW_UPDATE_IMAGE_FILE_NAME);
	    			ret_flag = 1;
    			}
			}
			
			if (ret_flag == 0)
			{
    
   				if (size == 0)
   				{	/* Send out End of File notify */
	    			nx_packet_data_append(my_packet,
                          eof_string,
                          sizeof(eof_string),
                          packet_pool,
                          TX_WAIT_FOREVER);
                          
        			status = nx_packet_length_get(my_packet, &length);
    				if ((status) || (length != (ULONG)sizeof(eof_string))) 	
					{
     		 			cli_print(tcp_client_cli, "nx_packet_length_get error 2\n");
      					ret_flag = 1;
    				}
    				
    				if (ret_flag == 0)
					{
    					/* Send the packet out!  */
    					//status = nx_tcp_socket_send(&client_socket, my_packet, 2000);
    					status = nx_tcp_socket_send(&client_socket, my_packet, 20000);
    					//cli_print(tcp_client_cli, "nx_tcp_socket_send  %d\n", status);
    					if (status != NX_SUCCESS)
						{
							cli_print(tcp_client_cli, "nx_tcp_socket_send fail\n");
							ret_flag = 1;
						}
					}
                          
                    //cli_print(tcp_client_cli, "Tx");      
	    			cli_print(tcp_client_cli, "Finish upload\n");
	    			break; /* break while loop */
    			}
			}
			
			if (ret_flag == 0)
			{
    			/* Write data into the packet payload!  */    
    			nx_packet_data_append(my_packet,
                          buffer,
                          size,
                          packet_pool,
                          TX_WAIT_FOREVER);

    			status = nx_packet_length_get(my_packet, &length);
    			if ((status) || (length != (ULONG)size)) 	
				{
      				cli_print(tcp_client_cli, "nx_packet_length_get error\n");
      				ret_flag = 1;
    			}
			}
			
			if (ret_flag == 0)
			{

    			/* Send the packet out!  */
    			//status = nx_tcp_socket_send(&client_socket, my_packet, 2000);
    			status = nx_tcp_socket_send(&client_socket, my_packet, 20000);
    			//cli_print(tcp_client_cli, "nx_tcp_socket_send  %d\n", status);
    			if (status != NX_SUCCESS)
				{
					cli_print(tcp_client_cli, "nx_tcp_socket_send fail %d\n", status);
					ret_flag = 1;
				}
				else
				{    
    				//cli_print(tcp_client_cli, "Tx: length %d\n", (int)length);
    				//cli_print(tcp_client_cli, "Tx: num %d\n",count);
    				if ((count % 100) == 0)
    					cli_print(tcp_client_cli, "Tx %d", count);
    				count++;	
				}
			}
			
			if (ret_flag == 0)
			{

    			status = nx_tcp_socket_receive(&client_socket,
                                   &received_packet,
                                   NX_WAIT_FOREVER);
    			//cli_print(tcp_client_cli, "Got something back from server\n");
    			//cli_print(tcp_client_cli, "Rx OK\n");
    			nx_packet_release(received_packet);

    			tx_thread_sleep(100);
			}
		}
  }
  
  tx_thread_sleep(2000);
  
  close(fw_fd);
  
  /* Disconnect from a previously established connection and wait a
	maximum of 400 timer ticks. */
  nx_tcp_socket_disconnect(&client_socket, 400);
  
  nx_tcp_socket_delete(&client_socket);
  
}
