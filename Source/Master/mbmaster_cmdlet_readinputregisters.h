/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_READINPUTREGISTERS.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_READINPUTREGISTERS_H__
#define MBMASTER_CMDLET_READINPUTREGISTERS_H__


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


#if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_READINPUTREGS_EN == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT16U                        iregStartAddress;
    CPU_INT16U                        iregQuantity;
} MBMASTER_CMDLET_READINPUTREGISTERS_REQUEST;


typedef struct {
    MBMASTER_EXCEPTION_CB             cbException;

    MBMASTER_IREGVALUE_START_CB       cbIRegValueStart;
    MBMASTER_IREGVALUE_UPDATE_CB      cbIRegValueUpdate;
    MBMASTER_IREGVALUE_END_CB         cbIRegValueEnd;
} MBMASTER_CMDLET_READINPUTREGISTERS_RESPONSE;


/*
*********************************************************************************************************
*                                       COMMAND-LET DESCRIPTOR
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_READIREGS_SOURCE
extern MBMASTER_CMDLET                                       g_MBMaster_CmdLet_FC04;
#endif

#define MBMASTER_CMDLETDESCRIPTOR_READINPUTREGISTERS     (&(g_MBMaster_CmdLet_FC04))


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_READINPUTREGS_EN == DEF_ENABLED)  */

#endif