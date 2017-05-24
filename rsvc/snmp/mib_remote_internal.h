/* Copyright (C) 2015 Autotalks Ltd. */
#ifndef _MIB_REMOTE_INTERNAL_H
#define _MIB_REMOTE_INTERNAL_H


/**
   Gets the remote service transport associated with remote MIB service.

   @param[in] service_ptr MIB service
   @param[out] transport Remote transport instance

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
mib_remote_service_transport_get(mib_service_t      *service_ptr,
                                 remote_transport_t **transport);

/**
   Gets the remote service transport_handle associated the remote MIB service
   transport.

   @param[in] service_ptr MIB service
   @param[out] transport_handle Remote transport handle

   @retval ::ATLK_OK if succeeded
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
mib_remote_service_transport_handle_get(mib_service_t *service_ptr,
                                        int *transport_handle);


#endif /* _MIB_REMOTE_INTERNAL_H */
