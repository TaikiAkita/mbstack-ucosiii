/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              SLAVE MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBSLAVE.C
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
#define MBSLAVE_SOURCE
#define MBSLAVE_MAIN_SOURCE

#include <mbslave.h>

#include <mbslave_cmdlet_common.h>
#include <mbslave_cmdtable.h>

#include <mbslave_cfg.h>

#include <mb_os_basetypes.h>
#include <mb_os.h>

#include <mb_constants.h>
#include <mb_core.h>
#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


/*
*********************************************************************************************************
*                                    MBSlave_Initialize()
*
* Description : Initialize a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) p_slavectable   Pointer to the command table of the slave.
*               (3) ifnbr           Modbus device interface ID of the slave.
*               (4) p_bufrcv        Pointer to the first element of the RX buffer.
*               (5) bufrcv_size     Size of the RX buffer.
*               (6) p_bufsnd        Pointer to the first element of the TX buffer.
*               (7) bufsnd_size     Size of the TX buffer.
*               (8) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE             No error occurred.
*                                       MB_ERROR_NULLREFERENCE    One of following error occurred:
*
*                                                                     (1) 'p_slave' is NULL.
*                                                                     (2) 'p_slavectable' is NULL.
*                                                                     (3) 'p_bufrcv' is NULL while 'bufrcv_size' is not zero.
*                                                                     (4) 'p_bufsnd' is NULL while 'bufsnd_size' is not zero.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBSlave_Initialize(
    MBSLAVE             *p_slave,
    MBSLAVE_CMDTABLE    *p_slavectable,
    MB_IFINDEX           ifnbr,
    CPU_INT08U          *p_bufrcv,
    CPU_SIZE_T           bufrcv_size,
    CPU_INT08U          *p_bufsnd,
    CPU_SIZE_T           bufsnd_size,
    MB_ERROR            *p_error
) {
    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_slavectable' parameter.  */
    if (p_slavectable == (MBSLAVE_CMDTABLE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_bufrcv' parameter.  */
    if ((p_bufrcv == (CPU_INT08U*)0) && (bufrcv_size != (CPU_SIZE_T)0U)) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_bufsnd' parameter.  */
    if ((p_bufsnd == (CPU_INT08U*)0) && (bufsnd_size != (CPU_SIZE_T)0U)) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Initialize all members.  */
    p_slave->iface                    = ifnbr;
    p_slave->address                  = (CPU_INT08U)0U;
    p_slave->bufRcv                   = p_bufrcv;
    p_slave->bufRcvSize               = bufrcv_size;
    p_slave->bufSnd                   = p_bufsnd;
    p_slave->bufSndSize               = bufsnd_size;
#if (MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN == DEF_ENABLED)
    p_slave->prevFrameFlags           = (MB_FRAMEFLAGS)0U;
#endif
#if (MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN == DEF_ENABLED) || (MB_CFG_SLAVE_GETLASTFRAMEFLAGS_EN == DEF_ENABLED)
    p_slave->curFrameFlags            = (MB_FRAMEFLAGS)0U;
#endif
    p_slave->cmdTable                 = p_slavectable;
#if (MB_CFG_SLAVE_GETLASTERROR_EN == DEF_ENABLED)
    p_slave->cmdLastError             = MB_ERROR_NONE;
#endif
#if (MB_CFG_SLAVE_LISTENONLY_EN == DEF_ENABLED)
    p_slave->listenOnly               = DEF_NO;
#endif
#if (MB_CFG_SLAVE_DELAYBEFOREREPLY_EN == DEF_ENABLED)
    p_slave->dlyBeforeReply           = (MB_TIMESPAN)0U;
#endif
#if (MB_CFG_SLAVE_BUSMESSAGECOUNTER_EN == DEF_ENABLED)
    p_slave->cntBusMessage            = (MB_COUNTERVALUE)0U;
#endif
#if (MB_CFG_SLAVE_BUSCOMMERRORCOUNTER_EN == DEF_ENABLED)
    p_slave->cntBusCommError          = (MB_COUNTERVALUE)0U;
#endif
#if (MB_CFG_SLAVE_SLAVEMESSAGECOUNTER_EN == DEF_ENABLED)
    p_slave->cntSlaveMessages         = (MB_COUNTERVALUE)0U;
#endif
#if (MB_CFG_SLAVE_SLAVEEXCEPTIONCOUNTER_EN == DEF_ENABLED)
    p_slave->cntSlaveExceptionError   = (MB_COUNTERVALUE)0U;
#endif
#if (MB_CFG_SLAVE_SLAVENORESPONSECOUNTER_EN == DEF_ENABLED)
    p_slave->cntSlaveNoResponse       = (MB_COUNTERVALUE)0U;
#endif

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


#if (MB_CFG_SLAVE_LISTENONLY_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MBSlave_EnterListenOnlyMode()
*
* Description : Let a Modbus slave enter listen-only mode.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*                                       MB_ERROR_SLAVE_LISTENONLYALREADYENTERED  Slave is already in listen-only mode.
*
* Return(s)   : None.
*
* Note(s)     : (1) If the function is called inside a command implementation, the Modbus slave would enter 
*                   listen-only mode immediately and the reply frame of the command won't be transmitted.
*********************************************************************************************************
*/

void MBSlave_EnterListenOnlyMode(
    MBSLAVE             *p_slave,
    MB_ERROR            *p_error
) {
    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Check whether already in listen-only mode.  */
    if (p_slave->listenOnly) {
        *p_error = MB_ERROR_SLAVE_LISTENONLYALREADYENTERED;
        goto MBSLAVE_LOMODEENTER_EXIT;
    }

    /*  Enter listen-only mode.  */
    p_slave->listenOnly = DEF_YES;

MBSLAVE_LOMODEENTER_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
}
#endif


#if (MB_CFG_SLAVE_LISTENONLY_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MBSlave_ExitListenOnlyMode()
*
* Description : Let a Modbus slave exit listen-only mode.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*                                       MB_ERROR_SLAVE_LISTENONLYALREADYEXITED   Slave is not in listen-only mode.
*
* Return(s)   : None.
*
* Note(s)     : (1) If the function is called inside a command implementation, the Modbus slave would exit 
*                   from listen-only mode immediately and the reply frame of the command will be transmitted.
*********************************************************************************************************
*/

void MBSlave_ExitListenOnlyMode(
    MBSLAVE             *p_slave,
    MB_ERROR            *p_error
) {
    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Check whether not in listen-only mode.  */
    if (!(p_slave->listenOnly)) {
        *p_error = MB_ERROR_SLAVE_LISTENONLYALREADYEXITED;
        goto MBSLAVE_LOMODEEXIT_EXIT;
    }

    /*  Exit listen-only mode.  */
    p_slave->listenOnly = DEF_NO;

MBSLAVE_LOMODEEXIT_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
}
#endif


/*
*********************************************************************************************************
*                                    MBSlave_GetAddress()
*
* Description : Get the address of a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*
* Return(s)   : The slave address.
*
* Note(s)     : (1) If the returned slave address is zero, it means that the slave address hasn't been properly
*                   configured yet and the slave could only process broadcast requests.
*********************************************************************************************************
*/

CPU_INT08U  MBSlave_GetAddress(
    MBSLAVE             *p_slave,
    MB_ERROR            *p_error
) {
    CPU_INT08U           address;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (MB_TIMESPAN)0U;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Get the delay.  */
    address = p_slave->address;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return address;
}


/*
*********************************************************************************************************
*                                    MBSlave_SetAddress()
*
* Description : Set the address of a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) address         New slave address.
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*
* Return(s)   : None.
*
* Note(s)     : (1) If the function is called inside a command implementation, the new address will take affect
*                   in next polling.
*               (2) If 'address' is zero, the slave would only receive broadcast requests.
*********************************************************************************************************
*/

void MBSlave_SetAddress(
    MBSLAVE             *p_slave,
    CPU_INT08U           address,
    MB_ERROR            *p_error
) {
    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Set the address.  */
    p_slave->address = address;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


#if (MB_CFG_SLAVE_DELAYBEFOREREPLY_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MBSlave_GetDelayBeforeReply()
*
* Description : Get the delay before replying of a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*
* Return(s)   : The delay (unit: milliseconds).
*********************************************************************************************************
*/

MB_TIMESPAN MBSlave_GetDelayBeforeReply(
    MBSLAVE             *p_slave,
    MB_ERROR            *p_error
) {
    MB_TIMESPAN   dly;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (MB_TIMESPAN)0U;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Get the delay.  */
    dly = p_slave->dlyBeforeReply;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return dly;
}
#endif  /*  #if (MB_CFG_SLAVE_DELAYBEFOREREPLY_EN == DEF_ENABLED)  */


#if (MB_CFG_SLAVE_DELAYBEFOREREPLY_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MBSlave_SetDelayBeforeReply()
*
* Description : Set the delay before replying of a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) dly             New delay (unit: milliseconds).
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*                                       MB_ERROR_OVERFLOW                        'dly' exceeds.
*
* Return(s)   : None.
*
* Note(s)     : (1) The value of 'dly' parameter must not be larger than the return value of MBOS_GetMaxTimeValue().
*               (2) If the function is called inside a command implementation, the new delay will take affect
*                   immediately.
*********************************************************************************************************
*/

void MBSlave_SetDelayBeforeReply(
    MBSLAVE             *p_slave,
    MB_TIMESPAN          dly,
    MB_ERROR            *p_error
) {
    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*
     *  Check 'dly' parameter.
     * 
     *  Note(s):
     *    [1] This check must be done no matter whether MB_CFG_ARG_CHK_EN is 
     *        settled to DEF_ENABLED or not.
     */
    if (dly > MBOS_GetMaxTimeValue()) {
        *p_error = MB_ERROR_OVERFLOW;
        return;
    }

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Set the delay.  */
    p_slave->dlyBeforeReply = dly;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
}
#endif  /*  #if (MB_CFG_SLAVE_DELAYBEFOREREPLY_EN == DEF_ENABLED)  */


#if (MB_CFG_SLAVE_GETLASTERROR_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MBSlave_GetLastError()
*
* Description : Get the last error thrown by a command implementation of a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*
* Return(s)   : The error code.
*********************************************************************************************************
*/

MB_ERROR  MBSlave_GetLastError(
    MBSLAVE             *p_slave,
    MB_ERROR            *p_error
) {
    MB_ERROR             lasterror;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return MB_ERROR_NONE;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Get the last error.  */
    lasterror = p_slave->cmdLastError;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return lasterror;
}
#endif  /*  #if (MB_CFG_SLAVE_GETLASTERROR_EN == DEF_ENABLED)  */


#if (MB_CFG_SLAVE_GETLASTFRAMEFLAGS_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MBSlave_GetLastFrameFlags()
*
* Description : Get the last frame flags of a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*
* Return(s)   : The frame flags.
*********************************************************************************************************
*/

MB_FRAMEFLAGS  MBSlave_GetLastFrameFlags(
    MBSLAVE             *p_slave,
    MB_ERROR            *p_error
) {
    MB_FRAMEFLAGS        flags;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (MB_FRAMEFLAGS)0U;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Get the last frame flags.  */
    flags = p_slave->curFrameFlags;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return flags;
}
#endif  /*  #if (MB_CFG_SLAVE_GETLASTFRAMEFLAGS_EN == DEF_ENABLED)  */


#if (MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MBSlave_GetPreviousFrameFlags()
*
* Description : Get the previous frame flags of a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*
* Return(s)   : The frame flags.
*********************************************************************************************************
*/

MB_FRAMEFLAGS  MBSlave_GetPreviousFrameFlags(
    MBSLAVE             *p_slave,
    MB_ERROR            *p_error
) {
    MB_FRAMEFLAGS        flags;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (MB_FRAMEFLAGS)0U;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Get the previous frame flags.  */
    flags = p_slave->prevFrameFlags;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return flags;
}
#endif  /*  #if (MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN == DEF_ENABLED)  */


#if (MB_CFG_SLAVE_GETCOUNTERVALUE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MBSlave_GetCounterValue()
*
* Description : Get the value of specific counter of a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) type            Counter type, one of following values:
*
*                                       MBSLAVE_COUNTERTYPE_BUSMESSAGE           Bus message counter.
*                                       MBSLAVE_COUNTERTYPE_BUSCOMMERROR         Bus communication error counter.
*                                       MBSLAVE_COUNTERTYPE_SLAVEMESSAGES        Slave messages counter.
*                                       MBSLAVE_COUNTERTYPE_SLAVEEXCEPTIONS      Slave exception errors counter.
*                                       MBSLAVE_COUNTERTYPE_SLAVENORESPONSE      Slave no response counter.
*
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*                                       MB_ERROR_INVALIDCOUNTER                  'type' contains an invalid value.
*
* Return(s)   : The counter value.
*********************************************************************************************************
*/

MB_COUNTERVALUE  MBSlave_GetCounterValue(
    MBSLAVE             *p_slave,
    MB_COUNTERTYPE       type,
    MB_ERROR            *p_error
) {
    MB_COUNTERVALUE      counter;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (MB_COUNTERVALUE)0U;
    }
#endif

    /*  Avoid variable unused warnings.  */
    (void)p_slave;
    (void)type;

    /*  Initialize local variables.  */
    counter = (MB_COUNTERVALUE)0U;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Get the counter.  */
    switch (type) {
#if (MB_CFG_SLAVE_BUSMESSAGECOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_BUSMESSAGE:
            counter = p_slave->cntBusMessage;
            break;
#endif
#if (MB_CFG_SLAVE_BUSCOMMERRORCOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_BUSCOMMERROR:
            counter = p_slave->cntBusCommError;
            break;
#endif
#if (MB_CFG_SLAVE_SLAVEMESSAGECOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_SLAVEMESSAGES:
            counter = p_slave->cntSlaveMessages;
            break;
#endif
#if (MB_CFG_SLAVE_SLAVEEXCEPTIONCOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_SLAVEEXCEPTIONS:
            counter = p_slave->cntSlaveExceptionError;
            break;
#endif
#if (MB_CFG_SLAVE_SLAVENORESPONSECOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_SLAVENORESPONSE:
            counter = p_slave->cntSlaveNoResponse;
            break;
#endif
        default:
            *p_error = MB_ERROR_INVALIDCOUNTER;
            goto MBSLAVE_GETCOUNTERVAL_EXIT;
    }

MBSLAVE_GETCOUNTERVAL_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    return counter;
}
#endif  /*  #if (MB_CFG_SLAVE_GETCOUNTERVALUE_EN == DEF_ENABLED)  */


#if (MB_CFG_SLAVE_CLEARCOUNTERVALUE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MBSlave_ClearCounterValue()
*
* Description : Clear the value of specific counter of a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) type            Counter type, one of following values:
*
*                                       MBSLAVE_COUNTERTYPE_BUSMESSAGE           Bus message counter.
*                                       MBSLAVE_COUNTERTYPE_BUSCOMMERROR         Bus communication error counter.
*                                       MBSLAVE_COUNTERTYPE_SLAVEMESSAGES        Slave messages counter.
*                                       MBSLAVE_COUNTERTYPE_SLAVEEXCEPTIONS      Slave exception errors counter.
*                                       MBSLAVE_COUNTERTYPE_SLAVENORESPONSE      Slave no response counter.
*
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*                                       MB_ERROR_INVALIDCOUNTER                  'type' contains an invalid value.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void  MBSlave_ClearCounterValue(
    MBSLAVE             *p_slave,
    MB_COUNTERTYPE       type,
    MB_ERROR            *p_error
) {
    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Avoid variable unused warnings.  */
    (void)p_slave;
    (void)type;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Get the counter.  */
    switch (type) {
#if (MB_CFG_SLAVE_BUSMESSAGECOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_BUSMESSAGE:
            p_slave->cntBusMessage = (MB_COUNTERVALUE)0U;
            break;
#endif
#if (MB_CFG_SLAVE_BUSCOMMERRORCOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_BUSCOMMERROR:
            p_slave->cntBusCommError = (MB_COUNTERVALUE)0U;
            break;
#endif
#if (MB_CFG_SLAVE_SLAVEMESSAGECOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_SLAVEMESSAGES:
            p_slave->cntSlaveMessages = (MB_COUNTERVALUE)0U;
            break;
#endif
#if (MB_CFG_SLAVE_SLAVEEXCEPTIONCOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_SLAVEEXCEPTIONS:
            p_slave->cntSlaveExceptionError = (MB_COUNTERVALUE)0U;
            break;
#endif
#if (MB_CFG_SLAVE_SLAVENORESPONSECOUNTER_EN == DEF_ENABLED)
        case MBSLAVE_COUNTERTYPE_SLAVENORESPONSE:
            p_slave->cntSlaveNoResponse = (MB_COUNTERVALUE)0U;
            break;
#endif
        default:
            *p_error = MB_ERROR_INVALIDCOUNTER;
            goto MBSLAVE_CLRCOUNTERVAL_EXIT;
    }

MBSLAVE_CLRCOUNTERVAL_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
}
#endif  /*  #if (MB_CFG_SLAVE_CLEARCOUNTERVALUE_EN == DEF_ENABLED)  */


/*
*********************************************************************************************************
*                                    MBSlave_Poll()
*
* Description : Poll a Modbus slave.
*
* Argument(s) : (1) p_slave         Pointer to the slave object.
*               (2) timeout         Timeout of polling for incoming frame.
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                            No error occurred.
*                                       MB_ERROR_NULLREFERENCE                   'p_slave' is NULL.
*                                       MB_ERROR_DEVICENOTEXIST                  Device interface ID is not valid.
*                                       MB_ERROR_DEVICENOTREGISTER               Device is not registered (initialized) yet.
*                                       MB_ERROR_DEVICENOTOPENED                 Device is not opened.
*                                       MB_ERROR_DEVICEFAIL                      Device operation failed.
*                                       MB_ERROR_OVERFLOW                        'timeout' parameter exceeds maximum allowed value.
*                                       MB_ERROR_TIMEOUT                         Timeout limit exceeds.
*                                       MB_ERROR_RXTOOMANY                       Too many receive requests.
*                                       MB_ERROR_TXTOOMANY                       Too many transmit requests.
*                                       MB_ERROR_OS_FGRP_FAILEDPEND              Failed to pend on a flag group object.
*                                       MB_ERROR_OS_FGRP_FAILEDPOST              Failed to post to a flag group object.
*                                       MB_ERROR_OS_MUTEX_FAILEDPEND             Failed to pend on a mutex object.
*                                       MB_ERROR_OS_TIMER_FAILEDCREATE           Failed to create timer object.
*                                       MB_ERROR_OS_TIMER_FAILEDSTART            Failed to start a timer object.
*                                       MB_ERROR_OS_TIME_FAILEDDELAY             Failed to delay.
*
* Return(s)   : None.
*
* Note(s)     : (1) Multi-thread(task)/nesting polling is not allowed.
*               (2) The value of 'timeout' parameter must not be larger than the return value of MBOS_GetMaxTimeValue().
*********************************************************************************************************
*/

void MBSlave_Poll(
    MBSLAVE              *p_slave,
    MB_TIMESPAN           timeout,
    MB_ERROR             *p_error
) {
    MB_FRAME              frameIn;
    MB_FRAMEFLAGS         frameInFlags;
    CPU_BOOLEAN           frameInBroadcast;

    MB_FRAME              frameOut;

    MB_ERROR              cmdletError;
    CPU_BOOLEAN           cmdletFound;
    MBSLAVE_CMDLET_FUNC   cmdletFunc;
    void                 *cmdletCtx;
    CPU_BOOLEAN           cmdletNoBroadcast;
    CPU_BOOLEAN           cmdletNoListenOnly;

    CPU_INT08U            cmdletResponseFnCode;
    CPU_SIZE_T            cmdletResponseDataSize;

    CPU_BOOLEAN           noReply;

    CPU_INT08U            ec;

#if (MB_CFG_SLAVE_DELAYBEFOREREPLY_EN == DEF_ENABLED)
    MB_TIMESPAN           dlyBeforeReply;
#endif

    struct {
        CPU_BOOLEAN       clrPolling:1;
        CPU_BOOLEAN       clrCriticalSect:1;
        CPU_INT08U        __padding:6;
    } gc;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_slave' parameter.  */
    if (p_slave == (MBSLAVE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Initialize local variables.  */
    gc.clrPolling       = DEF_NO;
    gc.clrCriticalSect  = DEF_NO;
    noReply             = DEF_NO;

    /*  No error by default.  */
    *p_error            = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();
    gc.clrCriticalSect = DEF_YES;

    /*  Try to enter polling state.  */
    if (p_slave->polling) {
        *p_error = MB_ERROR_SLAVE_STILLPOLLING;
        goto MBSLAVE_POLL_EXIT;
    }
    p_slave->polling = DEF_YES;
    gc.clrPolling    = DEF_YES;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
    gc.clrCriticalSect = DEF_NO;

    /*
     *  Stage 1: Receive a frame.
     */

    /*  Receive a frame.  */
    MB_ReceiveFrame(
        p_slave->iface,
        p_slave->bufRcv,
        p_slave->bufRcvSize,
        &(frameIn),
        &(frameInFlags),
        timeout,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBSLAVE_POLL_EXIT;
    }

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();
    gc.clrCriticalSect = DEF_YES;

    /*  Save the frame flags.  */
#if (MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN == DEF_ENABLED)
    p_slave->prevFrameFlags  = p_slave->curFrameFlags;
#endif
#if (MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN == DEF_ENABLED) || (MB_CFG_SLAVE_GETLASTFRAMEFLAGS_EN == DEF_ENABLED)
    p_slave->curFrameFlags   = frameInFlags;
#endif

    /*  Drop the frame if marked.  */
    if ((frameInFlags & MB_FRAMEFLAGS_DROP) != (MB_FRAMEFLAGS)0) {
#if (MB_CFG_SLAVE_BUSCOMMERRORCOUNTER_EN == DEF_ENABLED)
        /*  Increase the bus communication error count.  */
        if (p_slave->cntBusCommError != MB_COUNTERVALUE_MAX) {
            ++(p_slave->cntBusCommError);
        }
#endif

        goto MBSLAVE_POLL_EXIT;
    }

#if (MB_CFG_SLAVE_BUSMESSAGECOUNTER_EN == DEF_ENABLED)
    /*  Increase the bus message count.  */
    if (p_slave->cntBusMessage != MB_COUNTERVALUE_MAX) {
        ++(p_slave->cntBusMessage);
    }
#endif

    /*  Get frame properties.  */
    if (frameIn.address == (CPU_INT08U)0U) {
        frameInBroadcast = DEF_YES;
    } else {
        frameInBroadcast = DEF_NO;
    }

    /*  Skip the frame if address is neither broadcast address nor slave address.  */
    if ((!frameInBroadcast) && (frameIn.address != p_slave->address)) {
        goto MBSLAVE_POLL_EXIT;
    }

    /*  Drop the frame if function code is invalid.  */
    if (
        frameIn.functionCode == ((CPU_INT08U)0U) || 
        frameIn.functionCode  > ((CPU_INT08U)MB_VALID_FUNCTION_CODES)
    ) {
        goto MBSLAVE_POLL_EXIT;
    }

#if (MB_CFG_SLAVE_SLAVEMESSAGECOUNTER_EN == DEF_ENABLED)
    /*  Increase the slave messages counter.  */
    if (p_slave->cntSlaveMessages != MB_COUNTERVALUE_MAX) {
        ++(p_slave->cntSlaveMessages);
    }
#endif

    /*
     *  Stage 2: Process a frame.
     */

    /*  Lookup the function code.  */
    cmdletFound = MBSlave_CmdTable_Lookup(
        p_slave->cmdTable,
        frameIn.functionCode,
        &(cmdletFunc),
        &(cmdletCtx),
        &(cmdletNoBroadcast),
        &(cmdletNoListenOnly),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBSLAVE_POLL_EXIT;
    }

    if (cmdletFound) {
        /*  Check whether the command is allowed for broadcast requests.  */
        if (frameInBroadcast && cmdletNoBroadcast) {
            noReply = DEF_YES;
            goto MBSLAVE_POLL_REPLY;
        }

#if (MB_CFG_SLAVE_LISTENONLY_EN == DEF_ENABLED)
        /*  Check whether the command is allowed in listen-only mode.  */
        if ((p_slave->listenOnly) && cmdletNoListenOnly) {
            noReply = DEF_YES;
            goto MBSLAVE_POLL_REPLY;
        }
#endif

        /*  Invoke the processor of the function code.  */
        cmdletFunc(
            frameIn.functionCode,
            frameIn.data,
            frameIn.dataLength,
            &(cmdletResponseFnCode),
            p_slave->bufSnd,
            p_slave->bufSndSize,
            &(cmdletResponseDataSize),
            cmdletCtx,
            &(cmdletError)
        );

        /*  Reply a 'Server Device Failure (0x04)' if process failed.  */
        if (cmdletError != MB_ERROR_NONE) {
#if (MB_CFG_SLAVE_GETLASTERROR_EN == DEF_ENABLED)
            p_slave->cmdLastError = cmdletError;
#endif
            ec = (CPU_INT08U)MB_APUEC_SERVERDEVICEFAILURE;
            goto MBSLAVE_POLL_ERRORFRAME;
        }

#if (MB_CFG_SLAVE_SLAVEEXCEPTIONCOUNTER_EN == DEF_ENABLED)
        /*  Increase the slave exception error counter (if needed).  */
        if (cmdletResponseFnCode > (CPU_INT08U)0x80U) {
            if (p_slave->cntSlaveExceptionError != MB_COUNTERVALUE_MAX) {
                ++(p_slave->cntSlaveExceptionError);
            }
        }
#endif

        /*  Build the response frame.  */
        frameOut.address = frameIn.address;
        frameOut.functionCode = cmdletResponseFnCode;
        frameOut.data = p_slave->bufSnd;
        frameOut.dataLength = cmdletResponseDataSize;

        goto MBSLAVE_POLL_REPLY;
    } else {
        /*  Reply an 'Illegal Function (0x01).  */
        ec = (CPU_INT08U)MB_APUEC_ILLEGALFUNCTION;
        goto MBSLAVE_POLL_ERRORFRAME;
    }

MBSLAVE_POLL_ERRORFRAME:
#if (MB_CFG_SLAVE_SLAVEEXCEPTIONCOUNTER_EN == DEF_ENABLED)
    /*  Increase the slave exception error counter.  */
    if (p_slave->cntSlaveExceptionError != MB_COUNTERVALUE_MAX) {
        ++(p_slave->cntSlaveExceptionError);
    }
#endif

    /*  Build an exception response frame.  */
    frameOut.address = frameIn.address;
    frameOut.functionCode = (CPU_INT08U)(frameIn.functionCode + (CPU_INT08U)0x80U);
    frameOut.data = p_slave->bufSnd;
    frameOut.dataLength = (CPU_SIZE_T)1U;
    p_slave->bufSnd[0] = ec;

MBSLAVE_POLL_REPLY:
#if (MB_CFG_SLAVE_DELAYBEFOREREPLY_EN == DEF_ENABLED)
    /*  Get the delay timespan before replying.  */
    dlyBeforeReply = p_slave->dlyBeforeReply;
#endif

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
    gc.clrCriticalSect = DEF_NO;

    /*
     *  Stage 3: Transmit response frame.
     */

#if (MB_CFG_SLAVE_LISTENONLY_EN == DEF_ENABLED)
    /*  Do NOT reply in listen-only mode.  */
    if (p_slave->listenOnly) {
        noReply = DEF_YES;
    }
#endif

    /*  Do NOT reply for broadcast requests.  */
    if (frameIn.address == (CPU_INT08U)0U) {
        noReply = DEF_YES;
    }

    /*  Transmit the response frame.  */
    if (!noReply) {
#if (MB_CFG_SLAVE_DELAYBEFOREREPLY_EN == DEF_ENABLED)
        /*  Do delaying.  */
        if (dlyBeforeReply != (MB_TIMESPAN)0U) {
            MBOS_Delay(dlyBeforeReply, p_error);
            if (*p_error != MB_ERROR_NONE) {
                goto MBSLAVE_POLL_EXIT;
            }
        }
#endif

        /*  Do transmission.  */
        MB_TransmitFrame(
            p_slave->iface,
            &(frameOut),
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            goto MBSLAVE_POLL_EXIT;
        }
    } else {
#if (MB_CFG_SLAVE_SLAVENORESPONSECOUNTER_EN == DEF_ENABLED)
        /*  Enter critical section.  */
        CPU_CRITICAL_ENTER();
        gc.clrCriticalSect = DEF_YES;

        /*  Increase the slave no response counter.  */
        if (p_slave->cntSlaveNoResponse != MB_COUNTERVALUE_MAX) {
            ++(p_slave->cntSlaveNoResponse);
        }

        /*  Exit critical section.  */
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;
#endif
    }

MBSLAVE_POLL_EXIT:
    /*  Exit polling state (if needed).  */
    if (gc.clrPolling) {
        p_slave->polling   = DEF_NO;
        gc.clrPolling      = DEF_NO;
    }

    /*  Exit critical section (if needed).  */
    if (gc.clrCriticalSect) {
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;
    }
}
