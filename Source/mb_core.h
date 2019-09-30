/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_CORE.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_CORE_H__
#define MB_CORE_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_cfg.h>

#include <mb_types.h>

#include <mbdrv_types.h>

#include <mb_os_basetypes.h>
#include <mb_os_types.h>

#include <cpu.h>

#include <lib_def.h>


#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*  Modbus serial line counter types.  */
#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED)
#define MB_COUNTERTYPE_PARITYERROR           ((MB_COUNTERTYPE)1U)
#endif
#if (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED)
#define MB_COUNTERTYPE_DATAOVERRUNERROR      ((MB_COUNTERTYPE)2U)
#endif
#if (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)
#define MB_COUNTERTYPE_FRAMEERROR            ((MB_COUNTERTYPE)3U)
#endif


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    MB_Initialize()
*
* Description : Initialize the Modbus module.
*
* Argument(s) : (1) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE       No error occurred.
*
* Return(s)   : None.
*
* Note(s)     : (1) You must call this function before using all other functions of the Modbus communication 
*                   module.
*               (2) You must call this function after OS and CPU initialized.
*********************************************************************************************************
*/

void MB_Initialize(
    MB_ERROR  *p_error
);


/*
*********************************************************************************************************
*                                    MB_RegisterDevice()
*
* Description : Register the Modbus device.
*
* Argument(s) : (1) p_drv     Pointer to the device driver.
*               (2) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_drv' is NULL.
*                                 MB_ERROR_DEVICEEXISTED           Device already exists.
*                                 MB_ERROR_DEVICENOFREE            No free device entry that can be used.
*                                 MB_ERROR_OS_MUTEX_FAILEDCREATE   Unable to create a mutex object.
*                                 MB_ERROR_OS_FGRP_FAILEDCREATE    Unable to create a flag group object.
*
* Return(s)   : The Modbus device interface ID.
*********************************************************************************************************
*/

MB_IFINDEX MB_RegisterDevice(
    MB_DRIVER *p_drv,
    MB_ERROR  *p_error
);


/*
*********************************************************************************************************
*                                    MB_OpenDevice()
*
* Description : Open a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) mode           Modbus transmission mode, one of following:
*
*                                      MB_TRMODE_RTU                RTU transmission mode.
*                                      MB_TRMODE_ASCI               ASCII transmission mode.
*
*               (3) p_serialsetup  Pointer to the Modbus serial configuration.
*               (4) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_NULLREFERENCE       'p_serialsetup' is NULL.
*                                      MB_ERROR_INVALIDPARAMETER    'mode' or 'p_serialsetup' contains invalid device configuration.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICEOPENED        Device is already opened.
*                                      MB_ERROR_DEVICEFAIL          Device failed to be opened.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void  MB_OpenDevice(
    MB_IFINDEX        ifnbr,
    MB_TRMODE         mode,
    MB_SERIAL_SETUP  *p_serialsetup,
    MB_ERROR         *p_error
);


/*
*********************************************************************************************************
*                                    MB_CloseDevice()
*
* Description : Close a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEBUSY          Device is still busy.
*                                      MB_ERROR_DEVICEFAIL          Device failed to be closed.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MB_CloseDevice(
    MB_IFINDEX  ifnbr,
    MB_ERROR   *p_error
);


#if (MB_CFG_CORE_SETMODE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_SetMode()
*
* Description : Set the transmission of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) mode           New transmission mode.
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_INVALIDPARAMETER    'mode' is not valid.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEBUSY          Device is still busy.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MB_SetMode(
    MB_IFINDEX   ifnbr,
    MB_TRMODE    mode,
    MB_ERROR    *p_error
);
#endif


#if (MB_CFG_CORE_GETMODE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetMode()
*
* Description : Get the transmission mode of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*
* Return(s)   : The transmission mode.
*********************************************************************************************************
*/

MB_TRMODE MB_GetMode(
    MB_IFINDEX   ifnbr,
    MB_ERROR    *p_error
);
#endif


#if (MB_CFG_CORE_SETLINEFEED_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_SetLineFeed()
*
* Description : Set the line-feed character of a Modbus device (for ASCII transmission mode only).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) lf             Line-feed character.
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_INVALIDPARAMETER    'lf' is not an ASCII character.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEMODEMISMATCH  Device is not in ASCII transmission mode.
*                                      MB_ERROR_DEVICEBUSY          Device is still busy.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MB_SetLineFeed(
    MB_IFINDEX   ifnbr,
    CPU_CHAR     lf,
    MB_ERROR    *p_error
);
#endif


#if (MB_CFG_CORE_GETLINEFEED_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetLineFeed()
*
* Description : Get the line-feed character of a Modbus device (for ASCII transmission mode only).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEMODEMISMATCH  Device is not in ASCII transmission mode.
*
* Return(s)   : The line-feed character.
*********************************************************************************************************
*/

CPU_CHAR MB_GetLineFeed(
    MB_IFINDEX   ifnbr,
    MB_ERROR    *p_error
);
#endif


#if (MB_CFG_CORE_SETCHARTIMEPRESCALE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_SetCharTimePrescale()
*
* Description : Set the prescale of character time of a Modbus device (for RTU transmission mode only).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) prescale       Time prescale.
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEMODEMISMATCH  Device is not in RTU transmission mode.
*                                      MB_ERROR_DEVICEBUSY          Device is still busy.
*                                      MB_ERROR_UNDERFLOW           'prescale' equals to zero.
*
* Return(s)   : None.
*
* Note(s)     : (1) The new 1.5/3.5 character time can be calculated with following formula:
*
*                      (t1.5)new = (t1.5)default * prescale
*                      (t3.5)new = (t3.5)default * prescale
*
*               (2) The prescale is set to 1 by default.
*********************************************************************************************************
*/

void MB_SetCharTimePrescale(
    MB_IFINDEX   ifnbr,
    CPU_INT16U   prescale,
    MB_ERROR    *p_error
);
#endif


#if (MB_CFG_CORE_GETCHARTIMEPRESCALE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetCharTimePrescale()
*
* Description : Get the prescale of character time of a Modbus device (for RTU transmission mode only).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEMODEMISMATCH  Device is not in RTU transmission mode.
*
* Return(s)   : The prescale.
*********************************************************************************************************
*/

CPU_INT16U MB_GetCharTimePrescale(
    MB_IFINDEX   ifnbr,
    MB_ERROR    *p_error
);
#endif


/*
*********************************************************************************************************
*                                    MB_WaitStartup()
*
* Description : Do startup wait.
*
*               [RTU transmission mode]:
*                   Wait for the serial line to be idle for 3.5 character time.
*
*               [ASCII transmission mode]:
*                   No delay (return immediately).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) timeout        Timeout value (unit: milliseconds, set to 0 to wait infinitely).
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                   No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST         'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER      Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED        Device is not opened.
*                                      MB_ERROR_DEVICEFAIL             Device operation failed.
*                                      MB_ERROR_OVERFLOW               'timeout' parameter exceeds maximum allowed value.
*                                      MB_ERROR_TIMEOUT                Timeout limit exceeds.
*                                      MB_ERROR_RXTOOMANY              Too many receive requests.
*                                      MB_ERROR_OS_FGRP_FAILEDPEND     Failed to pend on a flag group object.
*                                      MB_ERROR_OS_FGRP_FAILEDPOST     Failed to post to a flag group object.
*                                      MB_ERROR_OS_MUTEX_FAILEDPEND    Failed to pend on a mutex object.
*                                      MB_ERROR_OS_TIMER_FAILEDCREATE  Failed to create timer object.
*                                      MB_ERROR_OS_TIMER_FAILEDSTART   Failed to start a timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) The value of 'timeout' parameter should be within following range:
*
*                       (0, MBOS_GetMaxTimeValue()]
*
*               (2) The function would be blocked (without respecting the 'timeout' parameter) until previous I/O finished.
*********************************************************************************************************
*/

void MB_WaitStartup(
    MB_IFINDEX             ifnbr,
    MB_TIMESPAN            timeout,
    MB_ERROR              *p_error
);


/*
*********************************************************************************************************
*                                    MB_ReceiveFrame()
*
* Description : Receive a Modbus frame from a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_buffer       Pointer to the first element of the data buffer.
*               (3) buffer_size    Size of the data buffer.
*               (4) p_frame        Pointer to the variable that receives the frame.
*               (5) p_frameflags   Pointer to the variable that receives the frame flags (optional, NULL if not needed):
*
*                                      MB_FRAMEFLAGS_DROP                Frame should be dropped.
*                                      MB_FRAMEFLAGS_BUFFEROVERFLOW      Data buffer is too small to contain 
*                                                                        the frame data.
*                                      MB_FRAMEFLAGS_CHECKSUMMISMATCH    CRC/LRC checksum mismatched.
*                                      MB_FRAMEFLAGS_TRUNCATED           Frame is truncated.
*                                      MB_FRAMEFLAGS_PARITYERROR         Parity error occurred.
*                                      MB_FRAMEFLAGS_OVERRUNERROR        Data overrun error occurred.
*                                      MB_FRAMEFLAGS_FRAMEERROR          Frame error occurred.
*                                      MB_FRAMEFLAGS_REDUNDANTBYTE       (RTU) One or more byte received after 1.5 character time exceeds.
*                                      MB_FRAMEFLAGS_INVALIDBYTE         (ASCII) One or more non-ASCII or invalid ASCII 
*                                                                        character(s) was/were received.
*
*               (6) timeout        Timeout value (unit: milliseconds).
*               (7) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                   No error occurred.
*                                      MB_ERROR_NULLREFERENCE          'p_frame' is NULL or 'p_buffer' is NULL while buffer_size is not zero.
*                                      MB_ERROR_INVALIDMODE            Device transmission mode is not supported.
*                                      MB_ERROR_DEVICENOTEXIST         'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER      Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED        Device is not opened.
*                                      MB_ERROR_DEVICEFAIL             Device operation failed.
*                                      MB_ERROR_UNDERFLOW              'timeout' parameter equals to zero.
*                                      MB_ERROR_OVERFLOW               'timeout' parameter exceeds maximum allowed value.
*                                      MB_ERROR_TIMEOUT                Timeout limit exceeds.
*                                      MB_ERROR_RXTOOMANY              Too many receive requests.
*                                      MB_ERROR_OS_FGRP_FAILEDPEND     Failed to pend on a flag group object.
*                                      MB_ERROR_OS_FGRP_FAILEDPOST     Failed to post to a flag group object.
*                                      MB_ERROR_OS_MUTEX_FAILEDPEND    Failed to pend on a mutex object.
*                                      MB_ERROR_OS_TIMER_FAILEDCREATE  Failed to create timer object.
*                                      MB_ERROR_OS_TIMER_FAILEDSTART   Failed to start a timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) The value of 'timeout' parameter should be within following range:
*
*                       (0, MBOS_GetMaxTimeValue()]
*
*               (2) The 'timeout' parameter only affects the time of waiting a frame on the serial line.
*               (3) The function would be blocked (without respecting the 'timeout' parameter) until previous I/O finished.
*********************************************************************************************************
*/

void MB_ReceiveFrame(
    MB_IFINDEX             ifnbr,
    CPU_INT08U            *p_buffer,
    CPU_SIZE_T             buffer_size,
    MB_FRAME              *p_frame,
    MB_FRAMEFLAGS         *p_frameflags,
    MB_TIMESPAN            timeout,
    MB_ERROR              *p_error
);


/*
*********************************************************************************************************
*                                    MB_TransmitFrame()
*
* Description : Transmit a Modbus frame through a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_frame        Pointer to the variable that receives the frame.
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                   No error occurred.
*                                      MB_ERROR_NULLREFERENCE          'p_frame' is NULL or 'pframe->data' is NULL while 'pframe->dataLength' is not zero.
*                                      MB_ERROR_INVALIDMODE            Device transmission mode is not supported.
*                                      MB_ERROR_DEVICENOTEXIST         'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER      Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED        Device is not opened.
*                                      MB_ERROR_DEVICEFAIL             Device operation failed.
*                                      MB_ERROR_TXTOOMANY              Too many transmit requests.
*                                      MB_ERROR_OS_FGRP_FAILEDPEND     Failed to pend on a flag group object.
*                                      MB_ERROR_OS_FGRP_FAILEDPOST     Failed to post to a flag group object.
*                                      MB_ERROR_OS_MUTEX_FAILEDPEND    Failed to pend on a mutex object.
*                                      MB_ERROR_OS_TIMER_FAILEDCREATE  Failed to create timer object.
*                                      MB_ERROR_OS_TIMER_FAILEDSTART   Failed to start a timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) The function would be blocked until previous I/O finished.
*********************************************************************************************************
*/

void MB_TransmitFrame(
    MB_IFINDEX             ifnbr,
    MB_FRAME              *p_frame,
    MB_ERROR              *p_error
);


#if (MB_CFG_CORE_GETCOUNTERVALUE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetCounterValue()
*
* Description : Get the value of the specific counter of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) type           Counter type, one of following values:
*
*                                      MB_COUNTERTYPE_PARITYERROR        Parity error counter.
*                                      MB_COUNTERTYPE_DATAOVERRUNERROR   Data overrun error counter.
*                                      MB_COUNTERTYPE_FRAMEERROR         Frame error counter.
*
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*                                      MB_ERROR_INVALIDCOUNTER           'type' parameter contains invalid value.
*
* Return(s)   : The counter value.
*********************************************************************************************************
*/

MB_COUNTERVALUE  MB_GetCounterValue(
    MB_IFINDEX             ifnbr,
    MB_COUNTERTYPE         type,
    MB_ERROR              *p_error
);
#endif


#if (MB_CFG_CORE_CLEARCOUNTERVALUE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_ClearCounterValue()
*
* Description : Clear the value of the specific counter of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) type           Counter type, one of following values:
*
*                                      MB_COUNTERTYPE_PARITYERROR        Parity error counter.
*                                      MB_COUNTERTYPE_DATAOVERRUNERROR   Data overrun error counter.
*                                      MB_COUNTERTYPE_FRAMEERROR         Frame error counter.
*
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*                                      MB_ERROR_INVALIDCOUNTER           'type' parameter contains invalid value.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void  MB_ClearCounterValue(
    MB_IFINDEX             ifnbr,
    MB_COUNTERTYPE         type,
    MB_ERROR              *p_error
);
#endif


#if (MB_CFG_CORE_GETLASTTXADDRESS == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetLastTxAddress()
*
* Description : Get the address of the last transmitted frame of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*
* Return(s)   : The address.
*
* Note(s)     : (1) If no frame transmitted yet, 0 would be returned.
*********************************************************************************************************
*/

CPU_INT08U  MB_GetLastTxAddress(
    MB_IFINDEX             ifnbr,
    MB_ERROR              *p_error
);
#endif  /*  #if (MB_CFG_CORE_GETLASTTXADDRESS == DEF_ENABLED)  */


#if (MB_CFG_CORE_GETLASTTXFUNCTIONCODE == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetLastTxFunctionCode()
*
* Description : Get the function code of the last transmitted frame of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*
* Return(s)   : The function code.
*
* Note(s)     : (1) If no frame transmitted yet, 0 would be returned.
*********************************************************************************************************
*/

CPU_INT08U  MB_GetLastTxFunctionCode(
    MB_IFINDEX             ifnbr,
    MB_ERROR              *p_error
);
#endif  /*  #if (MB_CFG_CORE_GETLASTTXFUNCTIONCODE == DEF_ENABLED)  */


#if (MB_CFG_CORE_GETLASTTXEXCEPTIONCODE == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetLastTxExceptionCode()
*
* Description : Get the exception code of the last transmitted exception frame of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*
* Return(s)   : The exception code.
*
* Note(s)     : (1) If no exception frame transmitted yet, 0 would be returned.
*********************************************************************************************************
*/

CPU_INT08U  MB_GetLastTxExceptionCode(
    MB_IFINDEX             ifnbr,
    MB_ERROR              *p_error
);
#endif  /*  #if (MB_CFG_CORE_GETLASTTXEXCEPTIONCODE == DEF_ENABLED)  */


#if (MB_CFG_CORE_CLEARLASTTXEXCEPTIONCODE == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_ClearLastTxExceptionCode()
*
* Description : Clear the exception code of the last transmitted exception frame of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*
* Return(s)   : None.
*
* Note(s)     : (1) After calling this function, it will become as if no exception frame was sent on the Modbus 
*                   device.
*********************************************************************************************************
*/

void  MB_ClearLastTxExceptionCode(
    MB_IFINDEX             ifnbr,
    MB_ERROR              *p_error
);
#endif  /*  #if (MB_CFG_CORE_CLEARLASTTXEXCEPTIONCODE == DEF_ENABLED)  */


#ifdef __cplusplus
}
#endif

#endif