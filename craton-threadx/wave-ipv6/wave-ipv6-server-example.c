/* Copyright (C) 2016 Autotalks Ltd. */
#include <assert.h>
#include <stdio.h>
#include <tx_api.h>
#include <nx_api.h>
#include <craton/net.h>

/*
  CRATON ThreadX IPv6 TCP Server example

  This example demonstrates basic usage of TCP protocol over IPv6 for code
  running on top of CRATON processor with ThreadX RTOS.

  Server thread is created. A TCP socket is created and listen to client port,
  after client connection server accept connection.

  The Server thread sends back TCP frames to client "Hello from server".
*/

/* TCP server port */
#define EXAMPLE_SERVER_PORT 6666

/* Pointer to `untrusted` network IP instance */
static NX_IP *untrusted_instance = NULL;
/* NetX TCP socket */
static NX_TCP_SOCKET  server_socket;
/* server thread */
static TX_THREAD server_thread;
static void thread_server_entry(ULONG thread_input);
/* thread stack */
static uint8_t example_server_thread_stack[0x1000];
/* Example thread priorities */
#define EXAMPLE_SERVER_THREAD_PRIORITY 40

static void thread_server_connect_received(NX_TCP_SOCKET *server_socket, UINT port);
static void thread_server_disconnect_received(NX_TCP_SOCKET *server_socket);

/* Example message format string */
#define EXAMPLE_SERVER_MESSAGE "Hello from server"

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
  ip_address.nxd_ip_address.v6[3] = 0x10;

  nrc = nxd_ipv6_global_address_set(untrusted_instance,
                                    &ip_address,
                                    64);
  assert(nrc == NX_SUCCESS);

  /* Create the main thread.  */
  trc = tx_thread_create(&server_thread,
                         "thread server",
                         thread_server_entry,
                         0,
                         example_server_thread_stack,
                         sizeof(example_server_thread_stack),
                         EXAMPLE_SERVER_THREAD_PRIORITY,
                         EXAMPLE_SERVER_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE,
                         TX_AUTO_START);
  assert(trc == TX_SUCCESS);
}

void thread_server_entry(ULONG thread_input)
{
  /* Not using input */
  (void)thread_input;
  /* NetX return value */
  UINT nrc;
  /* NetX TCP packet pointer for TX and RX */
  NX_PACKET *tx_packet;
  NX_PACKET *rx_packet;
  /* status of IP instance */
  ULONG actual_status;
  /* packet length */
  ULONG length;
  /* NetX packet pool pointer */
  NX_PACKET_POOL *packet_pool = NULL;

  /* Wait 5 seconds for the IP thread to finish its initialization and
     for the IPv6 stack to finish DAD process. */
  tx_thread_sleep(5000);

  printf("\n***Starting TCP Server***\n");

  /* Ensure the IP instance has been initialized.  */
  nrc = nx_ip_status_check(untrusted_instance,
                              NX_IP_INITIALIZE_DONE,
                              &actual_status,
                              100);
  assert(nrc == NX_SUCCESS);

  packet_pool = untrusted_instance->nx_ip_default_packet_pool;
  assert(packet_pool);

  /* Create a socket.  */
  nrc =  nx_tcp_socket_create(untrusted_instance,
                              &server_socket,
                              "Server Socket",
                              NX_IP_NORMAL,
                              NX_FRAGMENT_OKAY,
                              NX_IP_TIME_TO_LIVE,
                              100,
                              NX_NULL,
                              thread_server_disconnect_received);
  assert(nrc == NX_SUCCESS);

  /* Setup this thread to listen.  */
  nrc = nx_tcp_server_socket_listen(untrusted_instance,
                                    EXAMPLE_SERVER_PORT,
                                    &server_socket,
                                    5,
                                    thread_server_connect_received);
  assert(nrc == NX_SUCCESS);

  /* Accept a client socket connection.  */
  nrc = nx_tcp_server_socket_accept(&server_socket,
                                       NX_WAIT_FOREVER);
  assert(nrc == NX_SUCCESS);
  printf("Someone was connected\n");

  /* Loop to receive and send packets  */
  while(1) {
    /* Receive a TCP message from the socket.  */
    nrc = nx_tcp_socket_receive(&server_socket,
                                &rx_packet,
                                NX_WAIT_FOREVER);
    assert(nrc == NX_SUCCESS);

    printf("Server received: \"%s\"\n", rx_packet->nx_packet_prepend_ptr);
    nx_packet_release(rx_packet);

    /* Allocate a packet.  */
    nrc = nx_packet_allocate(packet_pool,
                             &tx_packet,
                             NX_TCP_PACKET,
                             NX_WAIT_FOREVER);
    assert(nrc == NX_SUCCESS);

    /* Write ABCs into the packet pay load!  */
    nx_packet_data_append(tx_packet,
                          EXAMPLE_SERVER_MESSAGE,
                          sizeof(EXAMPLE_SERVER_MESSAGE),
                          packet_pool,
                          TX_WAIT_FOREVER);

    nrc = nx_packet_length_get(tx_packet, &length);
    if ((nrc) || (length != sizeof(EXAMPLE_SERVER_MESSAGE))) {
      assert(0);
    }
    nrc = nx_tcp_socket_send(&server_socket,
                             tx_packet,
                             NX_WAIT_FOREVER);
    assert(nrc == NX_SUCCESS);
  }
}

void  thread_server_connect_received(NX_TCP_SOCKET *socket_ptr, UINT port)
{
  /* Not using input */
  (void)socket_ptr;
  (void)port;
  printf("Connection received\n");
}


void  thread_server_disconnect_received(NX_TCP_SOCKET *socket)
{
  /* Not using input */
  (void)socket;
  printf("Disconnection received\n");
}
