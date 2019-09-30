/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              SLAVE MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBSLAVE_CMDLET_COMMON.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBSLAVE_CMDLET_COMMON_H__
#define MBSLAVE_CMDLET_COMMON_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

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
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

/*  Command implementation type.  */
typedef void (*MBSLAVE_CMDLET_FUNC)(
    CPU_INT08U                                 request_fncode,
    CPU_INT08U                                *p_request_data,
    CPU_SIZE_T                                 request_data_size,
    CPU_INT08U                                *p_response_fncode,
    CPU_INT08U                                *p_response_buffer,
    CPU_SIZE_T                                 response_buffer_size,
    CPU_SIZE_T                                *p_response_data_size,
    void                                      *p_cmdlet_ctx,
    MB_ERROR                                  *p_error
);

/*  Command-specific callback types.  */
typedef CPU_BOOLEAN (*MBSLAVE_VALIDATECOIL_CB)(
    CPU_INT16U  address, 
    void       *p_arg, 
    MB_ERROR   *p_error
);

typedef CPU_BOOLEAN (*MBSLAVE_VALIDATEDISCRETEINPUT_CB)(
    CPU_INT16U  address, 
    void       *p_arg, 
    MB_ERROR   *p_error
);

typedef CPU_BOOLEAN (*MBSLAVE_VALIDATEINPUTREGISTER_CB)(
    CPU_INT16U  address, 
    void       *p_arg, 
    MB_ERROR   *p_error
);

typedef CPU_BOOLEAN (*MBSLAVE_VALIDATEHOLDINGREGISTER_CB)(
    CPU_INT16U  address, 
    void       *p_arg, 
    MB_ERROR   *p_error
);

typedef CPU_BOOLEAN (*MBSLAVE_READCOIL_CB)(
    CPU_INT16U  address, 
    void       *p_arg, 
    MB_ERROR   *p_error
);

typedef CPU_BOOLEAN (*MBSLAVE_READDISCRETEINPUT_CB)(
    CPU_INT16U  address, 
    void       *p_arg, 
    MB_ERROR   *p_error
);

typedef CPU_INT16U  (*MBSLAVE_READINPUTREGISTER_CB)(
    CPU_INT16U  address, 
    void       *p_arg, 
    MB_ERROR   *p_error
);

typedef CPU_INT16U  (*MBSLAVE_READHOLDINGREGISTER_CB)(
    CPU_INT16U  address, 
    void       *p_arg, 
    MB_ERROR   *p_error
);

typedef void (*MBSLAVE_WRITECOIL_CB)(
    CPU_INT16U   address, 
    CPU_BOOLEAN  value,
    void        *p_arg, 
    MB_ERROR    *p_error
);

typedef void (*MBSLAVE_WRITEHOLDINGREGISTER_CB)(
    CPU_INT16U   address, 
    CPU_INT16U   value,
    void        *p_arg, 
    MB_ERROR    *p_error
);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_SLAVE_EN == DEF_ENABLED)  */

#endif