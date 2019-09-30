/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_READDISCRETEINPUTS.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_READDISCRETEINPUTS_H__
#define MBMASTER_CMDLET_READDISCRETEINPUTS_H__


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


#if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_READDISCRETEINPUTS_EN == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT16U                                   discreteInputStartAddress;
    CPU_INT16U                                   discreteInputQuantity;
} MBMASTER_CMDLET_READDISCRETEINPUTS_REQUEST;


typedef struct {
    MBMASTER_EXCEPTION_CB                        cbException;

    MBMASTER_DISCRETEINPUTVALUE_START_CB         cbDiscreteInputValueStart;
    MBMASTER_DISCRETEINPUTVALUE_UPDATE_CB        cbDiscreteInputValueUpdate;
    MBMASTER_DISCRETEINPUTVALUE_END_CB           cbDiscreteInputValueEnd;
} MBMASTER_CMDLET_READDISCRETEINPUTS_RESPONSE;


/*
*********************************************************************************************************
*                                       COMMAND-LET DESCRIPTOR
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_READDISCRETEINPUTS_SOURCE
extern MBMASTER_CMDLET                                       g_MBMaster_CmdLet_FC02;
#endif

#define MBMASTER_CMDLETDESCRIPTOR_READDISCRETEINPUTS     (&(g_MBMaster_CmdLet_FC02))


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_READDISCRETEINPUTS_EN == DEF_ENABLED)  */

#endif