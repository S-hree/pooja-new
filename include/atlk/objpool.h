/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_OBJPOOL_H
#define _ATLK_OBJPOOL_H

#include <platform/common.h>
#include <atlk/mutex.h>

/**
   @file
   Generic thread-safe fixed-sized object pool.

   Allows for FIFO allocation of fixed-sized objects
   from a constant memory region.
*/

/* Object pool internal object */
struct _atlk_objpool_object {
  struct _atlk_objpool_object *next;
  uint8_t user_object[0];
};

#define _ATLK_OBJPOOL_OBJECT_SIZE_LONG(_object_size) \
  (DIV_ROUND_UP(((_object_size) + sizeof(struct _atlk_objpool_object)), sizeof(long)))

#define _ATLK_OBJPOOL_MEMORY_SIZE_LONG(_object_size, _count) \
  ((_count) * _ATLK_OBJPOOL_OBJECT_SIZE_LONG(_object_size))

/**
   Calculate the needed memory size of an object pool in bytes.

   This macro can be used for static and dynamic memory allocations for
   an object pool.

   Static memory allocation:
   @code{.c}
   // Allocate enough memory for an object pool with 32 objects, each of 8 bytes
   long my_objpool_area[ATLK_OBJPOOL_MEMORY_SIZE(8, 32)];
   @endcode

   Dynamic memory allocation:
   @code{.c}
   // Allocate enough memory for an object pool with 32 objects, each of 8 bytes
   long *my_objpool_area = malloc(ATLK_OBJPOOL_MEMORY_SIZE(8, 32));
   @endcode
*/
#define ATLK_OBJPOOL_MEMORY_SIZE(_object_size, _count) \
  (_ATLK_OBJPOOL_MEMORY_SIZE_LONG(_object_size, _count) * sizeof(long))

/** Object constructor callback */
typedef atlk_rc_t (*atlk_objpool_ctor_t)(void *object, void *opaque);

/** Object destructor callback */
typedef void (*atlk_objpool_dtor_t)(void *object, void *opaque);

/** Object pool descriptor */
typedef struct {
  /** Object pool name */
  const char *name;

  /** Number of objects in the pool */
  size_t object_count;

  /** Object size (in sizeof long) */
  size_t object_size_long;

  /** Pool memory area */
  long *memory;

  /** Memory area size (in sizeof long) */
  size_t memory_size_long;

  /** Head of the free objects list */
  struct _atlk_objpool_object *free_head;

  /** Tail of the free objects list */
  struct _atlk_objpool_object *free_tail;

  /** Pool magic */
  uint32_t magic;

  /** Lock protecting from concurrent access to the pool */
  atlk_mutex_t lock;

  /** User object destructor */
  atlk_objpool_dtor_t dtor;

  /** User data for the destructor */
  void *opaque;

} atlk_objpool_t;

/** Object pool static initializer */
#define ATLK_OBJPOOL_INIT {   \
  .name = NULL,               \
  .object_count = 0,          \
  .object_size_long = 0,      \
  .memory = NULL,             \
  .memory_size_long = 0,      \
  .free_head = NULL,          \
  .free_tail = NULL,          \
  .magic = 0,                 \
  .lock = ATLK_MUTEX_INIT,    \
  .dtor = NULL,               \
  .opaque = NULL              \
}

/** Object pool dynamic initializer */
atlk_inline void
atlk_objpool_init(atlk_objpool_t *objpool)
{
  *objpool = (atlk_objpool_t)ATLK_OBJPOOL_INIT;
}

/**
   Create object pool.

   Memory must be aligned on a 'long' boundary.
   Memory area size (in bytes) must be at least the size returned by
   ATLK_OBJPOOL_MEMORY_SIZE() macro.

   @param[in,out] objpool Reference to an initialized object pool
   @param[in[ name Object pool name
   @param[in] memory Memory area for the pool
   @param[in] object_size Object size for this pool (in bytes)
   @param[in] object_count Number of objects in the pool
   @param[in] ctor Object constructor (optional)
   @param[in] dtor Object destructor (optional)
   @param[in] opaque User data for the constructor/destructor (optional)

   @retval ATLK_OK if succeeded
   @retval ATLK_E_INVALID_STATE if object pool is not initialized
   @return Error code if failed for other reason
*/
atlk_rc_t atlk_must_check
atlk_objpool_create(atlk_objpool_t *objpool, const char *name,
  long *memory, size_t object_size, size_t object_count,
  atlk_objpool_ctor_t ctor, atlk_objpool_dtor_t dtor, void *opaque);

/**
   Destroy object pool.

   @param[in,out] objpool Object pool to destroy
*/
void
atlk_objpool_destroy(atlk_objpool_t *objpool);

/**
   Allocate object from an object pool.

   @param[in,out] objpool Object pool to allocate from
   @param[out] object Allocated object

   @retval ::ATLK_OK if succeeded
   @retval ::ATLK_E_OUT_OF_MEMORY if memory could not be allocated
   @return Error code if failed for other reason
*/
atlk_rc_t atlk_must_check
atlk_objpool_alloc(atlk_objpool_t *objpool, void **object);

/**
   Free object from an object pool.

   @param[in] object Object to free
*/
void
atlk_objpool_free(void *object);

#endif /* _ATLK_OBJPOOL_H */
