#include <stdio.h>
#include <ctype.h>

#include <atlk/sdk.h>
#include <platform/common.h>
#include <atlk/hexdump.h>
#include <atlk/memio.h>

#define HEXDUMP_MAX_BUF_LEN 16
#define HEXDUMP_ASCII_COLUMN (HEXDUMP_MAX_BUF_LEN * 3 + 1)
#define HEXDUMP_MAX_LINE_LEN (HEXDUMP_ASCII_COLUMN + HEXDUMP_MAX_BUF_LEN + 1)

static void
hexdump_to_buffer(const void *buf, size_t buf_len, 
  char line[HEXDUMP_MAX_LINE_LEN], int ascii)
{
  const uint8_t *ptr = buf;
  char *pos = &line[0];
  int i;

  if (!buf_len) {
    *pos = '\0';
    return;
  }

  if (buf_len > HEXDUMP_MAX_LINE_LEN) {
    buf_len = HEXDUMP_MAX_LINE_LEN;
  }

  for (i = 0; i < buf_len - 1; i++) {
    pos += sprintf(pos, "%02x ", ptr[i]);
  }
  pos += sprintf(pos, "%02x", ptr[i]);

  if (ascii) {
    char *ascii_pos = &line[HEXDUMP_ASCII_COLUMN];

    while (pos < ascii_pos) {
      *(pos++) = ' ';
    }

    for (i = 0; i < buf_len; i++) {
      unsigned char ch = ptr[i];
      *(pos++) = (isascii((int)ch) && isprint((int)ch)) ? ch : '.';
    }

    *pos = '\0'; 
  }
}

void
print_hexdump(const void *buf, size_t len, int ascii)
{
	const uint8_t *ptr = buf;
	int i, line_len, remaining = len;
	char line[HEXDUMP_MAX_LINE_LEN];

	for (i = 0; i < len; i += HEXDUMP_MAX_BUF_LEN) {
		line_len = min(remaining, HEXDUMP_MAX_BUF_LEN);
		remaining -= HEXDUMP_MAX_BUF_LEN;

		hexdump_to_buffer(ptr + i, line_len, line, ascii);
    TR_INFO("%.8x: %s", i, line);
	}
}

atlk_rc_t
hexstr_to_buffer(const char *hexstr, size_t hexstr_len,
  uint8_t *buffer, size_t *buffer_len)
{
  struct memio str, buf;
  atlk_rc_t rc;

  memio_init(&str, (void *)hexstr, hexstr_len);
  memio_init(&buf, buffer, *buffer_len);

  if (hexstr_len % 2) {
    uint8_t byte;

    rc = memio_read_uint8(&str, &byte);
    if (atlk_error(rc)) {
      return rc;
    }

    rc = memio_reserve(&buf, sizeof(uint8_t));
    if (atlk_error(rc)) {
      return rc;
    }

    hexstr_len -= sizeof(byte);
    sscanf((const char *)&byte, "%hhx", &buffer[0]);
  }

  while (hexstr_len) {
    uint8_t hexbyte[2];
    uint8_t *dest = buf.next;

    rc = memio_read_uint8(&str, &hexbyte[0]);
    if (atlk_error(rc)) {
      return rc;
    }

    rc = memio_read_uint8(&str, &hexbyte[1]);
    if (atlk_error(rc)) {
      return rc;
    }

    rc = memio_reserve(&buf, sizeof(uint8_t));
    if (atlk_error(rc)) {
      return rc;
    }

    hexstr_len -= sizeof(hexbyte);
    sscanf((const char *)&hexbyte, "%2hhx", dest);
  }

  *buffer_len = memio_consumed(&buf, buffer);

  return ATLK_OK;
}

