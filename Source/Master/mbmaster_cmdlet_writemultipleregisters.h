/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_WRITEMULTIPLEREGISTERS.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_WRITEMULTIPLEREGISTERS_H__
#define MBMASTER_CMDLET_WRITEMULTIPLEREGISTERS_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mbmaster_cfg.h>

#include <mbmaster_cmdlet_common.h>

#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLEREGS == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT16U                        hregStartAddress;
    CPU_INT16U                       *hregValues;
    CPU_INT16U                        hregQuantity;
} MBMASTER_CMDLET_WRITEMULTIPLEREGISTERS_REQUEST;


typedef struct {
    MBMASTER_EXCEPTION_CB             cbException;

    MBMASTER_HREGSWRITTEN_CB          cbComplete;
} MBMASTER_CMDLET_WRITEMULTIPLEREGISTERS_RESPONSE;


/*
*********************************************************************************************************
*                                       COMMAND-LET DESCRIPTOR
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_WRITEHREGS_SOURCE
extern MBMASTER_CMDLET                                         g_MBMaster_CmdLet_FC10;
#endif

#define MBMASTER_CMDLETDESCRIPTOR_WRITEMULTIPLEREGISTERS   (&(g_MBMaster_CmdLet_FC10))


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLEREGS == DEF_ENABLED)  */

#endif