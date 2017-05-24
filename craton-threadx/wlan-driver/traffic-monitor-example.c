/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <craton/syslog.h>
#include <craton/wlan_driver.h>

#include <atlk/v2x.h>
#include <atlk/v2x_service.h>

/*
  CRATON WLAN Driver Traffic Monitor Example

  This example demonstrates basic usage of WLAN driver traffic monitor API.

  Two traffic monitor callbacks are registered: a RX callback and a TX
  callback. Both callbacks are registered on the first WLAN device.

  A single thread is spawned in which broadcast frames with protocol ID 0x102
  are sent on interface index 1. Received frames are discarded (to avoid
  running out of memory due to full RX queue).

  Example output is sent to Syslog at log level INFO. It is important to set
  Syslog sink to UDP so as traffic monitor callbacks will return quickly
  (printing to console takes a lot of time and should not be done within
  callbacks).
*/

/* Example threads priorities */
#define TX_THREAD_PRIORITY 40
#define RX_THREAD_PRIORITY 41

#if defined __CRATON_NO_ARC || defined __CRATON_ARC1

/* Transmitting thread */
static TX_THREAD thread;
static uint8_t thread_stack[0x1000];
static void thread_entry(ULONG input);

/* Shared V2X service */
static v2x_service_t *v2x_service = NULL;

/* Shared V2X socket */
static v2x_socket_t *v2x_socket = NULL;

/* Interface index for example messages */
#define IF_INDEX 1

/* Protocol identifier for example messages */
#define PROTO_ID 0x102ULL

/* Length of payload used in this example */
#define PAYLOAD_LENGTH 45

/* Cleanup any allocated resources */
static void cleanup(void)
{
  v2x_socket_delete(v2x_socket);
  v2x_service_delete(v2x_service);
}

/* WLAN device ID used by traffic monitor */
#define WLAN_DEV_ID 0

/* Callback called on each received frame */
static void
rx_callback(const wlan_frame_t *frame, const wlan_rx_frame_info_t *info);

/* Callback called on each sent frame */
static void
tx_callback(const wlan_frame_t *frame, const wlan_tx_frame_info_t *info);

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* V2X socket configuration */
  v2x_socket_config_t socket_config = V2X_SOCKET_CONFIG_INIT;

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
  rc = v2x_socket_create(v2x_service, &v2x_socket,
                         &socket_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Register traffic monitor TX callback */
  rc = wlan_tx_traffic_monitor_set(WLAN_DEV_ID, tx_callback);
  if (atlk_error(rc)) {
    fprintf(stderr, "wlan_tx_traffic_monitor_set: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Register traffic monitor RX callback */
  rc = wlan_rx_traffic_monitor_set(WLAN_DEV_ID, rx_callback);
  if (atlk_error(rc)) {
    fprintf(stderr, "wlan_rx_traffic_monitor_set: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Create TX thread */
  trv = tx_thread_create(&thread, "thread",
                         thread_entry, 0,
                         thread_stack,
                         sizeof(thread_stack),
                         TX_THREAD_PRIORITY,
                         TX_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);
  
  return;

error:
  cleanup();
}

void thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Send parameters */
  v2x_send_params_t send_params = V2X_SEND_PARAMS_INIT;
  /* Receive parameters */
  v2x_receive_params_t receive_params = V2X_RECEIVE_PARAMS_INIT;
  /* Not using input */
  (void)input;

  /* Set transmit power to -10 dBm */
  send_params.power_dbm8 = -80;

  while (1) {
    /* Payload of all-zeros */
    char buf[PAYLOAD_LENGTH] = { 0 };
    /* Size of payload */
    size_t size = sizeof(buf);

    /* Transmit V2X PDU */
    rc = v2x_send(v2x_socket, buf, size, &send_params, NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_send: %s\n", atlk_rc_to_str(rc));
      goto error;
    }

    /* Recieve and discard frame - to avoid a full queue */
    size = sizeof(buf);
    rc = v2x_receive(v2x_socket, buf, &size, &receive_params, NULL);
    if (atlk_error(rc) && rc != ATLK_E_NOT_READY) {
      fprintf(stderr, "v2x_receive: %s\n", atlk_rc_to_str(rc));
      goto error;
    }

    /* Sleep 1 second between transmissions */
    usleep(1000000);
  }

error:
  cleanup();
}

/* Callback called on each received frame */
static void
rx_callback(const wlan_frame_t *frame, const wlan_rx_frame_info_t *info)
{
  syslog(LOG_INFO, "Received frame on device ID %u", info->device_id);
  syslog(LOG_INFO, "Total frame length %d bytes, data rate %.1f Mbps",
         (int)frame->frame_header_size + (int)frame->frame_body_size,
         (float)info->datarate / 2.0);
  syslog(LOG_INFO, "Frame was received in MAC HW at: %llu us",
         info->rx_time_us);
  syslog(LOG_INFO, "RX complete ISR occured at: %llu us",
         info->rx_isr_time_us);
}

/* Callback called on each sent frame */
static void
tx_callback(const wlan_frame_t *frame, const wlan_tx_frame_info_t *info)
{
  syslog(LOG_INFO, "Sent frame on device ID %u", info->device_id);
  syslog(LOG_INFO, "Total frame length %d bytes, data rate %.1f Mbps",
         (int)frame->frame_header_size + (int)frame->frame_body_size,
         (float)info->datarate / 2.0);
  syslog(LOG_INFO, "Frame was queued in TX queue at: %llu us",
         info->tx_queue_time_us);
  syslog(LOG_INFO, "TX complete ISR occured at: %llu us",
         info->tx_isr_time_us);
}

#else /* __CRATON_NO_ARC || __CRATON_ARC1 */

void craton_user_init(void)
{
}

#endif  /* __CRATON_NO_ARC || __CRATON_ARC1 */
