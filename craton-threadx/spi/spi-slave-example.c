/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>

#include <tx_api.h>

#include <atlk/sdk.h>
#include <craton/gpio_driver.h>
#include <craton/spi_driver.h>
#include <craton/cache.h>

#include "spi-common.h"

/*
  CRATON ThreadX SPI Slave Example

  This example demonstrates basic usage of SPI for code running on
  top of CRATON processor with ThreadX RTOS.

  The example demonstrates how to send and receive data via SPI.

  The flow of the example is:
  1. Master sends a message to slave.
  2. Slave compares the message to expected message, if message is valid
     print success message to console else, prints error message to console.
  3. Master and salve set second transaction parameters,
     master waits for slave for predefined period, and initiate transaction,
     in which master receives from slave.
  4. Master compares the message to expected message, if message is valid
     print success message to console else, print error message to console.

  NOTE: Slave should start before master time out.

 */


/* SPI slave device */
spi_device_t *spi_slave_dev;
/* SPI slave sempahore */
TX_SEMAPHORE spi_slave_semaphore;
/* SPI slave thread */
TX_THREAD spi_slave_thread;
/* SPI slave thread stack */
uint8_t spi_slave_thread_stack[4096];

static void
spi_slave_wake_up(void)
{
  ULONG trv = tx_semaphore_put(&spi_slave_semaphore);
  assert(trv == TX_SUCCESS);
}

static void
spi_slave_sleep(void)
{
  ULONG trv = tx_semaphore_get(&spi_slave_semaphore, TX_WAIT_FOREVER);
  assert(trv == TX_SUCCESS);
}

static void
spi_slave_init_spi(void)
{
  spi_config_t spi_cfg = SPI_CONFIG_INIT;
  atlk_rc_t rc = ATLK_OK;

  spi_cfg.device_id = 0;
  spi_cfg.device_mode = SPI_MODE_SLAVE;
  spi_cfg.data_bits = 8;
  spi_cfg.tx_dma_channel = 1;
  spi_cfg.rx_dma_channel = 0;
  spi_cfg.clock_polarity = SPI_CLOCK_POLARITY_IDLE_LOW;
  spi_cfg.clock_phase = SPI_CLOCK_PHASE_1ST_EDGE;

  rc = spi_device_init(&spi_cfg, &spi_slave_dev);
  if (atlk_error(rc)) {
    fprintf(stderr, "spi_device_init: %s\n", atlk_rc_to_str(rc));
  }

  return;
}

static void
spi_slave_callback(spi_dma_transfer_t *transfer)
{
  (void)transfer;
  spi_slave_wake_up();
}

void
spi_slave_test(void)
{
  atlk_rc_t rc = ATLK_OK;
  spi_dma_transfer_t transfer = SPI_DMA_TRANSFER_INIT;
  char cache_line_aligned msg_tx[CACHE_LINE_SIZE];
  char cache_line_aligned msg_rx[CACHE_LINE_SIZE];

  transfer.device_ptr = spi_slave_dev;

  /* set buffers */
  strncpy(msg_tx, EXAMPLE_SPI_MSG_SLAVE_2_MASTER, sizeof(msg_tx) - 1);
  memset(msg_rx, 0, sizeof(msg_rx));

  /* set transaction parameters */
  transfer.tx_buffer_ptr = NULL;
  transfer.tx_buffer_size = 0;
  transfer.rx_buffer_ptr = msg_rx;
  transfer.rx_buffer_size = sizeof(msg_rx);
  transfer.data_size = sizeof(EXAMPLE_SPI_MSG_MASTER_2_SLAVE);
  transfer.completion_handler = spi_slave_callback;

  /* Start transaction */
  rc = spi_dma_transfer_start(&transfer);
  if (atlk_error(rc)) {
    fprintf(stderr, "spi_dma_transfer_start: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Wait for DMA to end transfer */
  spi_slave_sleep();

  if (strcmp(msg_rx, EXAMPLE_SPI_MSG_MASTER_2_SLAVE) != 0) {
    printf("Wrong! Message has been received from Slave: %s expected:%s \n",
        msg_rx, EXAMPLE_SPI_MSG_MASTER_2_SLAVE);
  }
  else {
    printf("Message received from master: %s\n", msg_rx);
  }

  /* set transaction parameters */
  transfer.tx_buffer_ptr = msg_tx;
  transfer.tx_buffer_size = sizeof(msg_tx);
  transfer.rx_buffer_ptr = NULL;
  transfer.rx_buffer_size = 0;
  transfer.data_size = sizeof(EXAMPLE_SPI_MSG_SLAVE_2_MASTER);
  transfer.completion_handler = spi_slave_callback;

  /* Start transaction */
  rc = spi_dma_transfer_start(&transfer);
  if (atlk_error(rc)) {
    fprintf(stderr, "spi_dma_transfer_start: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Wait for DMA to end transfer */
  spi_slave_sleep();

}

static void
spi_slave_thread_entry(ULONG opaque)
{
  (void)opaque;

  spi_slave_init_spi();

  spi_slave_test();
}

void
craton_user_init(void)
{
  ULONG trv;

  trv = tx_semaphore_create(&spi_slave_semaphore, "SPI slave semaphore", 0);
  assert(trv == TX_SUCCESS);

  trv = tx_thread_create(&spi_slave_thread, "SPI slave thread",
                         spi_slave_thread_entry, 0,
                         spi_slave_thread_stack, sizeof(spi_slave_thread_stack),
                         5, 5, TX_NO_TIME_SLICE, TX_AUTO_START);
  assert(trv == TX_SUCCESS);

  return;
}
