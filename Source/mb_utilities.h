/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_UTILITIES.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_UTILITIES_H__
#define MB_UTILITIES_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_types.h>

#include <mbdrv_types.h>

#include <cpu.h>


#ifdef __cplusplus
extern  "C" {
#endif

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    MBUtil_GetSerialCharacterTime()
*
* Description : Get the time that each character would cost on the serial line (unit: microseconds).
*
* Argument(s) : (1) p_setup  Pointer to the variable that stores the serial port configuration.
*               (2) p_error  Pointer to the variable that receives the error code from this function:
*
*                                MB_ERROR_NONE              No error occurred.
*                                MB_ERROR_NULLREFERENCE     'p_setup' is NULL.
*                                MB_ERROR_INVALIDPARAMETER  Serial port configuration corrupted.
*
* Return(s)   : The character time.
*
* Note(s)     : (1) The return value is ensured to be zero if failed.
*               (2) The return value is ensured to be positive if succeed.
*********************************************************************************************************
*/

CPU_INT32U MBUtil_GetSerialCharacterTime(
    MB_SERIAL_SETUP *p_setup,
    MB_ERROR        *p_error
);


/*
*********************************************************************************************************
*                                  MBUtil_GetHalfSerialCharacterTime()
*
* Description : Get the time that half character would cost on the serial line (unit: microseconds).
*
* Argument(s) : (1) p_setup  Pointer to the variable that stores the serial port configuration.
*               (2) p_error  Pointer to the variable that receives the error code from this function:
*
*                                MB_ERROR_NONE              No error occurred.
*                                MB_ERROR_NULLREFERENCE     'p_setup' is NULL.
*                                MB_ERROR_INVALIDPARAMETER  Serial port configuration corrupted.
*
* Return(s)   : The half character time.
*
* Note(s)     : (1) The return value is ensured to be zero if failed.
*               (2) The return value is ensured to be positive if succeed.
*********************************************************************************************************
*/

CPU_INT32U MBUtil_GetHalfSerialCharacterTime(
    MB_SERIAL_SETUP *p_setup,
    MB_ERROR        *p_error
);


#ifdef __cplusplus
}
#endif

#endif