/* Copyright (C) 2015-2016 Autotalks Ltd. */
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <platform/common.h>
#include <atlk/thread.h>
#include <atlk/sizes.h>
#include <atlk/nmea.h>
#include <atlk/nmea_io.h>
#include <atlk/nmea_io_internal.h>

#if defined __CRATON__
#include <craton/uart_driver.h>
#elif defined ATLK_OS_POSIX
#include <termios.h>
#endif /* __CRATON__ */

/* Input thread resources */
static uint8_t input_poll_thread_stack[ATLK_SZ_8K];
static atlk_thread_t input_poll_thread = ATLK_THREAD_INIT;
static atlk_thread_sched_t input_poll_thread_sched = ATLK_THREAD_SCHED_INIT;
static void input_poll_thread_entry(void *args);

/* Whether input polling is active (TODO: Should this be mutex-protected?) */
static int input_poll_started = 0;

/* Whether NMEA I/O is opened -- a singleton for now */
static int nmea_io_opened = 0;

#if defined __QNXNTO__
static int
cfsetspeed(struct termios *termios,
           speed_t speed)
{
  int rv = 0;

  rv = cfsetispeed(termios, speed);
  if (rv != 0) {
    TR_ERROR("cfsetispeed failed (errno=%d)", errno);
    return rv;
  }

  rv = cfsetospeed(termios, speed);
  if (rv != 0) {
    TR_ERROR("cfsetospeed failed (errno=%d)", errno);
  }

  return rv;
}
#endif /* __QNXNTO__ */

#ifdef ATLK_OS_POSIX
static void
_cfmakeraw(struct termios *tty)
{
  tty->c_iflag &= ~(IMAXBEL | IGNBRK | BRKINT | PARMRK |
                    ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  tty->c_oflag &= ~OPOST;
  tty->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tty->c_cflag &= ~(CSIZE | PARENB);
  tty->c_cflag |= CS8;
  tty->c_cc[VMIN] = 1;
  tty->c_cc[VTIME] = 0;
}
#endif /* ATLK_OS_POSIX */

#ifdef __CRATON__
static int
dev_name_to_uart_device_id(const char *dev_name)
{
  if (strncmp(dev_name, "/dev/uart", 9) || dev_name[10] != '\0') {
    goto error;
  }

  switch (dev_name[9] - '0') {
  case 0:
    return UART_DEVICE_ID_0;
  case 1: 
    return UART_DEVICE_ID_1;
  case 2: 
    return UART_DEVICE_ID_2;
  default:
    break;
  }

error:
  TR_ERROR("Unknown device: %s", dev_name);
  return -1;
}
#endif /* __CRATON__ */

atlk_rc_t atlk_must_check
nmea_io_dev_name_set(nmea_io_t *nmea_io,
            const char *dev_name)
{
  size_t dev_name_len = 0;

  BUG_ON(!nmea_io);

  if (atlk_unlikely(!dev_name)) {
    TR_ERROR_NO_ARGS("Device name is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (!strncmp(nmea_io->dev_name, dev_name, sizeof(nmea_io->dev_name))) {
    TR_WARNING("Device '%s' is already opened", dev_name);
    return ATLK_E_INVALID_STATE;
  }

  dev_name_len = strlen(dev_name);
  if (atlk_unlikely(dev_name_len + 1 >= sizeof(nmea_io->dev_name))) {
    TR_ERROR("Device name '%s' is too long", dev_name);
    return ATLK_E_INVALID_ARG;
  }

  (void)strncpy(nmea_io->dev_name, dev_name, sizeof(nmea_io->dev_name));

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_io_open(nmea_io_t *nmea_io,
            const nmea_io_config_t *config)
{
#ifdef ATLK_OS_POSIX
  struct termios tty;
#endif /* ATLK_OS_POSIX */
#ifdef __CRATON__
  int dev_id = -1;
#endif /* __CRATON__ */
  int rv = -1, fd = -1, flags = 0;
  atlk_rc_t rc = ATLK_OK;

  if (atlk_unlikely(!nmea_io || !config)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(nmea_io->fd != -1)) {
    TR_ERROR_NO_ARGS("NMEA I/O is already open or has not been initialized");
    return ATLK_E_INVALID_STATE;
  }

  if (atlk_unlikely(nmea_io_opened)) {
    TR_ERROR("Only one instance of NMEA I/O supported");
    return ATLK_E_UNSUPPORTED;
  }

  rc = nmea_io_dev_name_set(nmea_io, config->dev_name);
  if (atlk_error(rc)) {
    return rc;
  }

#ifdef ATLK_OS_POSIX
  flags = O_RDWR | O_NOCTTY | O_SYNC;
#endif /* ATLK_OS_POSIX */

  fd = open(nmea_io->dev_name, flags);
  if (fd < 0) {
    TR_ERROR("Open '%s' failed (errno=%d)", nmea_io->dev_name, errno);
    return ATLK_E_IO_ERROR;
  }

#ifdef __CRATON__
  dev_id = dev_name_to_uart_device_id(nmea_io->dev_name);
  if (dev_id == -1) {
    TR_ERROR_NO_ARGS("Converting 'dev_name' to device ID failed");
    goto error;
  }

  rc = uart_speed_set((uart_device_id_t)dev_id, config->speed_bps);
  if (atlk_error(rc)) {
    TR_ERROR("Setting UART speed failed (rc=%d)", rc);
    goto error;
  }
#endif /* __CRATON__ */

#ifdef ATLK_OS_POSIX
  memset(&tty, 0, sizeof(tty));

  _cfmakeraw(&tty);
  rv = cfsetspeed(&tty, config->speed_bps);
  if (rv < 0) {
    rc = ATLK_E_IO_ERROR;
    goto error;
  }

  rv = tcsetattr(fd, TCSADRAIN, &tty);
  if (rv < 0) {
    TR_ERROR("Termios set attributes failed (errno=%d)", errno);
    rc = ATLK_E_IO_ERROR;
    goto error;
  }
#endif /* ATLK_OS_POSIX */
 
  nmea_io->fd = fd; 
  nmea_io->handler = config->handler;
  nmea_io->nmea = config->nmea;

  /* Initialize debug params */
  nmea_io->input_callback = NULL;
  nmea_io->output_callback = NULL;
  memset(&nmea_io->stats, 0, sizeof(nmea_io->stats));

  /* Store input thread's scheduling params for later */
  input_poll_thread_sched = config->sched_params;

  TR_DEBUG_NO_ARGS("NMEA I/O opened");
  nmea_io_opened = 1;

  return ATLK_OK;

error:
  rv = close(fd);
  if (rv < 0) {
    TR_ERROR("Close failed (errno=%d)", errno);
  }

  return rc;
}

atlk_rc_t 
nmea_io_close(nmea_io_t *nmea_io)
{
  int rv = 0;
  atlk_rc_t rc = ATLK_OK;

  if (atlk_unlikely(!nmea_io)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(nmea_io->fd == -1)) {
    TR_DEBUG_NO_ARGS("NMEA I/O is not open");
    return ATLK_OK;
  }

  if (input_poll_started) {
    rc = nmea_io_input_poll_stop(nmea_io);
    if (atlk_error(rc)) {
      TR_ERROR("Stopping NMEA input poll failed (rc=%d)", rc);
      return rc;
    }
  }

  rv = close(nmea_io->fd);
  if (rv < 0) {
    TR_ERROR("Close failed (errno=%d)", errno);
    return ATLK_E_UNSPECIFIED;
  }
  
  *nmea_io = (nmea_io_t)NMEA_IO_INIT;

  TR_DEBUG_NO_ARGS("NMEA I/O closed");
  nmea_io_opened = 0;

  return ATLK_OK;
}

atlk_inline atlk_rc_t atlk_must_check
nmea_io_input_poll_check_args(const nmea_io_t *nmea_io)
{
  if (atlk_unlikely(!nmea_io)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(nmea_io->fd == -1)) {
    TR_ERROR_NO_ARGS("NMEA I/O is not open");
    return ATLK_E_INVALID_STATE;
  }

  if (atlk_unlikely(!nmea_io->handler)) {
    TR_ERROR_NO_ARGS("NMEA sentence handler is not configured");
    return ATLK_E_INVALID_STATE;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_io_input_poll_start(nmea_io_t *nmea_io)
{
  atlk_thread_attr_t thread_attr = ATLK_THREAD_ATTR_INIT;
  atlk_rc_t rc = ATLK_OK;

  rc = nmea_io_input_poll_check_args(nmea_io);
  if (atlk_error(rc)) {
    return rc;
  }

  if (input_poll_started) {
    TR_DEBUG_NO_ARGS("NMEA input poll is already active");
    return ATLK_OK;
  }

  /* Configure and spawn input poll thread */
  thread_attr.name = "NMEA I/O: Input polling";
  thread_attr.stack = input_poll_thread_stack;
  thread_attr.stack_size = sizeof(input_poll_thread_stack);
  thread_attr.priority = input_poll_thread_sched.priority;

  rc = atlk_thread_create(&input_poll_thread, &thread_attr,
                          input_poll_thread_entry, (void *)nmea_io);
  if (atlk_error(rc)) {
    TR_ERROR("Input polling thread creation failed (rc=%d)", rc);
    return rc;
  }
  
  input_poll_started = 1;
  TR_DEBUG_NO_ARGS("NMEA input poll started");

  return ATLK_OK;
}

static void
input_poll_thread_entry(void *arg)
{
  nmea_io_t *nmea_io = (nmea_io_t *)arg;
  atlk_rc_t rc = ATLK_OK;

  while (1) {
    char sentence[NMEA_P_LEN_MAX];

    rc = nmea_io_read_sentence(nmea_io, sentence, sizeof(sentence),
                               &atlk_wait_forever);
    if (atlk_error(rc)) {
      usleep(MICRO_PER_MILLI);     /* Sleep to avoid potential busy-loop */
      continue;
    }

    rc = nmea_io->handler(nmea_io->nmea, sentence);
    if (atlk_error(rc)) {
      TR_DEBUG("NMEA handler failed (rc=%d)", rc);
    }
  }
}

atlk_rc_t atlk_must_check
nmea_io_input_poll_stop(nmea_io_t *nmea_io)
{
  atlk_rc_t rc = ATLK_OK;

  rc = nmea_io_input_poll_check_args(nmea_io);
  if (atlk_error(rc)) {
    return rc;
  }

  if (!input_poll_started) {
    TR_DEBUG_NO_ARGS("NMEA input poll is not active");
    return ATLK_OK;
  }

  atlk_thread_cancel(&input_poll_thread);

  input_poll_started = 0;
  TR_DEBUG_NO_ARGS("NMEA input poll stopped");

  return ATLK_OK;
}

int
nmea_io_input_poll_enabled(const nmea_io_t *nmea_io)
{
  (void)nmea_io;
  return input_poll_started;
}

atlk_inline atlk_rc_t atlk_must_check
nmea_io_sentence_check_args(const nmea_io_t *nmea_io,
            const char *sentence,
            size_t sentence_size,
            const atlk_wait_t *wait)
{
  if (atlk_unlikely(!nmea_io || !sentence)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(sentence_size < 2)) {
    TR_ERROR_NO_ARGS("Invalid sentence size, too small");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(nmea_io->fd == -1)) {
    TR_ERROR_NO_ARGS("NMEA I/O is not open");
    return ATLK_E_INVALID_STATE;
  }

  if (atlk_unlikely(!wait || wait->wait_type != ATLK_WAIT_FOREVER)) {
    TR_ERROR_NO_ARGS("Unsupported wait specification");
    return ATLK_E_UNSUPPORTED;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_io_read_sentence(nmea_io_t *nmea_io,
            char *sentence,
            size_t sentence_size,
            const atlk_wait_t *wait)
{
  size_t cur = 0;
  atlk_rc_t rc = ATLK_OK;

  rc = nmea_io_sentence_check_args(nmea_io, sentence, sentence_size, wait);
  if (atlk_error(rc)) {
    return rc;
  }

  /* Sub-optimal naive implementation (TODO) */
  while (1) {
    char c = 0; 
    ssize_t rv = 0;
    
    rv = read(nmea_io->fd, &c, sizeof(c));
    if (rv == 0) {
      usleep(1000);     /* Sleep to avoid potential busy-loop */
      continue;
    }
    else if (rv != sizeof(c)) {
      TR_ERROR("Read failed, returned %zd (errno=%d)", rv, errno);
      rc = ATLK_E_UNSPECIFIED;
      break;
    }
    sentence[cur++] = c;

    if (c == '\n') {
      break;
    }
    else if (cur + 1 == sentence_size) {
      TR_DEBUG_NO_ARGS("Sentence size is too small");
      rc = ATLK_E_BUFFER_TOO_SMALL;
      break;
    }
  }

  sentence[cur] = '\0';

  if (nmea_io->input_callback) {
    nmea_io->input_callback(sentence, cur, NMEA_IO_DIRECTION_INPUT);
  }
  nmea_io->stats.rx_bytes_cnt += cur;
  
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_sentence_check(sentence, cur + 1);
  if (atlk_error(rc)) {
    nmea_io->stats.rx_nmea_sentence_error_cnt++;
    return rc;
  }
  nmea_io->stats.rx_nmea_sentence_cnt++;

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_io_write_sentence(nmea_io_t *nmea_io,
            const char *sentence,
            size_t sentence_size,
            const atlk_wait_t *wait)
{
  ssize_t rv = 0, len = 0;
  atlk_rc_t rc = ATLK_OK;

  rc = nmea_io_sentence_check_args(nmea_io, sentence, sentence_size, wait);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_sentence_check(sentence, sentence_size);
  if (atlk_error(rc)) {
    nmea_io->stats.tx_nmea_sentence_error_cnt++;
    return rc; 
  }

  len = sentence_size - 1;

  rv = write(nmea_io->fd, sentence, len);
  if (rv < 0) {
    TR_ERROR("Write failed, returned %zd (errno=%d)", rv, errno);
    return ATLK_E_UNSPECIFIED;
  }
  else if (rv == 0) {
    TR_DEBUG_NO_ARGS("Write returned 0");
    return ATLK_E_NOT_READY;
  }
  else if (rv != len) {
    TR_ERROR("Partial write, %zd/%zd written", rv, len);
    rc = ATLK_E_IO_ERROR;
  }
  else {
    nmea_io->stats.tx_nmea_sentence_cnt++;
  }

  fdatasync(nmea_io->fd);

  if (nmea_io->output_callback) {
    nmea_io->output_callback(sentence, rv, NMEA_IO_DIRECTION_OUTPUT);
  }
  nmea_io->stats.tx_bytes_cnt += rv;

  return rc;
}

atlk_rc_t atlk_must_check
nmea_io_stats_get(const nmea_io_t *nmea_io,
         nmea_io_stats_t *stats)
{
  if (atlk_unlikely(!nmea_io || !stats)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  *stats = nmea_io->stats;
  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_io_stats_reset(nmea_io_t *nmea_io)
{
  if (atlk_unlikely(!nmea_io)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  memset(&nmea_io->stats, 0, sizeof(nmea_io->stats));
  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_io_input_callback_set(nmea_io_t *nmea_io,
            nmea_io_data_callback_t input_callback)
{
  if (atlk_unlikely(!nmea_io)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  nmea_io->input_callback = input_callback;
  return ATLK_OK;
}

atlk_rc_t atlk_must_check
nmea_io_output_callback_set(nmea_io_t *nmea_io,
            nmea_io_data_callback_t output_callback)
{
  if (atlk_unlikely(!nmea_io)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  nmea_io->output_callback = output_callback;
  return ATLK_OK;
}

#ifdef ATLK_OS_POSIX

#ifndef B14400
#define B14400 14400
#endif
#ifndef B230400
#define B230400 230400
#endif
#ifndef B460800
#define B460800 460800
#endif
#ifndef B921600
#define B921600 921600
#endif

atlk_inline speed_t
craton_to_posix_speed(uart_speed_bps_t speed_bps)
{
  switch (speed_bps) {
  case UART_SPEED_4800_BPS:
    return B4800;
  case UART_SPEED_9600_BPS:
    return B9600;
  case UART_SPEED_14400_BPS:
    return B14400;
  case UART_SPEED_19200_BPS:
    return B19200;
  case UART_SPEED_38400_BPS:
    return B38400;
  case UART_SPEED_57600_BPS:
    return B57600;
  case UART_SPEED_115200_BPS:
    return B115200;
  case UART_SPEED_230400_BPS:
    return B230400;
  case UART_SPEED_460800_BPS:
    return B460800;
  case UART_SPEED_921600_BPS:
    return B921600;
  default:
    BUG();
  }
}
#endif /* ATLK_OS_POSIX */

atlk_rc_t atlk_must_check
nmea_io_speed_set(nmea_io_t *nmea_io,
            uart_speed_bps_t speed_bps)
{
#ifdef ATLK_OS_POSIX
  int rv = -1;
  struct termios tty;
  speed_t speed;
#endif /* ATLK_OS_POSIX */

#ifdef __CRATON__
  int dev_id = -1;
  atlk_rc_t rc = ATLK_OK;

  dev_id = dev_name_to_uart_device_id(nmea_io->dev_name);
  if (dev_id == -1) {
    TR_ERROR_NO_ARGS("Converting 'dev_name' to device ID failed");
    return ATLK_E_INVALID_STATE;
  }

  rc = uart_speed_set((uart_device_id_t)dev_id, speed_bps);
  if (atlk_error(rc)) {
    TR_ERROR("Setting UART speed failed (rc=%d)", rc);
    return rc;
  }
#endif /* __CRATON__ */
  
#ifdef ATLK_OS_POSIX
  memset(&tty, 0, sizeof(tty));

  speed = craton_to_posix_speed(speed_bps);
  cfsetspeed(&tty, speed);
  
  rv = tcsetattr(nmea_io->fd, TCSADRAIN, &tty);
  if (rv < 0) {
    TR_ERROR("Termios configuartion failed (errno=%d)", errno);
    return ATLK_E_UNSPECIFIED;
  }
#endif /* ATLK_OS_POSIX */

  /* TODO: Sleep to let UART device speed change settle */
  usleep(100 * MICRO_PER_MILLI);

  TR_DEBUG("NMEA I/O device speed set to: %" PRIu32, speed_bps);

  return ATLK_OK;
}

/* Array is ordered for performance (i.e. by likelyhood) */
static const uart_speed_bps_t speed_array[] = {
  UART_SPEED_230400_BPS,    /* Autotalks firmware NMEA speed */
  UART_SPEED_9600_BPS,      /* Telit firmware NMEA speed */
  UART_SPEED_115200_BPS,    /* Autotalks legacy firmware NMEA speed */
  UART_SPEED_921600_BPS,
  UART_SPEED_460800_BPS,
  UART_SPEED_57600_BPS,  
  UART_SPEED_38400_BPS,
  UART_SPEED_19200_BPS,
  UART_SPEED_14400_BPS,  
  UART_SPEED_4800_BPS
};

atlk_rc_t atlk_must_check
nmea_io_nmea_speed_sense(nmea_io_t *nmea_io,
            uart_speed_bps_t *speed_bps)
{
  size_t speed_i = 0;
  uint32_t bytes = 0;
  atlk_rc_t rc = ATLK_OK;

  if (atlk_unlikely(!nmea_io)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(nmea_io->fd == -1)) {
    TR_ERROR_NO_ARGS("NMEA I/O is not open");
    return ATLK_E_INVALID_STATE;
  }

  if (atlk_unlikely(!input_poll_started)) {
    TR_ERROR_NO_ARGS("NMEA speed sense requires active input polling");
    return ATLK_E_INVALID_STATE;
  }

  bytes = nmea_io->stats.rx_bytes_cnt;

  while (speed_i < ARRAY_SIZE(speed_array)) {
    uint32_t sentences = nmea_io->stats.rx_nmea_sentence_cnt;

    rc = nmea_io_speed_set(nmea_io, speed_array[speed_i]);
    if (atlk_error(rc)) {
      return rc;
    }

    /* Worse case is a device configured to 1Hz */
    usleep(MICRO_PER_UNIT);

    if (sentences < nmea_io->stats.rx_nmea_sentence_cnt) {
      if (speed_bps) {
        *speed_bps = speed_array[speed_i];
      }
      TR_INFO("Sensed NMEA speed: %" PRIu32 " bits/s", speed_array[speed_i]);
      return ATLK_OK;
    }

    ++speed_i;
  }

  if (bytes == nmea_io->stats.rx_bytes_cnt) {
    TR_ERROR_NO_ARGS("NMEA I/O device seems dead, nothing was read");
    return ATLK_E_IO_ERROR;
  }

  TR_ERROR_NO_ARGS("Failed to sense NMEA speed");
  return ATLK_E_UNSPECIFIED;
}

#ifdef __CRATON__
atlk_rc_t atlk_must_check
nmea_io_dev_change(nmea_io_t *nmea_io,
            const char *dev_name,
            const uart_speed_bps_t *speed_bps)
{
  int fd = -1, rv = 0;
  atlk_rc_t rc = ATLK_OK;

  BUG_ON(!nmea_io || !nmea_io_opened);

  rc = nmea_io_dev_name_set(nmea_io, dev_name);
  if (atlk_error(rc)) {
    return rc;
  }

  rc = nmea_io_input_poll_stop(nmea_io);
  if (atlk_error(rc)) {
    return rc;
  }

  rv = close(nmea_io->fd);
  if (rv < 0) {
    TR_ERROR("Close failed (errno=%d)", errno);
    return ATLK_E_UNSPECIFIED;
  }

  nmea_io->fd = -1;
  
  fd = open(nmea_io->dev_name, 0);
  if (fd < 0) {
    TR_ERROR("Open '%s' failed (errno=%d)", nmea_io->dev_name, errno);
    return ATLK_E_IO_ERROR;
  }

  nmea_io->fd = fd; 

  if (speed_bps) {
    rc = nmea_io_speed_set(nmea_io, *speed_bps);
    if (atlk_error(rc)) {
      return rc;
    }
  }

  rc = nmea_io_input_poll_start(nmea_io);
  if (atlk_error(rc)) {
    return rc;
  }

  TR_INFO("Device changed to '%s'", nmea_io->dev_name);
  return ATLK_OK;
}
#endif /* __CRATON__ */
