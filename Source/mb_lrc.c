/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_LRC.C
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
#define MB_LRC_SOURCE

#include <mb_lrc.h>
#include <mb_cfg.h>

#include <mbport_lrc.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    MBLRC_Initialize()
*
* Description : Initialize a LRC checksum context.
*
* Argument(s) : ctx  The LRC context.
*
* Return(s)   : None.
*
* Note(s)     : (1) 'ctx' is assumed to be not NULL.
*               (2) This function is not thread(task)-safe.
*********************************************************************************************************
*/

void MBLRC_Initialize(MBLRC_CTX *ctx) {
    ctx->lrc = (CPU_INT08U)0x00U;
}


/*
*********************************************************************************************************
*                                      MBLRC_Update()
*
* Description : Update a LRC checksum context with specified character.
*
* Argument(s) : ctx  The LRC context.
*               ch   The character.
*
* Return(s)   : None.
*
* Note(s)     : (1) 'ctx' is assumed to be not NULL.
*               (2) This function is not thread(task)-safe.
*********************************************************************************************************
*/

void MBLRC_Update(MBLRC_CTX *ctx, CPU_INT08U ch) {
    ctx->lrc += ch;
}


/*
*********************************************************************************************************
*                                      MBLRC_Final()
*
* Description : Finalize a LRC checksum context.
*
* Argument(s) : ctx  The LRC context.
*
* Return(s)   : The LRC checksum.
*
* Note(s)     : (1) 'ctx' is assumed to be not NULL.
*               (2) This function is not thread(task)-safe.
*********************************************************************************************************
*/

CPU_INT08U MBLRC_Final(MBLRC_CTX *ctx) {
    CPU_INT08U lrc = ctx->lrc;
    lrc ^= (CPU_INT08U)0xFF;
    lrc++;
    return lrc;
}


/*
*********************************************************************************************************
*                                      MBLRCUtil_HiLo()
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

void MBLRCUtil_HiLo(CPU_INT08U lrc, CPU_INT08U *p_hi, CPU_INT08U *p_lo) {
    MBPort_LRCUtil_HiLo(lrc, p_hi, p_lo);
}

#endif  /*  #if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)  */