/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>

#include <tx_api.h>
#include <tx_posix.h>

#include <libcli.h>

#include <atlk/sdk.h>

#include <craton/uart_driver.h>
#include <craton/cli.h>

/* TODO: This example is a draft -- requires simplification */

/*
  CRATON Bluetooth device connected via SPI2UART Example

  This example demonstrates basic usage of the Bluetooth device over SPI2UART
  for code running on top of CRATON processor with ThreadX RTOS.

  The example demonstrates how to initialize, send and receive data
  via SPI2UART to/from the Bluetooth device.

  The flow of the example is:
  3 CLI commands are initialized at power-up:
    * bt_init - Used to initialize the Bluetooth device.
    * bt_tx   - Used to send a data to the Bluetooth device.
    * bt_rx   - Used to receive data from the Bluetooth device.

  NOTE: Host should initiate the pairing (binding) and connect to the CRATON
        over Bluetooth following CRATON power-up.
*/

/* BT device command strings */
#define BT_DEVICE_MAX_CMD_STRING_SIZE 256
/* BT device response strings */
#define MAX_BT_RESPONSE_STRLEN 256

/* SPBT2632C2A Bluetooth chip AT commands */
#define BT_ENTER_COMMAND_MODE                 "^#^$^%"
#define BT_ENTER_CMD_EXPECTED_RESPONSE        "AT-AB -CommandMode-"
#define BT_CHANGE_BAUDRATE_GENERIC_CMD        "AT+AB ChangeBaud"
#define BT_CHANGE_BAUDRATE_EXPECTED_RESPONSE  "AT-AB Baudrate Changed"
#define BT_FLOW_CONTROL_ENABLE_CMD            "AT+AB StreamingSerial Disable"
#define BT_FLOW_CONTROL_ENABLE_EXPECTED_RESP  "AT-AB StreamingSerial Disabled"

#define CLI_PRINT(fmt, ...) cli_print(cli, fmt, ## __VA_ARGS__)

/* Global */
/* File descriptor for BT device */
static int serial_fd;
static bool b_bt_device_initialized = false;

/* BT Command buffer */
char g_bt_cmd_str[BT_DEVICE_MAX_CMD_STRING_SIZE];
/* BT change baudrate command buffer */
char g_bt_cmd_change_baudrate_str[BT_DEVICE_MAX_CMD_STRING_SIZE];
/* BT response buffer */
static char g_bt_response_buf[MAX_BT_RESPONSE_STRLEN];

/* BT device name */
static char *uart_name = "/dev/uart2";

/* Definitions */
typedef struct {
  char *request;
  char *expected_response;
} bluetooth_device_init_script_req_reply_t;

/* Global */
/*
 * SPBT2632C2A BT device Init script:
 * =================================
 * bt_tx ^#^$^%
 * bt_rx 256
 * bt_tx AT+AB ChangeBaud 921600
 * bt_rx 256
 * bt_uart_hw_baudrate_change 921600
 * */
bluetooth_device_init_script_req_reply_t g_bt_SPBT2632C2A_init_script[] = {
    { BT_ENTER_COMMAND_MODE, BT_ENTER_CMD_EXPECTED_RESPONSE },
    { BT_FLOW_CONTROL_ENABLE_CMD, BT_FLOW_CONTROL_ENABLE_EXPECTED_RESP },
    { g_bt_cmd_change_baudrate_str, BT_CHANGE_BAUDRATE_EXPECTED_RESPONSE }
};

static int cmd_bt_device_init(struct cli_def *cli,
                              const char *command,
                              char *argv[],
                              int argc);

static int cmd_bt_tx(struct cli_def *cli,
                     const char *command,
                     char *argv[],
                     int argc);

static int cmd_bt_rx(struct cli_def *cli,
                     const char *command,
                     char *argv[],
                     int argc);

static void print_hex(struct cli_def *cli, char *buf, unsigned int size);

static ssize_t bt_response_handle(struct cli_def *cli,
                                  int fd,
                                  char *rx_buf,
                                  uint32_t size);

static atlk_rc_t bt_device_init(struct cli_def *cli, unsigned int baudrate)
{
  int ret;
  int size;
  int num_commands;
  int i;
  int size_to_read;
  ssize_t len;
  atlk_rc_t rc = ATLK_OK;

  /* Prepare baudrate change BT AT command */
  ret = snprintf(g_bt_cmd_change_baudrate_str,
                  sizeof(g_bt_cmd_change_baudrate_str),
                  "%s %d",
                  BT_CHANGE_BAUDRATE_GENERIC_CMD,
                  baudrate);
  if (ret < 0) {
    CLI_PRINT("%s", "bt_device_init(): Error 1");
    return ATLK_E_UNSPECIFIED;
  }

  num_commands = sizeof(g_bt_SPBT2632C2A_init_script) /
                 sizeof(bluetooth_device_init_script_req_reply_t);

  CLI_PRINT("Starting BT initialization script, num_commands = %d\n",
              num_commands);

  /* Send initialization script and check responses from BT device */
  for (i = 0; i < num_commands; i++)
  {
    if (0 == strcmp(BT_ENTER_COMMAND_MODE,
                    g_bt_SPBT2632C2A_init_script[i].request)) {
      size = snprintf(g_bt_cmd_str,
                       sizeof(g_bt_cmd_str),
                       "%s",
                       g_bt_SPBT2632C2A_init_script[i].request);
    }
    else {
      /* Need '\n' at the end to issue the AT-command to BT device */
      size = snprintf(g_bt_cmd_str,
                       sizeof(g_bt_cmd_str),
                       "%s\r\n",
                       g_bt_SPBT2632C2A_init_script[i].request);
    }

    CLI_PRINT("[%d] Sending BT command: %s\n", i, g_bt_cmd_str);

    len = write(serial_fd, g_bt_cmd_str, size);
    if (len < 0) {
      CLI_PRINT("Error! Failed to write to BT, len = %d\n", len);
    }
    else {
      CLI_PRINT("BT command successfully written to device, len = %d",
                 len);
    }

    /* Wait for BT device to respond, it takes some time for the BT device
     * to respond and send the response to the SPI2UART device...
     * It takes about ~0.009sec == 9msec ((1/115200)*9*128) to write 128
     * bytes at 115200 (the slowest UART baudrate) ==>
     * so we wait 20msec. */
    usleep(20000);

    /* Handle response */
    size = strlen(g_bt_SPBT2632C2A_init_script[i].expected_response);

    /* The read API of the SPI2UART driver is non-blocking ==>
     * We read up to 256 bytes but if only 10 bytes are received at the
     * time of reading it will not block.
     */
    size_to_read = MAX_BT_RESPONSE_STRLEN;

    CLI_PRINT("[%d] Expecting BT response: %s , size = %d, "
              "size_in_rx_fifo=%d\n",
               i,
               g_bt_SPBT2632C2A_init_script[i].expected_response,
               size,
               size_to_read);

    /* Read response from UART */
    len = bt_response_handle(cli, serial_fd, g_bt_cmd_str, size_to_read);
    g_bt_cmd_str[size] = (char )0x00;

    CLI_PRINT("BT response: %s len=%d...    ", g_bt_cmd_str, len);
    if (0 != strcmp(g_bt_cmd_str,
                    g_bt_SPBT2632C2A_init_script[i].expected_response))
    {
      CLI_PRINT("%s", "Error! BT response is not equal to Expected_response.\n");
      CLI_PRINT("%s", "Aborting...\n");
      return ATLK_E_UNSPECIFIED;
    }
    else {
      CLI_PRINT("%s", "OK\n");
    }
  }

  /* Now it is OK to change the baudrate of the SPI2UART device */
  rc = uart_speed_set(UART_DEVICE_ID_2, baudrate);
  if (atlk_error(rc)) {
    CLI_PRINT("Error! Failed to change UART baudrate, rc = %d\n", rc);
    return rc;
  }

  CLI_PRINT("%s", "BT initialization sequence successful.\n");

  return rc;
}

static void print_hex(struct cli_def *cli, char *buf, unsigned int size)
{
  const unsigned int line_size_hex_bytes = 8;
  unsigned int i;
  unsigned int j;
  unsigned int max;
  unsigned int delta;
  char hex_output_buf[line_size_hex_bytes * 8];

  i = 0;
  while (i < size)
  {
    max = line_size_hex_bytes;
    delta = size - i;
    max = (delta < 8) ? delta : line_size_hex_bytes;

    /* Empty string */
    hex_output_buf[0] = 0x00;
    for (j = 0; j < max; j++)
    {
      snprintf(hex_output_buf,
                sizeof(hex_output_buf),
                "%s %02x",
                hex_output_buf,
                buf[i + j]);
    }

    CLI_PRINT("%s", hex_output_buf);

    i += max;
  }

}

static ssize_t bt_response_handle(struct cli_def *cli,
                                  int fd,
                                  char *rx_buf,
                                  uint32_t size)
{
  ssize_t len = 0;

  len = read(fd, rx_buf, (ssize_t)size);
  if (len < 0) {
    fprintf(stderr, "Error! Failed to read from BT");
    return -1;
  }

  CLI_PRINT("bt_response_handle(): BT receive: Len = %d\n data_rx:", (int)len);

  print_hex(cli, rx_buf, len);

  return len;
}

atlk_rc_t bt_spi2uart_example_cli_connect(int serial_file_desc)
{
  atlk_rc_t rc;
  unsigned int i;
  cli_instance_t *cli = NULL;

  /* BT device not initialized yet ... */
  b_bt_device_initialized = FALSE;

  /* Initialize file descriptor for SPI2UART */
  serial_fd = serial_file_desc;

  for (i = CLI_INSTANCE_TYPE_UART; i <= CLI_INSTANCE_TYPE_TELNET; i++)
  {
    /* Get CRATON UART CLI instance */
    rc = cli_instance_get(&cli, i);
    if (atlk_error(rc)) {
      return rc;
    }

    /* register bt_tx CMD */
    cli_register_command(cli, NULL, "bt_tx", cmd_bt_tx,
        PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Sends data over BT UART");

    /* register bt_rx CMD */
    cli_register_command(cli, NULL, "bt_rx", cmd_bt_rx,
           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Receives data from BT UART");

    /* register bt_spi2uart_example_bt_device_init CMD */
    cli_register_command(cli,
                          NULL,
                          "bt_device_init",
                          cmd_bt_device_init,
                          PRIVILEGE_UNPRIVILEGED, MODE_ANY,
                          "Initializes bluetooth device");
  }

  return ATLK_OK;
}


static int cmd_bt_device_init(struct cli_def *cli,
                              const char *command,
                              char *argv[],
                              int argc)
{
  atlk_rc_t rc;
  unsigned int baudrate;

  if (b_bt_device_initialized)
  {
    CLI_PRINT("%s", "Error! BT device already initialized...");
    return CLI_ERROR_ARG;
  }

  if ('?' == argv[0][0]) {
    CLI_PRINT("%s", command);
    return CLI_OK;
  }

  if (argc < 1) {
    CLI_PRINT("%s", "Initializes BT device\n"
              "usage: <baudrate>\n");
    return CLI_ERROR_ARG;
  }
  else {
    baudrate = atol(argv[0]);
    CLI_PRINT("baudrate = %d\n", baudrate);

    switch (baudrate) {
    case UART_SPEED_921600_BPS:
    case UART_SPEED_460800_BPS:
    case UART_SPEED_230400_BPS:
    case UART_SPEED_115200_BPS:
      break;
    default:
      CLI_PRINT("%s", "Error! invalid baudrate.");
      CLI_PRINT("%s", "Valid baudrates are: 921600/460800/230400/115200\n");
      return CLI_ERROR;
    }
  }

  rc = bt_device_init(cli, baudrate);
  if (atlk_error(rc)) {
    CLI_PRINT("Error! Failed to initialize the bluetooth device "
              "rc = %d\n", rc);
    return rc;
  }

  b_bt_device_initialized = TRUE;

  return CLI_OK;
}

static int cmd_bt_tx(struct cli_def *cli, const char *command,
                                char *argv[], int argc)
{
  ssize_t len;
  char *str_to_tx;
  int i;
  char str[256];

  if ('?' == argv[0][0]) {
    CLI_PRINT("%s", command);
    return CLI_OK;
  }

  if (argc < 1) {
    CLI_PRINT("%s", "bt_tx - Sends data to Bluetooth device\n"
                    "usage: bt_tx [string#1] [string#2] ... [string#N-1]");
    return CLI_ERROR_ARG;
  }
  else {
    if (argc > 1)
    {
      strcpy(str, argv[0]);
      for (i = 1; i < argc; i++)
      {
        snprintf(str, sizeof(str), "%s %s", str, argv[i]);
      }
      snprintf(str, sizeof(str), "%s\n", str);

      str_to_tx = str;
    }
    else {
        str_to_tx = argv[0];
    }

    CLI_PRINT("str_to_tx = %s\n", str_to_tx);
  }

  len = write(serial_fd, str_to_tx, strlen(str_to_tx));
  if (len < 0) {
    fprintf(stderr, "Failed to write to BT\n");
  }
  else {
    CLI_PRINT("bt_tx successfully written "
               "string=%s to BT UART, len=%d\n",
               str_to_tx,
               len);
  }

  return CLI_OK;
}

static int cmd_bt_rx(struct cli_def *cli, const char *command,
    char *argv[], int argc)
{
  int len;
  unsigned int size_to_rx;

  if ('?' == argv[0][0]) {
    CLI_PRINT("%s", command);
    return CLI_OK;
  }

  if (argc < 1) {
    CLI_PRINT("%s", "bt_rx - Receives data from Bluetooth device\n"
                    "usage: bt_rx [num_bytes_to_rx]");
    return CLI_ERROR_ARG;
  }
  else {
    size_to_rx = atol(argv[0]);
    CLI_PRINT("size_to_rx = %d\n", size_to_rx);
    if (size_to_rx > MAX_BT_RESPONSE_STRLEN)
    {
      CLI_PRINT("Error! Max size_to_rx is %d\n", MAX_BT_RESPONSE_STRLEN);
      return CLI_ERROR_ARG;
    }
  }

  /* Read response from UART */
  len = bt_response_handle(cli, serial_fd, g_bt_response_buf, size_to_rx);
  g_bt_response_buf[len] = (char )0x00;
  CLI_PRINT("\nBT response: %s, len=%d\n", g_bt_response_buf, len);

  return CLI_OK;
}

atlk_rc_t bt_spi2uart_example_init(void)
{
  /* atlk return code */
  atlk_rc_t rc = ATLK_OK;

  printf("UART name is %s\n", uart_name);

  /* Open BT device */
  serial_fd = open(uart_name, 0);
  if (serial_fd < 0) {
    fprintf(stderr, "Failed to open \"%s\", rc=%d\n", uart_name, serial_fd);
    return ATLK_E_UNSPECIFIED;
  }

  /* Connect CLI */
  rc = bt_spi2uart_example_cli_connect(serial_fd);
  if (atlk_error(rc)) {
    fprintf(stderr, "Failed to connect to CRATON CLI\n");
    return ATLK_E_UNSPECIFIED;
  }

  return rc;
}

void craton_user_init(void)
{
  atlk_rc_t rc = ATLK_OK;

  printf("Initialize bt-spi2uart example ...");

  rc = bt_spi2uart_example_init();
  if (atlk_error(rc)) {
    fprintf(stderr,
            "bt-spi2uart example init failed: %s\n",
            atlk_rc_to_str(rc));
    return;
  }

  printf("OK\n");
}
