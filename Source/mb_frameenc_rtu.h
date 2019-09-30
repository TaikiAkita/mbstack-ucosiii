/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_FRAMEENC_RTU.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_FRAMEENC_RTU_H__
#define MB_FRAMEENC_RTU_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_cfg.h>
#include <mb_crc16.h>
#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT08U    state;

    CPU_INT08U   *dataReadPtr;
    CPU_SIZE_T    dataReadSize;

    CPU_INT08U    crcHi;
    CPU_INT08U    crcLo;
    MBCRC16_CTX   crcCtx;

    MB_FRAME     *frame;
} MB_FRAMEENC_RTU;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    MBFrameEncRTU_Initialize()
*
* Description : Initialize a Modbus RTU frame encoder.
*
* Argument(s) : (1) p_encoder     Pointer to the encoder.
*               (2) p_frame       Pointer to the frame.
*               (3) p_error       Pointer to the variable that receives error code from this function:
*
*                                     MB_ERROR_NONE           No error occurred.
*                                     MB_ERROR_NULLREFERENCE  'p_encoder' or 'p_frame' is NULL.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) Caller is allowed to reinitialize a Modbus RTU frame encoder at anytime by calling this 
*                   function.
*********************************************************************************************************
*/

void MBFrameEncRTU_Initialize(
    MB_FRAMEENC_RTU    *p_encoder,
    MB_FRAME           *p_frame,
    MB_ERROR           *p_error
);


/*
*********************************************************************************************************
*                                    MBFrameEncRTU_Next()
*
* Description : Get the next byte that should be transmitted to the serial line.
*
* Argument(s) : (1) p_encoder     Pointer to the encoder.
*               (2) p_error       Pointer to the variable that receives error code from this function:
*
*                                     MB_ERROR_NONE                   No error occurred.
*                                     MB_ERROR_NULLREFERENCE          'p_encoder' is NULL.
*                                     MB_ERROR_FRAMEENC_INVALIDSTATE  The encoder is in an invalid 
*                                                                     state (Never occurs unless the 
*                                                                     encoder is not initialized).
*                                     MB_ERROR_FRAMEENC_FRAMEEND      The frame is already ended.
*
* Return(s)   : The byte to be transmitted.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_INT08U  MBFrameEncRTU_Next(
    MB_FRAMEENC_RTU    *p_encoder,
    MB_ERROR           *p_error
);


/*
*********************************************************************************************************
*                                    MBFrameEncRTU_HasNext()
*
* Description : Get whether there is remaining byte to be transmitted to the serial line.
*
* Argument(s) : (1) p_encoder     Pointer to the encoder.
*               (2) p_error       Pointer to the variable that receives error code from this function:
*
*                                     MB_ERROR_NONE                   No error occurred.
*                                     MB_ERROR_NULLREFERENCE          'p_encoder' is NULL.
*                                     MB_ERROR_FRAMEENC_INVALIDSTATE  The encoder is in an invalid 
*                                                                     state (Never occurs unless the 
*                                                                     encoder is not initialized).
*
* Return(s)   : DEF_YES if so, DEF_NO if not.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_BOOLEAN MBFrameEncRTU_HasNext(
    MB_FRAMEENC_RTU    *p_encoder,
    MB_ERROR           *p_error
);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */

#endif