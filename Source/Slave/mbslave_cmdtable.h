/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              SLAVE MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBSLAVE_CMDTABLE.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBSLAVE_CMDTABLE_H__
#define MBSLAVE_CMDTABLE_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mbslave_cmdlet_common.h>
#include <mbslave_cfg.h>

#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_SLAVE_EN == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#if (MB_CFG_SLAVE_CMDTABLE_COMPACT_EN == DEF_ENABLED)
#define MBSLAVE_CMDTABLE_TABLESIZE           ((CPU_SIZE_T)(MB_CFG_SLAVE_CMDTABLE_COMPACT_TABLELEN))
#else
#define MBSLAVE_CMDTABLE_TABLESIZE           ((CPU_SIZE_T)(MB_VALID_FUNCTION_CODES))
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT08U              functionCode;
    MBSLAVE_CMDLET_FUNC     cmdlet;
    void                   *cmdletCtx;
    CPU_BOOLEAN             initialized:1;
    CPU_BOOLEAN             noBroadcast:1;
    CPU_BOOLEAN             noListenOnlyMode:1;
    CPU_INT08U              __padding:5;
} MBSLAVE_CMDTABLE_ITEM;

typedef struct {
    MBSLAVE_CMDTABLE_ITEM   table[MBSLAVE_CMDTABLE_TABLESIZE];
    CPU_SIZE_T              tableItemCnt;
} MBSLAVE_CMDTABLE;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    MBSlave_CmdTable_Initialize()
*
* Description : Initialize a Modbus slave command table.
*
* Argument(s) : (1) p_table       Pointer to the table.
*               (2) p_error       Pointer to the variable that receives error code from this function:
*
*                                     MB_ERROR_NONE             No error occurred.
*                                     MB_ERROR_NULLREFERENCE    'p_table' is NULL.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBSlave_CmdTable_Initialize(
    MBSLAVE_CMDTABLE     *p_table,
    MB_ERROR             *p_error
);


/*
*********************************************************************************************************
*                                    MBSlave_CmdTable_Add()
*
* Description : Add a command (function code) to the Modbus slave command table.
*
* Argument(s) : (1) p_table          Pointer to the table.
*               (2) fncode           Function code.
*               (3) cmdlet           Command implementation of the function code.
*               (4) p_cmdlet_ctx     'p_arg' parameter passed to the command implementation.
*               (5) no_broadcast     DEF_YES if the function is not allowed for broadcast requests, DEF_NO otherwise.
*               (6) no_listenonly    DEF_YES if the function is not allowed in listen-only mode.
*               (7) p_error          Pointer to the variable that receives error code from this function:
*
*                                        MB_ERROR_NONE                       No error occurred.
*                                        MB_ERROR_NULLREFERENCE              'p_table' is NULL.
*                                        MB_ERROR_SLAVE_FUNCTIONCODEINVALID  Function code is invalid.
*                                        MB_ERROR_SLAVE_FUNCTIONCODEEXISTED  Fucntion code already exists.
*                                        MB_ERROR_SLAVE_NOFREETABLEITEM      Table is full.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBSlave_CmdTable_Add(
    MBSLAVE_CMDTABLE     *p_table,
    CPU_INT08U            fncode,
    MBSLAVE_CMDLET_FUNC   cmdlet,
    void                 *p_cmdlet_ctx,
    CPU_BOOLEAN           no_broadcast,
    CPU_BOOLEAN           no_listenonly,
    MB_ERROR             *p_error
);


/*
*********************************************************************************************************
*                                    MBSlave_CmdTable_Lookup()
*
* Description : Lookup a command (function code) from the Modbus slave command table.
*
* Argument(s) : (1) p_table          Pointer to the table.
*               (2) fncode           Function code.
*               (3) p_cmdlet         Pointer to the variable that receives the command implementation of the function code.
*               (4) pp_cmdlet_ctx    Pointer to the variable that receives the 'p_arg' parameter passed to the command implementation.
*               (5) p_nobroadcast    Pointer to the variable that receives whether the function is not allowed for broadcast requests.
*               (6) p_nolistenonly   Pointer to the variable that receives whether the function is not allowed in listen-only mode.
*               (7) p_error          Pointer to the variable that receives error code from this function:
*
*                                        MB_ERROR_NONE                       No error occurred.
*                                        MB_ERROR_NULLREFERENCE              'p_table', 'p_cmdlet', 'p_nobroadcast' or 'p_nolistenonly' is NULL.
*                                        MB_ERROR_SLAVE_FUNCTIONCODEINVALID  Function code is invalid.
*
* Return(s)   : DEF_YES if found, DEF_NO if not.
*
* Note(s)     : (1) Variables pointed by 'p_cmdlet', 'p_nobroadcast' and 'p_nolistenonly' are kept untouched if not found.
*********************************************************************************************************
*/

CPU_BOOLEAN MBSlave_CmdTable_Lookup(
    MBSLAVE_CMDTABLE     *p_table,
    CPU_INT08U            fncode,
    MBSLAVE_CMDLET_FUNC  *p_cmdlet,
    void                **pp_cmdlet_ctx,
    CPU_BOOLEAN          *p_nobroadcast,
    CPU_BOOLEAN          *p_nolistenonly,
    MB_ERROR             *p_error
);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_SLAVE_EN == DEF_ENABLED)  */

#endif