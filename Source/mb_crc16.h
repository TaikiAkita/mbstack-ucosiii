/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_CRC16.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_CRC16_H__
#define MB_CRC16_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_cfg.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT16U crc;
} MBCRC16_CTX;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    MBCRC16_Initialize()
*
* Description : Initialize a CRC-16 context.
*
* Argument(s) : ctx  The CRC-16 context.
*
* Return(s)   : None.
*
* Note(s)     : (1) 'ctx' is assumed to be not NULL.
*               (2) This function is not thread(task)-safe.
*********************************************************************************************************
*/

void MBCRC16_Initialize(MBCRC16_CTX *ctx);


/*
*********************************************************************************************************
*                                      MBCRC16_Update()
*
* Description : Update a CRC-16 context with specified character.
*
* Argument(s) : ctx  The CRC-16 context.
*               ch   The character.
*
* Return(s)   : None.
*
* Note(s)     : (1) 'ctx' is assumed to be not NULL.
*               (2) This function is not thread(task)-safe.
*********************************************************************************************************
*/

void MBCRC16_Update(MBCRC16_CTX *ctx, CPU_INT08U ch);


/*
*********************************************************************************************************
*                                      MBCRC16_Final()
*
* Description : Finalize a CRC-16 context.
*
* Argument(s) : ctx  The CRC-16 context.
*
* Return(s)   : The CRC-16 checksum.
*
* Note(s)     : (1) 'ctx' is assumed to be not NULL.
*               (2) This function is not thread(task)-safe.
*********************************************************************************************************
*/

CPU_INT16U MBCRC16_Final(MBCRC16_CTX *ctx);


/*
*********************************************************************************************************
*                                     MBCRC16Util_HiLo()
*
* Description : Split a calculated CRC-16 value to two-bytes representation.
*
* Argument(s) : crc   The CRC checksum.
*               p_hi  Pointer to the variable that receives the high-order byte.
*               p_lo  Pointer to the variable that receives the low-order byte.
*
* Return(s)   : None.
*
* Note(s)     : (1) 'p_hi' and 'p_lo' are assumed to be not NULL.
*********************************************************************************************************
*/

void MBCRC16Util_HiLo(
    CPU_INT16U   crc,
    CPU_INT08U  *p_hi,
    CPU_INT08U  *p_lo
);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */

#endif