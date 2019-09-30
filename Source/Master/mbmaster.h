/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBMASTER_H__
#define MBMASTER_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mbmaster_cfg.h>

#include <mbmaster_cmdlet_common.h>
#include <mbmaster_cmdlet_readcoils.h>
#include <mbmaster_cmdlet_readdiscreteinputs.h>
#include <mbmaster_cmdlet_readholdregisters.h>
#include <mbmaster_cmdlet_readinputregisters.h>
#include <mbmaster_cmdlet_writesinglecoil.h>
#include <mbmaster_cmdlet_writesingleregister.h>
#include <mbmaster_cmdlet_writemultiplecoils.h>
#include <mbmaster_cmdlet_writemultipleregisters.h>
#include <mbmaster_cmdlet_maskwriteregister.h>
#include <mbmaster_cmdlet_rwmultipleregisters.h>

#include <mb_os_types.h>
#include <mb_os_basetypes.h>

#include <mb_types.h>

#include <lib_def.h>

#include <app_cfg.h>


#if (MB_CFG_MASTER_EN == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    MB_IFINDEX          iface;

    CPU_INT08U         *bufRxTx;
    CPU_SIZE_T          bufRxTxSize;

    MB_TIMESPAN         dlyTurnAround;

    CPU_BOOLEAN         busy;
} MBMASTER;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                   MBMaster_Initialize()
*
* Description : Initialize a Modbus master.
*
* Argument(s) : (1) p_master              Pointer to the master object.
*               (2) ifnbr                 Modbus device interface ID of the slave.
*               (3) p_buf                 Pointer to the first element of the data buffer.
*               (4) buf_size              Size of the data buffer.
*               (5) p_error               Pointer to the variable that receives error code from this function:
*
*                                             MB_ERROR_NONE                    No error occurred.
*                                             MB_ERROR_NULLREFERENCE           One of following conditions occurred:
*
*                                                                                  (1) 'p_master' is NULL.
*                                                                                  (2) 'p_buf' is NULL while 'buf_size' is not zero.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBMaster_Initialize(
    MBMASTER            *p_master,
    MB_IFINDEX           ifnbr,
    CPU_INT08U          *p_buf,
    CPU_SIZE_T           buf_size,
    MB_ERROR            *p_error
);


/*
*********************************************************************************************************
*                                   MBMaster_SetTurnAroundDelay()
*
* Description : Set the turn-around delay of a Modbus master.
*
* Argument(s) : (1) p_master              Pointer to the master object.
*               (2) dly                   Delay time (unit: milliseconds, 0 to disable).
*               (3) p_error               Pointer to the variable that receives error code from this function:
*
*                                             MB_ERROR_NONE                    No error occurred.
*                                             MB_ERROR_NULLREFERENCE           'p_master' is NULL.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBMaster_SetTurnAroundDelay(
    MBMASTER            *p_master,
    MB_TIMESPAN          dly,
    MB_ERROR            *p_error
);


/*
*********************************************************************************************************
*                                   MBMaster_GetTurnAroundDelay()
*
* Description : Get the turn-around delay of a Modbus master.
*
* Argument(s) : (1) p_master              Pointer to the master object.
*               (2) p_error               Pointer to the variable that receives error code from this function:
*
*                                             MB_ERROR_NONE                    No error occurred.
*                                             MB_ERROR_NULLREFERENCE           'p_master' is NULL.
*
* Return(s)   : The delay time (unit: milliseconds, 0 means disabled).
*********************************************************************************************************
*/

MB_TIMESPAN  MBMaster_GetTurnAroundDelay(
    MBMASTER            *p_master,
    MB_ERROR            *p_error
);


/*
*********************************************************************************************************
*                                   MBMaster_Post()
*
* Description : Post a Modbus request to specific slave.
*
* Argument(s) : (1) p_master              Pointer to the master object.
*               (2) slave                 Slave address.
*               (3) p_cmdlet              Pointer to the command-let object.
*               (4) p_request             Pointer to the request object.
*               (5) p_response            Pointer to the response object.
*               (6) p_responsearg         'p_arg' parameter passed to response callbacks.
*               (7) timeout               Timeout for receiving the response frame from the slave (unit: milliseconds, 0 to wait infinitely).
*               (8) p_error               Pointer to the variable that receives error code from this function:
*
*                                             MB_ERROR_NONE                    No error occurred.
*                                             MB_ERROR_NULLREFERENCE           One of following conditions occurred:
*
*                                                                                  (1) 'p_master' is NULL.
*                                                                                  (2) 'p_cmdlet' is NULL.
*                                                                                  (3) 'p_request' is NULL.
*                                                                                  (4) 'p_response' is NULL.
*
*                                             MB_ERROR_MASTER_STILLBUSY        Master is still busy.
*                                             MB_ERROR_MASTER_TXBADREQUEST     Bad request parameter.
*                                             MB_ERROR_MASTER_TXBUFFERLOW      Data buffer is too small.
*                                             MB_ERROR_MASTER_RXTRUNCATED      Frame data is truncated.
*                                             MB_ERROR_MASTER_RXINVALIDFORMAT  Frame data contains invalid format (or value).
*                                             MB_ERROR_MASTER_CALLBACKFAILED   Error occurred while calling external callbacks.
*                                             MB_ERROR_OVERFLOW                'timeout' parameter exceeds maximum allowed value.
*                                             MB_ERROR_TIMEOUT                 Timeout limit exceeds.
*                                             MB_ERROR_DEVICENOTEXIST          Device interface ID is not valid.
*                                             MB_ERROR_DEVICENOTREGISTER       Device is not registered (initialized) yet.
*                                             MB_ERROR_DEVICENOTOPENED         Device is not opened.
*                                             MB_ERROR_DEVICEFAIL              Device operation failed.
*                                             MB_ERROR_RXTOOMANY               Too many receive requests on the device.
*                                             MB_ERROR_TXTOOMANY               Too many transmit requests on the device.
*                                             MB_ERROR_OS_FGRP_FAILEDPEND      Failed to pend on a flag group object.
*                                             MB_ERROR_OS_FGRP_FAILEDPOST      Failed to post to a flag group object.
*                                             MB_ERROR_OS_MUTEX_FAILEDPEND     Failed to pend on a mutex object.
*                                             MB_ERROR_OS_TIMER_FAILEDCREATE   Failed to create timer object.
*                                             MB_ERROR_OS_TIMER_FAILEDSTART    Failed to start a timer object.
*                                             MB_ERROR_OS_TIME_FAILEDDELAY     Failed to delay.
*                                             MB_ERROR_OS_TIME_FAILEDGET       Failed to get system tick count.
*
* Return(s)   : None.
*
* Note(s)     : (1) 'p_request' must point to a valid command-let request (MBMASTER_CMDLET_*_REQUEST) object.
*               (2) 'p_response' must point to a valid command-let response (MBMASTER_CMDLET_*_RESPONSE) object.
*               (3) Multi-thread(task)/nesting posting is not allowed. If you do that, a 'MB_ERROR_MASTER_STILLBUSY' error would 
*                   be thrown.
*********************************************************************************************************
*/

void MBMaster_Post(
    MBMASTER            *p_master,
    CPU_INT08U           slave,
    MBMASTER_CMDLET     *p_cmdlet,
    void                *p_request,
    void                *p_response,
    void                *p_responsearg,
    MB_TIMESPAN          timeout,
    MB_ERROR            *p_error
);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED)  */

#endif