/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include <tx_api.h>

#include <craton/i2s_driver.h>
#include <craton/cache.h>

/* Example sample rate */
#define EXAMPLE_SAMPLE_RATE 20e3

/* DMA playback structure */
static i2s_dma_playback_t i2s_dma_playback = I2S_DMA_PLAYBACK_INIT;

/* Replay completion semaphore */
static TX_SEMAPHORE semaphore;

/* Audio input from example sine buffer */
static int8_t i2s_buf[1 << 16] cache_line_aligned;

/* Generate sound samples with a sine wave whose frequency changes from
   zero to 1KHz every second.
*/
static void
generate_sine(int8_t *samples, size_t num_samples)
{
  uint32_t i;

  for (i = 0; i < num_samples; i++) {
    double t0 = (double)i / EXAMPLE_SAMPLE_RATE;
    double t1 = sin(t0 * M_PI / 2) * 500;
    samples[i] = (int8_t)(sin((t1 * 2.0 * M_PI)) * INT8_MAX);
  }
}

static void
i2s_playback_done(i2s_dma_playback_t *playback)
{
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;
  /* Not using parameter */
  (void)playback;

  trv = tx_semaphore_put(&semaphore);
  assert(trv == TX_SUCCESS);
}

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* ThreadX return value */
  ULONG trv = TX_SUCCESS;

  /* Create a semaphore */
  trv = tx_semaphore_create(&semaphore, "i2s_example_semaphore", 0);
  assert(trv == TX_SUCCESS);

  /* Generate an audio sample buffer */
  generate_sine(i2s_buf, sizeof(i2s_buf));

  /* Set both stereo channels to play the same sample buffer */
  i2s_dma_playback.left_sample_buffer_ptr = i2s_buf;
  i2s_dma_playback.right_sample_buffer_ptr = i2s_buf;
  i2s_dma_playback.sample_buffer_size = sizeof(i2s_buf);

  /* Set i2s_playback_done() as the completion handler */
  i2s_dma_playback.completion_handler = i2s_playback_done;

  /* Start playback */
  rc = i2s_dma_playback_start(&i2s_dma_playback);
  if (atlk_error(rc)) {
    fprintf(stderr, "i2s_dma_playback_start: %s\n", atlk_rc_to_str(rc));
    return;
  }

  /* Wait for the operation to complete */
  trv = tx_semaphore_get(&semaphore, TX_WAIT_FOREVER);
  assert(trv == TX_SUCCESS);

  printf("I2S playback complete, status: %d\n",
         i2s_dma_playback.playback_status);
}
