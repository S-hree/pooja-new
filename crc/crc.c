#include <stdio.h>
#include <atlk/crc.h>
#include <platform/common.h>

#define BITS_IN_BYTE 8
#define ALIGN_TO_BYTE(width) ((width) - BITS_IN_BYTE)
void
crc_compute(const crc_lut_t *lut,
            const void *buf,
            size_t buf_size,
            uint32_t *crc_ptr)
{
  const uint8_t *p;

  BUG_ON(lut == NULL);
  BUG_ON(buf == NULL);
  BUG_ON(crc_ptr == NULL);

  uint32_t crc = *crc_ptr;
  p = buf;

  while (buf_size--) {
      crc = lut->lut[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
  }

  *crc_ptr = crc;
}
