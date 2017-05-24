/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>
#include <nx_api.h>

#include <craton/net.h>

/*
  CRATON ThreadX WAVE IPv6 Example

  This example demonstrates basic usage of WAVE IPv6 for code running on
  top of CRATON processor with ThreadX RTOS.

  Two threads are created -- a TX thread and a RX thread. A UDP socket is
  created per thread.

  The TX thread sends multicast UDP frames while the RX thread receives UDP
  frames and prints their content.

  @todo: This example is not currently supported in multi-core SDK.
*/

/* UDP server and client ports */
#define EXAMPLE_SERVER_PORT 6666
#define EXAMPLE_CLIENT_PORT 6667

/* Example thread priorities */
#define EXAMPLE_TX_THREAD_PRIORITY 40
#define EXAMPLE_RX_THREAD_PRIORITY 41

/* TX thread */
static TX_THREAD example_tx_thread;
static uint8_t example_tx_thread_stack[0x1000];
static void example_tx_thread_entry(ULONG input);

/* RX thread */
static TX_THREAD example_rx_thread;
static uint8_t example_rx_thread_stack[0x1000];
static void example_rx_thread_entry(ULONG input);

/* Pointer to `untrusted` network IP instance */
static NX_IP *untrusted_instance = NULL;

/* Example message format string: Example <seq_num> */
static const char example_msg_fmt[] = "Example %" PRIu32;

/* Example message string maximum length */
static const size_t example_msg_size_max = sizeof(example_msg_fmt) + 10;

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* IPv6 address */
  NXD_ADDRESS ipv6_address;
  /* IPv4 address */
  ULONG ipv4_address;

  /* Get `untrusted` IP instance */
  rc = net_ip_untrusted_instance_get(&untrusted_instance);
  if (atlk_error(rc)) {
    fprintf(stderr, "net_ip_untrusted_instance_get: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Enable IPv6 on the `untrusted` IP instance */
  nrv = nxd_ipv6_enable(untrusted_instance);
  assert(nrv == NX_SUCCESS);
  
  /* Get IPv4 address of the `untrusted` IP instance */
  nrv = nx_ip_interface_info_get(untrusted_instance, 0, NULL, &ipv4_address,
                                 NULL, NULL, NULL, NULL);
  assert(nrv == NX_SUCCESS);

  /* Set IPv6 address based on retrieved IPv4 address */
  ipv6_address.nxd_ip_version = NX_IP_VERSION_V6;
  ipv6_address.nxd_ip_address.v6[0] = 0x20010000;
  ipv6_address.nxd_ip_address.v6[1] = 0x0;
  ipv6_address.nxd_ip_address.v6[2] = 0x0;
  ipv6_address.nxd_ip_address.v6[3] = ipv4_address;

  /* Set IPv6 address of `untrusted` IP instance */
  nrv = nxd_ipv6_global_address_set(untrusted_instance, &ipv6_address, 64);
  assert(nrv == NX_SUCCESS);

  /* Create TX thread */
  trv = tx_thread_create(&example_tx_thread, "example_tx_thread",
                         example_tx_thread_entry, 0,
                         example_tx_thread_stack,
                         sizeof(example_tx_thread_stack),
                         EXAMPLE_TX_THREAD_PRIORITY,
                         EXAMPLE_TX_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  /* Create RX thread */
  trv = tx_thread_create(&example_rx_thread, "example_rx_thread",
                         example_rx_thread_entry, 0,
                         example_rx_thread_stack,
                         sizeof(example_rx_thread_stack),
                         EXAMPLE_RX_THREAD_PRIORITY,
                         EXAMPLE_RX_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;
}

void example_tx_thread_entry(ULONG input)
{
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* NetX UDP socket */
  NX_UDP_SOCKET udp_socket;
  /* NetX UDP packet pointer */
  NX_PACKET *udp_packet = NULL;
  /* NetX packet pool pointer */
  NX_PACKET_POOL *packet_pool = NULL;
  /* IPv6 multicast address */
  NXD_ADDRESS ipv6_multicast_address;
  /* Message counter */
  uint32_t msg_count = 0;
  /* Not using input */
  (void)input;

  /* Point to the `untrusted` IP instance packet pool */
  packet_pool = untrusted_instance->nx_ip_default_packet_pool;

  /* Arbitrary multicast IPv6 address */
  ipv6_multicast_address.nxd_ip_version = NX_IP_VERSION_V6;
  ipv6_multicast_address.nxd_ip_address.v6[0] = 0xFF020000;
  ipv6_multicast_address.nxd_ip_address.v6[1] = 0x0;
  ipv6_multicast_address.nxd_ip_address.v6[2] = 0x0;
  ipv6_multicast_address.nxd_ip_address.v6[3] = 0x1;

  /* Create a UDP socket for sending UDP packets */
  nrv = nx_udp_socket_create(untrusted_instance, &udp_socket, "udp_tx_socket",
                             NX_IP_NORMAL, NX_FRAGMENT_OKAY, 0x80, 5);
  assert(nrv == NX_SUCCESS);

  /* Bind UDP socket to example client port */
  nrv = nx_udp_socket_bind(&udp_socket, EXAMPLE_CLIENT_PORT, TX_WAIT_FOREVER);
  assert(nrv == NX_SUCCESS);

  while (1) {
    /* TX buffer */
    char buf[example_msg_size_max];

    /* Print message into buffer (with terminating \0) and update its size */
    size_t size = 1 + snprintf(buf, sizeof(buf), example_msg_fmt, msg_count);
    msg_count++;

    /* Allocate a packet from the packet pool */
    nrv = nx_packet_allocate(packet_pool, &udp_packet, NX_UDP_PACKET,
                             TX_WAIT_FOREVER);
    assert(nrv == NX_SUCCESS);

    /* Write an arbitrary message */
    nx_packet_data_append(udp_packet, buf, size, packet_pool, TX_WAIT_FOREVER);
    
    /* Send packet to the server */
    nrv = nxd_udp_socket_send(&udp_socket, udp_packet, &ipv6_multicast_address,
                              EXAMPLE_SERVER_PORT);
    assert(nrv == NX_SUCCESS);

    /* Sleep 1 second between transmissions */
    usleep(1000000);
  }
}

void example_rx_thread_entry(ULONG input)
{
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* NetX UDP socket */
  NX_UDP_SOCKET udp_socket;
  /* NetX UDP packet */
  NX_PACKET *udp_packet = NULL;
  /* Not using input */
  (void)input;

  /* Create a UDP socket for receiving UDP packets */
  nrv = nx_udp_socket_create(untrusted_instance, &udp_socket, "udp_rx_socket",
                             NX_IP_NORMAL, NX_FRAGMENT_OKAY, 0x80, 5);
  assert(nrv == NX_SUCCESS);

  /* Bind UDP socket to example server port */
  nrv = nx_udp_socket_bind(&udp_socket, EXAMPLE_SERVER_PORT, TX_WAIT_FOREVER);
  assert(nrv == NX_SUCCESS);

  while (1) {
    /* Receive a UDP packet */
    nrv = nx_udp_socket_receive(&udp_socket, &udp_packet, TX_WAIT_FOREVER);
    assert(nrv == NX_SUCCESS);

    /* Print length and first bytes of received packet */
    if (udp_packet->nx_packet_prepend_ptr[udp_packet->nx_packet_length - 1]
        != '\0') {
      printf("Received a bad message (not zero-terminated)\n");
    }
    else {
      printf("Received message: \"%s\"\n", udp_packet->nx_packet_prepend_ptr);
    }

    /* Release the packet */
    nx_packet_release(udp_packet);
  }
}
