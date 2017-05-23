/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>
#include <nx_api.h>

#include <atlk/v2x.h>
#include <atlk/v2x_service.h>

#include <craton/net.h>

/*
  CRATON V2X-UDP Bridge Example

  This example demonstrates using the V2X API and the NetX-Duo API in order
  to construct a V2X-UDP bridge application.

  To simplify the example, server IP address is hard-coded under the
  SERVER_ADDR define. Users are free to change this value as required. 

  Two threads are created -- a TX thread and a RX thread. A V2X service is
  retrieved and a V2X socket is created, bound to Protocol ID 0x0FA1; these
  are used by both threads. Additionally, a NetX-Duo UDP socket is created,
  bound to port 2002.

  The TX thread receives UDP frames and transmits their content via the V2X
  API. The RX thread receives frames from V2X API and transmits their content
  over UDP to the server.

  @todo This example is not currently supported in multi-core SDK. 
*/

/* UDP server IP address */
#define SERVER_ADDR (10 << 24 | 10 << 16 | 1 << 8 | 121 << 0)

/* UDP ports to receive and transmit on */
#define UDP_TX_PORT 2001
#define UDP_RX_PORT 2002

/* V2X Protocol ID */
#define PROTOCOL_ID 0x0FA1ULL

/* Used V2X interface index */
#define IF_INDEX 1

/* Maximum message size (excluding IP/UDP headers) */
#define MESSAGE_SIZE_MAX 1450

/* Example threads priorities */
#define TX_THREAD_PRIORITY 40
#define RX_THREAD_PRIORITY 41

#if defined __CRATON_NO_ARC || defined __CRATON_ARC1

/* Transmitting thread */
static TX_THREAD tx_thread;
static uint8_t tx_thread_stack[0x2000];
static void tx_thread_entry(ULONG input);

/* Receiving thread */
static TX_THREAD rx_thread;
static uint8_t rx_thread_stack[0x2000];
static void rx_thread_entry(ULONG input);

/* Shared V2X service */
static v2x_service_t *v2x_service = NULL;

/* Shared V2X socket */
static v2x_socket_t *v2x_socket = NULL;

/* Shared UDP socket */
static NX_UDP_SOCKET udp_socket;

/* Trusted IP instance packet pool */
NX_PACKET_POOL *packet_pool = NULL;

/* Cleanup any allocated resources */
static void cleanup(void)
{
  v2x_socket_delete(v2x_socket);
  v2x_service_delete(v2x_service);
}

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* V2X socket configuration */
  v2x_socket_config_t socket_config = V2X_SOCKET_CONFIG_INIT;
  /* NetX trusted IP instance */
  NX_IP *ip_instance = NULL;

  /* Get default V2X service instance */
  rc = v2x_default_service_get(&v2x_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set socket configuration */
  socket_config.if_index = IF_INDEX;
  socket_config.protocol.protocol_id = PROTOCOL_ID;

  /* Create a V2X socket */
  rc = v2x_socket_create(v2x_service, &v2x_socket, &socket_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Get trusted NetX-Duo IP instance */
  rc = net_ip_trusted_instance_get(&ip_instance);
  if (atlk_error(rc)) {
    fprintf(stderr, "net_ip_trusted_instance_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set pointer to trusted IP instance packet pool */
  packet_pool = ip_instance->nx_ip_default_packet_pool;

  /* Create a UDP socket */
  nrv = nx_udp_socket_create(ip_instance, &udp_socket, "udp_socket",
                             NX_IP_NORMAL, NX_FRAGMENT_OKAY, 0x80, 20);
  assert(nrv == NX_SUCCESS);

  /* Bind the UDP socket to the UDP receive port */
  nrv = nx_udp_socket_bind(&udp_socket, UDP_RX_PORT, NX_NO_WAIT);
  assert(nrv == NX_SUCCESS);

  /* Create TX thread */
  trv = tx_thread_create(&tx_thread, "tx_thread",
                         tx_thread_entry, 0,
                         tx_thread_stack,
                         sizeof(tx_thread_stack),
                         TX_THREAD_PRIORITY,
                         TX_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  /* Create RX thread */
  trv = tx_thread_create(&rx_thread, "rx_thread",
                         rx_thread_entry, 0,
                         rx_thread_stack,
                         sizeof(rx_thread_stack),
                         RX_THREAD_PRIORITY,
                         RX_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);
  
  return;

error:
  cleanup();
}

void tx_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* Send parameters */
  v2x_send_params_t send_params = V2X_SEND_PARAMS_INIT;
  /* NetX packet */
  NX_PACKET *udp_packet = NULL;
  /* Message buffer */
  uint8_t message[MESSAGE_SIZE_MAX];
  /* Message size */
  size_t size;
  /* Not using input */
  (void)input;

  while (1) {
    /* Receive a UDP packet (wait forever until it arrives) */
    nrv = nx_udp_socket_receive(&udp_socket, &udp_packet, TX_WAIT_FOREVER);
    assert(nrv == NX_SUCCESS);

    /* Copy packet data into local message buffer */
    size = sizeof(message);
	nrv = nx_packet_data_retrieve(udp_packet, message, (ULONG *)&size);
    assert(nrv == NX_SUCCESS);

	/* Release UDP packet */
    nrv = nx_packet_release(udp_packet);
    assert(nrv == NX_SUCCESS);

    /* Transmit V2X PDU */
    rc = v2x_send(v2x_socket, message, size, &send_params, NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_send: %s\n", atlk_rc_to_str(rc));
      goto error;
    }
  }

error:
  cleanup();
}

void rx_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* Received V2X parameters */
  v2x_receive_params_t receive_params = V2X_RECEIVE_PARAMS_INIT;
  /* NetX packet */
  NX_PACKET *udp_packet = NULL;
  /* Message buffer */
  uint8_t message[MESSAGE_SIZE_MAX];
  /* Message size */
  size_t size;
  /* Not using input */
  (void)input;

  while (1) {
    /* Receive frame (wait forever until it arrives) */
    size = sizeof(message);
    rc = v2x_receive(v2x_socket, message, &size, &receive_params,
                     &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_receive: %s\n", atlk_rc_to_str(rc));
      goto error;
    }

    /* Allocate a packet from the packet pool */
    nrv = nx_packet_allocate(packet_pool, &udp_packet,
                             NX_UDP_PACKET, TX_WAIT_FOREVER);
    assert(nrv == NX_SUCCESS);

    /* Copy received V2X message into packet data */
    nx_packet_data_append(udp_packet, message, size,
                          packet_pool, TX_WAIT_FOREVER);

    /* Send UDP packet */
    nrv = nx_udp_socket_send(&udp_socket, udp_packet,
                             SERVER_ADDR, UDP_TX_PORT);
    assert(nrv == NX_SUCCESS);
  }

error:
  cleanup();
}

#else /* __CRATON_NO_ARC || __CRATON_ARC1 */

void craton_user_init(void)
{
}

#endif  /* __CRATON_NO_ARC || __CRATON_ARC1 */
