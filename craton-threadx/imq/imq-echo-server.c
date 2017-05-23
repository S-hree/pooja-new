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

  This file implements an IMQ echo server which runs on the ARM CPU.
  
  An IMQ service socket is bound-to and listened upon. A thread is created in
  which a connection is accepted; IMQ messages are received in a loop and sent
  back to their origin (i.e. echoed back).
*/

/* IMQ echo server thread priority */
#define IMQ_ECHO_SERVER_THREAD_PRIORITY 60

/* IMQ echo server thread */
static TX_THREAD imq_echo_server_thread;
static uint8_t imq_echo_server_thread_stack[0x2000];
static void imq_echo_server_thread_entry(ULONG input);

/* IMQ echo server service socket */
static imq_socket_t service_socket = IMQ_SOCKET_INIT;

/* IMQ echo server data socket */
static imq_socket_t data_socket = IMQ_SOCKET_INIT;

/* Cleanup any allocated resources */
static void example_cleanup(void)
{
  imq_close(&data_socket);
  imq_close(&service_socket);
}

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* IMQ service configuration */
  imq_service_config_t config = IMQ_SERVICE_CONFIG_INIT;

  /* Bind IMQ echo server socket */
  rc = imq_bind(&service_socket, IMQ_ECHO_SERVER_ADDRESS);
  if (atlk_error(rc)) {
    fprintf(stderr, "imq_bind: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set socket configuration parameters */
  config.server_to_client_config.queue_mtu = IMQ_ECHO_SERVER_QUEUE_MTU;
  config.server_to_client_config.queue_length = IMQ_ECHO_SERVER_QUEUE_LENGTH;
  config.client_to_server_config.queue_mtu = IMQ_ECHO_SERVER_QUEUE_MTU;
  config.client_to_server_config.queue_length = IMQ_ECHO_SERVER_QUEUE_LENGTH;

  /* Give the socket a descriptive name */
  config.service_name = "imq_echo_server";

  /* Listen on IMQ echo server socket */
  rc = imq_listen(&service_socket, &config);
  if (atlk_error(rc)) {
    fprintf(stderr, "imq_listen: %s\n", atlk_rc_to_str(rc));
    goto error;
  }
  
  /* Create IMQ echo server thread */
  trv = tx_thread_create(&imq_echo_server_thread, "imq_echo_server_thread",
                         imq_echo_server_thread_entry, 0,
                         imq_echo_server_thread_stack,
                         sizeof(imq_echo_server_thread_stack),
                         IMQ_ECHO_SERVER_THREAD_PRIORITY,
                         IMQ_ECHO_SERVER_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;

error:
  example_cleanup();
}

static void imq_echo_server_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Not using input */
  (void)input;

  /* Accept an IMQ connection */
  rc = imq_accept(&service_socket, &data_socket, &atlk_wait_forever);
  if (atlk_error(rc)) {
    fprintf(stderr, "imq_accept: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  printf("IMQ echo server started...\n");

  while (1) {
    /* Receive/send buffer */
    char buf[IMQ_ECHO_SERVER_QUEUE_MTU];
    /* Receive/send size */
    size_t size = sizeof(buf);

    /* Receive a IMQ message */
    rc = imq_receive(&data_socket, buf, &size, &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr, "imq_receive: %s\n", atlk_rc_to_str(rc));
      goto error;
    }
      
    /* Send IMQ echo message */
    rc = imq_send(&data_socket, buf, size, NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "imq_send: %s\n", atlk_rc_to_str(rc));
      goto error;
    }
  }

error:
  example_cleanup();
}
