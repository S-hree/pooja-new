/* Copyright (C) 2016 Autotalks Ltd. */
#include <assert.h>
#include <stdio.h>
#include <tx_api.h>
#include <nx_api.h>
#include <craton/net.h>

/*
  CRATON ThreadX IPv6 TCP Client example

  This example demonstrates basic usage of TCP protocol over IPv6 for code
  running on top of CRATON processor with ThreadX RTOS.

  Client thread is created. A TCP socket is created and connected to
  any port (bind) and after it connected to server.

  The Client thread sends TCP frames to server "Hello from client" and waits
  for response from server.

  The client side should be started after server side.
*/

/* TCP server port */
#define EXAMPLE_SERVER_PORT 6666

/* Pointer to `untrusted` network IP instance */
static NX_IP *untrusted_instance = NULL;
/* NetX TCP socket */
static NX_TCP_SOCKET client_socket;
/* client thread */
static TX_THREAD client_thread;
static void thread_client_entry(ULONG thread_input);
/* thread stack */
static uint8_t example_client_thread_stack[0x1000];
/* Example thread priorities */
#define EXAMPLE_CLIENT_THREAD_PRIORITY 40

/* Example message format string */
#define EXAMPLE_CLIENT_MESSAGE "Hello from client"

void craton_user_init(void)
{
  /* NetX return value */
  UINT nrc;
  /* ThreadX return value */
  ULONG trc = TX_SUCCESS;
  /* Auto-talks return code */
  atlk_rc_t rc;
  /* IPv6 address */
  NXD_ADDRESS ip_address;

  /* Get `untrusted` IP instance */
  rc = net_ip_untrusted_instance_get(&untrusted_instance);
  assert(rc == ATLK_OK);

  /* Enable IPv6 */
  nrc = nxd_ipv6_enable(untrusted_instance);
  assert(nrc == NX_SUCCESS);

  /* Enable both ICMPv4 and ICMPv6 */
  nrc = nxd_icmp_enable(untrusted_instance);
  assert(nrc == NX_SUCCESS);

  ip_address.nxd_ip_version = NX_IP_VERSION_V6;
  ip_address.nxd_ip_address.v6[0] = 0x20010000;
  ip_address.nxd_ip_address.v6[1] = 0;
  ip_address.nxd_ip_address.v6[2] = 0;
  ip_address.nxd_ip_address.v6[3] = 0x11;

  nrc = nxd_ipv6_global_address_set(untrusted_instance,
                                    &ip_address,
                                    64);
  assert(nrc == NX_SUCCESS);

  /* Create the main thread.  */
  trc = tx_thread_create(&client_thread,
                         "thread client",
                          thread_client_entry,
                          0,
                          example_client_thread_stack,
                          sizeof(example_client_thread_stack),
                          EXAMPLE_CLIENT_THREAD_PRIORITY,
                          EXAMPLE_CLIENT_THREAD_PRIORITY,
                          TX_NO_TIME_SLICE,
                          TX_AUTO_START);
  assert(trc == TX_SUCCESS);
}

/* Define the test threads.  */
void thread_client_entry(ULONG thread_input)
{
  /* Not using input */
  (void)thread_input;
  /* NetX return value */
  UINT nrc;
  /* NetX TCP packet pointer for TX and RX */
  NX_PACKET *tx_packet;
  NX_PACKET *rx_packet;
  /* packet length */
  ULONG length;
  /* IPv6 server address */
  NXD_ADDRESS server_ipv6_address;
  /* NetX packet pool pointer */
  NX_PACKET_POOL *packet_pool = NULL;

  /* Wait 5 seconds for the IP thread to finish its initialization and
     for the IPv6 stack to finish DAD process. */
  tx_thread_sleep(5000);

  printf("\n***Starting TCP Client***\n");

  server_ipv6_address.nxd_ip_version = NX_IP_VERSION_V6;
  server_ipv6_address.nxd_ip_address.v6[0] = 0x20010000;
  server_ipv6_address.nxd_ip_address.v6[1] = 0;
  server_ipv6_address.nxd_ip_address.v6[2] = 0;
  server_ipv6_address.nxd_ip_address.v6[3] = 0x10;

  packet_pool = untrusted_instance->nx_ip_default_packet_pool;
  assert(packet_pool);

  /* Create a socket.  */
  nrc = nx_tcp_socket_create(untrusted_instance,
                                &client_socket,
                                "Client Socket",
                                NX_IP_NORMAL,
                                NX_FRAGMENT_OKAY,
                                NX_IP_TIME_TO_LIVE,
                                200,
                                NX_NULL, NX_NULL);
  assert(nrc == NX_SUCCESS);

  nrc =  nx_tcp_client_socket_bind(&client_socket,
                                   NX_ANY_PORT,
                                   NX_WAIT_FOREVER);
  assert(nrc == NX_SUCCESS);

  /* Attempt to connect the socket.  */
  nrc = nxd_tcp_client_socket_connect(&client_socket,
                                      &server_ipv6_address,
                                      EXAMPLE_SERVER_PORT,
                                      NX_WAIT_FOREVER);
  assert(nrc == NX_SUCCESS);

  printf("Connection established\n");

  /* Loop to send and receive packets  */
  while (1) {
    /* Allocate a packet.  */
    nrc = nx_packet_allocate(packet_pool,
                                &tx_packet,
                                NX_TCP_PACKET,
                                NX_WAIT_FOREVER);
    assert(nrc == NX_SUCCESS);

    /* Write ABCs into the packet payload!  */
    nx_packet_data_append(tx_packet,
                          EXAMPLE_CLIENT_MESSAGE,
                          sizeof(EXAMPLE_CLIENT_MESSAGE),
                          packet_pool,
                          TX_WAIT_FOREVER);

    nrc = nx_packet_length_get(tx_packet, &length);
    if ((nrc) || (length != sizeof(EXAMPLE_CLIENT_MESSAGE))) {
      assert(0);
    }

    /* Send the packet out!  */
    nrc = nx_tcp_socket_send(&client_socket, tx_packet, TX_WAIT_FOREVER);
    assert(nrc == NX_SUCCESS);

    /* Wait for packet from server  */
    nrc = nx_tcp_socket_receive(&client_socket,
                                   &rx_packet,
                                   NX_WAIT_FOREVER);
    assert(nrc == NX_SUCCESS);

    printf("Client received: \"%s\"\n", rx_packet->nx_packet_prepend_ptr);
    nx_packet_release(rx_packet);

    /* wait 1 second */
    tx_thread_sleep(1000);
  }
}
