/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>
#include <nx_api.h>

#include <craton/net.h>

/*
  CRATON NetX RAW Packet Receive Example

  This example demonstrates how to receive RAW frames over Ethernet using
  NetX-Duo API for code running on top of CRATON processor with ThreadX RTOS.

  A single thread is created in which frames are received in a loop. Frame
  content is dumped to console.

  @todo: This example is not currently supported in multi-core SDK.
*/

/* Example thread priority */
#define EXAMPLE_THREAD_PRIORITY 41

/* Ethernet interface index in trusted IP instance */
#define ETH_IF_INDEX 0

#if defined __CRATON_NO_ARC

/* Example thread */
static TX_THREAD example_thread;
static uint8_t example_thread_stack[0x1000];
static void example_thread_entry(ULONG input);

/* Pointer to trusted IP instance */
static NX_IP *trusted_instance = NULL;

static void
raw_example_print_buffer(const uint8_t *buf, size_t buf_len)
{
  size_t i;

  for (i = 0; i < buf_len; i++) {
    printf("%02x ", buf[i]);
  }
  printf("\n");
}

void craton_user_init(void)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;

  printf("NetX RAW packet receive example.\n");

  /* Get trusted IP instance */
  rc = net_ip_trusted_instance_get(&trusted_instance);
  if (atlk_error(rc)) {
    fprintf(stderr, "net_ip_trusted_instance_get: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Enable RAW packet support on trusted instance. Note that IPv4, ARP and
     IPv6 packets will not be received (they are processed internally by the
     NetX-Duo stack).
   */
  nrv = nx_raw_packet_enable(trusted_instance);
  assert(nrv == NX_SUCCESS);

  /* Create example thread */
  trv = tx_thread_create(&example_thread, "example_thread",
                         example_thread_entry, 0,
                         example_thread_stack,
                         sizeof(example_thread_stack),
                         EXAMPLE_THREAD_PRIORITY,
                         EXAMPLE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;
}

void example_thread_entry(ULONG input)
{
  /* NetX return value */
  ULONG nrv = NX_SUCCESS;
  /* Not using input */
  (void)input;

  while (1) {
    /* NetX packet */
    NX_PACKET *raw_packet = NULL;

    /* Receive a RAW packet (except for IPv4, ARP and IPv6 packets) */
    printf("Receiving RAW packet...\n");
    nrv = nx_raw_packet_receive(trusted_instance, ETH_IF_INDEX,
                                &raw_packet, TX_WAIT_FOREVER);
    assert(nrv == NX_SUCCESS);

    /* Print received packet content */
    printf("Received RAW packet content:\n");
    raw_example_print_buffer(raw_packet->nx_packet_prepend_ptr,
                             raw_packet->nx_packet_length);

    /* Release the packet */
    nx_packet_release(raw_packet);
  }
}

#else /* __CRATON_NO_ARC */

void craton_user_init(void)
{
}

#endif /* __CRATON_NO_ARC */
