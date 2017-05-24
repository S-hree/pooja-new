/*
 * sl_config.h - getting_started_ap sample application
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
#ifndef __SL_CONFIG_H__
#define __SL_CONFIG_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

#include <tx_api.h>
#include <assert.h>

/**/
#define LOOP_FOREVER() \
            {\
                while(1) tx_thread_sleep(100); \
            }

#define ASSERT_ON_ERROR(error_code) \
            do {\
                /* Handling the error-codes is specific to the application */ \
                if (error_code < 0) assert(0); \
                /* else, continue w/ execution */ \
            } while(0)

#define pal_Memset(x,y,z)   memset((void *)x,y,z)
#define pal_Memcpy(x,y,z)   memcpy((void *)x, (const void *)y, z)
#define pal_Memcmp(x,y,z)   memcmp((const void *)x, (const void *)y, z)
#define pal_Strlen(x)       strlen((const char *)x)
#define pal_Strcmp(x,y)     strcmp((const char *)x, (const char *)y)
#define pal_Strcpy(x,y)     strcpy((char *)x, (const char *)y)
#define pal_Strstr(x,y)     strstr((const char *)x, (const char *)y)
#define pal_Strncmp(x,y,z)  strncmp((const char *)x, (const char *)y, z)
#define pal_Strcat(x,y)     strcat((char *)x, (const char *)y)

/*
 * Values for below macros shall be modified per the access-point's (AP) properties
 * SimpleLink device will connect to following AP when the application is executed
 */
/* Configuration of the device when it comes up in Station-mode */
extern char SSID_NAME[];
extern char PASSKEY[];
extern int SEC_TYPE;
extern int PASSKEY_LEN;

/* Configuration of the device when it comes up in AP-mode */
extern char SSID_AP_MODE[];
extern char PASSWORD_AP_MODE[];
extern int SEC_TYPE_AP_MODE;

/*
 * Values for below macros shall be modified based on current time
 */
#define DATE        24      /* Current Date */
#define MONTH       7       /* Month */
#define YEAR        2014    /* Current year */
#define HOUR        17      /* Time - hours */
#define MINUTE      30      /* Time - minutes */
#define SECOND      0       /* Time - seconds */

#define SUCCESS             0

/* Status bits - These are used to set/reset the corresponding bits in a 'status_variable' */
typedef enum{
    STATUS_BIT_CONNECTION =  0, /* If this bit is:
                                 *      1 in a 'status_variable', the device is connected to the AP
                                 *      0 in a 'status_variable', the device is not connected to the AP
                                 */

    STATUS_BIT_STA_CONNECTED,    /* If this bit is:
                                  *      1 in a 'status_variable', client is connected to device
                                  *      0 in a 'status_variable', client is not connected to device
                                  */

    STATUS_BIT_IP_ACQUIRED,       /* If this bit is:
                                   *      1 in a 'status_variable', the device has acquired an IP
                                   *      0 in a 'status_variable', the device has not acquired an IP
                                   */

    STATUS_BIT_IP_LEASED,           /* If this bit is:
                                      *      1 in a 'status_variable', the device has leased an IP
                                      *      0 in a 'status_variable', the device has not leased an IP
                                      */

    STATUS_BIT_CONNECTION_FAILED,   /* If this bit is:
                                     *      1 in a 'status_variable', failed to connect to device
                                     *      0 in a 'status_variable'
                                     */

    STATUS_BIT_P2P_NEG_REQ_RECEIVED,/* If this bit is:
                                     *      1 in a 'status_variable', connection requested by remote wifi-direct device
                                     *      0 in a 'status_variable',
                                     */
    STATUS_BIT_SMARTCONFIG_DONE,    /* If this bit is:
                                     *      1 in a 'status_variable', smartconfig completed
                                     *      0 in a 'status_variable', smartconfig event couldn't complete
                                     */

    STATUS_BIT_SMARTCONFIG_STOPPED  /* If this bit is:
                                     *      1 in a 'status_variable', smartconfig process stopped
                                     *      0 in a 'status_variable', smartconfig process running
                                     */

}e_StatusBits;

#define SET_STATUS_BIT(status_variable, bit)    status_variable |= ((unsigned long)1<<(bit))
#define CLR_STATUS_BIT(status_variable, bit)    status_variable &= ~((unsigned long)1<<(bit))
#define GET_STATUS_BIT(status_variable, bit)    (0 != (status_variable & ((unsigned long)1<<(bit))))

#define IS_CONNECTED(status_variable)             GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_CONNECTION)
#define IS_STA_CONNECTED(status_variable)         GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_STA_CONNECTED)
#define IS_IP_ACQUIRED(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_ACQUIRED)
#define IS_IP_LEASED(status_variable)             GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_LEASED)
#define IS_CONNECTION_FAILED(status_variable)     GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_CONNECTION_FAILED)
#define IS_P2P_NEG_REQ_RECEIVED(status_variable)  GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_P2P_NEG_REQ_RECEIVED)
#define IS_SMARTCONFIG_DONE(status_variable)      GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_SMARTCONFIG_DONE)
#define IS_SMARTCONFIG_STOPPED(status_variable)   GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_SMARTCONFIG_STOPPED)

typedef void (*simpleLinkSockEventHandler_t)(SlSockEvent_t*);
typedef void (*simpleLinkHttpServerCallback_t)(SlHttpServerEvent_t *,SlHttpServerResponse_t *pSlHttpServerResponse);
typedef void (*simpleLinkGeneralEventHandler_t)(SlDeviceEvent_t *pSlDeviceEvent);
typedef void (*simpleLinkNetAppEventHandler_t)(SlNetAppEvent_t *pSlNetApp);
typedef void (*simpleLinkWlanEventHandler_t)(SlWlanEvent_t *pSlWlanEvent);

extern simpleLinkSockEventHandler_t simpleLinkSockEventHandler;
extern simpleLinkWlanEventHandler_t simpleLinkWlanEventHandler;
extern simpleLinkHttpServerCallback_t simpleLinkHttpServerCallback;
extern simpleLinkNetAppEventHandler_t simpleLinkNetAppEventHandler;
extern simpleLinkGeneralEventHandler_t simpleLinkGeneralEventHandler;

extern void CLI_Configure(void);
extern int CLI_Write(unsigned char *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__SL_CONFIG_H__*/
