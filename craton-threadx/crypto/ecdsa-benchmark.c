/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>

#include <tx_api.h>

#include <atlk/ecc_service.h>

#if defined __CRATON_NO_ARC || defined __CRATON_ARC1

/*
  CRATON ThreadX ECDSA Benchmark

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

/* Total number of requests per benchmark */
#define NUM_REQUESTS 10000

/*
  Number of incomplete requests at any point in time.
  CRATON ECDSA HW will provide less than maximum throughput
  if this number is less than 3.
*/
#define NUM_ONGOING 3

/* Priority of benchmark thread */
#define ECDSA_BENCHMARK_THREAD_PRIORITY 40

/* Benchmark descriptor */
struct benchmark {
  const char *curve_name;
  ecc_request_t request;
};

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

static ecc_service_t *service = NULL;
static ecc_socket_t *socket = NULL;
static TX_THREAD ecdsa_benchmark_thread;
static uint8_t ecdsa_benchmark_thread_stack[1 << 12];

static void
run_benchmark(const struct benchmark *benchmark)
{
  atlk_rc_t rc;
  ecc_response_t response;

  /* Start benchmark */
  printf("Benchmarking ECDSA verification with curve \"%s\"...\n",
     benchmark->curve_name);
  uint32_t start_time = tx_time_get();

  /* Start a few requests to take advantage of HW parallelism */
  for (int i = 0; i < NUM_ONGOING; i++) {
    rc = ecc_request_send(socket, &benchmark->request, NULL);
    assert(!atlk_error(rc));
  }

  /* Start a new request whenever an ongoing request completes */
  for (int i = 0; i < NUM_REQUESTS - NUM_ONGOING; i++) {
    rc = ecc_response_receive(socket, &response, &atlk_wait_forever);
    assert(!atlk_error(rc));
    assert(response.rc == ECC_OK);

    rc = ecc_request_send(socket, &benchmark->request, NULL);
    assert(!atlk_error(rc));
  }

  /* Wait for all ongoing requests to complete */
  for (int i = 0; i < NUM_ONGOING; i++) {
    rc = ecc_response_receive(socket, &response, &atlk_wait_forever);
    assert(!atlk_error(rc));
    assert(response.rc == ECC_OK);
  }

  /* Finish benchmark */
  int32_t elapsed_time = tx_time_get() - start_time;
  printf("ECDSA verification throughput with curve \"%s\" is %.1f Hz\n",
     benchmark->curve_name,
     (float)NUM_REQUESTS / ((float)elapsed_time / TX_TICK_RATE));
}

void ecdsa_benchmark_thread_entry(ULONG input)
{
  (void)input;

  printf("*** Start of ECDSA benchmark suite ***\n");

  for (size_t i = 0; i < sizeof(benchmarks) / sizeof(benchmarks[0]); i++) {
    run_benchmark(&benchmarks[i]);
  }

  printf("*** End of ECDSA benchmark suite ***\n");
}

void craton_user_init(void)
{
  UINT trv;
  atlk_rc_t rc;

  rc = ecc_default_service_get(&service);
  assert(!atlk_error(rc));

  rc = ecc_socket_create(service, &socket);
  assert(!atlk_error(rc));

  trv = tx_thread_create(&ecdsa_benchmark_thread,
                         "ecdsa_benchmark_thread",
                         ecdsa_benchmark_thread_entry, 0,
                         ecdsa_benchmark_thread_stack,
                         sizeof(ecdsa_benchmark_thread_stack),
                         ECDSA_BENCHMARK_THREAD_PRIORITY,
                         ECDSA_BENCHMARK_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);
}

#else /* __CRATON_NO_ARC || __CRATON_ARC1 */

void craton_user_init(void)
{
}

#endif /* __CRATON_NO_ARC || __CRATON_ARC1 */
