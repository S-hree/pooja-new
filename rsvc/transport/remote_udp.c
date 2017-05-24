/* Copyright (C) 2013-2015 Autotalks Ltd. */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef __CRATON__
#include <craton/net.h>
#include <nxd_bsd.h>
#else /* __CRATON__ */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#endif /* __CRATON__ */

#include <platform/common.h>

#include <atlk/atomic.h>
#include <atlk/compiler.h>
#include <atlk/bpool.h>
#include <atlk/remote.h>

#include <atlk/remote_internal.h>

#define REMOTE_TRANSPORT_MAX 2

#ifdef __CRATON__
#define INADDR_NONE 0xFFFFFFFF
#endif

/**
   Definition of the internal remote transport structure
 */
struct remote_transport {
  /* Remote transport vtable
   *** IMPORTANT NOTE: Must be the first variable in this struct *** */
  const remote_transport_vtable_t *vtable;

  /* Internal variables specific to the UDP remote_transport */
  /* Remote IP address */
  uint32_t        remote_ipv4_address;

  /* Local IP address (used to indicate which network interface to use) */
  uint32_t        local_ipv4_address;

  /* Max round-trip time in msec */
  uint32_t        max_rtt_ms;

  /* Number of handles allocated */
  atlk_atomic_t num_handles_allocated;

  /* Statistics of the remote_udp transport */
  remote_transport_statistics_t statistics;
};

typedef struct remote_transport remote_transport;

/**
 Allocates a transport handle (Associated with a logical stream)
 @param[in] transport_ptr Remote transport
 @param[in,out] transport_handle Handle (Associated with logical stream)
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
static atlk_must_check atlk_rc_t
remote_ip_handle_allocate(remote_transport_t *transport_ptr,
                          int *transport_handle);

/**
 Deallocates a transport handle (Associated with a logical stream)
 @param[in] transport_ptr Remote transport
 @param[in] transport_handle Handle to deallocate.
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
static atlk_must_check atlk_rc_t
remote_ip_handle_deallocate(remote_transport_t *transport_ptr,
                            int transport_handle);

/**
 Send a packet over the transport
 @param[in] transport_ptr Pointer to remote transport handler
 @param[in] handle transport handle
 @param[in] remote_port Remote port
 @param[in] pdu SNMP packet data
 @param[in] size Size of PDU in bytes
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
static atlk_must_check atlk_rc_t
remote_ip_transport_send(remote_transport_t *transport_ptr,
                         int transport_handle,
                         uint16_t remote_port,
                         const void *pdu,
                         size_t *size);

/**
 Get a packet over the transport
 @param[in] transport_ptr Pointer to remote transport handler
 @param[in] pdu SNMP packet data
 @param[in, size size Size of PDU in bytes
 @param[in] max_rtt_ms Timeout in milliseconds for response
 @param[out] remote_port remote port from which the packet arrived
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
static atlk_must_check atlk_rc_t
remote_ip_transport_receive(remote_transport_t *transport_ptr,
                            int transport_handle,
                            void *pdu,
                            size_t *size,
                            uint32_t max_rtt_ms,
                            uint16_t *remote_port);

/**
   Gets the IP address and port number associated with the transport->handle

   @param[in] transport_ptr Remote transport pointer
   @param[in] transport_handle Stream handle associated with remote transport
   @param[out] local_ip_address local IP address assigned to the socket
   @param[out] local_port_number local port number assigned to the socket

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
static atlk_must_check atlk_rc_t
remote_ip_transport_local_ip_port_get(remote_transport_t *transport_ptr,
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
static atlk_must_check atlk_rc_t
remote_ip_transport_local_port_set(remote_transport_t *transport_ptr,
                                   int transport_handle,
                                   uint16_t local_port_number);

/**
   Sets the remote ip address associated with the transport->handle

   @param[in] transport_ptr Remote transport pointer
   @param[in] remote_ip_address remote ip address associated with transport

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
static atlk_must_check atlk_rc_t
remote_ip_transport_remote_ip_address_set(remote_transport_t *transport_ptr,
                                          uint32_t remote_ip_address);

/**
 Deletes a remote UDP transport
 @param[in] transport_ptr Pointer to remote transport handler
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
static atlk_must_check atlk_rc_t
remote_ip_transport_delete(remote_transport_t *transport_ptr);


/**
 Get statistics of the transport
 @param[in] transport_ptr Remote transport
 @param[out] statistics transport statistics
 @retval ::ATLK_OK upon success
 @return Error code if failed
 */
static atlk_must_check atlk_rc_t
remote_ip_transport_statistics_get(remote_transport_t *transport_ptr,
                                   remote_transport_statistics_t *statistics);

#define REMOTE_TRANSPORT_VTABLE_INITIALIZE { \
    .handle_allocate = remote_ip_handle_allocate, \
    .handle_deallocate = remote_ip_handle_deallocate, \
    .receive = remote_ip_transport_receive, \
    .send = remote_ip_transport_send, \
    .local_ip_port_get = remote_ip_transport_local_ip_port_get, \
    .local_port_set = remote_ip_transport_local_port_set, \
    .remote_ip_address_set = remote_ip_transport_remote_ip_address_set, \
    .delete_transport = remote_ip_transport_delete, \
    .statistics_get = remote_ip_transport_statistics_get \
  }

static const remote_transport_vtable_t g_remote_udp_vtable =
    REMOTE_TRANSPORT_VTABLE_INITIALIZE;

/**
   Block pool stack to be used for the remote transports
 */
#define REMOTE_UDP_POOL_STACK_SIZE \
  ATLK_BPOOL_AREA_SIZE(sizeof(remote_transport), REMOTE_TRANSPORT_MAX)

static uint8_t g_pool_stack[REMOTE_UDP_POOL_STACK_SIZE];

static atlk_bpool_t manager_pool;

/**
   Static flag to determine whether the module is Initialized
 */
static int remote_client_initialized = 0;

/**
   Initialization of Remote client
*/
static atlk_must_check atlk_rc_t
remote_ip_init(void)
{
  atlk_rc_t rc;

  atlk_bpool_init(&manager_pool);

  rc = atlk_bpool_create(&manager_pool,
                         "remote manager pool",
                         (long *)g_pool_stack,
                         sizeof(remote_transport),
                         REMOTE_TRANSPORT_MAX);
  if (atlk_error(rc)) {
    TR_ERROR("Block pool creation failed with error %d", rc);
    return rc;
  }

  return rc;
}

static atlk_must_check atlk_rc_t
remote_ip_handle_allocate(remote_transport_t *transport_ptr,
                          int *transport_handle)
{
  int socket_fd;
  remote_transport *transport = transport_ptr;
  int rv;
  struct sockaddr_in socket_addr_in;

  if (atlk_unlikely(!transport_ptr)) {
    TR_ERROR("transport_ptr is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!transport_handle)) {
    TR_ERROR("transport_handle is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Create the socket */
  socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (socket_fd == -1) {
    TR_ERROR("could not open UDP socket, errno = %d", -errno);
    return ATLK_E_CONNECTION_REFUSED;
  }

  if (transport->local_ipv4_address != 0) {
    /* Bind to local IPv4 address set by user */
    memset(&socket_addr_in, 0, sizeof(socket_addr_in));

    socket_addr_in.sin_family = AF_INET;
    socket_addr_in.sin_port = htons(0);
    socket_addr_in.sin_addr.s_addr = transport->local_ipv4_address;
    rv = bind(socket_fd,
              (struct sockaddr *)&socket_addr_in,
              sizeof(socket_addr_in));
    if (FAILED(rv)) {
      TR_ERROR("bind failed, error: %d", -errno);
      close(socket_fd);
      return errno_to_atlk_rc(rv);
    }
  }

  /* Return the transport handle to user */
  *transport_handle = socket_fd;

  /* Increase the number of handles allocated */
  /* The remote object can be accessed from separate tasks in the system*/
  atlk_atomic_inc(&transport->num_handles_allocated);

  return ATLK_OK;
}

static atlk_must_check atlk_rc_t
remote_ip_handle_deallocate(remote_transport_t *transport_ptr,
                            int transport_handle)
{
  int rc;
  int socket_fd = transport_handle;
  remote_transport *transport = transport_ptr;

  if (atlk_unlikely(!transport_ptr)) {
    TR_ERROR("transport_ptr argument is NULL");
  }

  BUG_ON(atlk_unlikely(transport->num_handles_allocated.counter <= 0));

  /* Close the socket */
  rc = close(socket_fd);
  if (FAILED(rc)) {
    TR_ERROR("could not close the UDP socket. error = %d", -errno);
    return ATLK_E_INVALID_STATE;
  }

  /* The remote object can be accessed from separate tasks in the system*/
  atlk_atomic_dec(&transport->num_handles_allocated);

  return ATLK_OK;
}

atlk_rc_t atlk_must_check
remote_ip_transport_create(const remote_ip_transport_config_t *config,
                           remote_transport_t **transport_ptr)
{
  atlk_rc_t rc;
  remote_transport *transport;

  if (atlk_unlikely(!transport_ptr || !config)) {
    TR_ERROR("Invalid input parameters");
    return ATLK_E_INVALID_ARG;
  }

  if (!remote_client_initialized) {
    rc = remote_ip_init();
    if (atlk_error(rc)) {
      TR_ERROR("Could not initialize remote client module, rc = %d", rc);
      return rc;
    }
  }

  rc = atlk_bpool_alloc(&manager_pool, (void **)&transport);
  if (atlk_error(rc)) {
    TR_ERROR("Failed to allocate block, rc = %d", rc);
    return rc;
  }

  /* Initialize vtable */
  transport->vtable = &g_remote_udp_vtable;

  transport->max_rtt_ms = config->max_rtt_ms;
  transport->remote_ipv4_address = config->remote_ipv4_address;
  transport->local_ipv4_address = config->local_ipv4_address;

  /* Initialize number of handles associated with transport object to 0 */
  transport->num_handles_allocated.counter = 0;

  /* Point to the transport */
  *transport_ptr = (remote_transport_t *)transport;

  remote_client_initialized = 1;

  return ATLK_OK;
}

static atlk_rc_t
remote_ip_transport_delete(remote_transport_t *transport_ptr)
{
  remote_transport *transport = transport_ptr;

  if (atlk_unlikely(transport == NULL)) {
    return ATLK_E_INVALID_ARG;
  }

  /* Do we have any open handles associated with this object? */
  BUG_ON(atlk_unlikely(transport->num_handles_allocated.counter > 0));

  atlk_bpool_free(transport);

  return ATLK_OK;
}

static atlk_rc_t atlk_must_check
remote_ip_transport_send(remote_transport_t *transport_ptr,
                         int transport_handle,
                         uint16_t remote_port,
                         const void *pdu,
                         size_t *size)
{
  int rv;
  int socket_fd = transport_handle;
  remote_transport *transport = transport_ptr;
  struct sockaddr_in dest_addr;
  void *pdu_ptr = (void *)pdu;

  if (atlk_unlikely(*size <= 0)) {
    TR_ERROR("*size <= 0, *size = %d", (unsigned int)*size);
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!transport_ptr || !pdu || !size)) {
    TR_ERROR("One or more arguments is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Prepare the socket send request */
  memset(&dest_addr, 0x00, sizeof(dest_addr));
  dest_addr.sin_family      = AF_INET;
  /* dest port */
  dest_addr.sin_port        = htons(remote_port);
  /* dest IPv4 address */
  dest_addr.sin_addr.s_addr = transport->remote_ipv4_address;
  if (dest_addr.sin_addr.s_addr == INADDR_NONE) {
    TR_ERROR("Invalid IP address %d",
             (unsigned int)transport->remote_ipv4_address);
    return ATLK_E_INVALID_ARG;
  }

  /* Socket send */
  rv = sendto(socket_fd,
              pdu_ptr,
              *size,
              0,
              (struct sockaddr *)&dest_addr,
              sizeof(dest_addr));
  if (FAILED(rv)) {
    TR_ERROR("Failed sending PDU, error = %d", -errno);
    /* Update statistics */
    transport->statistics.packets_tx_fail_count++;
    *size = 0;
    return errno_to_atlk_rc(rv);
  }

  *size = (size_t)rv;

  /* Update statistics */
  transport->statistics.packets_tx_count++;
  transport->statistics.total_bytes_tx += *size;

  return ATLK_OK;
}

static atlk_rc_t atlk_must_check
remote_ip_transport_receive(remote_transport_t *transport_ptr,
                            int transport_handle,
                            void *pdu,
                            size_t *size,
                            uint32_t max_rtt_ms,
                            uint16_t *remote_port)
{
  int socket_fd = transport_handle;
  int                rv;
  struct sockaddr_in agent_addr;
  socklen_t          addr_len;
  int                socket_fdmax;
  fd_set             read_socket_fds;
  struct timeval     timeout;
  struct timeval     *timeout_ptr;
  uint32_t           _max_rtt_ms;
  remote_transport   *transport = transport_ptr;

  if (atlk_unlikely(*size <= 0)) {
    TR_ERROR("*size <= 0, *size = %d", (unsigned int)*size);
    return ATLK_E_INVALID_ARG;
  }

  if (atlk_unlikely(!transport_ptr || !pdu || !size)) {
    TR_ERROR("One or more arguments is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Configure the timeout according to user request */
  timeout_ptr = &timeout;
  switch (max_rtt_ms) {
  case REMOTE_TRANSPORT_NO_TIMEOUT:
    /* Setup no timeout... */
    timeout_ptr = NULL;
    break;
  case REMOTE_TRANSPORT_DEFAULT_RTT:
    _max_rtt_ms = transport->max_rtt_ms;
    break;
  default:
    _max_rtt_ms = max_rtt_ms;
    break;
  }
  if (timeout_ptr != NULL) {
    timeout.tv_sec = _max_rtt_ms / MILLI_PER_UNIT;
    timeout.tv_usec = (_max_rtt_ms % MILLI_PER_UNIT) * MICRO_PER_MILLI;
  }

  addr_len = sizeof(agent_addr);

  /* Preparation for select() */
  FD_ZERO(&read_socket_fds);
  FD_SET(socket_fd, &read_socket_fds);
  socket_fdmax = socket_fd + 1;

  rv = select(socket_fdmax, &read_socket_fds, NULL, NULL, timeout_ptr);
  if (FAILED(rv)) {
    TR_ERROR("Select failed, error: %d", -errno);
    return ATLK_E_UNSPECIFIED;
  }

  if (rv == 0) {
    /* Timeout occured */
    TR_DEBUG("PDU receive timed out");
    return ATLK_E_TIMEOUT;
  }

  rv = recvfrom(socket_fd,
                pdu,
                *size,
                0,
                (struct sockaddr *)&agent_addr,
                &addr_len);
  if (FAILED(rv)) {
    TR_ERROR("Failed to receive PDU, error = %d", -errno);
    *size = 0;
    return errno_to_atlk_rc(rv);
  }

  /* Always positive here */
  *size = (size_t)rv;

  /* Set the remote address to be agent_addr */
  transport->remote_ipv4_address = agent_addr.sin_addr.s_addr;

  /* return the remote port of the packet to user */
  *remote_port = htons(agent_addr.sin_port);

  /* Update statistics */
  transport->statistics.packets_rx_count++;
  transport->statistics.total_bytes_rx += *size;

  return ATLK_OK;
}

static atlk_must_check atlk_rc_t
remote_ip_transport_local_ip_port_get(remote_transport_t *transport_ptr,
                                      int transport_handle,
                                      struct in_addr *local_ip_address,
                                      uint16_t *local_port_number)
{
  int rv;
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  int socket_fd = transport_handle;

  if (atlk_unlikely(!transport_ptr ||
                    !local_ip_address ||
                    !local_port_number)) {
    TR_ERROR("One or more arguments is NULL");
    return ATLK_E_INVALID_ARG;
  }

  /* Get local IP address and port_number */
  rv = getsockname(socket_fd, (struct sockaddr *)&sin, &len);
  if (FAILED(rv)) {
    TR_ERROR("getsockname() failed");
    return ATLK_E_UNSPECIFIED;
  }
  else {
    /* Copy the IP address */
    memcpy(local_ip_address, &sin.sin_addr, sizeof(struct in_addr));
    /* Copy the local IP */
    *local_port_number = ntohs(sin.sin_port);
  }

  return ATLK_OK;
}

static atlk_must_check atlk_rc_t
remote_ip_transport_local_port_set(remote_transport_t *transport_ptr,
                                   int transport_handle,
                                   uint16_t local_port_number)
{
  int rv;
  int socket_fd = transport_handle;
  struct sockaddr_in socket_addr_in;

  if (atlk_unlikely(!transport_ptr)) {
    TR_ERROR("One or more arguments is NULL");
    return ATLK_E_INVALID_ARG;
  }

  memset(&socket_addr_in, 0, sizeof(socket_addr_in));

  socket_addr_in.sin_family = AF_INET;
  socket_addr_in.sin_port = htons(local_port_number);
  socket_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
  rv = bind(socket_fd,
            (struct sockaddr *)&socket_addr_in,
            sizeof(socket_addr_in));
  if (FAILED(rv)) {
    TR_ERROR("bind failed, error: %d", -errno);
    return errno_to_atlk_rc(rv);
  }

  return ATLK_OK;
}

static atlk_must_check atlk_rc_t
remote_ip_transport_remote_ip_address_set(remote_transport_t *transport_ptr,
                                          uint32_t remote_ip_address)
{
  remote_transport *transport = transport_ptr;

  if (atlk_unlikely(!transport_ptr)) {
    TR_ERROR("One or more arguments is NULL");
    return ATLK_E_INVALID_ARG;
  }

  if (transport->remote_ipv4_address == 0) {
    /* Update the remote ip address */
    transport->remote_ipv4_address = remote_ip_address;
  }

  return ATLK_OK;
}

static atlk_must_check atlk_rc_t
remote_ip_transport_statistics_get(remote_transport_t *transport_ptr,
                                   remote_transport_statistics_t *statistics)
{
  remote_transport *transport = transport_ptr;

  if (atlk_unlikely(!transport_ptr || !statistics)) {
    TR_ERROR("One or more arguments is NULL");
    return ATLK_E_INVALID_ARG;
  }

  memcpy(statistics,
         &transport->statistics,
         sizeof(remote_transport_statistics_t));

  return ATLK_OK;
}
