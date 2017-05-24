/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdlib.h>

#include <atlk/bpool.h>
#include <atlk/compiler.h>
#include <atlk/sdk.h>
#include <atlk/remote.h>

#include <atlk/mib_service.h>
#include <atlk/mib_remote.h>

#include <atlk/remote_internal.h>

/** mib_service default initializer */

#define MIB_SERVICE_INIT { \
    .transport = NULL, \
    .transport_handle = REMOTE_TRANSPORT_HANDLE_UNASSIGNED \
};
/* Assumption: Only a single MIB service is required per process
 *             ==> Singleton
 */
struct mib_service{
  /* remote_transport struct */
  remote_transport_t *transport;

  /* transport_handle - Handle to the remote_transport stream */
  int transport_handle;

};

atlk_rc_t atlk_must_check
mib_remote_service_create(remote_transport_t                *transport,
                          const mib_remote_service_config_t *config,
                          mib_service_t                     **service_ptr)
{
  atlk_rc_t rc;
  int transport_handle;
  mib_service_t *service = NULL;

  /* Checking config struct ptr is not done for now, (can be NULL) */

  /* check that transport is valid */
  if (atlk_unlikely(!transport)) {
    TR_ERROR("MIB transport pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* check that service_ptr is valid */
  if (atlk_unlikely(!service_ptr)) {
    TR_ERROR("service_ptr is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Allocate remote transport_handle */
  rc = remote_transport_handle_allocate(transport, &transport_handle);
  if (atlk_error(rc)) {
    return rc;
  }

  service = malloc(sizeof(*service));
  if (atlk_unlikely(!service)) {
    TR_ERROR("Failed to create mib_service");
    rc = remote_transport_handle_deallocate(transport, transport_handle);
    if (atlk_error(rc)) {
      TR_ERROR("Error deallocating the transport, rc = %d", rc);
    }
    return ATLK_E_OUT_OF_MEMORY;
  }

  /* Clear mib service */
  memset(service, 0x00, sizeof(*service));

  /* Assign transport */
  service->transport = transport;
  service->transport_handle = transport_handle;

  *service_ptr = service;

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
mib_remote_service_transport_get(mib_service_t      *service_ptr,
                                 remote_transport_t **transport)
{
  mib_service_t *svc = service_ptr;

  /* check that service_ptr is valid */
  if (atlk_unlikely(!service_ptr)) {
    TR_ERROR("MIB service_ptr pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* check that transport ptr is valid */
  if (atlk_unlikely(!transport)) {
    TR_ERROR("transport is NULL");
    return ATLK_E_INVALID_ARG;
  }

  *transport = svc->transport;

  return ATLK_OK;
}


atlk_rc_t atlk_must_check
mib_remote_service_transport_handle_get(mib_service_t *service_ptr,
                                        int *transport_handle)
{
  mib_service_t *svc = service_ptr;

  /* check that service_ptr is valid */
  if (atlk_unlikely(!service_ptr)) {
    TR_ERROR("MIB service_ptr pointer is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* check that transport ptr is valid */
  if (atlk_unlikely(!transport_handle)) {
    TR_ERROR("transport is NULL");
    return ATLK_E_INVALID_ARG;
  }

  *transport_handle = svc->transport_handle;

  return ATLK_OK;
}

atlk_rc_t
mib_default_service_get(mib_service_t **service)
{
  return ATLK_E_UNSUPPORTED;
}

atlk_rc_t
mib_service_delete(mib_service_t *service)
{
  atlk_rc_t rc;
  mib_service_t *svc = service;

  /* check that service_ptr is valid */
  if (atlk_unlikely(service == NULL)) {
    TR_ERROR("service pointer is invalid");
    return ATLK_E_INVALID_ARG;
  }

  /* Deallocate remote transport_handle */
  rc = remote_transport_handle_deallocate(svc->transport,
                                          svc->transport_handle);
  if (atlk_error(rc)) {
    TR_ERROR("Fail to delete transport handle");
    return rc;
  }

  /* On Host, the MIB service is a global ==>
   * Must set the transport pointer to NULL */
  svc->transport = NULL;

  free(service);

  return ATLK_OK;
}
