/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

#include <atlk/v2x.h>
#include <atlk/v2x_service.h>

#include <atlk/remote.h>
#include <atlk/v2x_remote.h>

/*
  Remote POSIX V2X Example

  This example demonstrates basic usage of the remote V2X API for code
  running externally to CRATON processor with an OS supporting POSIX.

  Two threads are created -- a TX thread and a RX thread. A V2X service is
  retrieved and a V2X socket is created; these are used by both threads.

  The TX thread sends a broadcast frame with protocol ID 0x102. The RX thread
  receives frames with protocol ID 0x102 and prints their content as well as
  receive power.
*/

/* TX thread */
static pthread_t tx_thread;
static void *tx_thread_entry(void *arg);

/* RX thread */
static pthread_t rx_thread;
static void *rx_thread_entry(void *arg);

/* Interface index used in this example */
#define IF_INDEX 1

/* Protocol identifier used in this example */
#define PROTO_ID 0x102ULL

/* Shared V2X socket */
static v2x_socket_t *v2x_socket = NULL;

/* Example message format string: Example <seq_num> */
static const char msg_fmt[] = "Example %u";

/* Example message string maximum length */
static const size_t msg_size_max = sizeof(msg_fmt) + 10;

int main(int argc, char *argv[])
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* POSIX return value */
  int rv = 0;
  /* Remote transport object */
  remote_transport_t *transport = NULL;
  /* Remote IP transport configuration parameters */
  remote_ip_transport_config_t config = REMOTE_IP_TRANSPORT_CONFIG_INIT;
  /* Local IPv4 address */
  uint32_t local_ipv4_addr;
  /* Server IPv4 address */
  uint32_t server_ipv4_addr;
  /* V2X service */
  v2x_service_t *v2x_service = NULL;
  /* V2X socket configuration parameters */
  v2x_socket_config_t v2x_socket_config = V2X_SOCKET_CONFIG_INIT;

  if ((argc != 3) || ((server_ipv4_addr = inet_addr(argv[1])) == INADDR_NONE)) {
    fprintf(stderr, "Usage: %s SERVER-IP4-ADDR LOCAL-IF-NAME\n", argv[0]);
    return EXIT_FAILURE;
  }

  /* Get local IPv4 address */
  rc = remote_util_local_ipv4_address_get(argv[2], &local_ipv4_addr);
  if (atlk_error(rc)) {
    fprintf(stderr, "remote_util_local_ipv4_address_get: %s\n",
            atlk_rc_to_str(rc));
    goto exit;
  }

  /* Set remote IP transport configuration parameters */
  config.local_ipv4_address = local_ipv4_addr;
  config.remote_ipv4_address = server_ipv4_addr;
  config.max_rtt_ms = 10;

  /* Create remote IP transport object */
  rc = remote_ip_transport_create(&config, &transport);
  if (atlk_error(rc)) {
    fprintf(stderr, "remote_ip_transport_create: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  /* Create the V2X service */
  rc = v2x_remote_service_create(transport, NULL, &v2x_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_remote_service_create: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  /* Set V2X socket configuration parameters */
  v2x_socket_config.if_index = IF_INDEX;
  v2x_socket_config.protocol.frame_type = V2X_FRAME_TYPE_DATA;
  v2x_socket_config.protocol.protocol_id = PROTO_ID;

  /* Create V2X socket */
  rc = v2x_socket_create(v2x_service, &v2x_socket, &v2x_socket_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  /* Create TX thread */
  rv = pthread_create(&tx_thread, NULL, tx_thread_entry, NULL);
  if (rv) {
    fprintf(stderr, "pthread_create: %s\n", strerror(rv));
    rc = ATLK_E_UNSPECIFIED;
    goto exit;
  }

  /* Create RX thread */
  rc = pthread_create(&rx_thread, NULL, rx_thread_entry, NULL);
  if (rv) {
    fprintf(stderr, "pthread_create: %s\n", strerror(rv));
    rc = ATLK_E_UNSPECIFIED;
    goto exit;
  }

  /* Wait forever */
  while (1) {
    usleep(1000000);
  }

exit:
  /* Clean-up resources */
  v2x_socket_delete(v2x_socket);
  v2x_service_delete(v2x_service);
  remote_transport_delete(transport);

  return atlk_error(rc);
}

static void *tx_thread_entry(void *arg)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Send parameters */
  v2x_send_params_t send_params = V2X_SEND_PARAMS_INIT;
  /* Message counter */
  uint32_t msg_count = 0;
  /* Not using input */
  (void)arg;

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
      return NULL;
    }

    /* Sleep 1 second between transmissions */
    usleep(1000000);
  }

  return NULL;
}

static void *rx_thread_entry(void *arg)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Not using input */
  (void)arg;

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
      return NULL;
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

  return NULL;
}
