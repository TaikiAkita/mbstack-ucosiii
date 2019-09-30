/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_MASKWRITEREGISTER.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_MASKWRITEREGISTER_H__
#define MBMASTER_CMDLET_MASKWRITEREGISTER_H__


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


#if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_MASKWRITEREG == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT16U                        hregAddress;
    CPU_INT16U                        hregAndMask;
    CPU_INT16U                        hregOrMask;
} MBMASTER_CMDLET_MASKWRITEREGISTER_REQUEST;


typedef struct {
    MBMASTER_EXCEPTION_CB             cbException;

    MBMASTER_HREGMASKWRITTEN_CB       cbComplete;
} MBMASTER_CMDLET_MASKWRITEREGISTER_RESPONSE;


/*
*********************************************************************************************************
*                                       COMMAND-LET DESCRIPTOR
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_MASKWRITEHREG_SOURCE
extern MBMASTER_CMDLET                                     g_MBMaster_CmdLet_FC16;
#endif

#define MBMASTER_CMDLETDESCRIPTOR_MASKWRITEREGISTER    (&(g_MBMaster_CmdLet_FC16))


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_MASKWRITEREG == DEF_ENABLED)  */

#endif