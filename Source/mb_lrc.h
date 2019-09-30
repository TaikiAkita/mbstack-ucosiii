/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_LRC.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_LRC_H__
#define MB_LRC_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_cfg.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT08U lrc;
} MBLRC_CTX;



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

void MBLRC_Initialize(MBLRC_CTX *ctx);


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

void MBLRC_Update(MBLRC_CTX *ctx, CPU_INT08U ch);


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

CPU_INT08U MBLRC_Final(MBLRC_CTX *ctx);


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

void MBLRCUtil_HiLo(CPU_INT08U lrc, CPU_INT08U *p_hi, CPU_INT08U *p_lo);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)  */

#endif