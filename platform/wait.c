#if defined __THREADX__
#include <tx_api.h>
#endif

#include <atlk/sdk.h>
#include <platform/common.h>

const atlk_wait_t atlk_wait_forever = { .wait_type = ATLK_WAIT_FOREVER };

#if defined __THREADX__
ULONG atlk_wait_to_tx_wait(const atlk_wait_t *wait)
{
  if (wait == NULL) {
    return TX_NO_WAIT;
  }

  switch (wait->wait_type) {
  case ATLK_WAIT_FOREVER:
    return TX_WAIT_FOREVER;

  case ATLK_WAIT_INTERVAL:
    return usec_to_tick_round_up(wait->wait_usec);

  default:
    BUG();
  }
}
#endif /* __THREADX__ */
