/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/http_server.h>

/*
  CRATON HTTP Example

  This example demonstrates basic usage of HTTP for code running on
  top of CRATON processor with ThreadX RTOS.

  The example demosntrates registration of HTTP module with couple of
  module functions to be invoked once HTTP POST/GET is issued
*/

#define ARRAY_COUNT(ARR) (sizeof(ARR) / sizeof(ARR[0]))

/** HTTP OK status code */
#define HTTP_STATUS_OK 200

/** HTTP server thread priority */
#define HTTP_SERVER_THREAD_PRIORITY 40

/** HTTP server thread time slice */
#define HTTP_SERVER_THREAD_TIME_SLICE 0

static void
print_info(const char *func_name,
           const char *resource,
           http_request_type_t request_type,
           const void *request_content,
           size_t request_content_size)
{
  printf("%s was called with resource %s\n", func_name, resource);
  printf("HTTP method is %d\n", request_type);
  if (request_content_size > 0) {
    size_t i;
    printf("HTTP request content is:\n");
    for (i = 0; i < request_content_size; i++) {
      printf("%c", ((char *)request_content)[i]);
    }
    printf("\n");
  }
  else {
    printf("No HTTP body content is available\n");
  }
}

atlk_rc_t
my_module_func1(const char *resource,
                http_request_type_t request_type,
                const void *request_content,
                size_t request_content_size,
                void *response_content,
                size_t *response_content_size,
                uint16_t *status_code)
{
  /** Unused parameter */
  (void)response_content;

  print_info(__func__,
             resource,
             request_type,
             request_content,
             request_content_size);

  /** Return HTTP OK status code */
  *status_code = HTTP_STATUS_OK;

  /** No response content to return */
  *response_content_size = 0;

  return ATLK_OK;
}

atlk_rc_t
my_module_func2(const char *resource,
                http_request_type_t request_type,
                const void *request_content,
                size_t request_content_size,
                void *response_content,
                size_t *response_content_size,
                uint16_t *status_code)
{
  /** Unused parameter */
  (void)response_content;

  print_info(__func__,
             resource,
             request_type,
             request_content,
             request_content_size);

  /** Return HTTP OK status code */
  *status_code = HTTP_STATUS_OK;

  /** No response content to return */
  *response_content_size = 0;

  return ATLK_OK;
}

static const http_url_entry_t example_entries[] = {
  {
    /** URL prefix must be identical to registered module name */
    .url = "my_module_func1",
    .url_handler = my_module_func1,
  },
  {
    /** URL prefix must be identical to registered module name */
    .url = "my_module_func2",
    .url_handler = my_module_func2,
  },
};

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;

  http_server_config_t config = HTTP_SERVER_CONFIG_INIT;

  /** Currently supported value for default_path_prefix is NULL */
  config.default_path_prefix = NULL;

  /* Set HTTP server scheduling parameters */
  config.sched_params.priority = HTTP_SERVER_THREAD_PRIORITY;
  config.sched_params.time_slice = HTTP_SERVER_THREAD_TIME_SLICE;

  rc = http_server_init(&config);
  if (atlk_error(rc)) {
    fprintf(stderr, "http_server_module_register: %s\n", atlk_rc_to_str(rc));
    return;
  }

  rc = http_server_module_register("my_module",
                                   example_entries,
                                   ARRAY_COUNT(example_entries));
  if (atlk_error(rc)) {
    fprintf(stderr, "http_server_module_register: %s\n", atlk_rc_to_str(rc));
    return;
  }

  printf("Module my_module is registered to HTTP server\n");

  return;
}
