/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/v2x.h>
#include <atlk/v2x_service.h>

/*
  CRATON ThreadX V2X Example

  This example demonstrates basic usage of V2X API for code running on top
  of CRATON processor with ThreadX RTOS.

  Two threads are created -- a TX thread and a RX thread. A V2X service is
  retrieved and a V2X socket is created; these are used by both threads.

  The TX thread sends a broadcast frame with protocol ID 0x102. The RX thread
  receives frames with protocol ID 0x102 and prints their content as well as
  receive power.
*/

/* Example threads priorities */
#define TX_THREAD_PRIORITY 40
#define RX_THREAD_PRIORITY 41

#if defined __CRATON_NO_ARC || defined __CRATON_ARC1

/* TX thread */
static TX_THREAD tx_thread;
static uint8_t tx_thread_stack[0x1000];
static void tx_thread_entry(ULONG input);

/* RX thread */
static TX_THREAD rx_thread;
static uint8_t rx_thread_stack[0x1000];
static void rx_thread_entry(ULONG input);

/* Interface index used in this example */
#define IF_INDEX 1

/* Protocol identifier used in this example */
#define PROTO_ID 0x102ULL

/* Shared V2X socket */
static v2x_socket_t *v2x_socket = NULL;

/* Example message format string: Example <seq_num> */
static const char msg_fmt[] = "Example %" PRIu32;

/* Example message string maximum length */
static const size_t msg_size_max = sizeof(msg_fmt) + 10;

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* V2X socket configuration */
  v2x_socket_config_t socket_config = V2X_SOCKET_CONFIG_INIT;
  /* V2X service */
  v2x_service_t *v2x_service = NULL;

  /* Get default V2X service instance */
  rc = v2x_default_service_get(&v2x_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set socket configuration */
  socket_config.if_index = IF_INDEX;
  socket_config.protocol.protocol_id = PROTO_ID;

  /* Create a V2X socket */
  rc = v2x_socket_create(v2x_service, &v2x_socket, &socket_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

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
  /* Clean-up resources */
  v2x_socket_delete(v2x_socket);
  v2x_service_delete(v2x_service);

  return;
}

void tx_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Send parameters */
  v2x_send_params_t send_params = V2X_SEND_PARAMS_INIT;
  /* Message counter */
  uint32_t msg_count = 0;
  /* Not using input */
  (void)input;

  /* Set transmit power to -10 dBm */
  send_params.power_dbm8 = -80;

  while (1) {
    /* TX buffer */
    char buf[msg_size_max];

    /* Print message into buffer (with terminating \0) and update its size */
    size_t size = 1 + snprintf(buf, sizeof(buf), msg_fmt, msg_count);
    msg_count++;

    printf("Example: Broadcast TX: \"%s\"\n", buf);

    /* Transmit V2X PDU */
    rc = v2x_send(v2x_socket, buf, size, &send_params, NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_send: %s\n", atlk_rc_to_str(rc));
      return;
    }

    /* Sleep 1 second between transmissions */
    usleep(1000000);
  }
}

void rx_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Not using input */
  (void)input;

  while (1) {
    /* RX buffer */
    char buf[msg_size_max];
    /* RX size */
    size_t size = sizeof(buf);
    /* Received V2X parameters */
    v2x_receive_params_t receive_params = V2X_RECEIVE_PARAMS_INIT;

    /* Receive frame (wait forever until it arrives) */
    rc = v2x_receive(v2x_socket, buf, &size, &receive_params,
                     &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_receive: %s\n", atlk_rc_to_str(rc));
      return;
    }

    /* Print source address of received frame */
    printf("Example: RX from %02x:%02x:%02x:%02x:%02x:%02x\n",
           receive_params.source_address.octets[0],
           receive_params.source_address.octets[1],
           receive_params.source_address.octets[2],
           receive_params.source_address.octets[3],
           receive_params.source_address.octets[4],
           receive_params.source_address.octets[5]);

    /* Obtain data as zero-terminated string */
    if (buf[size - 1] != '\0') {
      printf("* Bad message (not zero-terminated)\n");
    }
    else {
      printf("* Message: \"%s\"\n", buf);
    }

    /* Print RX power */
    if (receive_params.power_dbm8 != V2X_POWER_DBM8_NA) {
      printf("* RX power: %.2f dBm\n",
             (double)receive_params.power_dbm8 / V2X_POWER_DBM8_PER_DBM);
    }
  }
}

#else /* __CRATON_NO_ARC || __CRATON_ARC1 */

void craton_user_init(void)
{
}

#endif /* __CRATON_NO_ARC || __CRATON_ARC1 */
