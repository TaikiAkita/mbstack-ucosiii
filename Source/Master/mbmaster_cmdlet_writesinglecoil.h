/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_WRITESINGLECOIL.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_WRITESINGLECOIL_H__
#define MBMASTER_CMDLET_WRITESINGLECOIL_H__


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


#if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_WRITESINGLECOIL == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT16U                        coilAddress;
    CPU_BOOLEAN                       coilValue;
} MBMASTER_CMDLET_WRITESINGLECOIL_REQUEST;


typedef struct {
    MBMASTER_EXCEPTION_CB             cbException;

    MBMASTER_COILWRITTEN_CB           cbComplete;
} MBMASTER_CMDLET_WRITESINGLECOIL_RESPONSE;


/*
*********************************************************************************************************
*                                       COMMAND-LET DESCRIPTOR
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_WRITECOIL_SOURCE
extern MBMASTER_CMDLET                                       g_MBMaster_CmdLet_FC05;
#endif

#define MBMASTER_CMDLETDESCRIPTOR_WRITESINGLECOIL        (&(g_MBMaster_CmdLet_FC05))


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_WRITESINGLECOIL == DEF_ENABLED)  */

#endif