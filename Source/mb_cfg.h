/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              SLAVE MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_CFG.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_CFG_H__
#define MB_CFG_H__


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

#ifndef MB_CFG_MASTER_EN
#define MB_CFG_MASTER_EN                                     DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_PARITYERRORCOUNTER_EN
#define MB_CFG_CORE_PARITYERRORCOUNTER_EN                    DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN
#define MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN               DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_FRAMEERRORCOUNTER_EN
#define MB_CFG_CORE_FRAMEERRORCOUNTER_EN                     DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_SETMODE_EN
#define MB_CFG_CORE_SETMODE_EN                               DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_GETMODE_EN
#define MB_CFG_CORE_GETMODE_EN                               DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_SETLINEFEED_EN
#define MB_CFG_CORE_SETLINEFEED_EN                           DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_GETLINEFEED_EN
#define MB_CFG_CORE_GETLINEFEED_EN                           DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_SETCHARTIMEPRESCALE_EN
#define MB_CFG_CORE_SETCHARTIMEPRESCALE_EN                   DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_GETCHARTIMEPRESCALE_EN
#define MB_CFG_CORE_GETCHARTIMEPRESCALE_EN                   DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_GETCOUNTERVALUE_EN
#define MB_CFG_CORE_GETCOUNTERVALUE_EN                       DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_CLEARCOUNTERVALUE_EN
#define MB_CFG_CORE_CLEARCOUNTERVALUE_EN                     DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_GETLASTTXADDRESS
#define MB_CFG_CORE_GETLASTTXADDRESS                         DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_GETLASTTXFUNCTIONCODE
#define MB_CFG_CORE_GETLASTTXFUNCTIONCODE                    DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_GETLASTTXEXCEPTIONCODE
#define MB_CFG_CORE_GETLASTTXEXCEPTIONCODE                   DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_CLEARLASTTXEXCEPTIONCODE
#define MB_CFG_CORE_CLEARLASTTXEXCEPTIONCODE                 DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_RTUMODE
#define MB_CFG_CORE_RTUMODE                                  DEF_DISABLED
#endif

#ifndef MB_CFG_CORE_ASCIIMODE
#define MB_CFG_CORE_ASCIIMODE                                DEF_DISABLED
#endif


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef MB_CFG_MAX_NBR_IF
#    error  "MB_CFG_MAX_NBR_IF must be defined in <app_cfg.h>."
#else
#    if (MB_CFG_MAX_NBR_IF  > 255U)
#        error  "Illegal MB_CFG_MAX_NBR_IF defined in <app_cfg.h>. It must not be greater than 255U."
#    endif
#    if (MB_CFG_MAX_NBR_IF == 0U)
#        error  "Illegal MB_CFG_MAX_NBR_IF defined in <app_cfg.h>. It must be greater than 0U."
#    endif
#endif

#if (MB_CFG_CORE_RTUMODE != DEF_ENABLED) && (MB_CFG_CORE_ASCIIMODE != DEF_ENABLED)
#    error  "No available transmission mode defined in <app_cfg.h>."
#endif


/*
*********************************************************************************************************
*                                           DEFINES
*********************************************************************************************************
*/


#endif