/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_TYPES.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_TYPES_H__
#define MB_TYPES_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mbport_limits.h>

#include <cpu.h>


#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*  Maximum value of counter value.  */
#define MB_COUNTERVALUE_MAX             ((MB_COUNTERVALUE)MBPORT_UINT32_MAX)


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

/*  Modbus error type.  */
typedef CPU_INT08U MB_ERROR;

/*  Modbus duplex mode type.  */
typedef CPU_INT08U MB_DUPLEXMODE;

/*  Modbus transmission mode type.  */
typedef CPU_INT08U MB_TRMODE;

/*  Modbus serial baudrate type.  */
typedef CPU_INT32U MB_BAUDRATE;

/*  Modbus serial data bits type.  */
typedef CPU_INT08U MB_DATABITS;

/*  Modbus serial stop bits type.  */
typedef CPU_INT08U MB_STOPBITS;

/*  Modbus serial parity mode type.  */
typedef CPU_INT08U MB_PARITY;

/*  Modbus interface index type.  */
typedef CPU_SIZE_T MB_IFINDEX;

/*  Modbus counter-type type.  */
typedef CPU_INT32U MB_COUNTERTYPE;

/*  Modbus counter-value type.  */
typedef CPU_INT32U MB_COUNTERVALUE;

/*  Modbus frame flag type.  */
typedef CPU_INT16U MB_FRAMEFLAGS;

/*  Modbus frame type.  */
typedef struct {
    CPU_INT08U   address;
    CPU_INT08U   functionCode;
    CPU_INT08U  *data;
    CPU_SIZE_T   dataLength;
} MB_FRAME;


#ifdef __cplusplus
}
#endif

#endif