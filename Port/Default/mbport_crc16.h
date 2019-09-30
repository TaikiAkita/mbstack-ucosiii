/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                               PORT LAYER
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
*                                         Default Implementation
*
* File      : MBPORT_CRC16.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBPORT_CRC16_H__
#define MBPORT_CRC16_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mbport_cfg.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                 MBPort_CRC16_InitializeTable()
*
* Description : Initialize the CRC-16 table.
*
* Argument(s) : None.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBPort_CRC16_InitializeTable();


/*
*********************************************************************************************************
*                                  MBPort_CRC16_GetTableItem()
*
* Description : Get an item from the CRC-16 table.
*
* Argument(s) : offset  The offset of the item.
*
* Return(s)   : The item.
*********************************************************************************************************
*/

CPU_INT16U MBPort_CRC16_GetTableItem(CPU_INT08U offset);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */

#endif