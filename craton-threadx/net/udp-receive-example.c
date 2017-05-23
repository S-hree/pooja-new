/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>

#include <tx_api.h>
#include <nx_api.h>

#include <craton/net.h>

/**
  CRATON Network Example

  This example demonstrates using the Network API on top of a CRATON
  processor with ThreadX RTOS.

  An `untrusted` IP network instance is obtained.
  
  The instance is used to create a UDP socket and bind it to the arbitrarily
  chosen port number 2009 using NetX-Duo API.

  A thread is created in which UDP packets are received in a loop. Received
  data is retrieved and the packet is released.
*/


/* UDP port to receive on */
#define UDP_RX_PORT 2009

/* Receiving thread parameters */
#define UDP_RX_THREAD_STACK_SIZE (1 << 12)
#define UDP_RX_THREAD_PRIORITY 40

/* Receiving thread */
static TX_THREAD udp_rx_thread;
static uint8_t udp_rx_thread_stack[UDP_RX_THREAD_STACK_SIZE];
static void udp_rx_thread_entry(ULONG input);

/* Example UDP socket */
static NX_UDP_SOCKET udp_socket;

void craton_user_init(void)
{
  /* API return code */
  atlk_rc_t rc = ATLK_OK;
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* NetX trusted IP instance */
  NX_IP *ip_instance = NULL;

  printf("Initializing example UDP server...\n");

  /* Get trusted IP instance */
  rc = net_ip_trusted_instance_get(&ip_instance);
  assert(!atlk_error(rc));

  /* Create a UDP socket */
  nrv = nx_udp_socket_create(ip_instance, &udp_socket, "example_udp_socket",
                             NX_IP_NORMAL, NX_FRAGMENT_OKAY, 0x80, 20);
  assert(nrv == NX_SUCCESS);

  /* Bind the UDP socket to the UDP port */
  nrv = nx_udp_socket_bind(&udp_socket, UDP_RX_PORT, NX_NO_WAIT);
  assert(nrv == NX_SUCCESS);

  /* Create UDP receive thread */
  trv = tx_thread_create(&udp_rx_thread, "example_udp_server_thread",
                         udp_rx_thread_entry, 0,
                         udp_rx_thread_stack, sizeof(udp_rx_thread_stack),
                         UDP_RX_THREAD_PRIORITY, UDP_RX_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;
}

void udp_rx_thread_entry(ULONG input)
{
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* NetX packet */
  NX_PACKET *udp_packet;
  /* Data buffer */
  uint8_t data_buffer[4];
  /* Data buffer size */
  ULONG data_buffer_size;
  /* Not using input */
  (void)input;

  printf("Example UDP server thread receiving on port %d\n", UDP_RX_PORT);

  while(1) {
    /* Receive a UDP packet */
    nrv = nx_udp_socket_receive(&udp_socket, &udp_packet, TX_WAIT_FOREVER);
    assert(nrv == NX_SUCCESS);

    printf("Example UDP server: received packet\n");

    /* Copy (possibly part of) packet data into local buffer */
    data_buffer_size = sizeof(data_buffer);
	nrv = nx_packet_data_retrieve(udp_packet, data_buffer, &data_buffer_size);
    assert(nrv == NX_SUCCESS);

	/* Release packet */
    nrv = nx_packet_release(udp_packet);
    assert(nrv == NX_SUCCESS);

	/* Print some data */
    if (data_buffer_size >= 4) {
      printf("... First 4 bytes are: 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
             data_buffer[0], data_buffer[1], data_buffer[2], data_buffer[3]);
    }
    else {
      printf("... Payload is shorter than 4 bytes\n");
    }
  }
}
