/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              SLAVE MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBSLAVE.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_SLAVE_H__
#define MB_SLAVE_H__

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mbslave_cmdlet_common.h>
#include <mbslave_cmdlet_maskwriteregister.h>
#include <mbslave_cmdlet_readcoils.h>
#include <mbslave_cmdlet_readdiscreteinputs.h>
#include <mbslave_cmdlet_readholdregisters.h>
#include <mbslave_cmdlet_readinputregisters.h>
#include <mbslave_cmdlet_rwmultipleregisters.h>
#include <mbslave_cmdlet_writemultiplecoils.h>
#include <mbslave_cmdlet_writemultipleregisters.h>
#include <mbslave_cmdlet_writesinglecoil.h>
#include <mbslave_cmdlet_writesingleregister.h>

#include <mbslave_cmdtable.h>

#include <mbslave_cfg.h>

#include <mb_os_types.h>

#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_SLAVE_EN == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*  Modbus slave counter types.  */
#if (MB_CFG_SLAVE_BUSMESSAGECOUNTER_EN == DEF_ENABLED)
#define  MBSLAVE_COUNTERTYPE_BUSMESSAGE          ((MB_COUNTERTYPE)1U)
#endif
#if (MB_CFG_SLAVE_BUSCOMMERRORCOUNTER_EN == DEF_ENABLED)
#define  MBSLAVE_COUNTERTYPE_BUSCOMMERROR        ((MB_COUNTERTYPE)2U)
#endif
#if (MB_CFG_SLAVE_SLAVEMESSAGECOUNTER_EN == DEF_ENABLED)
#define  MBSLAVE_COUNTERTYPE_SLAVEMESSAGES       ((MB_COUNTERTYPE)3U)
#endif
#if (MB_CFG_SLAVE_SLAVEEXCEPTIONCOUNTER_EN == DEF_ENABLED)
#define  MBSLAVE_COUNTERTYPE_SLAVEEXCEPTIONS     ((MB_COUNTERTYPE)4U)
#endif
#if (MB_CFG_SLAVE_SLAVENORESPONSECOUNTER_EN == DEF_ENABLED)
#define  MBSLAVE_COUNTERTYPE_SLAVENORESPONSE     ((MB_COUNTERTYPE)5U)
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    MB_IFINDEX          iface;

    CPU_INT08U          address;

    CPU_INT08U         *bufRcv;
    CPU_SIZE_T          bufRcvSize;

    CPU_INT08U         *bufSnd;
    CPU_SIZE_T          bufSndSize;

#if (MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN == DEF_ENABLED)
    MB_FRAMEFLAGS       prevFrameFlags;
#endif
#if (MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN == DEF_ENABLED) || (MB_CFG_SLAVE_GETLASTFRAMEFLAGS_EN == DEF_ENABLED)
    MB_FRAMEFLAGS       curFrameFlags;
#endif

    MBSLAVE_CMDTABLE   *cmdTable;
#if (MB_CFG_SLAVE_GETLASTERROR_EN == DEF_ENABLED)
    MB_ERROR            cmdLastError;
#endif

#if (MB_CFG_SLAVE_LISTENONLY_EN == DEF_ENABLED)
    CPU_BOOLEAN         listenOnly;
#endif

#if (MB_CFG_SLAVE_DELAYBEFOREREPLY_EN == DEF_ENABLED)
    MB_TIMESPAN         dlyBeforeReply;
#endif

#if (MB_CFG_SLAVE_BUSMESSAGECOUNTER_EN == DEF_ENABLED)
    MB_COUNTERVALUE          cntBusMessage;
#endif
#if (MB_CFG_SLAVE_BUSCOMMERRORCOUNTER_EN == DEF_ENABLED)
    MB_COUNTERVALUE          cntBusCommError;
#endif
#if (MB_CFG_SLAVE_SLAVEMESSAGECOUNTER_EN == DEF_ENABLED)
    MB_COUNTERVALUE          cntSlaveMessages;
#endif
#if (MB_CFG_SLAVE_SLAVEEXCEPTIONCOUNTER_EN == DEF_ENABLED)
    MB_COUNTERVALUE          cntSlaveExceptionError;
#endif
#if (MB_CFG_SLAVE_SLAVENORESPONSECOUNTER_EN == DEF_ENABLED)
    MB_COUNTERVALUE          cntSlaveNoResponse;
#endif

    CPU_BOOLEAN         polling;
} MBSLAVE;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

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
);


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
);
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
);
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
);


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
);


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
);
#endif


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
);
#endif


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
);
#endif


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
);
#endif


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
);
#endif


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
);
#endif


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
);
#endif


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
);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_SLAVE_EN == DEF_ENABLED)  */

#endif