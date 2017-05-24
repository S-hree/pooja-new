#include <atlk/sdk.h>
#include <platform/common.h>
#include <atlk/bpool.h>

#ifndef __THREADX__
#define ATLK_BPOOL_MAGIC 0xAA11BEEF
#endif /* __THREADX__ */

atlk_rc_t
atlk_bpool_create(atlk_bpool_t *bpool, char *name,
  long *area, size_t block_size, size_t block_count)
{
#ifdef __THREADX__
  UINT rc;
#else
  struct _atlk_bpool_block *block;
  uint32_t i;
  atlk_rc_t rc;
#endif /* __THREADX__ */

  /* Validate arguments */
  if (!bpool || !area) {
    TR_ERROR_NO_ARGS("Mandatory argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if ((uintptr_t)area % sizeof(long)) {
    TR_ERROR_NO_ARGS("Pool area must be aligned to long");
    return ATLK_E_INVALID_ARG;
  }

  /* Make sure the pool is not already created */
  if (bpool->created) {
    TR_ERROR_NO_ARGS("Pool is not initialized or already created");
    return ATLK_E_INVALID_STATE;
  }

#ifdef __THREADX__
  /* We assume these conditions in the pool memory size macros */
  BUILD_BUG_ON(sizeof(void *) != sizeof(UCHAR *));
  BUILD_BUG_ON(sizeof(long) != sizeof(ULONG));

  rc = tx_block_pool_create(&bpool->pool, name, block_size, area,
    ATLK_BPOOL_AREA_SIZE(block_size, block_count));
  BUG_ON(rc != TX_SUCCESS);

#else
  /* We assume these conditions in the pool memory size macros */
  BUILD_BUG_ON(sizeof(*block) != sizeof(void *));

  /* Create pool lock */
  rc = atlk_mutex_create(&bpool->lock, name);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to create pool lock (rc=%d)", rc);
    return rc;
  }

  bpool->name = name;
  bpool->block_count = block_count;
  bpool->block_size_long = _ATLK_BPOOL_BLOCK_SIZE_LONG(block_size);
  bpool->area = area;
  bpool->area_size_long = _ATLK_BPOOL_AREA_SIZE_LONG(block_size, block_count);
  bpool->magic = ATLK_BPOOL_MAGIC;

  block = (struct _atlk_bpool_block *)bpool->area;
  bpool->free_list = block;
  for (i = 0; i < bpool->block_count - 1; i++) {
    block->next = (struct _atlk_bpool_block *)((long *)block +
      bpool->block_size_long);
    block = block->next;
  }
  block->next = NULL;
#endif

  /* Mark the pool as created */
  bpool->created = 1;

  return ATLK_OK;
}

void
atlk_bpool_destroy(atlk_bpool_t *bpool)
{
  if (bpool->created) {
#ifdef __THREADX__
    UINT rc;

    rc = tx_block_pool_delete(&bpool->pool);
    BUG_ON(rc != TX_SUCCESS);
#else
    atlk_mutex_destroy(&bpool->lock);
#endif
    bpool->created = 0;
  }
}

atlk_rc_t
atlk_bpool_alloc(atlk_bpool_t *bpool, void **user_block)
{
  if (!bpool || !user_block) {
    TR_ERROR_NO_ARGS("Mandatory argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

#ifdef __THREADX__
  UINT rc;

  rc = tx_block_allocate(&bpool->pool, user_block, TX_NO_WAIT);

  switch (rc) {
    case TX_SUCCESS:
      return ATLK_OK;
    case TX_NO_MEMORY:
      return ATLK_E_OUT_OF_MEMORY;
    default:
      BUG();
      return ATLK_E_UNSPECIFIED;
  }
#else
  struct _atlk_bpool_block *block;
  atlk_rc_t rc = ATLK_OK;

  atlk_mutex_lock(&bpool->lock);

  block = bpool->free_list;
  if (!block) {
    rc = ATLK_E_OUT_OF_MEMORY;
    goto unlock;
  }

  bpool->free_list = bpool->free_list->next;
  block->next = (struct _atlk_bpool_block *)&bpool->magic;
  *user_block = &block->user_block;

unlock:
  atlk_mutex_unlock(&bpool->lock);

  return rc;
#endif /* __THREADX__ */
}

void
atlk_bpool_free(void *user_block)
{
  if (!user_block) {
    return;
  }

#ifdef __THREADX__
  UINT rc;

  rc = tx_block_release(user_block);
  BUG_ON(rc != TX_SUCCESS);
#else
  struct _atlk_bpool_block *block;
  atlk_bpool_t *bpool;
  uint32_t *magic;

  block = container_of(user_block, struct _atlk_bpool_block, user_block);
  magic = (uint32_t *)block->next;
  BUG_ON(*magic != ATLK_BPOOL_MAGIC);
  bpool = container_of(magic, atlk_bpool_t, magic);

  atlk_mutex_lock(&bpool->lock);

  BUG_ON((long *)block < bpool->area);
  BUG_ON((long *)block > bpool->area + bpool->area_size_long);

  block->next = bpool->free_list;
  bpool->free_list = block;

  atlk_mutex_unlock(&bpool->lock);
#endif /* __THREADX__ */
}

