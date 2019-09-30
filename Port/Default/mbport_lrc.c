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
* File      : MBPORT_LRC.C
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

#define MBPORT_SOURCE
#define MBPORT_LRC_SOURCE

#include <mbport_lrc.h>
#include <mbport_cfg.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static const CPU_INT08U g_LRCHiLoTable[] = {
    (CPU_INT08U)'0',
    (CPU_INT08U)'1',
    (CPU_INT08U)'2',
    (CPU_INT08U)'3',
    (CPU_INT08U)'4',
    (CPU_INT08U)'5',
    (CPU_INT08U)'6',
    (CPU_INT08U)'7',
    (CPU_INT08U)'8',
    (CPU_INT08U)'9',
    (CPU_INT08U)'A',
    (CPU_INT08U)'B',
    (CPU_INT08U)'C',
    (CPU_INT08U)'D',
    (CPU_INT08U)'E',
    (CPU_INT08U)'F'
};


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

void MBPort_LRCUtil_HiLo(CPU_INT08U lrc, CPU_INT08U *p_hi, CPU_INT08U *p_lo) {
    CPU_INT08U  offset;

    /*  Get the high-order byte.  */
    offset = (CPU_INT08U)((lrc & 0xF0U) >> 4);
    *p_hi  = g_LRCHiLoTable[(CPU_SIZE_T)offset];

    /*  Get the low-order byte.  */
    offset = (CPU_INT08U) (lrc & 0x0FU);
    *p_lo  = g_LRCHiLoTable[(CPU_SIZE_T)offset];
}

#endif  /*  #if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)  */