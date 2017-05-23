/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/sdk.h>
#include <atlk/v2x.h>
#include <atlk/v2x_service.h>
#include <atlk/mib_service.h>
#include <atlk/mibs/wlan-mib.h>

/*
  CRATON ThreadX IEEE 1609.4 Channel Switching Example

  This example demonstrates basic usage of 1609.4 channel switching for
  code running on top of CRATON processor with ThreadX RTOS.

  Channel switching is configured such that channel A takes the role of CCH
  and channel B takes the role of SCH.
  
  Two sockets are created and bound to interface indexes associated with
  channel A and B (a virtual interface index in the later case).

  To simplify, this example sends the same frame on CCH and SCH every 20
  milliseconds in a single thread. Frames are physically transmitted when
  their channel is enabled, otherwise they are queued.
  
  @todo Note that sync tolerance is not currently implemented, i.e.
  when active, channel switching is enabled regardless of system time status.
*/

/* Example thread priority */
#define TX_THREAD_PRIORITY 40

#if defined __CRATON_NO_ARC || defined __CRATON_ARC1

/* TX thread */
static TX_THREAD tx_thread;
static uint8_t tx_thread_stack[0x1000];
static void tx_thread_entry(ULONG input);

/* V2X sockets */
static v2x_socket_t *v2x_socket_cch = NULL;
static v2x_socket_t *v2x_socket_sch = NULL;

/* CCH interface index used in this example */
#define IF_INDEX_CCH 1

/* Protocol identifier for example messages */
#define PROTO_ID 0x102ULL

/* CCH and SCH frequencies used in this example */
#define FREQ_MHZ_CCH 5860
#define FREQ_MHZ_SCH 5890

/* Example message format string: <ch>: Example <seq_num> */
static const char msg_fmt[] = "%s: Example %" PRIu32;

/* Example message string maximum length */
static const size_t msg_size_max = sizeof(msg_fmt) + 10;

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* V2X socket configuration */
  v2x_socket_config_t config = V2X_SOCKET_CONFIG_INIT;
  /* V2X service */
  v2x_service_t *v2x_service = NULL;
  /* MIB service */
  mib_service_t *mib_service = NULL;
  /* Channel switching state */ 
  mib_RowStatus_t state = MIB_RowStatus_notInService;
  /* SCH interface index */
  int32_t if_index_sch;

  /* Get default V2X service instance */
  rc = v2x_default_service_get(&v2x_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_default_service_get: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Get default MIB service instance */
  rc = mib_default_service_get(&mib_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set socket configuration for CCH */
  config.if_index = IF_INDEX_CCH;
  config.protocol.protocol_id = PROTO_ID;

  /* Create a V2X socket bound to CCH */
  rc = v2x_socket_create(v2x_service, &v2x_socket_cch, &config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Get SCH interface index */
  rc = mib_get_wlanCsIfIndexB(mib_service, IF_INDEX_CCH, &if_index_sch);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_get_wlanCsIfIndexB: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set socket configuration for SCH */
  config.if_index = (v2x_if_index_t)if_index_sch;
  config.protocol.protocol_id = PROTO_ID;

  /* Create a V2X socket bound to SCH */
  rc = v2x_socket_create(v2x_service, &v2x_socket_sch, &config);
  if (atlk_error(rc)) {
    fprintf(stderr, "v2x_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set CCH frequency */
  rc = mib_set_wlanCsFrequencyA(mib_service, IF_INDEX_CCH, FREQ_MHZ_CCH);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_wlanCsFrequencyA: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Set SCH frequency */
  rc = mib_set_wlanCsFrequencyB(mib_service, IF_INDEX_CCH, FREQ_MHZ_SCH);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_wlanCsFrequencyB: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Activate 1609.4 channel switching */
  rc = mib_set_wlanCsRowStatus(mib_service, IF_INDEX_CCH,
                               MIB_RowStatus_active);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_set_wlanCsRowStatus: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  while (state != MIB_RowStatus_active) {
    /* Wait for 1 millisecond and recheck if channel switching has started */ 
    usleep(1000);

    /* Get 1609.4 channel switching state */
    rc = mib_get_wlanCsRowStatus(mib_service, IF_INDEX_CCH,
                                 &state);
    if (atlk_error(rc)) {
      fprintf(stderr, "mib_get_wlanCsRowStatus: %s\n", atlk_rc_to_str(rc));
      goto error;
    }
  }

  /* Create TX thread */
  trv = tx_thread_create(&tx_thread, "tx_thread",
                         tx_thread_entry, 0,
                         &tx_thread_stack,
                         sizeof(tx_thread_stack),
                         TX_THREAD_PRIORITY,
                         TX_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;

error:
  /* Clean-up resources */
  mib_service_delete(mib_service);
  v2x_socket_delete(v2x_socket_cch);
  v2x_socket_delete(v2x_socket_sch);
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
    size_t size = 1 + snprintf(buf, sizeof(buf), msg_fmt, "CCH", msg_count);

    /* Transmit V2X PDU on CCH */
    rc = v2x_send(v2x_socket_cch, buf, size, &send_params, NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_send: %s\n", atlk_rc_to_str(rc));
      return;
    }

    /* Print message into buffer (with terminating \0) and update its size */
    size = 1 + snprintf(buf, sizeof(buf), msg_fmt, "SCH", msg_count);

    /* Transmit V2X PDU on SCH */
    rc = v2x_send(v2x_socket_sch, buf, size, &send_params, NULL);
    if (atlk_error(rc)) {
      fprintf(stderr, "v2x_send: %s\n", atlk_rc_to_str(rc));
      return;
    }

    /* Increment message count */
    msg_count++;

    /* Sleep 20 milliseconds between transmissions */
    usleep(20000);
  }
}

#else /* __CRATON_NO_ARC || __CRATON_ARC1 */

void craton_user_init(void)
{
}

#endif /* __CRATON_NO_ARC || __CRATON_ARC1 */
