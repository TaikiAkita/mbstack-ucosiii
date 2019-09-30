/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_COMMON.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBMASTER_CMDLET_COMMON_H__
#define MBMASTER_CMDLET_COMMON_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef void (*MBMASTER_EXCEPTION_CB)(
    CPU_INT08U     ec,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_COILVALUE_START_CB)(
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_COILVALUE_UPDATE_CB)(
    CPU_INT16U     coilAddress,
    CPU_BOOLEAN    coilValue,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_COILVALUE_END_CB)(
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_COILWRITTEN_CB)(
    CPU_INT16U     coilAddress,
    CPU_BOOLEAN    coilValue,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_COILSWRITTEN_CB)(
    CPU_INT16U     coilStartAddress,
    CPU_INT16U     coilQuantity,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_DISCRETEINPUTVALUE_START_CB)(
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_DISCRETEINPUTVALUE_UPDATE_CB)(
    CPU_INT16U     dciAddress,
    CPU_BOOLEAN    dciValue,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_DISCRETEINPUTVALUE_END_CB)(
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_HREGVALUE_START_CB)(
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_HREGVALUE_UPDATE_CB)(
    CPU_INT16U     hregAddress,
    CPU_INT16U     hregValue,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_HREGVALUE_END_CB)(
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_HREGWRITTEN_CB)(
    CPU_INT16U     hregAddress,
    CPU_INT16U     hregValue,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_HREGMASKWRITTEN_CB)(
    CPU_INT16U     hregAddress,
    CPU_INT16U     hregAndMask,
    CPU_INT16U     hregOrMask,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_HREGSWRITTEN_CB)(
    CPU_INT16U     hregStartAddress,
    CPU_INT16U     hregQuantity,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_IREGVALUE_START_CB)(
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_IREGVALUE_UPDATE_CB)(
    CPU_INT16U     hregAddress,
    CPU_INT16U     hregValue,
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_IREGVALUE_END_CB)(
    void          *p_arg,
    MB_ERROR      *p_error
);

typedef void (*MBMASTER_CMDLET_REQUESTHANDLER)(
    CPU_INT08U   slave,
    void        *p_request,
    CPU_INT08U  *p_buffer,
    CPU_SIZE_T   buffer_size,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
);

typedef void (*MBMASTER_CMDLET_RESPONSEHANDLER)(
    CPU_INT08U   slave,
    void        *p_request,
    void        *p_response,
    void        *p_responsearg,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
);

typedef struct {
    MBMASTER_CMDLET_REQUESTHANDLER    cbRequestHandler;
    MBMASTER_CMDLET_RESPONSEHANDLER   cbResponseHandler;
} MBMASTER_CMDLET;


#ifdef __cplusplus
}
#endif

#endif