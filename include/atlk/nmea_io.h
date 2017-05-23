/* Copyright (C) 2015-2016 Autotalks Ltd. */
#ifndef _ATLK_NMEA_IO_H
#define _ATLK_NMEA_IO_H

#include <atlk/sdk.h>
#include <atlk/os.h>
#include <atlk/uart.h>
#include <atlk/nmea.h>

#ifdef __cplusplus
extern "C" {
#endif

/** NMEA I/O direction */
typedef enum {
  /** Data was read from device */
  NMEA_IO_DIRECTION_INPUT = 0,

  /** Data was written to device */
  NMEA_IO_DIRECTION_OUTPUT,

} nmea_io_direction_t;

/**
   NMEA I/O data callback.

   @see ::nmea_io_input_callback_set and ::nmea_io_output_callback_set.

   @param[in] data I/O data
   @param[in] data_size Data size in bytes
   @param[in] direction Data direction
*/
typedef void
(*nmea_io_data_callback_t)(const void *data,
            size_t data_size,
            nmea_io_direction_t direction);

/** NMEA I/O status */
typedef struct {
  /** Count of bytes read via NMEA I/O */
  uint32_t rx_bytes_cnt;

  /** Count of valid NMEA sentences read via NMEA I/O */
  uint32_t rx_nmea_sentence_cnt;

  /** Count of invalid NMEA sentences read via NMEA I/O */
  uint32_t rx_nmea_sentence_error_cnt;

  /** Count of bytes written via NMEA I/O */
  uint32_t tx_bytes_cnt;

  /** Count of valid NMEA sentences written via NMEA I/O */
  uint32_t tx_nmea_sentence_cnt;

  /** Count of inavlid NMEA sentences requested to be written via NMEA I/O */
  uint32_t tx_nmea_sentence_error_cnt;

} nmea_io_stats_t;

/** NMEA I/O device name buffer */
#define NMEA_IO_DEV_NAME_SIZE 32

/** NMEA I/O object */
typedef struct {
  /** I/O file descriptor */
  int fd;

  /** I/O device name */
  char dev_name[NMEA_IO_DEV_NAME_SIZE];

  /**
     NMEA sentence handler.

     Callback is called each time a new valid NMEA sentence is available,
     when polling input is active.
  */
  nmea_sentence_handler_t handler;

  /** NMEA object */
  nmea_t *nmea;

  /** NMEA I/O input data callback */
  nmea_io_data_callback_t input_callback;

  /** NMEA I/O output data callback */
  nmea_io_data_callback_t output_callback;

  /** NMEA I/O status */
  nmea_io_stats_t stats;

} nmea_io_t;

/** NMEA I/O object default initializer */
#define NMEA_IO_INIT {      \
  .fd = -1,                 \
  .dev_name = { 0 },        \
  .handler = NULL,          \
  .nmea = NULL,             \
  .input_callback = NULL,   \
  .output_callback = NULL,  \
  .stats = { 0 }            \
}

/**
   Get NMEA I/O file descriptor.

   To be used when non-NMEA I/O is required, e.g. for proprietary binary
   protocol I/O, binary RTCM data input, etc.

   Typically, a user should stop polling input via ::nmea_io_input_poll_stop
   if input binary data is expected.

   @param[in] nmea_io NMEA I/O object

   @return NMEA I/O file descriptor
*/
atlk_inline int
nmea_io_fd_get(nmea_io_t *nmea_io)
{
  return nmea_io->fd;
}

/** NMEA I/O object configuration parameters */
typedef struct {
  /** I/O device name */
  const char *dev_name;

  /** I/O device speed in bits/s */
  uart_speed_bps_t speed_bps;

  /** Input thread scheduling parameters */
  atlk_thread_sched_t sched_params;

  /** NMEA sentence handler */
  nmea_sentence_handler_t handler;

  /** NMEA object */
  nmea_t *nmea;

} nmea_io_config_t;

/** NMEA I/O object configuration parameters default initializer */
#define NMEA_IO_CONFIG_INIT {               \
  .dev_name = NULL,                         \
  .speed_bps = 0,                           \
  .sched_params = ATLK_THREAD_SCHED_INIT,   \
  .handler = NULL,                          \
  .nmea = NULL                              \
}

/**
   Open NMEA I/O.

   @note This function does not start input polling, user needs to do so
   explicitly via ::nmea_io_input_poll_start.

   @param[out] nmea_io NMEA I/O object
   @param[in] config Configuration parameters

   @retval ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_open(nmea_io_t *nmea_io,
            const nmea_io_config_t *config);

/**
   Start polling NMEA input.

   @param[in,out] nmea_io NMEA I/O object

   @retval ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_input_poll_start(nmea_io_t *nmea_io);

/**
   Stop polling NMEA input.

   @param[in,out] nmea_io NMEA I/O object

   @retval ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_input_poll_stop(nmea_io_t *nmea_io);

/**
   Whether polling NMEA input is enabled.

   @param[in] nmea_io NMEA I/O object
   
   @return 1 if polling is enabled, 0 otherwise.
*/
int
nmea_io_input_poll_enabled(const nmea_io_t *nmea_io);

/**
   Close NMEA I/O.

   @note This function stops input polling if it is active.

   @param[in,out] nmea_io NMEA I/O object

   @retval ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t
nmea_io_close(nmea_io_t *nmea_io);

/**
   Read NMEA sentence.

   NMEA sentence is checked for validity. On succcess, @p sentence is
   guaranteed to be null-terminated.

   @warning Should not be used when input polling is active (this is not
   checked in code, it's the user's responsibility).

   @param[in,out] nmea_io NMEA I/O object
   @param[out] sentence Read sentence
   @param[in] sentence_size Maximum sentence size in chars
   @param[in] wait Wait specification (optional)

   @todo Currently only @p wait of type ::ATLK_WAIT_FOREVER is supported.

   @retval ATLK_OK if succeeded
   @return ::ATLK_E_BUFFER_TOO_SMALL if @p sentence_size is too small
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_read_sentence(nmea_io_t *nmea_io,
            char *sentence,
            size_t sentence_size,
            const atlk_wait_t *wait);

/**
   Write NMEA sentence.

   NMEA sentence is checked for validity. Sentence is expected to be
   null-terminated.

   @param[in,out] nmea_io NMEA I/O object
   @param[in] sentence Sentence to write
   @param[in] sentence_size Sentence size in chars, including null-termination
   @param[in] wait Wait specification (optional)

   @todo Currently only @p wait of type ::ATLK_WAIT_FOREVER is supported.

   @retval ATLK_OK if succeeded
   @return ::ATLK_E_IO_ERROR on partial write
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_write_sentence(nmea_io_t *nmea_io,
            const char *sentence,
            size_t sentence_size,
            const atlk_wait_t *wait);

/**
   Get NMEA I/O status.

   Statistics are only collected for calls of ::nmea_io_write_sentence and
   ::nmea_io_read_sentence. They are not collected when user reads/writes
   directly from/to underlying file descriptor (gotten via ::nmea_io_fd_get).

   @param[in] nmea_io NMEA I/O object
   @param[out] value NMEA I/O status value

   @retval ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_stats_get(const nmea_io_t *nmea_io,
            nmea_io_stats_t *value);

/**
   Reset NMEA I/O status.

   @param[in,out] nmea_io NMEA I/O object

   @retval ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_stats_reset(nmea_io_t *nmea_io);

/**
   Set input data callback.

   When registered, @p input_callback is called on all data read via
   ::nmea_io_read_sentence, regardless of whether data represents valid
   NMEA sentences or not.

   @param[in,out] nmea_io NMEA I/O object
   @param[in] input_callback Input data callback function

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
 */
atlk_rc_t atlk_must_check
nmea_io_input_callback_set(nmea_io_t *nmea_io,
            nmea_io_data_callback_t input_callback);

/**
   Set output data callback.

   When registered, @p input_callback is called on all data written via
   ::nmea_io_write_sentence, regardless of whether data represents valid
   NMEA sentences or not.

   @param[in,out] nmea_io NMEA I/O object
   @param[in] output_callback Output data callback function

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
 */
atlk_rc_t atlk_must_check
nmea_io_output_callback_set(nmea_io_t *nmea_io,
            nmea_io_data_callback_t output_callback);

/**
   Set I/O device speed.

   @param[in] nmea_io NMEA I/O object
   @param[in] speed_bps Speed in bits/s

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_speed_set(nmea_io_t *nmea_io,
            uart_speed_bps_t speed_bps);

/**
   Sense NMEA speed.

   Function is blocking and may take up to several seconds to return. Upon
   success, NMEA I/O device speed is set to sensed NMEA speed.

   @warning User should not stop NMEA input polling while function is
   executing (this is not checked in code, it's the user's responsibility).

   @param[in] nmea_io NMEA I/O object
   @param[out] speed_bps Sensed speed in bits/s (optional)

   @retval ::ATLK_OK if succeeded
   @retval ::ATLK_E_IO_ERROR if device seems dead (nothing is read)
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_nmea_speed_sense(nmea_io_t *nmea_io,
            uart_speed_bps_t *speed_bps);

#ifdef __CRATON__
/**
   Change NMEA I/O device.

   @note This function is required for testing purposes.

   @param[in,out] nmea_io NMEA I/O object
   @param[in] dev_name NMEA I/O device name
   @param[in] speed_bps NMEA I/O speed in bits/s (optional)

   @retval ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
nmea_io_dev_change(nmea_io_t *nmea_io,
            const char *dev_name,
            const uart_speed_bps_t *speed_bps);
#endif /* __CRATON__ */

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_NMEA_IO_H */
