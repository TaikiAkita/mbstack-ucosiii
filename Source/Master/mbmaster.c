/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER.C
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define MB_SOURCE
#define MBMASTER_SOURCE
#define MBMASTER_MAIN_SOURCE

#include <mbmaster_cfg.h>

#include <mbmaster.h>

#include <mbmaster_cmdlet_common.h>

#include <mb_os_basetypes.h>
#include <mb_os_types.h>
#include <mb_os.h>

#include <mb_core.h>
#include <mb_constants.h>
#include <mb_types.h>

#include <mb_constants.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_MASTER_EN == DEF_ENABLED)

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
) {
    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_master' parameter.  */
    if (p_master == (MBMASTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_buf' parameter.  */
    if ((p_buf == (CPU_INT08U*)0) && (buf_size != (CPU_SIZE_T)0U)) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Initialize all members.  */
    p_master->iface = ifnbr;
    p_master->bufRxTx = p_buf;
    p_master->bufRxTxSize = buf_size;
    p_master->dlyTurnAround = (MB_TIMESPAN)0U;
    p_master->busy = DEF_NO;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


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
) {
    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_master' parameter.  */
    if (p_master == (MBMASTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Set the turn around delay.  */
    p_master->dlyTurnAround = dly;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


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
) {
    MB_TIMESPAN          dly;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_master' parameter.  */
    if (p_master == (MBMASTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (MB_TIMESPAN)0U;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Get the turn around delay.  */
    dly = p_master->dlyTurnAround;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return dly;
}


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
) {
    CPU_SR_ALLOC();

    MB_FRAME       frame;
    MB_FRAMEFLAGS  frameFlags;
    CPU_BOOLEAN    frameCanProcess;

    struct {
        CPU_BOOLEAN    clrCriticalSect:1;
        CPU_BOOLEAN    clrBusy:1;
        CPU_INT08U     __padding:6;
    } gc;

    MB_TIMESPAN    rxTimeout;

    MB_TIMESPAN    dlyTurnAround;

    MB_SYSTICK     ticksTimeout;
    MB_SYSTICK     ticksBeforeRX;
    MB_SYSTICK     ticksAfterRX;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_master' parameter.  */
    if (p_master == (MBMASTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_cmdlet' parameter.  */
    if (p_cmdlet == (MBMASTER_CMDLET*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_request' parameter.  */
    if (p_request == (void*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_response' parameter.  */
    if (p_response == (void*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'timeout' parameter.  */
    if (timeout > MBOS_GetMaxTimeValue()) {
        *p_error = MB_ERROR_OVERFLOW;
        return;
    }
#endif

    /*  Initialize local variables.  */
    gc.clrCriticalSect  = DEF_NO;
    gc.clrBusy          = DEF_NO;
    ticksBeforeRX       = (MB_SYSTICK)0U;

    /*  No error by default.  */
    *p_error            = MB_ERROR_NONE;

    /*  Convert timeout to ticks.  */
    if (timeout != (MB_TIMESPAN)0U) {
        ticksTimeout = MBOS_TimeToTickCount(
            timeout,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            goto MBMASTER_POST_EXIT;
        }
    } else {
        ticksTimeout = (MB_SYSTICK)0U;
    }

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();
    gc.clrCriticalSect = DEF_YES;

    /*  Try to acquire the busy lock.  */
    if (p_master->busy) {
        *p_error = MB_ERROR_MASTER_STILLBUSY;
        goto MBMASTER_POST_EXIT;
    }
    p_master->busy = DEF_YES;
    gc.clrBusy = DEF_YES;

    /*  Make the request frame.  */
    p_cmdlet->cbRequestHandler(
        slave,
        p_request,
        p_master->bufRxTx,
        p_master->bufRxTxSize,
        &(frame),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBMASTER_POST_EXIT;
    }

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
    gc.clrCriticalSect = DEF_NO;

    /*  Transmit the request frame.  */
    MB_TransmitFrame(
        p_master->iface,
        &(frame),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBMASTER_POST_EXIT;
    }

    /*  Fast path: Broadcast requests have no response frame.  */
    if (slave == (CPU_INT08U)0U) {
        /*  Enter critical section.  */
        CPU_CRITICAL_ENTER();
        gc.clrCriticalSect = DEF_YES;

        /*  Get the turn around delay.  */
        dlyTurnAround = p_master->dlyTurnAround;

        /*  Exit critical section.  */
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;

        if (dlyTurnAround != (MB_TIMESPAN)0U) {
            /*  Wait for turn around.  */
            MBOS_Delay(
                dlyTurnAround,
                p_error
            );
            /*if (*p_error != MB_ERROR_NONE) {
                goto MBMASTER_POST_EXIT;
            }*/
        }

        goto MBMASTER_POST_EXIT;
    }

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();
    gc.clrCriticalSect = DEF_YES;

    while (DEF_YES) {
        if (ticksTimeout != (MB_SYSTICK)0U) {
            /*  Get the system tick count before RX operation.  */
            ticksBeforeRX = MBOS_GetTickCount(p_error);
            if (*p_error != MB_ERROR_NONE) {
                goto MBMASTER_POST_EXIT;
            }

            /*  Convert remaining timeout ticks to timespan.  */
            rxTimeout = MBOS_TickCountToTime(ticksTimeout, p_error);
            if (*p_error != MB_ERROR_NONE) {
                goto MBMASTER_POST_EXIT;
            }
        } else {
            rxTimeout = (MB_TIMESPAN)0U;
        }

        /*  Exit critical section.  */
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;

        /*  Receive a response frame.  */
        MB_ReceiveFrame(
            p_master->iface,
            p_master->bufRxTx,
            p_master->bufRxTxSize,
            &(frame),
            &(frameFlags),
            rxTimeout,
            p_error
        );
        switch (*p_error) {
            case MB_ERROR_TIMEOUT:
                frameCanProcess = DEF_NO;
                break;
            case MB_ERROR_NONE:
                frameCanProcess = DEF_YES;
                break;
            default:
                goto MBMASTER_POST_EXIT;
        }

        /*  Enter critical section.  */
        CPU_CRITICAL_ENTER();
        gc.clrCriticalSect = DEF_YES;

        /*  Try to process the response frame.  */
        if (frameCanProcess) {
            if ((frameFlags & MB_FRAMEFLAGS_DROP) == (MB_FRAMEFLAGS)0) {
                p_cmdlet->cbResponseHandler(
                    slave,
                    p_request,
                    p_response,
                    p_responsearg,
                    &(frame),
                    p_error
                );
                switch (*p_error) {
                    case MB_ERROR_MASTER_RXINVALIDSLAVE:
                        break;
                    case MB_ERROR_NONE:
                    default:
                        goto MBMASTER_POST_EXIT;
                }
            }
        }

        if (ticksTimeout != (MB_SYSTICK)0U) {
            /*  Get the system tick count after RX operation.  */
            ticksAfterRX = MBOS_GetTickCount(p_error);
            if (*p_error != MB_ERROR_NONE) {
                goto MBMASTER_POST_EXIT;
            };

            /*  Get the ticks elapsed during the RX operation.  */
            ticksAfterRX -= ticksBeforeRX;

            /*  Throw a timeout error if the elapsed ticks is not lower than the timeout ticks.  */
            if (ticksAfterRX >= ticksTimeout) {
                *p_error = MB_ERROR_TIMEOUT;
                goto MBMASTER_POST_EXIT;
            }

            /*  Cost the elapsed ticks.  */
            ticksTimeout -= ticksAfterRX;
        }
    }

MBMASTER_POST_EXIT:
    /*  Release the busy lock (if needed).  */
    if (gc.clrBusy) {
        p_master->busy = DEF_NO;
        gc.clrBusy = DEF_NO;
    }

    /*  Clean up the critical section (if needed).  */
    if (gc.clrCriticalSect) {
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;
    }
}

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED)  */