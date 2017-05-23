/* Copyright (C) 2012-2016 Autotalks Ltd. */
#ifndef _REMOTE_INTERNAL_H
#define _REMOTE_INTERNAL_H

#ifndef __CRATON__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#else
#include <craton/net.h>
#include <nxd_bsd.h>
#endif
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <platform/common.h>
#include <atlk/remote.h>
#include <atlk/rsvc_protocol.h>
#include <atlk/rsvc_v2x_protocol.h>
#include <atlk/rsvc_ecc_protocol.h>

/* Default Round Trip Time (i.e. use the default configured for transport) */
#define REMOTE_TRANSPORT_DEFAULT_RTT (0xFFFFFFFF)
/* No timeout*/
#define REMOTE_TRANSPORT_NO_TIMEOUT  (0xFFFFFFFE)

/* Unassigned transport handle value */
#define REMOTE_TRANSPORT_HANDLE_UNASSIGNED -1

/* Max remote PDU size */
#define RSVC_V2X_MAX_PDU_SIZE (1400 + \
            MAX(sizeof(dx_rsvc_v2x_receive_indication_header_t), \
                sizeof(dx_rsvc_v2x_send_req_header_t)))

#define RSVC_ECC_MAX_PDU_SIZE 1024

#define RSVC_MAX_PDU_SIZE \
            MAX(RSVC_V2X_MAX_PDU_SIZE, RSVC_ECC_MAX_PDU_SIZE)

typedef struct {
  /* Number of packets Tx successfully */
  uint32_t packets_tx_count;

  /* Number of packets failed to Tx */
  uint32_t packets_tx_fail_count;

  /* Number of packets received */
  uint32_t packets_rx_count;

  /* Total bytes received */
  uint64_t total_bytes_rx;

  /* Total bytes sent */
  uint64_t total_bytes_tx;

} remote_transport_statistics_t;

/* Generic vtable interface for the remote transport layer */
typedef struct {
  /**
   Allocates a transport handle (Associated with a logical stream)
   @param[in] transport_ptr Remote transport
   @param[in,out] transport_handle Handle (Associated with logical stream)
   @retval ::ATLK_OK upon success
   @return Error code if failed
   */
  atlk_must_check atlk_rc_t
  (*handle_allocate)(remote_transport_t *transport_ptr,
                     int *transport_handle);

  /**
   Deallocates a transport handle (Associated with a logical stream)
   @param[in] transport_ptr Remote transport
   @param[in] transport_handle Handle to deallocate.
   @retval ::ATLK_OK upon success
   @return Error code if failed
   */
  atlk_must_check atlk_rc_t
  (*handle_deallocate)(remote_transport_t *transport_ptr,
                       int transport_handle);

  /**
   Send PDU
   @param[in] transport_ptr Remote transport
   @param[in] transport_handle Handle (Associated with logical stream)
   @param[in] remote_port Remote port
   @param[in] pdu Pointer to pdu
   @param[in,out] size Size of PDU in bytes (IN) ==> Size actually sent (OUT)
   @retval ::ATLK_OK upon success
   @return Error code if failed
   */
  atlk_must_check atlk_rc_t
  (*send)(remote_transport_t *transport_ptr,
          int transport_handle,
          uint16_t remote_port,
          const void *pdu,
          size_t *size);

  /**
   Receive PDU
   @param[in] transport_ptr Remote transport
   @param[in] transport_handle Handle (Associated with logical stream)
   @param[in] pdu Pointer to pdu
   @param[in] size Size of data to send
   @param[in] max_rtt_ms Timeout:
              - REMOTE_TRANSPORT_DEFAULT_RTT => default transport timeout
              - REMOTE_TRANSPORT_NO_TIMEOUT => No timeout
              - Other value => Override transport timeout with this value
   @param[out] remote_port remote port from which the packet arrived
   @retval ::ATLK_OK upon success
   @return Error code if failed
   */
  atlk_must_check atlk_rc_t
  (*receive)(remote_transport_t *transport_ptr,
             int transport_handle,
             void *pdu,
             size_t *size,
             uint32_t max_rtt_max,
             uint16_t *remote_port);

  /**
     Gets the IP address and port number associated with the transport->handle

     @param[in] transport_ptr Remote transport pointer
     @param[in] handle Handle to stream associated with remote transport
     @param[in] local_ip_address local IP address assigned to the socket
     @param[in] local_port_number local port number assigned to the socket

     @retval ::ATLK_OK if succeeded
     @return Error code if failed
  */
  atlk_must_check atlk_rc_t
  (*local_ip_port_get)(remote_transport_t *transport_ptr,
                       int transport_handle,
                       struct in_addr *local_ip_address,
                       uint16_t *local_port_number);

  /**
     Sets the local port number associated with the transport->handle

     @param[in] transport_ptr Remote transport pointer
     @param[in] transport_handle Stream handle associated with remote transport
     @param[in] local_port_number local port number assigned to the socket

     @retval ::ATLK_OK if succeeded
     @return Error code if failed
  */
  atlk_rc_t atlk_must_check
  (*local_port_set)(remote_transport_t *transport_ptr,
                    int transport_handle,
                    uint16_t local_port_number);

  /**
     Sets the remote ip address associated with the transport->handle

     @param[in] transport_ptr Remote transport pointer
     @param[in] remote_ip_address remote ip address associated with transport

     @retval ::ATLK_OK if succeeded
     @return Error code if failed
  */
  atlk_must_check atlk_rc_t
  (*remote_ip_address_set)(remote_transport_t *transport_ptr,
                           uint32_t remote_ip_address);

  /**
   Delete transport
   @param[in] transport_ptr Remote transport
   @retval ::ATLK_OK upon success
   @return Error code if failed
   */
  atlk_must_check atlk_rc_t
  (*delete_transport)(remote_transport_t *transport_ptr);

  /**
   Get statistics of the transport
   @param[in] transport_ptr Remote transport
   @param[out] statistics transport statistics
   @retval ::ATLK_OK upon success
   @return Error code if failed
   */
  atlk_must_check atlk_rc_t
  (*statistics_get)(remote_transport_t *transport_ptr,
                    remote_transport_statistics_t *statistics);

} remote_transport_vtable_t;

/**
 Send PDU
 @param[in] transport_ptr Remote transport
 @param[in] transport_handle Handle (Associated with logical stream)
 @param[in] remote_port Remote port
 @param[in] pdu Pointer to pdu
 @param[in,out] size Size of PDU in bytes (IN) ==> Size actually sent (OUT)
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
remote_transport_send(remote_transport_t *transport_ptr,
                      int transport_handle,
                      uint16_t  remote_port,
                      const void *pdu,
                      size_t *size);

/**
 Receive PDU
 @param[in] transport_ptr Remote transport
 @param[in] transport_handle Handle (Associated with logical stream)
 @param[in] pdu Pointer to pdu
 @param[in] size Size of data received
 @param[in] max_rtt_ms Timeout:
            - REMOTE_TRANSPORT_DEFAULT_RTT => use default transport timeout
            - REMOTE_TRANSPORT_NO_TIMEOUT => No timeout
            - Other value => Override default transport timeout with the value
 @param[out] remote_port remote port from which the packet arrived
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_must_check atlk_rc_t
remote_transport_receive(remote_transport_t *transport_ptr,
                         int transport_handle,
                         void *pdu,
                         size_t *size,
                         uint32_t max_rtt_ms,
                         uint16_t *remote_port);

/**
   Allocates a remote transport handle to stream of data (i.e. associate
   with port_number).

   @param[in] transport_ptr Remote transport pointer
   @param[out] handle Handle to stream associated with remote transport

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
remote_transport_handle_allocate(remote_transport_t *transport_ptr,
                                 int *handle);

/**
   Allocates a remote transport handle to stream of data (i.e. associate
   with port_number).

   @param[in] transport_ptr Remote transport pointer
   @param[in] handle Handle to stream associated with remote transport

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
remote_transport_handle_deallocate(remote_transport_t *transport_ptr,
                                   int handle);

/**
   Gets the IP address and port number associated with the transport->handle

   @param[in] transport_ptr Remote transport pointer
   @param[in] handle Handle to stream associated with remote transport
   @param[out] local_ip_address local IP address assigned to the socket
   @param[out] local_port_number local port number assigned to the socket

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
remote_transport_local_ip_port_get(remote_transport_t *transport_ptr,
                                   int transport_handle,
                                   struct in_addr *local_ip_address,
                                   uint16_t *local_port_number);

/**
   Sets the local port number associated with the transport->handle

   @param[in] transport_ptr Remote transport pointer
   @param[in] transport_handle Stream handle associated with remote transport
   @param[in] local_port_number local port number assigned to the socket

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
remote_transport_local_port_set(remote_transport_t *transport_ptr,
                                int transport_handle,
                                uint16_t local_port_number);


/**
 Get statistics of the transport
 @param[in] transport_ptr Remote transport
 @param[out] statistics transport statistics
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
atlk_rc_t atlk_must_check
remote_transport_statistics_get(remote_transport_t *transport_ptr,
                                remote_transport_statistics_t *statistics);

/**
   Sets the remote ip address associated with the transport->handle

   @param[in] transport_ptr Remote transport pointer
   @param[in] remote_ip_address remote ip address associated with transport

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_must_check atlk_rc_t
remote_transport_ip_address_set(remote_transport_t *transport_ptr,
                                uint32_t remote_ip_address);

#endif /* _REMOTE_INTERNAL_H */
