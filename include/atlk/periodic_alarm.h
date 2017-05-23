/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_PERIODIC_ALARM_H
#define _ATLK_PERIODIC_ALARM_H

#include <tx_api.h>

#include <atlk/sdk.h>
#include <atlk/thread.h>

/**
   @file
  Periodic timer - handle creation and removal of periodic timers from process,
  Callback function is handled trough thread creation which enable us to use MUTEX
*/

struct _periodic_alarm;

/** Callback function type for timer expiration handler */
typedef void (*periodic_alarm_handler_t)(struct _periodic_alarm *);

typedef struct _periodic_alarm {
  atlk_thread_t thread;
  periodic_alarm_handler_t handler;
  TX_TIMER  timer;
  TX_EVENT_FLAGS_GROUP event;
  uint64_t  period_us;
} periodic_alarm_t;

#define PERIODIC_ALARM_INIT { .thread = ATLK_THREAD_INIT, }

typedef struct {
  atlk_thread_attr_t *thread_attr;
  uint64_t alarm_period_us;  
} periodic_alarm_attr_t;

#define PERIODIC_ALARM_ATTR_INIT { 0 }

/**
    Initialize timer and binds it to handler.

    @param[in,out] alarm Identifies the timer and use to arm or disarm it.
    @param[in] attributes related to the periodic thread and the interval period in microseconds.
    @param[in] handler Callback function to be bind with timer.

    @retval 0 if success
    @return Negative value if failed.
*/
atlk_rc_t atlk_must_check
periodic_alarm_start(periodic_alarm_t *alarm,
            const periodic_alarm_attr_t *attr, 
			periodic_alarm_handler_t handler);

/**
    Terminates timer and returns signal to default behavior.

    @param[in,out] alarm Identifies the timer and use to arm or disarm it

    @retval 0 if success
    @return Negative value if failed.
*/
void
periodic_alarm_stop(periodic_alarm_t *alarm);

/**
    change timer period.

    @param[in,out] alarm Identifies the timer and use to arm or disarm it.
    @param[in] new_period_us New period value in microseconds.
*/
void
periodic_alarm_change(periodic_alarm_t *alarm, uint64_t new_period_us);

#endif /* _ATLK_PERIODIC_ALARM_H */
