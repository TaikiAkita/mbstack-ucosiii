/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_CRC16.C
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
#define MB_CRC16_SOURCE

#include <mb_crc16.h>
#include <mb_cfg.h>

#include <mbport_crc16.h>
#include <mbport_limits.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)

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

void MBCRC16_Initialize(MBCRC16_CTX *ctx) {
    ctx->crc = MBPORT_UINT16_MAX;
}


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

void MBCRC16_Update(MBCRC16_CTX *ctx, CPU_INT08U ch) {
    CPU_INT08U offset;
    CPU_INT16U table_item;
    offset = (CPU_INT08U)((ctx->crc ^ (CPU_INT16U)ch) & (CPU_INT16U)0xFFU);
    table_item = MBPort_CRC16_GetTableItem(offset);
    ctx->crc = ((ctx->crc >> 8) ^ table_item);
}


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

CPU_INT16U MBCRC16_Final(MBCRC16_CTX *ctx) {
    return ctx->crc;
}


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
) {
    *p_lo = (CPU_INT08U)(crc & (CPU_INT16U)0xFFU);
    *p_hi = (CPU_INT08U)(crc >> 8);
}

#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */