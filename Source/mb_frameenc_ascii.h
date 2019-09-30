/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_FRAMEENC_ASCII.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_FRAMEENC_ASCII_H__
#define MB_FRAMEENC_ASCII_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_cfg.h>

#include <mb_lrc.h>
#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)

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

    MBLRC_CTX     lrcCtx;

    CPU_INT08U    lf;

    CPU_INT08U    partial;

    MB_FRAME     *frame;
} MB_FRAMEENC_ASCII;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    MBFrameEncASCII_Initialize()
*
* Description : Initialize a Modbus ASCII frame encoder.
*
* Argument(s) : (1) p_encoder     Pointer to the encoder.
*               (2) p_frame       Pointer to the frame.
*               (3) lf            Line-feed character.
*               (3) p_error       Pointer to the variable that receives error code from this function:
*
*                                     MB_ERROR_NONE           No error occurred.
*                                     MB_ERROR_NULLREFERENCE  'p_encoder' or 'p_frame' is NULL.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) Caller is allowed to reinitialize a Modbus ASCII frame encoder at anytime by calling this 
*                   function.
*********************************************************************************************************
*/

void MBFrameEncASCII_Initialize(
    MB_FRAMEENC_ASCII      *p_encoder,
    MB_FRAME               *p_frame,
    CPU_CHAR                lf,
    MB_ERROR               *p_error
);


/*
*********************************************************************************************************
*                                    MBFrameEncASCII_Next()
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

CPU_INT08U MBFrameEncASCII_Next(
    MB_FRAMEENC_ASCII      *p_encoder,
    MB_ERROR               *p_error
);


/*
*********************************************************************************************************
*                                    MBFrameEncASCII_HasNext()
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

CPU_BOOLEAN MBFrameEncASCII_HasNext(
    MB_FRAMEENC_ASCII      *p_encoder,
    MB_ERROR               *p_error
);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)  */

#endif