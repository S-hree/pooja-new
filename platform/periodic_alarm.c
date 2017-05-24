#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <platform/common.h>

#ifdef __THREADX__

#include <atlk/periodic_alarm.h>

#define PERIODIC_ALARM_F_EXPIRED 0x1
#define PERIODIC_ALARM_F_STOP 0x2

void periodic_handler(void *param)
{
  periodic_alarm_t *periodic_alarm = (periodic_alarm_t *)param;

  while (1) {
    uint32_t actual_flags;
    uint32_t tx_rc = tx_event_flags_get(&periodic_alarm->event,
                                        PERIODIC_ALARM_F_EXPIRED | PERIODIC_ALARM_F_STOP, 
                                        TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
    BUG_ON(tx_rc != TX_SUCCESS);
    if (actual_flags & PERIODIC_ALARM_F_STOP) {
      break;
    }
    periodic_alarm->handler(periodic_alarm);
  }
}

static void timer_expiration_function(uint32_t arg)
{
  periodic_alarm_t *alarm = (periodic_alarm_t *)arg;
  uint32_t tx_rc = tx_event_flags_set(&alarm->event, PERIODIC_ALARM_F_EXPIRED, TX_OR);
  BUG_ON(tx_rc);
}

atlk_rc_t
periodic_alarm_start(periodic_alarm_t *alarm, const periodic_alarm_attr_t *attr, 
						periodic_alarm_handler_t handler)
{
  int rc;
  uint32_t tx_rc;
  uint32_t period_ticks = usec_to_tick_round_up(attr->alarm_period_us);

  alarm->handler = handler;
  alarm->period_us =attr->alarm_period_us;
  tx_rc = tx_event_flags_create(&alarm->event, "period_alarm.event");
  BUG_ON(tx_rc != TX_SUCCESS);

  /* create thread with periodic alarm as argument */
  rc = atlk_thread_create(&alarm->thread, attr->thread_attr, periodic_handler, alarm);
  if (atlk_error(rc)) {
    TR_ERROR("Failed atlk_thread_create (%d)",rc);
    return rc;
  }

  tx_rc = tx_timer_create(&alarm->timer, "periodic_alarm.timer", timer_expiration_function,
                          (uint32_t)alarm, 1, period_ticks, TX_AUTO_ACTIVATE);
  BUG_ON(tx_rc != TX_SUCCESS);

  return ATLK_OK;
}

/* destroy timer and sets process to default signal behavior */
void periodic_alarm_stop(periodic_alarm_t *periodic_alarm)
{
  uint32_t tx_rc = tx_timer_deactivate(&periodic_alarm->timer);
  BUG_ON(tx_rc != TX_SUCCESS);

  tx_rc = tx_timer_delete(&periodic_alarm->timer);
  BUG_ON(tx_rc != TX_SUCCESS);

  tx_rc = tx_event_flags_set(&periodic_alarm->event, PERIODIC_ALARM_F_STOP, TX_OR);
  BUG_ON(tx_rc != TX_SUCCESS);

  atlk_thread_join(&periodic_alarm->thread);

  tx_rc = tx_event_flags_delete(&periodic_alarm->event);
  BUG_ON(tx_rc != TX_SUCCESS);
}

/* change current period of timer to new period */
void
periodic_alarm_change(periodic_alarm_t *alarm, uint64_t new_period_us)
{
  uint32_t period_ticks = usec_to_tick_round_up(new_period_us);
  uint32_t tx_rc = tx_timer_deactivate(&alarm->timer);
  BUG_ON(tx_rc != TX_SUCCESS);
  tx_rc = tx_timer_change(&alarm->timer, 1, period_ticks);
  BUG_ON(tx_rc != TX_SUCCESS);
  tx_rc = tx_timer_activate(&alarm->timer);
  BUG_ON(tx_rc != TX_SUCCESS);
  alarm->period_us = new_period_us;
}

#endif /* __THREADX__ */
