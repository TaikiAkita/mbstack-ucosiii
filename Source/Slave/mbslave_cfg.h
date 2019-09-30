/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              SLAVE MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBSLAVE_CFG.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBSLAVE_CFG_H__
#define MBSLAVE_CFG_H__


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

#ifndef MB_CFG_SLAVE_EN
#define MB_CFG_SLAVE_EN                                      DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_CMDTABLE_COMPACT_EN
#define MB_CFG_SLAVE_CMDTABLE_COMPACT_EN                     DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_READCOILS
#define MB_CFG_SLAVE_BUILTIN_CMDLET_READCOILS                DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_READDISCRETEINPUTS
#define MB_CFG_SLAVE_BUILTIN_CMDLET_READDISCRETEINPUTS       DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_READHOLDINGREGS
#define MB_CFG_SLAVE_BUILTIN_CMDLET_READHOLDINGREGS          DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_READINPUTREGS
#define MB_CFG_SLAVE_BUILTIN_CMDLET_READINPUTREGS            DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_WRITESINGLECOIL
#define MB_CFG_SLAVE_BUILTIN_CMDLET_WRITESINGLECOIL          DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_WRITESINGLEREG
#define MB_CFG_SLAVE_BUILTIN_CMDLET_WRITESINGLEREG           DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_WRITEMULTIPLECOILS
#define MB_CFG_SLAVE_BUILTIN_CMDLET_WRITEMULTIPLECOILS       DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_WRITEMULTIPLEREGS
#define MB_CFG_SLAVE_BUILTIN_CMDLET_WRITEMULTIPLEREGS        DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_MASKWRITEREG
#define MB_CFG_SLAVE_BUILTIN_CMDLET_MASKWRITEREG             DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUILTIN_CMDLET_READWRITEMULTIPLEREGS
#define MB_CFG_SLAVE_BUILTIN_CMDLET_READWRITEMULTIPLEREGS    DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN
#define MB_CFG_SLAVE_GETPREVIOUSFRAMEFLAGS_EN                DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_GETLASTFRAMEFLAGS_EN
#define MB_CFG_SLAVE_GETLASTFRAMEFLAGS_EN                    DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_GETLASTERROR_EN
#define MB_CFG_SLAVE_GETLASTERROR_EN                         DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_GETCOUNTERVALUE_EN
#define MB_CFG_SLAVE_GETCOUNTERVALUE_EN                      DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_CLEARCOUNTERVALUE_EN
#define MB_CFG_SLAVE_CLEARCOUNTERVALUE_EN                    DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_DELAYBEFOREREPLY_EN
#define MB_CFG_SLAVE_DELAYBEFOREREPLY_EN                     DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_LISTENONLY_EN
#define MB_CFG_SLAVE_LISTENONLY_EN                           DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUSMESSAGECOUNTER_EN
#define MB_CFG_SLAVE_BUSMESSAGECOUNTER_EN                    DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_BUSCOMMERRORCOUNTER_EN
#define MB_CFG_SLAVE_BUSCOMMERRORCOUNTER_EN                  DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_SLAVEMESSAGECOUNTER_EN
#define MB_CFG_SLAVE_SLAVEMESSAGECOUNTER_EN                  DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_SLAVEEXCEPTIONCOUNTER_EN
#define MB_CFG_SLAVE_SLAVEEXCEPTIONCOUNTER_EN                DEF_DISABLED
#endif

#ifndef MB_CFG_SLAVE_SLAVENORESPONSECOUNTER_EN
#define MB_CFG_SLAVE_SLAVENORESPONSECOUNTER_EN               DEF_DISABLED
#endif


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if (defined(MB_CFG_SLAVE_CMDTABLE_COMPACT_EN)) && (MB_CFG_SLAVE_CMDTABLE_COMPACT_EN == DEF_ENABLED)
#    if (defined(MB_CFG_SLAVE_CMDTABLE_COMPACT_TABLELEN))
#        if (MB_CFG_SLAVE_CMDTABLE_COMPACT_TABLELEN == 0U) || (MB_CFG_SLAVE_CMDTABLE_COMPACT_TABLELEN > 255U)
#            error "Illegal MB_CFG_SLAVE_CMDTABLE_COMPACT_TABLELEN defined in <app_cfg.h>. It should be within range (0U, 255U]."
#        endif
#    else
#        error "MB_CFG_SLAVE_CMDTABLE_COMPACT_EN is defined but MB_CFG_SLAVE_CMDTABLE_COMPACT_TABLELEN is not defined in <app_cfg.h>."
#    endif
#endif


/*
*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************
*/


#endif