/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_ATOMIC_H
#define _ATLK_ATOMIC_H

#include <atlk/sdk.h>

#if defined __GNUC__ && !defined __arc__
#define ATLK_ATOMIC_USE_GCC_BUILTIN

#elif defined __THREADX__
#include <tx_api.h>
#define ATLK_ATOMIC_USE_THREADX_API

#else
#error "Neither GCC atomic builtins nor ThreadX API are available"
#endif

typedef struct {
  volatile int counter;
} atlk_atomic_t;

#define ATLK_ATOMIC_INIT(i) { (i) }

#define atlk_atomic_read(v) ((v)->counter)

#define atlk_atomic_set(v, i) (((v)->counter) = (i))

atlk_inline int
atlk_atomic_add(int i, atlk_atomic_t *v)
{
#ifdef ATLK_ATOMIC_USE_GCC_BUILTIN
  return __sync_add_and_fetch(&v->counter, i);
#endif /* ATLK_ATOMIC_USE_GCC_BUILTIN */

#ifdef ATLK_ATOMIC_USE_THREADX_API
  int counter;
  UINT old_posture = tx_interrupt_control(TX_INT_DISABLE);
  v->counter += 1;
  counter = v->counter;
  tx_interrupt_control(old_posture);
  return counter;
#endif /* ATLK_ATOMIC_USE_THREADX_API */
}

atlk_inline int
atlk_atomic_sub(int i, atlk_atomic_t *v)
{
#ifdef ATLK_ATOMIC_USE_GCC_BUILTIN
  return __sync_sub_and_fetch(&v->counter, i);
#endif /* ATLK_ATOMIC_USE_GCC_BUILTIN */

#ifdef ATLK_ATOMIC_USE_THREADX_API
  int counter;
  UINT old_posture = tx_interrupt_control(TX_INT_DISABLE);
  v->counter -= 1;
  counter = v->counter;
  tx_interrupt_control(old_posture);
  return counter;
#endif /* ATLK_ATOMIC_USE_THREADX_API */
}

atlk_inline int
atlk_atomic_inc(atlk_atomic_t *v)
{
  return atlk_atomic_add(1, v);
}

atlk_inline int
atlk_atomic_dec(atlk_atomic_t *v)
{
  return atlk_atomic_sub(1, v);
}

atlk_inline int
atlk_atomic_dec_and_test(atlk_atomic_t *v)
{
  return !atlk_atomic_dec(v);
}

#endif /* _ATLK_ATOMIC_H */
