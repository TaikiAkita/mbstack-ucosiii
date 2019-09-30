/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_CONSTANTS.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_CONSTANTS_H__
#define MB_CONSTANTS_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_cfg.h>

#include <lib_def.h>


#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*  Error codes.*/
#define MB_ERROR_NONE                                ((MB_ERROR)0U)
#define MB_ERROR_INVALIDPARAMETER                    ((MB_ERROR)1U)
#define MB_ERROR_OVERFLOW                            ((MB_ERROR)2U)
#define MB_ERROR_UNDERFLOW                           ((MB_ERROR)3U)
#define MB_ERROR_NULLREFERENCE                       ((MB_ERROR)4U)
#define MB_ERROR_TIMEOUT                             ((MB_ERROR)5U)
#define MB_ERROR_INVALIDCOUNTER                      ((MB_ERROR)6U)
#define MB_ERROR_INVALIDMODE                         ((MB_ERROR)7U)

#define MB_ERROR_DEVICEFAIL                         ((MB_ERROR)10U)
#define MB_ERROR_DEVICEOPENED                       ((MB_ERROR)11U)
#define MB_ERROR_DEVICENOTOPENED                    ((MB_ERROR)12U)
#define MB_ERROR_DEVICEBUSY                         ((MB_ERROR)13U)
#define MB_ERROR_DEVICEEXISTED                      ((MB_ERROR)14U)
#define MB_ERROR_DEVICENOFREE                       ((MB_ERROR)15U)
#define MB_ERROR_DEVICENOTEXIST                     ((MB_ERROR)16U)
#define MB_ERROR_DEVICENOTREGISTER                  ((MB_ERROR)17U)
#define MB_ERROR_DEVICEMODEMISMATCH                 ((MB_ERROR)18U)

#define MB_ERROR_RXTOOMANY                          ((MB_ERROR)30U)
#define MB_ERROR_TXTOOMANY                          ((MB_ERROR)31U)

#define MB_ERROR_FRAMEDEC_INVALIDSTATE              ((MB_ERROR)40U)
#define MB_ERROR_FRAMEDEC_INVALIDCHAR               ((MB_ERROR)41U)

#define MB_ERROR_FRAMEENC_INVALIDSTATE              ((MB_ERROR)50U)
#define MB_ERROR_FRAMEENC_FRAMEEND                  ((MB_ERROR)51U)

#define MB_ERROR_BUFFETCHER_BUFFEREND               ((MB_ERROR)70U)

#define MB_ERROR_BUFEMITTER_BUFFEREND               ((MB_ERROR)75U)

#define MB_ERROR_OS_MUTEX_FAILEDCREATE             ((MB_ERROR)100U)
#define MB_ERROR_OS_MUTEX_FAILEDDISPOSE            ((MB_ERROR)101U)
#define MB_ERROR_OS_MUTEX_FAILEDPEND               ((MB_ERROR)102U)
#define MB_ERROR_OS_MUTEX_FAILEDPOST               ((MB_ERROR)103U)

#define MB_ERROR_OS_FGRP_FAILEDCREATE              ((MB_ERROR)110U)
#define MB_ERROR_OS_FGRP_FAILEDDISPOSE             ((MB_ERROR)111U)
#define MB_ERROR_OS_FGRP_FAILEDPEND                ((MB_ERROR)112U)
#define MB_ERROR_OS_FGRP_FAILEDPOST                ((MB_ERROR)113U)
#define MB_ERROR_OS_FGRP_FAILEDRDYFLAGS            ((MB_ERROR)114U)

#define MB_ERROR_OS_TIMER_FAILEDCREATE             ((MB_ERROR)120U)
#define MB_ERROR_OS_TIMER_FAILEDDISPOSE            ((MB_ERROR)121U)
#define MB_ERROR_OS_TIMER_FAILEDSTART              ((MB_ERROR)122U)
#define MB_ERROR_OS_TIMER_FAILEDSTOP               ((MB_ERROR)123U)

#define MB_ERROR_OS_TIME_FAILEDDELAY               ((MB_ERROR)125U)
#define MB_ERROR_OS_TIME_FAILEDGET                 ((MB_ERROR)126U)

#define MB_ERROR_SLAVE_REQUESTTRUNCATED            ((MB_ERROR)150U)
#define MB_ERROR_SLAVE_RESPONSETRUNCATED           ((MB_ERROR)151U)
#define MB_ERROR_SLAVE_CALLBACKFAILED              ((MB_ERROR)152U)
#define MB_ERROR_SLAVE_FUNCTIONCODEINVALID         ((MB_ERROR)153U)
#define MB_ERROR_SLAVE_FUNCTIONCODEEXISTED         ((MB_ERROR)154U)
#define MB_ERROR_SLAVE_NOFREETABLEITEM             ((MB_ERROR)155U)
#define MB_ERROR_SLAVE_LISTENONLYALREADYENTERED    ((MB_ERROR)156U)
#define MB_ERROR_SLAVE_LISTENONLYALREADYEXITED     ((MB_ERROR)157U)
#define MB_ERROR_SLAVE_STILLPOLLING                ((MB_ERROR)158U)

#define MB_ERROR_MASTER_RXBUFFERLOW                ((MB_ERROR)160U)
#define MB_ERROR_MASTER_TXBUFFERLOW                ((MB_ERROR)160U)
#define MB_ERROR_MASTER_RXTRUNCATED                ((MB_ERROR)161U)
#define MB_ERROR_MASTER_RXINVALIDFNCODE            ((MB_ERROR)162U)
#define MB_ERROR_MASTER_RXINVALIDFORMAT            ((MB_ERROR)163U)
#define MB_ERROR_MASTER_RXINVALIDSLAVE             ((MB_ERROR)164U)
#define MB_ERROR_MASTER_TXBADREQUEST               ((MB_ERROR)165U)

#define MB_ERROR_MASTER_STILLBUSY                  ((MB_ERROR)168U)

#define MB_ERROR_MASTER_CALLBACKFAILED             ((MB_ERROR)170U)

/*  Half-duplex modes.  */
#define MB_HALFDUPLEX_RECEIVE                   ((MB_DUPLEXMODE)0U)
#define MB_HALFDUPLEX_TRANSMIT                  ((MB_DUPLEXMODE)1U)

/*  Modbus transmission modes.  */
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
#define MB_TRMODE_RTU                               ((MB_TRMODE)1U)
#endif
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
#define MB_TRMODE_ASCII                             ((MB_TRMODE)2U)
#endif

/*  Serial baudrates.  */
#define MB_SERIAL_BAUDRATE_110                 ((MB_BAUDRATE)110UL)
#define MB_SERIAL_BAUDRATE_300                 ((MB_BAUDRATE)300UL)
#define MB_SERIAL_BAUDRATE_1200               ((MB_BAUDRATE)1200UL)
#define MB_SERIAL_BAUDRATE_2400               ((MB_BAUDRATE)2400UL)
#define MB_SERIAL_BAUDRATE_4800               ((MB_BAUDRATE)4800UL)
#define MB_SERIAL_BAUDRATE_9600               ((MB_BAUDRATE)9600UL)
#define MB_SERIAL_BAUDRATE_19200             ((MB_BAUDRATE)19200UL)
#define MB_SERIAL_BAUDRATE_38400             ((MB_BAUDRATE)38400UL)
#define MB_SERIAL_BAUDRATE_57600             ((MB_BAUDRATE)57600UL)
#define MB_SERIAL_BAUDRATE_115200           ((MB_BAUDRATE)115200UL)
#define MB_SERIAL_BAUDRATE_230400           ((MB_BAUDRATE)230400UL)
#define MB_SERIAL_BAUDRATE_460800           ((MB_BAUDRATE)460800UL)
#define MB_SERIAL_BAUDRATE_921600           ((MB_BAUDRATE)921600UL)

/*  Serial data bits.  */
#define MB_SERIAL_DATABITS_7                      ((MB_DATABITS)7U)
#define MB_SERIAL_DATABITS_8                      ((MB_DATABITS)8U)

/*  Serial stop bits.  */
#define MB_SERIAL_STOPBITS_1                      ((MB_STOPBITS)1U)
#define MB_SERIAL_STOPBITS_1D5                    ((MB_STOPBITS)2U)
#define MB_SERIAL_STOPBITS_2                      ((MB_STOPBITS)3U)

/*  Serial parity modes.  */
#define MB_SERIAL_PARITY_NONE                       ((MB_PARITY)0U)
#define MB_SERIAL_PARITY_ODD                        ((MB_PARITY)1U)
#define MB_SERIAL_PARITY_EVEN                       ((MB_PARITY)2U)
#define MB_SERIAL_PARITY_MARK                       ((MB_PARITY)3U)
#define MB_SERIAL_PARITY_SPACE                      ((MB_PARITY)4U)

/*  Modbus frame flags.  */
#define MB_FRAMEFLAGS_DROP                      ((MB_FRAMEFLAGS)1U)
#define MB_FRAMEFLAGS_BUFFEROVERFLOW            ((MB_FRAMEFLAGS)2U)
#define MB_FRAMEFLAGS_CHECKSUMMISMATCH          ((MB_FRAMEFLAGS)4U)
#define MB_FRAMEFLAGS_TRUNCATED                 ((MB_FRAMEFLAGS)8U)
#define MB_FRAMEFLAGS_REDUNDANTBYTE            ((MB_FRAMEFLAGS)16U)
#define MB_FRAMEFLAGS_INVALIDBYTE              ((MB_FRAMEFLAGS)32U)
#define MB_FRAMEFLAGS_PARITYERROR              ((MB_FRAMEFLAGS)64U)
#define MB_FRAMEFLAGS_OVERRUNERROR            ((MB_FRAMEFLAGS)128U)
#define MB_FRAMEFLAGS_FRAMEERROR              ((MB_FRAMEFLAGS)256U)

/*  (OS module only) Timer modes.  */
#define MB_TIMER_MODE_ONESHOT                    ((MB_TIMERMODE)1U)
#define MB_TIMER_MODE_PERIODIC                   ((MB_TIMERMODE)2U)

/*  Count of valid Modbus function codes.  */
#define MB_VALID_FUNCTION_CODES                  ((CPU_INT08U)127U)

/*  Modbus application protocol exception codes.  */
#define MB_APUEC_ILLEGALFUNCTION                ((CPU_INT08U)0x01U)
#define MB_APUEC_ILLEGALDATAADDRESS             ((CPU_INT08U)0x02U)
#define MB_APUEC_ILLEGALDATAVALUE               ((CPU_INT08U)0x03U)
#define MB_APUEC_SERVERDEVICEFAILURE            ((CPU_INT08U)0x04U)
#define MB_APUEC_ACKNOWLEDGE                    ((CPU_INT08U)0x05U)
#define MB_APUEC_SERVERDEVICEBUSY               ((CPU_INT08U)0x06U)
#define MB_APUEC_MEMORYPARITYERROR              ((CPU_INT08U)0x08U)
#define MB_APUEC_GWPATHUNAVAILABLE              ((CPU_INT08U)0x0AU)
#define MB_APUEC_GWTARGETFAILEDTORESPOND        ((CPU_INT08U)0x0BU)

/*  Modbus function codes.  */
#define MB_FNCODE_READCOILS                     ((CPU_INT08U)0x01U)
#define MB_FNCODE_READDISCRETEINPUTS            ((CPU_INT08U)0x02U)
#define MB_FNCODE_READHOLDINGREGISTERS          ((CPU_INT08U)0x03U)
#define MB_FNCODE_READINPUTREGISTERS            ((CPU_INT08U)0x04U)
#define MB_FNCODE_WRITESINGLECOIL               ((CPU_INT08U)0x05U)
#define MB_FNCODE_WRITESINGLEREGISTER           ((CPU_INT08U)0x06U)
#define MB_FNCODE_READEXCEPTIONSTATUS           ((CPU_INT08U)0x07U)
#define MB_FNCODE_DIAGNOSTICS                   ((CPU_INT08U)0x08U)
#define MB_FNCODE_GETCOMMEVENTCOUNTER           ((CPU_INT08U)0x0BU)
#define MB_FNCODE_GETCOMMEVENTLOG               ((CPU_INT08U)0x0CU)
#define MB_FNCODE_WRITEMULTIPLECOILS            ((CPU_INT08U)0x0FU)
#define MB_FNCODE_WRITEMULTIPLEREGISTERS        ((CPU_INT08U)0x10U)
#define MB_FNCODE_REPORTSERVERID                ((CPU_INT08U)0x11U)
#define MB_FNCODE_READFILERECORD                ((CPU_INT08U)0x14U)
#define MB_FNCODE_WRITEFILERECORD               ((CPU_INT08U)0x15U)
#define MB_FNCODE_MASKWRITEREGISTER             ((CPU_INT08U)0x16U)
#define MB_FNCODE_READWRITEMULTIPLEREGISTERS    ((CPU_INT08U)0x17U)
#define MB_FNCODE_READFIFOQUEUE                 ((CPU_INT08U)0x18U)
#define MB_FNCODE_ENCAPSULATEDINTERFACE         ((CPU_INT08U)0x2BU)


#ifdef __cplusplus
}
#endif

#endif