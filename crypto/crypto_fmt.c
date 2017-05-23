#ifndef __KERNEL__
#include <sys/types.h> /* ssize_t */
#endif /* __KERNEL__ */

#include <platform/common.h>

#include <crypto/crypto_fmt.h>

void
fmt_i2osp(const uint32_t *input,
	        uint8_t *output_ptr,
	        size_t u32_len)
{
  uint8_t *output = output_ptr;
  ssize_t i;
  for (i = u32_len - 1; i >= 0; --i) {
    uint32_t elem = input[i];
    *output = (uint8_t)(elem >> 24);
    output++;
    *output = (uint8_t)((elem & 0xff0000) >> 16);
    output++;
    *output = (uint8_t)((elem & 0xff00) >> 8);
    output++;
    *output = (uint8_t)(elem & 0xff);
    output++;
  }
}

void
fmt_os2ip(const uint8_t *input_ptr,
	        uint32_t *output,
	        size_t u32_len)
{
  const uint8_t *input = input_ptr;
  ssize_t i;
  for (i = u32_len - 1; i >= 0; --i) {
    uint32_t elem = (uint32_t)*input << 24;
    input++;
    elem |= (uint32_t)*input << 16;
    input++;
    elem |= (uint32_t)*input << 8;
    input++;
    elem |= (uint32_t)*input;
    input++;
    output[i] = elem;
  }
}

