/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_H__
#define MB_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_cfg.h>

#include <mb_core.h>
#include <mb_constants.h>
#include <mb_types.h>
#include <mb_utilities.h>

#include <mbdrv_types.h>

#include <mb_os_basetypes.h>
#include <mb_os_types.h>

#if (MB_CFG_SLAVE_EN == DEF_ENABLED)
#include <mbslave.h>
#endif

#if (MB_CFG_MASTER_EN == DEF_ENABLED)
#include <mbmaster.h>
#endif


#endif