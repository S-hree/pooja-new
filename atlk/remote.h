/* Copyright (C) 2015 Autotalks Ltd. */
#ifndef _ATLK_REMOTE_H
#define _ATLK_REMOTE_H

#include <atlk/sdk.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   Transport for remote service access
*/

/** Remote transport instance */
typedef struct remote_transport remote_transport_t;

/** IP remote transport configuration */
typedef struct {
  /** IPv4 server address in network order */
  uint32_t remote_ipv4_address;

  /** Max round-trip time in milliseconds */
  uint32_t max_rtt_ms;

  /** Local IPv4 address in network order */
  uint32_t local_ipv4_address;

} remote_ip_transport_config_t;

/** IP remote transport configuration default initializer */
#define REMOTE_IP_TRANSPORT_CONFIG_INIT {   \
  .remote_ipv4_address = 0,                 \
  .max_rtt_ms = 0,                          \
  .local_ipv4_address = 0,                  \
}

/**
   Create IP-based remote transport instance.

   @param[in] config IP remote transport configuration
   @param[out] transport_ptr Remote transport pointer

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
remote_ip_transport_create(const remote_ip_transport_config_t *config,
            remote_transport_t **transport_ptr);

/**
   Delete a remote transport instance.

   @param[in] transport_ptr Remote transport pointer

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t
remote_transport_delete(remote_transport_t *transport_ptr);

/**
   Get local IPv4 address of a Network interface.

   @param[in] interface_name Interface name (e.g. "eth0")
   @param[out] local_ipv4_address Local IPv4 address

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
remote_util_local_ipv4_address_get(const char *interface_name,
            uint32_t *local_ipv4_address);

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_REMOTE_H */
