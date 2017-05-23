/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/sdk.h>
#include <atlk/can.h>
#include <atlk/can_service.h>

/*
  CRATON ThreadX CAN Example

  This example demonstrates basic usage of Autotalks CAN API
  for code running on top of CRATON processor with ThreadX RTOS.

  Two threads are created -- sending thread and receiving thread.

  The sending thread transmits a 5-octet CAN message with ID 0x50,
  every 300 milliseconds. The receiving thread receives CAN messages
  and displays them on the system's debug console.

  To see the demo in action you should execute can-example.img on two
  PANGAEA4 units whose CAN ports are connected.
*/

/* CAN ID used in this example */
#define EXAMPLE_CAN_ID 0x50UL

/* CAN device ID used in this example */
#define EXAMPLE_CAN_DEVICE_ID 0

/* Thread priorities */
#define CAN_SEND_THREAD_PRIORITY 40
#define CAN_RECEIVE_THREAD_PRIORITY 41

/* CAN send thread */
static TX_THREAD can_send_thread;
static uint8_t can_send_thread_stack[0x1000];
static void can_send_thread_entry(ULONG input);

/* CAN receive thread */
static TX_THREAD can_receive_thread;
static uint8_t can_receive_thread_stack[0x1000];
static void can_receive_thread_entry(ULONG input);

/* CAN service */
static can_service_t *can_service = NULL;

/* CAN socket */
static can_socket_t *can_socket = NULL;

/* CAN ID filter array */
static const can_id_filter_t filter_array[] = {
  { .can_id = 0, .can_id_mask = 0 },
};

/* Release allocated resources */
static void example_cleanup(void)
{
  can_socket_delete(can_socket);
  can_service_delete(can_service);
}

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* CAN socket configuration */
  can_socket_config_t socket_config = CAN_SOCKET_CONFIG_INIT;

  /* Get default CAN service instance */
  rc = can_default_service_get(&can_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "can_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set socket configuration */
  socket_config.filter_array_ptr = filter_array;
  socket_config.filter_array_size = 1;
  socket_config.device_id = EXAMPLE_CAN_DEVICE_ID;

  /* Create CAN socket */
  rc = can_socket_create(can_service, &can_socket, &socket_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "can_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Create CAN send thread */
  trv = tx_thread_create(&can_send_thread, "can_send_thread",
                         can_send_thread_entry, 0,
                         can_send_thread_stack,
                         sizeof(can_send_thread_stack),
                         CAN_SEND_THREAD_PRIORITY,
                         CAN_SEND_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  /* Create CAN receive thread */
  trv = tx_thread_create(&can_receive_thread, "can_receive_thread",
                         can_receive_thread_entry, 0,
                         can_receive_thread_stack,
                         sizeof(can_receive_thread_stack),
                         CAN_RECEIVE_THREAD_PRIORITY,
                         CAN_RECEIVE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;

error:
  example_cleanup();
}

void can_send_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* CAN message data to send */
  uint8_t data[] = { 1, 2, 3, 4, 5 };
  /* Not using input */
  (void)input;

  while (1) {
    /* Send CAN message with CAN ID 0x50 */
    rc = can_send(can_socket, data, sizeof(data), EXAMPLE_CAN_ID, NULL);
    if (rc == ATLK_E_OUT_OF_MEMORY) {
      /* Sleeping for 1 millisecond to avoid a busy loop. The 'out of memory'
         error is expected when the TX queue is full.
       */
      usleep(1000);
      continue;
    }
    else if (atlk_error(rc)) {
      fprintf(stderr, "can_send: %s\n", atlk_rc_to_str(rc));
      goto error;
    }
    
    /* Print sent CAN message */
    printf("Example: Sent CAN message \"%d,%d,%d,%d,%d\", ID 0x%lx\n",
           data[0], data[1], data[2], data[3], data[4], EXAMPLE_CAN_ID);

    /* Change message content */
    data[0]++;

    /* Wait for 300 milliseconds */
    usleep(300000);
  }

error:
  example_cleanup();
}

void can_receive_thread_entry(ULONG input)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Received CAN message data */
  uint8_t data[CAN_DATA_SIZE_MAX];
  /* Received CAN message data size */
  size_t data_size = sizeof(data);
  /* Received CAN ID */
  can_id_t can_id;
  /* Not using input */
  (void)input;

  while (1) {
    /* Receive CAN message */
    rc = can_receive(can_socket, data, &data_size, &can_id,
                     &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr, "can_receive: %s\n", atlk_rc_to_str(rc));
      goto error;
    }

    /* Print received CAN message */
    printf("Example: Received CAN message \"%d,%d,%d,%d,%d\", ID 0x%lx\n",
           data[0], data[1], data[2], data[3], data[4], can_id);
  }

error:
  example_cleanup();
}
