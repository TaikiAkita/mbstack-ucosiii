/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                            OS ABSTRACT LAYER
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
*                                             uC/OS-III Port
*
* File      : MB_OS_TYPES.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_OS_TYPES_H__
#define MB_OS_TYPES_H__

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_os_basetypes.h>

#include <os.h>


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

/*  OS mutex type.  */
typedef struct {
    OS_MUTEX          mutex;
} MB_MUTEX;

/*  OS flag group type.  */
typedef struct {
    OS_FLAG_GRP       fgrp;
} MB_FLAGGROUP;

/*  OS timer type.  */
typedef struct {
    OS_TMR            tmr;
    MB_TIMERCALLBACK  cb;
    void             *cb_arg;
} MB_TIMER;

/*  OS flags type.  */
typedef OS_FLAGS      MB_FLAGS;

/*  OS option type.  */
typedef OS_OPT        MB_OPT;

/*  OS system tick type.  */
typedef OS_TICK       MB_SYSTICK;

/*  OS timespan type.  */
typedef CPU_INT32U    MB_TIMESPAN;


#endif