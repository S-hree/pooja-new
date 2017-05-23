/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <craton/imq.h>

#include "imq-echo-server.h"

/*
  CRATON ThreadX IMQ Example

  This example demonstrates basic usage of IMQ API for code running on
  top of CRATON processor with ThreadX RTOS.

  This file implements an IMQ client which runs on ARC1 CPU.
  
  A thread is created in which a socket is connected; IMQ messages are sent
  and then received in a loop. Messages are compared and are expected to be
  identical.
*/

#ifdef __CRATON_ARC1

/* IMQ client thread priority */
#define IMQ_CLIENT_THREAD_PRIORITY 30

/* IMQ client thread */
static TX_THREAD imq_client_thread;
static uint8_t imq_client_thread_stack[0x2000];
static void imq_client_thread_entry(ULONG input);

/* IMQ client data socket */
static imq_socket_t data_socket = IMQ_SOCKET_INIT;

/* Example message format string: Example <seq_num> */
static const char example_msg_fmt[] = "Example %" PRIu32;

/* Cleanup any allocated resources */
static void example_cleanup(void)
{
  imq_close(&data_socket);
}

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;

  /* Create IMQ client thread */
  trv = tx_thread_create(&imq_client_thread, "imq_client_thread",
                         imq_client_thread_entry, 0,
                         imq_client_thread_stack,
                         sizeof(imq_client_thread_stack),
                         IMQ_CLIENT_THREAD_PRIORITY,
                         IMQ_CLIENT_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;
}

static void imq_client_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Message counter */
  uint32_t msg_count = 0;
  /* Not using input */
  (void)input;

  /* Connect to IMQ echo server */
  rc = imq_connect(&data_socket, IMQ_ECHO_SERVER_ADDRESS, &atlk_wait_forever);
  if (atlk_error(rc)) {
    fprintf(stderr, "imq_connect: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  while (1) {
    /* Receive buffer */
    char rx_buf[IMQ_ECHO_SERVER_QUEUE_MTU];
    /* Receive buffer size */ 
    size_t rx_size = IMQ_ECHO_SERVER_QUEUE_MTU;
    /* Send buffer */
    char tx_buf[IMQ_ECHO_SERVER_QUEUE_MTU];
    /* Send buffer size */ 
    size_t tx_size  = 1 + snprintf(tx_buf, sizeof(tx_buf),
                                   example_msg_fmt, msg_count);
    
    printf("Sending IMQ message: \"%s\"\n", tx_buf);

    /* Send IMQ message to echo server */
    rc = imq_send(&data_socket, tx_buf, tx_size, NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "imq_send: %s\n", atlk_rc_to_str(rc));
      goto error;
    }

    /* Receive IMQ message from echo server */
    rc = imq_receive(&data_socket, rx_buf, &rx_size, &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr, "imq_receive: %s\n", atlk_rc_to_str(rc));
      goto error;
    }

    /* Compare received and sent messages */
    if (rx_size == tx_size && strncmp(tx_buf, rx_buf, rx_size) == 0) {
      printf("Echo message \"%s\" received successfully.\n", rx_buf);
    }
    else {
      fprintf(stderr, "Error! Messages differ.\n");
      goto error;
    }

    /* Increment message counter */
    msg_count++;

    /* Sleep 100ms between transmissions */
    usleep(100000);
  }

error:
  example_cleanup();
}

#else /* __CRATON_ARC1 */

void craton_user_init(void)
{
}

#endif /* __CRATON_ARC1 */
