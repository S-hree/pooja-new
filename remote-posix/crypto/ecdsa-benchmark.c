/* Copyright (C) 2013-2016 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

#include <atlk/sdk.h>
#include <atlk/remote.h>
#include <atlk/ecc_remote.h>
#include <atlk/ecc_service.h>

/*
  CRATON Remote ECDSA Benchmark

  This program benchmarks CRATON ECDSA verification API with some of the
  elliptic curves supported by the API. The same verification request is
  used over and over for each curve. All verification are intended to result
  in success. This is checked using assert().

  To take advantage of CRATON HW parallelism the average number of
  ongoing requests (i.e. started but not completed) should be more than 1.
  Having more than 3 ongoing requests will not result in any throughput gains
  but is supported up to an implementation-defined upper bound. For reference,
  in SDK 4.5 this upper bound is 256.
*/

/* Compile-time number of elements in array */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Total number of requests per benchmark */
#define NUM_REQUESTS 10000

/* Max round trip time in usec */
#define EXAMPLE_MAX_RTT_USEC 100000

/*
  Number of incomplete requests at any point in time.
  CRATON ECDSA HW will provide less than maximum throughput
  if this number is less than 3.
*/
#define NUM_ONGOING 8

/* Is valid response? */
#define VALID_RESPONSE(last_received, last_sent) (last_received <= last_sent)

/* Benchmark descriptor */
struct benchmark {
  const char *curve_name;
  ecc_request_t request;
};

/* Remote transport object */
remote_transport_t *transport = NULL;

/* Array of benchmark descriptors (one per curve) */
static const struct benchmark benchmarks[] = {
  {
    .curve_name = "NIST P256",
    .request =  {
      .context = {
        .request_id = 0,
        .request_type = ECC_REQUEST_TYPE_VERIFY, 
        .curve = ECC_CURVE_NIST_P256
      },
      .params.verify_params = {
        .public_key = {
        	.point_type = ECC_POINT_UNCOMPRESSED,
        	.x_coordinate = {
        	  .value = {
        	    0xbc3fdd5d, 0x620d0a14, 0x5d867d8b, 0x286867ec,
        	    0x92c47d90, 0x8a772d43, 0x44eb3895, 0x26f3751e
        	  }
        	},
        	.y_coordinate = {
        	  .value = {
        	    0x96fc56f1, 0xf79baeaa, 0xff5b3542, 0xb7ffb678,
        	    0xc22d9ddb, 0x3dc0cb4d, 0xf0e24af5, 0x1606db3b
        	  }
  	      }
        },
        .digest = {
        	.value = {
        	  0xb9, 0x3d, 0x12, 0xb2, 0xc6, 0x02, 0x7b, 0x0b,
        	  0xa4, 0xd4, 0xd8, 0xc2, 0xbc, 0x20, 0xda, 0x88,
        	  0x8b, 0xe2, 0x42, 0x2f, 0x08, 0x9b, 0xe3, 0x24,
        	  0x3a, 0x6c, 0x44, 0xe5, 0x0d, 0xde, 0xf0, 0xcb
        	},
	        .value_size = 32
        },
        .signature = {
        	.r_scalar = {
        	  .value = {
        	    0x4e3a775c, 0x71a5c259, 0xfad57a8e, 0xd1e45591,
        	    0x030fbb65, 0x94d2300b, 0x7ceccd7d, 0xbc70ad36
        	  }
        	},
        	.s_scalar = {
        	  .value = {
        	    0xbc05d39c, 0xd2c5f32b, 0xf10502c6, 0xb91de10c,
        	    0x8599d089, 0x0873e8ae, 0x7b137225, 0xd51dd454
        	  }
        	}
        }
      }
    }
  },

  {
    .curve_name = "Brainpool P256t1",
    .request = {
      .context = {
        .request_id = 0,
        .request_type = ECC_REQUEST_TYPE_VERIFY, 
        .curve = ECC_CURVE_BRAINPOOL_P256t1
      },
      .params.verify_params = {
        .public_key = {
        	.point_type = ECC_POINT_UNCOMPRESSED,
        	.x_coordinate = {
        	  .value = {
        	    0xfc7fc794, 0x3a85ed28, 0xc3ebcaaf, 0x2a326938,
        	    0xef9ed9c7, 0x779df5c2, 0x6e220a95, 0x6344dff5
        	  }
        	},
        	.y_coordinate = {
        	  .value = {
        	    0x365babff, 0xf6cfc69f, 0xc2a9f394, 0x94cd22bf,
        	    0x46cbf110, 0x273452ae, 0xf55a41f3, 0x2e2e94a8
        	  }
        	}
        },
        .digest = {
        	.value = {
        	  0x8e, 0x89, 0x03, 0x45, 0x87, 0x5b, 0xef, 0x0b,
        	  0xaa, 0xa0, 0xe0, 0x98, 0xbf, 0xf2, 0x78, 0xdd,
        	  0xbf, 0x00, 0xee, 0x06, 0xcc, 0x08, 0x07, 0xa9,
        	  0xd8, 0xf6, 0x4c, 0x93, 0x29, 0xb0, 0xd2, 0x2d
        	},
        	.value_size = 32
        },
        .signature = {
        	.r_scalar = {
        	  .value = {
        	    0x15a73647, 0xb0ed3efa, 0x6f44c325, 0x7607b1a5,
        	    0xa06cf2a1, 0xc5f298a9, 0x13c2c3bc, 0x9168331f
        	  }
        	},
        	.s_scalar = {
        	  .value = {
        	    0xec7d28a2, 0x396dbb17, 0xbfc33ae6, 0xf0832dd6,
        	    0x2adf90bb, 0x4b422130, 0x46ad044f, 0x353f89ca
        	  }
        	}
        }
      }
    }
  }
};

static ecc_service_t *ecc_benchmark_service = NULL;
static ecc_socket_t *ecc_benchmark_socket = NULL;

static void
run_benchmark(const struct benchmark *benchmark);

void ecdsa_benchmark_start(unsigned int input);
void ecdsa_latency_benchmark_start(unsigned int input);

int main(int argc, char *argv[])
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* Remote IP transport configuration parameters */
  remote_ip_transport_config_t config = REMOTE_IP_TRANSPORT_CONFIG_INIT;
  /* Local IPv4 address */
  uint32_t local_ipv4_addr;
  /* Server IPv4 address */
  uint32_t server_ipv4_addr;

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
  config.max_rtt_ms = 20;

  /* Create remote IP transport object */
  rc = remote_ip_transport_create(&config, &transport);
  if (atlk_error(rc)) {
    fprintf(stderr, "remote_ip_transport_create: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  /* Create the ECC service */
  rc = ecc_remote_service_create(transport, NULL, &ecc_benchmark_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "ecc_remote_service_create: %s\n", atlk_rc_to_str(rc));
    goto exit;
  }

  /* Create socket */
  rc = ecc_socket_create(ecc_benchmark_service, &ecc_benchmark_socket);
  assert(!atlk_error(rc));

  /* start throughput test */
  ecdsa_benchmark_start(0);

  /* Start latency test */
  ecdsa_latency_benchmark_start(0);

exit:
  /* Clean-up resources */
  ecc_socket_delete(ecc_benchmark_socket);
  ecc_service_delete(ecc_benchmark_service);
  remote_transport_delete(transport);

  return atlk_error(rc);
}

static void
timestamp_get(struct timespec *ts)
{
  int rv;

  rv = clock_gettime(CLOCK_MONOTONIC, ts);
  if (rv != 0) {
    printf("Error getting the current time: rv = %d", rv);
  }
}

static void
timestamp_delta_calc(const struct timespec *new,
                     const struct timespec *old,
                     int32_t *delta_ms)
{
  if ((new->tv_nsec - old->tv_nsec) < 0) {
    *delta_ms = (new->tv_sec - old->tv_sec - 1) * 1000;;
    *delta_ms += (1000000000 + new->tv_nsec - old->tv_nsec) / 1000000;
  }
  else {
     *delta_ms = (new->tv_sec - old->tv_sec) * 1000;
     *delta_ms += (new->tv_nsec - old->tv_nsec) / 1000000;
  }
}

static void
send_fake_request(const struct benchmark *benchmark)
{
  atlk_rc_t rc;
  ecc_request_t request;
  ecc_response_t response;
  atlk_wait_t wait = ATLK_WAIT_INIT;
  
  /* Send 1 fake request in order to update the Host ARP table */
  /* Setup a long wait */
  wait.wait_type = ATLK_WAIT_INTERVAL;
  wait.wait_usec = 1000000;
  request = benchmark->request;
  /* Set a high request_id for the fake packet */
  request.context.request_id = NUM_REQUESTS - 1;
  rc = ecc_request_send(ecc_benchmark_socket, &request, NULL);
  assert(!atlk_error(rc));
  /* Wait for the response from the target, for 1 second */
  usleep(100000);
  /* Read the response */
  rc = ecc_response_receive(ecc_benchmark_socket, &response, &wait);
  if (rc != ATLK_E_TIMEOUT) {    
    assert(!atlk_error(rc));
  }
}

/* Buffer for sent timestamps */
static struct timespec packet_sent_timestamp[NUM_REQUESTS];

/* Buffer for storing all round trip times for each request */
static int32_t packet_rtt_ms[NUM_REQUESTS];

/** requests received */
static int request_id_received[NUM_REQUESTS];

static void
run_latency_benchmark(const struct benchmark *benchmark)
{
  atlk_rc_t rc;
  ecc_response_t response = ECC_RESPONSE_INIT;
  struct timespec ts_end;
  int32_t num_received;
  int32_t num_timeouts;
  int32_t num_sent;
  int32_t average_packet_rtt_ms;
  int32_t max_packet_rtt_ms;
  ecc_request_t request = ECC_REQUEST_INIT;
  struct timespec ts_now;
  int i;
  atlk_wait_t wait = ATLK_WAIT_INIT;

  memset(request_id_received, 0x00, sizeof(request_id_received));
  memset(packet_rtt_ms, 0x00, sizeof(packet_rtt_ms));
  memset(packet_sent_timestamp, 0x00, sizeof(packet_sent_timestamp));

  /* Start benchmark */
  printf("Benchmarking latency of ECDSA verification with curve \"%s\"...\n",
         benchmark->curve_name);
  
  /* Setup wait, not forever */
  wait.wait_type = ATLK_WAIT_INTERVAL;
  wait.wait_usec = EXAMPLE_MAX_RTT_USEC;

  /* Start a few requests to take advantage of HW parallelism */
  num_sent = 0;
  request = benchmark->request;
  request.context.request_id = 0;
  for (i = 0; i < NUM_ONGOING; i++) {
    timestamp_get(&packet_sent_timestamp[request.context.request_id]);
    rc = ecc_request_send(ecc_benchmark_socket, &request, NULL);
    assert(!atlk_error(rc));
    num_sent++;
    request.context.request_id++;
  }

  /* Start a new request whenever an ongoing request completes */
  num_received = 0;
  num_timeouts = 0;
  for (i = 0; i < NUM_REQUESTS - NUM_ONGOING; i++) {
    rc = ecc_response_receive(ecc_benchmark_socket, &response, &wait);
    if (rc != ATLK_E_TIMEOUT) {
      assert(!atlk_error(rc));
      assert(response.rc == ECC_OK);
      num_received++;

      if (VALID_RESPONSE(response.context.request_id, request.context.request_id)) {
        request_id_received[response.context.request_id] = 1;
      }
    }
    else {
      num_timeouts++;
    }

    if (VALID_RESPONSE(response.context.request_id, request.context.request_id)) {
      timestamp_get(&ts_now);
      timestamp_delta_calc(&ts_now,
                           &packet_sent_timestamp[response.context.request_id],
                           &packet_rtt_ms[response.context.request_id]);
    }

    timestamp_get(&packet_sent_timestamp[request.context.request_id]);
    rc = ecc_request_send(ecc_benchmark_socket, &request, NULL);
    assert(!atlk_error(rc));

    request.context.request_id++;
    num_sent++;
  }

  /* Wait for all ongoing requests to complete */
  while ((num_received + num_timeouts) < NUM_REQUESTS) {
    rc = ecc_response_receive(ecc_benchmark_socket, &response, &wait);
    if (rc != ATLK_E_TIMEOUT) {
      assert(!atlk_error(rc));
      assert(response.rc == ECC_OK);
      num_received++;

      if (VALID_RESPONSE(response.context.request_id, request.context.request_id)) {
        request_id_received[response.context.request_id] = 1;
      }
    }
    else {
      num_timeouts++;
    }

    timestamp_get(&ts_now);
    if (VALID_RESPONSE(response.context.request_id, request.context.request_id)) {
      timestamp_delta_calc(&ts_now,
                           &packet_sent_timestamp[response.context.request_id],
                           &packet_rtt_ms[response.context.request_id]);
    } 
  }

  timestamp_get(&ts_end);

  /* Calculate max and average delta in msec */
  max_packet_rtt_ms = 0;
  average_packet_rtt_ms = 0;
  for (i = 0; i < NUM_REQUESTS; i++) {
    if (request_id_received[i]) {
      if (max_packet_rtt_ms < packet_rtt_ms[i]) {
        max_packet_rtt_ms = packet_rtt_ms[i];
      }

      average_packet_rtt_ms += packet_rtt_ms[i];
    }
  }

  average_packet_rtt_ms /= num_received;

  printf("Latency of ECDSA verification with curve \"%s\": "
         "max = %d ms, avg = %d ms\n",
         benchmark->curve_name,
         (int)max_packet_rtt_ms,
         (int)average_packet_rtt_ms);
}

static void
run_benchmark(const struct benchmark *benchmark)
{
  atlk_rc_t rc;
  ecc_response_t response = ECC_RESPONSE_INIT;
  struct timespec ts_start;
  struct timespec ts_end;
  int32_t elapsed_time_ms;
  size_t num_received;
  size_t num_timeouts;
  size_t num_sent;
  ecc_request_t request = ECC_REQUEST_INIT;
  atlk_wait_t wait = ATLK_WAIT_INIT;

  memset(request_id_received, 0x00, sizeof(request_id_received));
  
  /* Send 1 fake request in order to update the Host ARP table */
  send_fake_request(benchmark);

  /* Setup wait, not forever */
  wait.wait_type = ATLK_WAIT_INTERVAL;
  wait.wait_usec = EXAMPLE_MAX_RTT_USEC;

  /* Start benchmark */
  printf("Benchmarking ECDSA verification with curve \"%s\"...\n",
         benchmark->curve_name);

  timestamp_get(&ts_start);

  /* Start a few requests to take advantage of HW parallelism */
  num_sent = 0;
  num_timeouts = 0;
  request = benchmark->request;
  request.context.request_id = 0;

  for (int i = 0; i < NUM_ONGOING; i++) {
    rc = ecc_request_send(ecc_benchmark_socket, &request, NULL);
    assert(!atlk_error(rc));
    num_sent++;
    request.context.request_id++;
  }

  /* Start a new request whenever an ongoing request completes */
  num_received = 0;
  for (int i = 0; i < NUM_REQUESTS - NUM_ONGOING; i++) {
    rc = ecc_response_receive(ecc_benchmark_socket, &response, &wait);
    if (rc != ATLK_E_TIMEOUT) {
      assert(!atlk_error(rc));
      assert(response.rc == ECC_OK);
      num_received++;

      request_id_received[response.context.request_id] = 1;
    }
    else {
      num_timeouts++;
    }

    rc = ecc_request_send(ecc_benchmark_socket, &request, NULL);
    assert(!atlk_error(rc));

    request.context.request_id++;
    num_sent++;
  }

  /* Wait for all ongoing requests to complete */
  while ((num_received + num_timeouts) < NUM_REQUESTS) {
    rc = ecc_response_receive(ecc_benchmark_socket, &response, &wait);
    if (rc != ATLK_E_TIMEOUT) {
      assert(!atlk_error(rc));
      assert(response.rc == ECC_OK);
      num_received++;

      request_id_received[response.context.request_id] = 1;
    }
    else {
      num_timeouts++;
    }
  }

  timestamp_get(&ts_end);

  /* Calculate benchmark */
  elapsed_time_ms = (ts_end.tv_sec - ts_start.tv_sec) * 1000;
  elapsed_time_ms += ((ts_end.tv_nsec - ts_start.tv_nsec) / 1000000);

  printf("ECDSA verification throughput with curve \"%s\" is %.1f Hz\n",
         benchmark->curve_name,
         (float)num_received / ((float)elapsed_time_ms / 1000));
}

void ecdsa_benchmark_start(unsigned int input)
{
  (void)input;

  printf("*** Start of ECDSA benchmark suite ***\n");

  for (size_t i = 0; i < ARRAY_SIZE(benchmarks); i++) {
    run_benchmark(&benchmarks[i]);
  }

  printf("*** End of ECDSA benchmark suite ***\n");
}

void ecdsa_latency_benchmark_start(unsigned int input)
{
  (void)input;

  printf("*** Start of ECDSA Latency benchmark suite ***\n");

  for (size_t i = 0; i < ARRAY_SIZE(benchmarks); i++) {
    run_latency_benchmark(&benchmarks[i]);
  }

  printf("*** End of ECDSA benchmark suite ***\n");
}
