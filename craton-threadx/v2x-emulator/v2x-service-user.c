/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/sdk.h>
#include <atlk/v2x_service.h>

#include <craton/v2x_emulator_init.h>

#include "v2x-emulator-example-common.h"

/*
  CRATON ThreadX V2X Emulator Example

  Following code is part of the V2X Emulator example. 
  
  For details regarding the V2X Emulator example 
  PLEASE see comments @sdk/example/craton-threadx/v2x-emulator

  The actual code in this file is base on the SDK V2X API example. 
  See @sdk/example/craton-threadx/v2x/ for more details. 

  V2X service is initiated and a socket opened. 

  A receive thread is created for V2X receive and V2X send is called to 
  broadcast frames. 

  Transmit prints the message to broadcast and receive thread the 
  message received
*/

/* Interface index for example messages */
#define SEND_IF_INDEX 1
#define RECEIVE_IF_INDEX 2

/* Protocol identifier for example messages */
#define EXAMPLE_PROTO_ID 0x102ULL

/* V2X RX thread priority */
#define V2X_RECEIVE_THREAD_PRIORITY 26

/* V2X RX thread priority */
#define V2X_STACK_SIZE 4096

/* Example message format string: Example <seq_num> */
static const char example_msg_fmt[] = "Example %" PRIu32;

/* Example message string maximum length */
static const size_t example_msg_size_max = sizeof(example_msg_fmt) + 10;

/* RX thread */
static TX_THREAD v2x_receive_thread;
static uint8_t v2x_receive_thread_stack[V2X_STACK_SIZE];

/* Shared V2X service */
static v2x_service_t *v2x_service = NULL;

/* Cleanup any allocated resources */
static void example_cleanup(v2x_socket_t *v2x_socket)
{
  v2x_socket_delete(v2x_socket);
  v2x_service_delete(v2x_service);
}

static void 
v2x_receive_thread_entry(ULONG input)
{
  /* V2X socket for sending frames */
  static v2x_socket_t *v2x_socket = NULL;
  /* V2X socket configuration */
  v2x_socket_config_t socket_config = V2X_SOCKET_CONFIG_INIT;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Not using input */
  (void)input;

  /* Set socket configuration */
  socket_config.if_index = RECEIVE_IF_INDEX;
  socket_config.protocol.protocol_id = EXAMPLE_PROTO_ID;

  /* Create a V2X socket for frame receive */
  rc = v2x_socket_create(v2x_service, 
                         &v2x_socket,
                         &socket_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto out; 
  }

  while (1) {
    /* RX buffer */
    char buf[example_msg_size_max];
    /* RX size */
    size_t size = sizeof(buf);
    /* Received V2X parameters */
    v2x_receive_params_t receive_params = V2X_RECEIVE_PARAMS_INIT;

    /* Receive frame (wait forever until it arrives) */
    rc = v2x_receive(v2x_socket, buf, &size, &receive_params,
                     &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_receive: %s\n", atlk_rc_to_str(rc));
      goto out; 
    }

    /* Obtain data as zero-terminated string */
    if (buf[size - 1] != '\0') {
      printf("* Bad message (not zero-terminated)\n");
    }
    else {
      printf("* v2x_receive  - Message RX  : I/F - %d, msg - %s\n", 
             socket_config.if_index, buf);
    }
  }

out:
  example_cleanup(v2x_socket); 
}

atlk_rc_t 
v2x_init_and_send(void)
{
  /* V2X socket for sending frames */
  static v2x_socket_t *v2x_socket = NULL;
  /* V2X socket configuration */
  v2x_socket_config_t socket_config = V2X_SOCKET_CONFIG_INIT;
  /* Message counter */
  uint32_t msg_count = 0;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;

  /* Get default V2X service instance */
  rc = v2x_default_service_get(&v2x_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "Failed to get V2X default service (rc=%d)\n",rc);
    v2x_service_delete(v2x_service);
    return rc; 
  }
  
  /* Create V2X API receive thread */ 
  trv = tx_thread_create(&v2x_receive_thread, "Example V2X UPD RX thread",
                         v2x_receive_thread_entry, 
                         0,
                         v2x_receive_thread_stack, 
                         sizeof(v2x_receive_thread_stack),
                         V2X_RECEIVE_THREAD_PRIORITY, 
                         V2X_RECEIVE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, 
                         TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  /* Set socket configuration */
  socket_config.if_index = SEND_IF_INDEX;
  socket_config.protocol.protocol_id = EXAMPLE_PROTO_ID;

  /* Create a V2X socket for frame send */
  rc = v2x_socket_create(v2x_service, 
                         &v2x_socket,
                         &socket_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto out; 
  }

  while (1) {
    /* Send parameters */
    v2x_send_params_t send_params = V2X_SEND_PARAMS_INIT;
    /* TX buffer */
    char buf[example_msg_size_max];

    /* Print message into buffer (with terminating \0) and update its size */
    size_t size = 1 + snprintf(buf, sizeof(buf), example_msg_fmt, msg_count);
    msg_count++;


    /* Transmit V2X PDU */
    rc = v2x_send(v2x_socket, buf, size, &send_params, NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "Failed to send frame over v2x_send (rc=%d)\n", rc);
      goto out;
    }

    /* Sleep 10 second between transmissions */
    usleep(10000000);
   }

out:
  example_cleanup(v2x_socket); 
  return rc;
}

#ifdef __CRATON_ARC1
void craton_user_init(void)
{
  /* V2X emulator configuration */ 
  v2x_emulator_config_t emulator_config = V2X_EMULATOR_CONFIG_INIT; 
  /* API return code */
  atlk_rc_t rc = ATLK_OK;

  /* Configure v2x_emulator_master */
  emulator_config.sched_params.priority = V2X_EMULATOR_THREAD_PRIORITY; 
  emulator_config.sched_params.time_slice = V2X_EMULATOR_TIME_SLICE; 
  emulator_config.imq_address = V2X_EMULATOR_IMQ_ADDRESS; 


  /* Create emulator */
  rc = v2x_emulator_slave_init(&emulator_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_receive: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Initiate the V2X API for the test */
  rc = v2x_init_and_send();
  if (atlk_error(rc)) {
    fprintf(stderr, "Failed to init V2X API for example (rc=%d)\n", rc); 
    return;
  }

  return; 
}
#endif /* __CRATON_ARC1 */ 

/* Override the implementation of v2x_default_service_get */ 
atlk_rc_t 
v2x_default_service_get(v2x_service_t **service_ptr)
{

#if defined(__CRATON_NO_ARC) || defined(__CRATON_ARC1)

  return v2x_emulator_service_get(service_ptr);

#else 

  return v2x_imq_service_get(service_ptr); 

#endif /* __CRATON_NO_ARC || __CRATON_ARC1 */     

}


