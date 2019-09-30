/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              SLAVE MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBSLAVE_CMDTABLE.C
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define MB_SOURCE
#define MBSLAVE_SOURCE
#define MBSLAVE_CMDTABLE_SOURCE

#include <mbslave_cmdlet_common.h>
#include <mbslave_cmdtable.h>
#include <mbslave_cfg.h>

#include <mb_constants.h>
#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_SLAVE_EN == DEF_ENABLED)

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
) {
    CPU_SIZE_T              i;
    MBSLAVE_CMDTABLE_ITEM  *p;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_table' parameter.  */
    if (p_table == (MBSLAVE_CMDTABLE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Clear the table.  */
    for (i = (CPU_SIZE_T)0U; i < MBSLAVE_CMDTABLE_TABLESIZE; ++i) {
        p                   = &(p_table->table[i]);
        p->initialized      = DEF_NO;
        p->functionCode     = (CPU_INT08U)0U;
        p->cmdlet           = (MBSLAVE_CMDLET_FUNC)0;
        p->cmdletCtx        = (void*)0;
        p->noBroadcast      = DEF_YES;
        p->noListenOnlyMode = DEF_YES;
    }
    p_table->tableItemCnt   = (CPU_SIZE_T)0U;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


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
) {
    MBSLAVE_CMDTABLE_ITEM     *item;

#if (MB_CFG_SLAVE_CMDTABLE_COMPACT_EN == DEF_ENABLED)
    CPU_SIZE_T            i, j, k;
#endif

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_table' parameter.  */
    if (p_table == (MBSLAVE_CMDTABLE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Check whether the function code is valid.  */
    if (
        fncode == ((CPU_INT08U)0U) || 
        fncode  > ((CPU_INT08U)MB_VALID_FUNCTION_CODES)
    ) {
        *p_error = MB_ERROR_SLAVE_FUNCTIONCODEINVALID;
        return;
    }

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Check whether the table is full.  */
    if (p_table->tableItemCnt == MBSLAVE_CMDTABLE_TABLESIZE) {
        *p_error = MB_ERROR_SLAVE_NOFREETABLEITEM;
        goto MBSLAVE_CMDTABLE_ADD_EXIT;
    }

#if (MB_CFG_SLAVE_CMDTABLE_COMPACT_EN == DEF_ENABLED)
    /*  Search for the correct place to insert the table item.  */
    for (i = (CPU_SIZE_T)0U; i < MBSLAVE_CMDTABLE_TABLESIZE; ++i) {
        item = &(p_table->table[i]);

        /*  If the item is not initialized, break immediately.  */
        if (!(item->initialized)) {
            break;
        }

        if (item->functionCode == fncode) {
            /*  Error: Function code existed.  */
            *p_error = MB_ERROR_SLAVE_FUNCTIONCODEEXISTED;
            goto MBSLAVE_CMDTABLE_ADD_EXIT;
        } else if (item->functionCode > fncode) {
            /*
             *  We want to insert the table item to here, so we have to move 
             *  all subsequent items to leave current item empty.
             */
            j = p_table->tableItemCnt;
            while (j > i) {
                k = (CPU_SIZE_T)(j - (CPU_SIZE_T)1U);
                p_table->table[j] = p_table->table[k];
                j = k;
            }

            /*  Stop at current item.  */
            break;
        }
    }
#else
    /*  Get the item directly.  */
    item = &(p_table->table[(CPU_SIZE_T)(fncode - (CPU_INT08U)1U)]);

    /*  Check whether the function code is already existed.  */
    if (item->initialized) {
        *p_error = MB_ERROR_SLAVE_FUNCTIONCODEEXISTED;
        goto MBSLAVE_CMDTABLE_ADD_EXIT;
    }
#endif

    /*  Save the table item.  */
    item->functionCode = fncode;
    item->cmdlet = cmdlet;
    item->cmdletCtx = p_cmdlet_ctx;
    item->noBroadcast = no_broadcast;
    item->noListenOnlyMode = no_listenonly;

    /*  Mark the table item as initialized.  */
    item->initialized = DEF_YES;

    /*  Increase the table item count.  */
    ++(p_table->tableItemCnt);

MBSLAVE_CMDTABLE_ADD_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
}


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
) {
    MBSLAVE_CMDTABLE_ITEM     *item;

    CPU_BOOLEAN                found;

#if (MB_CFG_SLAVE_CMDTABLE_COMPACT_EN == DEF_ENABLED)
    CPU_SIZE_T                 mid;
    CPU_SIZE_T                 left;
    CPU_SIZE_T                 right;
#else
#endif

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_table' parameter.  */
    if (p_table == (MBSLAVE_CMDTABLE*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return DEF_NO;
    }

    /*  Check 'p_cmdlet' parameter.  */
    if (p_cmdlet == (MBSLAVE_CMDLET_FUNC*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return DEF_NO;
    }

    /*  Check 'pp_cmdlet_ctx' parameter.  */
    if (pp_cmdlet_ctx == (void**)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return DEF_NO;
    }

    /*  Check 'p_nobroadcast' parameter.  */
    if (p_nobroadcast == (CPU_BOOLEAN*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return DEF_NO;
    }

    /*  Check 'p_nolistenonly' parameter.  */
    if (p_nolistenonly == (CPU_BOOLEAN*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return DEF_NO;
    }
#endif
    /*  Check whether the function code is valid.  */
    if (
        fncode == ((CPU_INT08U)0U) || 
        fncode  > ((CPU_INT08U)MB_VALID_FUNCTION_CODES)
    ) {
        *p_error = MB_ERROR_SLAVE_FUNCTIONCODEINVALID;
        return DEF_NO;
    }

    /*  Not found by default.  */
    found = DEF_NO;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

#if (MB_CFG_SLAVE_CMDTABLE_COMPACT_EN == DEF_ENABLED)
    /*  Search the function code.  */
    left = (CPU_SIZE_T)0U;
    right = p_table->tableItemCnt;
    while (left < right) {
        mid   = (((CPU_INT32U)(left + right)) >> (CPU_INT32U)1U);
        item  = &(p_table->table[mid]);
        if (item->functionCode == fncode) {
            goto MBSLAVE_CMDTABLE_LOOKUP_FOUND;
        } else if (item->functionCode < fncode) {
            left = (CPU_INT32U)(mid + (CPU_INT32U)1U);
        } else {
            right = mid;
        }
    }

    goto MBSLAVE_CMDTABLE_LOOKUP_EXIT;

MBSLAVE_CMDTABLE_LOOKUP_FOUND:
#else
    /*  Get the table item directly.  */
    item = &(p_table->table[(CPU_SIZE_T)(fncode - (CPU_INT08U)1U)]);

    /*  Check whether the table item is initialized.  */
    if (!(item->initialized)) {
        goto MBSLAVE_CMDTABLE_LOOKUP_EXIT;
    }
#endif

    /*  Mark that the item was found.  */
    found = DEF_YES;

    /*  Pass command handler and other parameters to upper application.  */
    *p_cmdlet = item->cmdlet;
    *pp_cmdlet_ctx = item->cmdletCtx;
    *p_nobroadcast = item->noBroadcast;
    *p_nolistenonly = item->noListenOnlyMode;

MBSLAVE_CMDTABLE_LOOKUP_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    return found;
}

#endif  /*  #if (MB_CFG_SLAVE_EN == DEF_ENABLED)  */
