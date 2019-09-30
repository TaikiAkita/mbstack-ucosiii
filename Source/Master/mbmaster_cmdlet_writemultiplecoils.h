/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_WRITEMULTIPLECOILS.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_WRITEMULTIPLECOILS_H__
#define MBMASTER_CMDLET_WRITEMULTIPLECOILS_H__


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


#if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLECOILS == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT16U                      coilStartAddress;
    CPU_BOOLEAN                    *coilValues;
    CPU_INT16U                      coilQuantity;
} MBMASTER_CMDLET_WRITEMULTIPLECOILS_REQUEST;


typedef struct {
    MBMASTER_EXCEPTION_CB           cbException;

    MBMASTER_COILSWRITTEN_CB        cbComplete;
} MBMASTER_CMDLET_WRITEMULTIPLECOILS_RESPONSE;


/*
*********************************************************************************************************
*                                       COMMAND-LET DESCRIPTOR
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_WRITECOILS_SOURCE
extern MBMASTER_CMDLET                                     g_MBMaster_CmdLet_FC0F;
#endif

#define MBMASTER_CMDLETDESCRIPTOR_WRITEMULTIPLECOILS   (&(g_MBMaster_CmdLet_FC0F))


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLECOILS == DEF_ENABLED)  */

#endif