/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CFG.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBMASTER_CFG_H__
#define MBMASTER_CFG_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <lib_def.h>

#include <app_cfg.h>


/*
*********************************************************************************************************
*                                           DEFAULT DEFINES
*********************************************************************************************************
*/

#ifndef MB_CFG_ARG_CHK_EN
#define MB_CFG_ARG_CHK_EN                                    DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_EN
#define MB_CFG_MASTER_EN                                     DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_READCOILS_EN
#define MB_CFG_MASTER_BUILTIN_CMDLET_READCOILS_EN            DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_READDISCRETEINPUTS_EN
#define MB_CFG_MASTER_BUILTIN_CMDLET_READDISCRETEINPUTS_EN   DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_READHOLDREGS_EN
#define MB_CFG_MASTER_BUILTIN_CMDLET_READHOLDREGS_EN         DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_READINPUTREGS_EN
#define MB_CFG_MASTER_BUILTIN_CMDLET_READINPUTREGS_EN        DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_WRITESINGLEREG
#define MB_CFG_MASTER_BUILTIN_CMDLET_WRITESINGLEREG          DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_WRITESINGLECOIL
#define MB_CFG_MASTER_BUILTIN_CMDLET_WRITESINGLECOIL         DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLECOILS
#define MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLECOILS      DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLEREGS
#define MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLEREGS       DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_MASKWRITEREG
#define MB_CFG_MASTER_BUILTIN_CMDLET_MASKWRITEREG            DEF_DISABLED
#endif

#ifndef MB_CFG_MASTER_BUILTIN_CMDLET_RWMULTIPLEREGS
#define MB_CFG_MASTER_BUILTIN_CMDLET_RWMULTIPLEREGS          DEF_DISABLED
#endif


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************
*/


#endif