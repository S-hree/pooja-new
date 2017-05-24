#include <atlk/sdk.h>
#include <platform/common.h>
#include <atlk/objpool.h>

#define ATLK_OBJPOOL_MAGIC 0xBABAD00D

atlk_inline struct _atlk_objpool_object *
atlk_objpool_object_next(struct _atlk_objpool_object *object,
  atlk_objpool_t *objpool)
{
  return (struct _atlk_objpool_object *)((long *)object + \
    objpool->object_size_long);
}

atlk_inline struct _atlk_objpool_object *
atlk_objpool_object_prev(struct _atlk_objpool_object *object,
  atlk_objpool_t *objpool)
{
  return (struct _atlk_objpool_object *)((long *)object - \
    objpool->object_size_long);
}

atlk_rc_t
atlk_objpool_create(atlk_objpool_t *objpool, const char *name,
  long *memory, size_t object_size, size_t object_count,
  atlk_objpool_ctor_t ctor, atlk_objpool_dtor_t dtor, void *opaque)
{
  struct _atlk_objpool_object *object;
  uint32_t i;
  atlk_rc_t rc;

  /* Validate arguments */
  if (!objpool || !memory) {
    TR_ERROR_NO_ARGS("Mandatory argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  if ((uintptr_t)memory % sizeof(long)) {
    TR_ERROR_NO_ARGS("Object pool memory area must be aligned to long");
    return ATLK_E_INVALID_ARG;
  }

  if (objpool->memory) {
    TR_ERROR_NO_ARGS("Object pool is not initialized or already created");
    return ATLK_E_INVALID_STATE;
  }

  /* Create object pool lock */
  rc = atlk_mutex_create(&objpool->lock, objpool->name);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to create object pool lock (rc=%d)", rc);
    return rc;
  }

  objpool->name = name;
  objpool->object_count = object_count;
  objpool->object_size_long = _ATLK_OBJPOOL_OBJECT_SIZE_LONG(object_size);
  objpool->memory = memory;
  objpool->memory_size_long = \
    _ATLK_OBJPOOL_MEMORY_SIZE_LONG(object_size, object_count);
  objpool->magic = ATLK_OBJPOOL_MAGIC;
  objpool->dtor = dtor;
  objpool->opaque = opaque;

  object = (struct _atlk_objpool_object *)objpool->memory;
  objpool->free_head = object;
  for (i = 0; i < objpool->object_count; i++) {
    object->next = atlk_objpool_object_next(object, objpool);
    if (ctor) {
      rc = ctor(&object->user_object, opaque);
      if (atlk_error(rc)) {
        goto destroy;
      }
    }
    object = object->next;
  }

  /* Fix the last block */
  object = atlk_objpool_object_prev(object, objpool);
  object->next = NULL;
  objpool->free_tail = object;

  return ATLK_OK;

destroy:
  if (dtor) {
    object = atlk_objpool_object_prev(object, objpool);
    while ((long *)object >= objpool->memory) {
      dtor(&object->user_object, opaque);
      object = atlk_objpool_object_prev(object, objpool);
    }
  }
  atlk_mutex_destroy(&objpool->lock);
  *objpool = (atlk_objpool_t)ATLK_OBJPOOL_INIT;

  return rc;
}

atlk_rc_t atlk_must_check
atlk_objpool_alloc(atlk_objpool_t *objpool, void **user_object)
{
  struct _atlk_objpool_object *object;
  atlk_rc_t rc = ATLK_OK;

  if (!objpool || !user_object) {
    TR_ERROR_NO_ARGS("Mandatory argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  atlk_mutex_lock(&objpool->lock);

  object= objpool->free_head;
  if (!object) {
    rc = ATLK_E_OUT_OF_MEMORY;
    goto unlock;
  }

  objpool->free_head = objpool->free_head->next;
  if (!objpool->free_head) {
    objpool->free_tail = NULL;
  }

  object->next = (struct _atlk_objpool_object *)&objpool->magic;
  *user_object =  &object->user_object;

unlock:
  atlk_mutex_unlock(&objpool->lock);

  return rc;
}

void
atlk_objpool_free(void *user_object)
{
  atlk_objpool_t *objpool; 
  struct _atlk_objpool_object *object;
  uint32_t *magic;

  if (!user_object) {
    return;
  }

  object = container_of(user_object, struct _atlk_objpool_object, user_object);
  magic = (uint32_t *)object->next;
  BUG_ON(*magic != ATLK_OBJPOOL_MAGIC);
  objpool = container_of(magic, atlk_objpool_t, magic);

  atlk_mutex_lock(&objpool->lock);

  BUG_ON((long *)object < objpool->memory);
  BUG_ON((long *)object > objpool->memory + objpool->memory_size_long);

  object->next = NULL;
  if (!objpool->free_tail) {
    objpool->free_head = object;
  }
  else {
    objpool->free_tail->next = object;
  }
  objpool->free_tail = object;

  atlk_mutex_unlock(&objpool->lock);
}

void
atlk_objpool_destroy(atlk_objpool_t *objpool)
{
  if (!objpool->memory) {
    return;
  }

  atlk_mutex_destroy(&objpool->lock);

  if (objpool->dtor) {
    struct _atlk_objpool_object *object;
    uint32_t i;

    object = (struct _atlk_objpool_object *)objpool->memory;
    for (i = 0; i < objpool->object_count; i++) {
      objpool->dtor(&object->user_object, objpool->opaque);
      object = atlk_objpool_object_next(object, objpool);
    }
  }

  *objpool = (atlk_objpool_t)ATLK_OBJPOOL_INIT;
}

