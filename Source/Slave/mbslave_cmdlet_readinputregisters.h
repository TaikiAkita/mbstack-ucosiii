/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              SLAVE MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBSLAVE_CMDLET_READINPUTREGISTERS.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBSLAVE_CMDLET_READINPUTREGISTERS_H__
#define MBSLAVE_CMDLET_READINPUTREGISTERS_H__

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


#if (MB_CFG_SLAVE_EN == DEF_ENABLED) && (MB_CFG_SLAVE_BUILTIN_CMDLET_READINPUTREGS == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif

/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

/*  Command-specific context type.  */
typedef struct {
    MBSLAVE_READINPUTREGISTER_CB      cbReadInputReg;
    MBSLAVE_VALIDATEINPUTREGISTER_CB  cbValidateInputReg;
    void                             *cbArg;
} MBSLAVE_READINPUTREGISTERS_CTX;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    MBSlave_CmdLet_ReadInputRegisters()
*
* Description : Command implementation of "Read Input Registers (0x04)".
*
* Argument(s) : (1) request_fncode        Function code of the request.
*               (2) p_request_data        Pointer to the first element of the request data.
*               (2) request_data_size     Size(length) of the request data.
*               (2) p_response_fncode     Pointer to the variable that receives the function code of the response.
*               (2) p_response_buffer     Pointer to the first element of the response data buffer.
*               (2) response_buffer_size  Size of the response data buffer.
*               (2) p_response_data_size  Pointer to the variable that receives the size of the response data.
*               (2) p_cmdlet_ctx          Pointer to the command-specific context.
*               (4) p_error               Pointer to the variable that receives error code from this function:
*
*                                             MB_ERROR_NONE                    No error occurred.
*                                             MB_ERROR_NULLREFERENCE           One of following conditions occurred:
*
*                                                                                  (1) 'p_request_data' is NULL while 'request_data_size' is not zero.
*                                                                                  (2) 'p_response_fncode' is NULL.
*                                                                                  (3) 'p_response_buffer' is NULL while 'response_buffer_size' is not zero.
*                                                                                  (4) 'p_response_data_size' is NULL.
*                                                                                  (5) 'p_cmdlet_ctx' is NULL.
*
*                                             MB_ERROR_SLAVE_REQUESTTRUNCATED  Request data is truncated.
*                                             MB_ERROR_SLAVE_RESPONSETRUNCATED Response data buffer is too small.
*                                             MB_ERROR_SLAVE_CALLBACKFAILED    Error occurred while calling callbacks.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) The 'p_cmdlet_ctx' pointer MUST point to a MBSLAVE_READINPUTREGISTERS_CTX object.
*********************************************************************************************************
*/

void MBSlave_CmdLet_ReadInputRegisters(
    CPU_INT08U               request_fncode,
    CPU_INT08U              *p_request_data,
    CPU_SIZE_T               request_data_size,
    CPU_INT08U              *p_response_fncode,
    CPU_INT08U              *p_response_buffer,
    CPU_SIZE_T               response_buffer_size,
    CPU_SIZE_T              *p_response_data_size,
    void                    *p_cmdlet_ctx,
    MB_ERROR                *p_error
);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_SLAVE_EN == DEF_ENABLED) && (MB_CFG_SLAVE_BUILTIN_CMDLET_READINPUTREGS == DEF_ENABLED)  */

#endif