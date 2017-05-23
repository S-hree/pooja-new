/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>

#include <tx_api.h>
#include <nxd_bsd.h>
#include <libcli.h>

#include <atlk/sdk.h>

#include <craton/v2x_emulator_init.h>
#include <craton/net.h>
#include <craton/cli.h>

#include "v2x-emulator-example-common.h"

/*
  CRATON ThreadX V2X Emulator Example


  Notes! 
  =========
  1. V2X Emulator is only available on CRATON ARM. 
  3. The emulator leaves the choice of external interface to the User glue code.

  Basic emulator flow concept 
  -----------------
  Function posfix send/receive are in respect to the v2x_emulator meaning:
  * The stack calls v2x_send to pass data to the V2X emulator. 
    The emulator stores the data and waits for a v2x_emulator_receive 
    to be called by the User glue code. 
    At this point the User glue code can send the data in which ever format and
    no which ever interface. 

  * Upon the reception of data by the User glue code on which ever interface 
    a v2x_emulator_send should be called to pass data to the emulator. 
    The emulator will store the data until v2x_receive is called by the stack. 
 
  See figure below for details. 

 ** v2x_send flow ** 
 ---------               ------------                        ----------
 |  V2X  |  v2x_send     |   V2X    |  v2x_emulator_receive  |  User  | send
 | Stack |------------->>| Emulator |---------------------->>|  Glue  |------->
 |       |               |          |                        |  Code  |
 ---------               ------------                        ---------- (Example
                                                                           UDP
 ** v2x_receive flow **                                                  socket)
 ---------               ------------                        ----------
 |  V2X  |  v2x_receive  |   V2X    |  v2x_emulator_send     |  User  | receive
 | Stack |<<-------------| Emulator |<<----------------------|  Glue  |<-------
 |       |               |          |                        |  Code  |
 ---------               ------------                        ----------

  User glue can implement a connection with any type of interface such as:
    * UDP socket 
    * Raw Ethernet socket 
    * UART 
   

  The example
  --------------

  This example demonstrates basic usage of V2X Emulator API for code running on 
  top of CRATON processor with ThreadX RTOS.

  A UDP socket is created and used to transmit the v2x frame and other 
  v2x attributes over the Ethrenet to a receiving server/board. 

  V2X Emulator master_init is called to create and get the emulator which will 
  be used to communicate with the external interface.

  V2X Emulator slave_init is called to start the emulator part which 
  will be needed to communicate with the V2X stack (for multi core see 
  v2x_service_user.c). 

  Override of the v2x_default_service_get is required to enable the V2X stack 
  to get an instance of v2x emulator in place of v2x hw service - i.e. RF I/F 
  (see file v2x_service_user.c). 

  Two threads are open the first is for UDP transmit, in it the 
  v2x_emulator_receive is called waiting for a v2x_send command. The data is 
  then transmitted to the UDP socket. 
  
  The other thread is used to receive in coming UDP frames and switching 
  form the received V2X interface to the second interface and then transferring 
  them to via v2x_emulator_send to v2x_receive. 

  Once image is loaded run cli command 
            ate> v2x_emulator_start <IP addr> 
      Where: 
        - A single board look back test can be run by entering the 
          boards IP addr. 
        - Board to Board/Host can be performed by entering 
          the second Board/Host IP. 

*/

/* Example V2X Interface index */ 
#define V2X_FIRST_IF_INDEX 1
#define V2X_SECOND_IF_INDEX 2

/* Example V2X emulator thread priority */
#define V2X_EMULATOR_SEND_THREAD_PRIORITY 25
#define V2X_EMULATOR_RECEIVE_THREAD_PRIORITY 26

/* Example V2X emulator stack size */
#define V2X_EMULATOR_STACK_SIZE 8192

/* Example V2X emulator max message size */ 
#define V2X_EMULATOR_MAX_MSG_SIZE  2048

/* Example UDP port */ 
#define UDP_PORT  2009

/* V2X emulator thread */ 
static TX_THREAD example_v2x_emulator_send_thread;
static uint8_t example_v2x_emulator_send_thread_stack[V2X_EMULATOR_STACK_SIZE];

/* V2X emulator thread */ 
static TX_THREAD example_v2x_emulator_receive_thread;
static uint8_t example_v2x_emulator_receive_thread_stack[V2X_EMULATOR_STACK_SIZE];

/* Example V2X emulator */
static v2x_emulator_t *v2x_emulator;

/* Example UDP shared socket */ 
static int socket_fd;

/* Example V2X emulator UDP payload */
typedef struct {
  /** V2X interface index */
  v2x_if_index_t if_index;
  /** V2X packet protocol */
  v2x_protocol_t protocol;
  /** V2X send parameters */
  eui48_t dest_address;
  v2x_datarate_t datarate;
  v2x_power_dbm8_t power_dbm8;
  /** data size */
  size_t data_size;
  /** data */
  uint8_t data[V2X_EMULATOR_MAX_MSG_SIZE];
}  udp_payload_t;

#ifdef __CRATON_NO_ARC
extern atlk_rc_t v2x_init_and_send(); 
#endif /* __CRATON_NO_ARC */ 

static atlk_rc_t 
open_udp_connection(char *ip_addr_str) 
{
  #define INADDR_NONE 0xFFFFFFFF
  uint32_t ip_addr; 
  struct sockaddr_in _sockaddr_in;
  int rc;

  /* Convert string ip to uint  */ 
  ip_addr = inet_addr(ip_addr_str);

  /* Create socket */
  socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (socket_fd == -1) {
    printf("Failed creating socket");
    return ATLK_E_UNSPECIFIED;
  }

  /* Bind socket */
  memset(&_sockaddr_in, 0, sizeof(_sockaddr_in));
  _sockaddr_in.sin_family = AF_INET;
  _sockaddr_in.sin_port = htons(UDP_PORT);
  _sockaddr_in.sin_addr.s_addr = INADDR_ANY;
  rc = bind(socket_fd, 
           (struct sockaddr *)&_sockaddr_in,
            sizeof(_sockaddr_in));
  if (rc == -1) {
    printf("Failed binding socket");
    return ATLK_E_UNSPECIFIED;
  }

  /* Set dest ip and port */
  memset(&_sockaddr_in, 0, sizeof(_sockaddr_in));
  _sockaddr_in.sin_family = AF_INET;
  _sockaddr_in.sin_port = htons(UDP_PORT);
  _sockaddr_in.sin_addr.s_addr = ip_addr;
  rc = connect(socket_fd,
              (struct sockaddr *)&_sockaddr_in,
               sizeof(_sockaddr_in));
  if (rc == -1) {
    printf("Failed connecting socket");
    return ATLK_E_UNSPECIFIED;
  }

  return ATLK_OK;
}


static void 
example_v2x_emulator_send_thread_entry(ULONG input)
{
  /* The V2X payload to send */
  udp_payload_t udp_payload;
  /* V2X receive params */ 
  v2x_receive_params_t receive_params = V2X_RECEIVE_PARAMS_INIT;
  /* ATLK return value */
  atlk_rc_t rc;
  int rv; 

  (void)input;

  while(1) {
  
    /* Receive a UDP packet.  */
    rv = recv(socket_fd, &udp_payload, sizeof(udp_payload), 0);

    if (rv == -1) {
      printf("Failed to receive on UDP socket (rv=%d)", rv);
      return;
    }

    /* Extract remaining data */
    receive_params.dest_address    = udp_payload.dest_address;
    receive_params.datarate        = udp_payload.datarate;
    receive_params.power_dbm8      = udp_payload.power_dbm8;
    
    /* Switch V2X I/F such that if sent on one received on the other */
    if (udp_payload.if_index == V2X_FIRST_IF_INDEX) {
      udp_payload.if_index = V2X_SECOND_IF_INDEX;
    }
    else {
      udp_payload.if_index = V2X_FIRST_IF_INDEX;
    }

    /* Send received data to V2X API - v2x_receive */
    rc = v2x_emulator_send(v2x_emulator, 
                           udp_payload.if_index, 
                           &udp_payload.protocol,
                           udp_payload.data, 
                           udp_payload.data_size, 
                           &receive_params,
                           NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "Failed to send data to V2X emulator (rc=%d)\n", rc);
      return;
    }
  }
}

/* V2X emulator receive buffer */ 
uint8_t receive_data[V2X_EMULATOR_MAX_MSG_SIZE];

static void 
example_v2x_emulator_receive_thread_entry(ULONG input)
{
  /* RX data size */ 
  size_t data_size; 
  /* V2X egress/ingress I/F number */ 
  v2x_if_index_t egress_if_index;
  /* V2X protocol */ 
  v2x_protocol_t protocol = V2X_PROTOCOL_INIT;
  /* V2X send params */ 
  v2x_send_params_t send_params = V2X_SEND_PARAMS_INIT;
 /* The V2X payload to send */
  udp_payload_t udp_payload;
  /* ATLK return value */
  atlk_rc_t rc;
  /*  return value */
  ssize_t rv;

  (void)input; 

  while(1) {

    /* Send data size */ 
    data_size = sizeof(receive_data); 

    /* Receive data form V2X API - v2x_send */
    rc = v2x_emulator_receive(v2x_emulator,
                              &egress_if_index, 
                              &protocol,
                              receive_data, 
                              &data_size, 
                              &send_params,
                              &atlk_wait_forever);
    if (atlk_error(rc)) {
      fprintf(stderr, "Failed to receive data from V2X emulator (rc=%d)\n", rc);
      return;
    }

    printf("* v2x_send     - Broadcast TX: I/F - %d, msg - %s\n",
           egress_if_index, receive_data);

    /* Build UDP payload to send */
    udp_payload.if_index    = egress_if_index;
    udp_payload.protocol    = protocol;
    udp_payload.dest_address = send_params.dest_address;
    udp_payload.datarate     = send_params.datarate;
    udp_payload.power_dbm8   = send_params.power_dbm8;

    /* Extract the data */ 
    udp_payload.data_size = data_size; 
    memcpy(udp_payload.data, receive_data, data_size); 

    data_size = sizeof(udp_payload_t) -
      V2X_EMULATOR_MAX_MSG_SIZE + udp_payload.data_size;

    rv = send(socket_fd, (CHAR *)&udp_payload, data_size, 0);
    if ((size_t)rv != data_size) {
      printf("Failed to send UDP packet (rv=%d)", rv);
      return;
    }
  }
}

atlk_rc_t
example_v2x_emulator(char *ip_addr)
{
  /* API return code */
  atlk_rc_t rc = ATLK_OK;
  /* ThreadX return value */
  UINT trv;

  /* Open a UDP connection  */ 
  rc = open_udp_connection(ip_addr); 
  if (atlk_error(rc)) {
    fprintf(stderr, "Failed to create UDP connection (rc=%d)\n", rc); 
    return rc; 
  }

  /* Create V2X emulator send thread */ 
  trv = tx_thread_create(&example_v2x_emulator_send_thread,
                         "Example V2X emulator send thread",
                         example_v2x_emulator_send_thread_entry, 0,
                         example_v2x_emulator_send_thread_stack, 
                         sizeof(example_v2x_emulator_send_thread_stack),
                         V2X_EMULATOR_SEND_THREAD_PRIORITY, 
                         V2X_EMULATOR_SEND_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, 
                         TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  /* Create V2X emulator receive thread */ 
  trv = tx_thread_create(&example_v2x_emulator_receive_thread,
                         "Example V2X emulator receive thread",
                         example_v2x_emulator_receive_thread_entry, 0,
                         example_v2x_emulator_receive_thread_stack, 
                         sizeof(example_v2x_emulator_receive_thread_stack),
                         V2X_EMULATOR_SEND_THREAD_PRIORITY, 
                         V2X_EMULATOR_SEND_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, 
                         TX_AUTO_START);
  assert(trv == TX_SUCCESS);

#ifdef __CRATON_NO_ARC

  /* Initiate the V2X API for the test */
  rc = v2x_init_and_send();
  if (atlk_error(rc)) {
    fprintf(stderr, "Failed to init V2X API for example (rc=%d)\n", rc); 
    return rc; 
  }

#endif /* ! __CRATON_NO_ARC */   

  return rc;
}

#define V2X_EMULATOR_CLI_HELP "Start V2X Emulator UDP example\n" \
  "usage: v2x_emulator_start <Destination IP>\n"

int cmd_example_v2x_emulator(struct cli_def *cli, 
                             const char *command, 
                             char *argv[], 
                             int argc)
{
  /* Autotalks return code */
  atlk_rc_t rc;

  (void)command;

  if ('?' == argv[0][0]) {
    cli_print(cli, "%s", V2X_EMULATOR_CLI_HELP);
    return CLI_OK;
  }

  if (argc != 1) {
    cli_print(cli, "Invalid parameters number");
    cli_print(cli, "%s", V2X_EMULATOR_CLI_HELP);
    return CLI_ERROR;
  }

  rc = example_v2x_emulator(argv[0]);
  if (atlk_error(rc)) {
    cli_print(cli, "V2X emulator example failed (rc=%d)", rc);
    return CLI_ERROR;
  }
  
  return CLI_OK;
}

void craton_user_init(void)
{
  /* V2X emulator configuration */ 
  v2x_emulator_config_t emulator_config = V2X_EMULATOR_CONFIG_INIT; 
  /* CLI instance */
  cli_instance_t *cli = NULL;
  /* CLI command handle */
  struct cli_command *command = NULL;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;

  printf("\n\n\nExample V2X emulator\n");

  /* Configure v2x_emulator_master */
  emulator_config.sched_params.priority = V2X_EMULATOR_THREAD_PRIORITY; 
  emulator_config.sched_params.time_slice = V2X_EMULATOR_TIME_SLICE; 

#ifndef __CRATON_NO_ARC    

  emulator_config.imq_address = V2X_EMULATOR_IMQ_ADDRESS; 
  
#else 

  /* Initiate the slave part of the emulator - emulator<->v2x stack*/
  rc = v2x_emulator_slave_init(&emulator_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_receive: %s\n", atlk_rc_to_str(rc));
    return;
  }

#endif /* __CRATON_NO_ARC */ 

  /* Initiate the master part of the emulator - emulator<->external I/F */
  rc = v2x_emulator_master_init(&v2x_emulator, &emulator_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "Failed to initialize V2X Emulator master (rc=%d)\n", rc); 
    return; 
  }

  /* Get CRATON UART CLI instance */
  rc = cli_instance_get(&cli, CLI_INSTANCE_TYPE_UART);
  if (atlk_error(rc)) {
    fprintf(stderr, "Failed to get cli instance (rc=%d)\n", rc);
    return;
  }

  /* Register example command */
  command = cli_register_command(cli, NULL, "v2x_emulator_start", 
                                 cmd_example_v2x_emulator,
                                 PRIVILEGE_UNPRIVILEGED, MODE_ANY,
                                 "Start example of v2x_emulator");
  if (command == NULL) {
    fprintf(stderr, "Failed to register cli command (returned NULL)\n");
    return;
  }
  
  printf("To run v2x_emulator example >>\n%s \n\n\n", V2X_EMULATOR_CLI_HELP);
}

