/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <atlk/os.h>
#include <atlk/compiler.h>
#include <atlk/bpool.h>
#include <atlk/remote.h>

#include <atlk/remote_internal.h>

/**
   Definition of the internal remote transport structure
 */
struct remote_transport {
  /* Remote transport vtable
   *** IMPORTANT NOTE: Must be the first variable in this struct *** */
  remote_transport_vtable_t *vtable;
};

typedef struct remote_transport remote_transport;

atlk_rc_t
remote_transport_delete(remote_transport_t *transport_ptr)
{
  atlk_rc_t rc;
  remote_transport *transport = transport_ptr;

  if (atlk_unlikely(transport_ptr == NULL)) {
    TR_ERROR("transport_ptr is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Delete the transport */
  rc = transport->vtable->delete_transport(transport_ptr);
  if (atlk_error(rc)) {
    TR_ERROR("Could not delete remote transport");
    return rc;
  }

  return ATLK_OK;
}

atlk_must_check atlk_rc_t
remote_transport_send(remote_transport_t *transport_ptr,
                      int transport_handle,
                      uint16_t remote_port,
                      const void *pdu,
                      size_t *size)
{
  atlk_rc_t rc;
  remote_transport *transport = transport_ptr;

  /* Check validity of pointers */
  if (atlk_unlikely(!transport_ptr || !pdu || !size)) {
    TR_ERROR("One or more pointers are NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Send PDU */
  rc = transport->vtable->send(transport_ptr,
                               transport_handle,
                               remote_port,
                               pdu,
                               size);
  if (atlk_error(rc)) {
    TR_ERROR("Could not send remote PDU");
    return rc;
  }

  return ATLK_OK;
}

atlk_must_check atlk_rc_t
remote_transport_receive(remote_transport_t *transport_ptr,
                         int transport_handle,
                         void *pdu,
                         size_t *size,
                         uint32_t max_rtt_ms,
                         uint16_t *remort_port)
{
  atlk_rc_t rc;
  remote_transport *transport = transport_ptr;

  /* Check validity of pointers */
  if (atlk_unlikely(!transport_ptr || !pdu || !size)) {
    TR_ERROR("One or more pointers are NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Receive PDU */
  rc = transport->vtable->receive(transport_ptr,
                                  transport_handle,
                                  pdu,
                                  size,
                                  max_rtt_ms,
                                  remort_port);
  if (atlk_error(rc)) {
    TR_DEBUG("Could not receive remote PDU");
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
remote_transport_handle_allocate(remote_transport_t *transport_ptr,
                                 int *handle)
{
  atlk_rc_t rc;
  remote_transport *transport = transport_ptr;

  /* Check validity of pointers */
  if (atlk_unlikely(!transport_ptr || !handle)) {
    TR_ERROR("One or more pointers are NULL");
    return ATLK_E_INVALID_ARG;
  }

  rc = transport->vtable->handle_allocate(transport, handle);
  if (atlk_error(rc)) {
    TR_ERROR("Could not allocate remote transport_handle");
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
remote_transport_handle_deallocate(remote_transport_t *transport_ptr,
                                   int handle)
{
  atlk_rc_t rc;
  remote_transport *transport = transport_ptr;

  /* Check validity of pointers */
  if (atlk_unlikely(!transport_ptr)) {
    TR_ERROR("One or more pointers are NULL");
    return ATLK_E_INVALID_ARG;
  }

  rc = transport->vtable->handle_deallocate(transport, handle);
  if (atlk_error(rc)) {
    TR_ERROR("Could not allocate remote transport_handle");
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
remote_transport_local_ip_port_get(remote_transport_t *transport_ptr,
                                   int transport_handle,
                                   struct in_addr *local_ip_address,
                                   uint16_t *local_port_number)
{
  atlk_rc_t rc;
  remote_transport *transport = transport_ptr;

  /* Check validity of pointers */
  if (atlk_unlikely(!transport_ptr ||
                    !local_ip_address ||
                    !local_port_number)) {
    TR_ERROR("One or more pointers are NULL");
    return ATLK_E_INVALID_ARG;
  }

  rc = transport->vtable->local_ip_port_get(transport,
                                            transport_handle,
                                            local_ip_address,
                                            local_port_number);
  if (atlk_error(rc)) {
    TR_ERROR("Could not get the local ip/port associated"
             "with transport/handle");
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
remote_transport_local_port_set(remote_transport_t *transport_ptr,
                                int transport_handle,
                                uint16_t local_port_number)
{
  atlk_rc_t rc;
  remote_transport *transport = transport_ptr;

  /* Check validity of pointers */
  if (atlk_unlikely(!transport_ptr)) {
    TR_ERROR("transport_ptr is NULL");
    return ATLK_E_INVALID_ARG;
  }

  rc = transport->vtable->local_port_set(transport,
                                         transport_handle,
                                         local_port_number);
  if (atlk_error(rc)) {
    TR_ERROR("Could not set the local port associated"
             "with transport/handle");
    return rc;
  }

  return ATLK_OK;
}

atlk_must_check atlk_rc_t
remote_transport_ip_address_set(remote_transport_t *transport_ptr,
                                uint32_t remote_ip_address)
{
  atlk_rc_t rc;
  remote_transport *transport = transport_ptr;

  /* Check validity of pointers */
  if (atlk_unlikely(!transport_ptr)) {
    TR_ERROR("transport_ptr is NULL");
    return ATLK_E_INVALID_ARG;
  }

  rc = transport->vtable->remote_ip_address_set(transport,
                                                remote_ip_address);
  if (atlk_error(rc)) {
    TR_ERROR("Could not set the remote ip address associated"
             "with transport/handle");
    return rc;
  }

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
remote_transport_statistics_get(remote_transport_t *transport_ptr,
                                remote_transport_statistics_t *statistics)
{
  atlk_rc_t rc;
  remote_transport *transport = transport_ptr;

  /* NULL check is in the function below */
  rc = transport->vtable->statistics_get(transport, statistics);
  if (atlk_error(rc)) {
    TR_ERROR("Could not get the transport statistics, rc = %d", rc);
    return rc;
  }

  return ATLK_OK;
}

#ifdef ATLK_OS_POSIX

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

atlk_rc_t atlk_must_check
remote_util_local_ipv4_address_get(const char *interface_name,
                                   uint32_t *local_ip4_addr)
{
   int fd;
   int rv;
   struct ifreq ifr;

   if (!interface_name) {
     TR_ERROR("interface_name is NULL");
     return ATLK_E_INVALID_ARG;
   }

   if (!local_ip4_addr) {
     TR_ERROR("local_ip4_addr is NULL");
     return ATLK_E_INVALID_ARG;
   }

   fd = socket(AF_INET, SOCK_DGRAM, 0);
   if (fd == -1) {
     TR_ERROR("Fail to open socket");
     return ATLK_E_UNSPECIFIED;
   }

   /* I want to get an IPv4 IP address */
   ifr.ifr_addr.sa_family = AF_INET;

   /* I want IP address attached to "eth0" */
   strncpy(ifr.ifr_name, interface_name, IFNAMSIZ-1);

   rv = ioctl(fd, SIOCGIFADDR, &ifr);
   if (rv != 0) {
     TR_ERROR("Failed to call ioctl SIOCGIFADDR %d", rv);
     close(fd);
     return ATLK_E_UNSPECIFIED;
   }

   close(fd);

   memcpy(local_ip4_addr,
          &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr,
          sizeof(*local_ip4_addr));

   return ATLK_OK;
}

#endif /* ATLK_OS_POSIX */
