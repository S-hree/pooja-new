/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _ATLK_BPOOL_H
#define _ATLK_BPOOL_H

#include <atlk/sdk.h>
#include <atlk/mutex.h>

/**
   @file
   Thread-safe fixed-sized block pool.

   Allows for allocation of fixed-sized blocks
   from a constant memory region.
*/

#define _ATLK_BPOOL_BLOCK_SIZE_LONG(_block_size) \
  (DIV_ROUND_UP(((_block_size) + sizeof(void *)), sizeof(long)))

#define _ATLK_BPOOL_AREA_SIZE_LONG(_block_size, _count) \
  ((_count) * _ATLK_BPOOL_BLOCK_SIZE_LONG(_block_size))

/** Macro to calculate the block pool size */
#define ATLK_BPOOL_AREA_SIZE(_block_size, _count) \
  (_ATLK_BPOOL_AREA_SIZE_LONG(_block_size, _count) * sizeof(long))

/** Macro to define block pool memory region */
#define ATLK_BPOOL_AREA_DEFINE(_area_name, _block_size, _count) \
  long _area_name[_ATLK_BPOOL_AREA_SIZE_LONG(_block_size, _count)]

#ifdef __THREADX__
#include <tx_api.h>
#else
struct _atlk_bpool_block {
  struct _atlk_bpool_block *next;
  uint8_t user_block[0];
};
#endif /* __THREADX__ */

/** Block pool descriptor */
typedef struct {
 /** Boolean flag used to indicate the pool is created */
  int created;

#ifdef __THREADX__
  TX_BLOCK_POOL pool;
#else
  /** Pool name */
  char *name;

  /** Number of blocks in the pool */
  size_t block_count;

  /** Actual pool block size (in sizeof long) */
  size_t block_size_long;

  /** Pool memory area */
  long *area;

  /** Pool size including alignment overheads (in sizeof long) */
  size_t area_size_long;

  /** Free link list to link all free blocks */
  struct _atlk_bpool_block *free_list;

  /** Pool magic number */
  uint32_t magic;

  /** Mutex protecting from concurrent access to the pool */
  atlk_mutex_t lock;

#endif /* __THREADX__ */
} atlk_bpool_t;

/** Block pool static initializer */
#ifdef __THREADX__
#define ATLK_BPOOL_INIT { 0 }
#else
#define ATLK_BPOOL_INIT {   \
  .created = 0,             \
  .name = NULL,             \
  .block_count = 0,         \
  .block_size_long = 0,     \
  .area = NULL,             \
  .area_size_long = 0,      \
  .free_list = NULL,        \
  .magic = 0,               \
  .lock = ATLK_MUTEX_INIT,  \
}
#endif /* __THREADX__ */

/** Block pool dynamic initializer */
atlk_inline void
atlk_bpool_init(atlk_bpool_t *bpool)
{
  *bpool = (atlk_bpool_t)ATLK_BPOOL_INIT;
}

/**
   Create block pool.

   Memory area must be:
     - Statically defined using ATLK_BPOOL_AREA_DEFINE()
   OR
     - Dynamically allocated with size at least ATLK_BPOOL_AREA_SIZE() bytes

   @param[in,out] bpool Block pool object reference
   @param[in] name Pool name
   @param[in,out] area Memory area
   @param[in] block_size Block size in bytes
   @param[in] block_count Number of blocks in the pool

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
atlk_bpool_create(atlk_bpool_t *bpool, char *name,
  long *area, size_t block_size, size_t block_count);

/**
   Destroy block pool.

   @param[in,out] bpool Block pool to destroy
*/
void
atlk_bpool_destroy(atlk_bpool_t *bpool);

/**
   Allocate block from block pool.

   @param[in,out] bpool Block pool object reference to allocate block from
   @param[out] user_block Allocated block

   @retval ::ATLK_OK if succeeded
   @retval ::ATLK_E_OUT_OF_MEMORY if memory could not be allocated
   @return Error code if failed for other reason
*/
atlk_rc_t atlk_must_check
atlk_bpool_alloc(atlk_bpool_t *bpool, void **user_block);

/**
   Free block from block pool.

   @param[in] user_block Block to free
*/
void
atlk_bpool_free(void *user_block);

#endif /* _ATLK_BPOOL_H */
