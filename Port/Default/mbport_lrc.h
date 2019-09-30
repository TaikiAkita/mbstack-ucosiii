/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                               PORT LAYER
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
*                                         Default Implementation
*
* File      : MBPORT_LRC.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBPORT_LRC_H__
#define MBPORT_LRC_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mbport_cfg.h>

#include <cpu.h>

#include <lib_cfg.h>


#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)

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
*                                  MBPort_LRCUtil_HiLo()
*
* Description : Split a calculated LRC value to two-bytes representation.
*
* Argument(s) : lrc   The LRC checksum.
*               p_hi  Pointer to the variable that receives the high-order byte.
*               p_lo  Pointer to the variable that receives the low-order byte.
*
* Return(s)   : None.
*
* Note(s)     : (1) 'p_hi' and 'p_lo' are assumed to be not NULL.
*********************************************************************************************************
*/

void MBPort_LRCUtil_HiLo(CPU_INT08U lrc, CPU_INT08U *p_hi, CPU_INT08U *p_lo);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)  */

#endif