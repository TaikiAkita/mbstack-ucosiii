/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_FRAMEDEC_RTU.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_FRAMEDEC_RTU_H__
#define MB_FRAMEDEC_RTU_H__


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
    CPU_INT08U   state;

    CPU_INT08U   address;
    CPU_INT08U   functionCode;
    CPU_INT08U  *dataBuffer;
    CPU_SIZE_T   dataBufferSize;
    CPU_INT08U  *dataBufferWritePtr;
    CPU_SIZE_T   dataBufferWrittenSize;
    CPU_INT08U   crcLo;
    CPU_INT08U   crcHi;

    MB_FRAMEFLAGS   flags;

    MBCRC16_CTX  crcContext;
} MB_FRAMEDEC_RTU;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    MBFrameDecRTU_Initialize()
*
* Description : Initialize a Modbus RTU frame decoder.
*
* Argument(s) : (1) p_decoder     Pointer to the decoder.
*               (2) p_buffer      Pointer to the first element of the buffer that receives the data.
*               (3) buffer_size   Size of the buffer that receives the data.
*               (4) p_error       Pointer to the variable that receives error code from this function:
*
*                                     MB_ERROR_NONE           No error occurred.
*                                     MB_ERROR_NULLREFERENCE  'p_decoder' is NULL or 'p_buffer' is NULL but 
*                                                             'buffer_size' is not NULL.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) Caller is allowed to reinitialize a Modbus RTU frame decoder at anytime by calling this 
*                   function.
*********************************************************************************************************
*/

void MBFrameDecRTU_Initialize(
    MB_FRAMEDEC_RTU   *p_decoder,
    CPU_INT08U        *p_buffer,
    CPU_SIZE_T         buffer_size,
    MB_ERROR          *p_error
);


/*
*********************************************************************************************************
*                                    MBFrameDecRTU_Update()
*
* Description : Update a Modbus RTU frame decoder with one byte received from the serial port.
*
* Argument(s) : (1) p_decoder     Pointer to the decoder.
*               (2) datum         The received byte.
*               (3) p_error       Pointer to the variable that receives error code from this function:
*
*                                     MB_ERROR_NONE                   No error occurred.
*                                     MB_ERROR_NULLREFERENCE          'p_decoder' is NULL.
*                                     MB_ERROR_FRAMEDEC_INVALIDSTATE  The decoder is in an invalid 
*                                                                     state (Never occurs unless the 
*                                                                     decoder is not initialized).
*                                                                     
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBFrameDecRTU_Update(
    MB_FRAMEDEC_RTU   *p_decoder,
    CPU_INT08U         datum,
    MB_ERROR          *p_error
);


/*
*********************************************************************************************************
*                                    MBFrameDecRTU_End()
*
* Description : End a Modbus RTU frame decoder.
*
* Argument(s) : (1) p_decoder     Pointer to the decoder.
*               (2) p_error       Pointer to the variable that receives error code from this function:
*
*                                     MB_ERROR_NONE                   No error occurred.
*                                     MB_ERROR_NULLREFERENCE          'p_decoder' is NULL.
*                                     MB_ERROR_FRAMEDEC_INVALIDSTATE  The decoder is in an invalid 
*                                                                     state, the decoder may already 
*                                                                     ended or not initialized yet.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) Caller is allowed to reinitialize the frame decoder by calling MBFrameDecRTU_Initialize().
*********************************************************************************************************
*/

void MBFrameDecRTU_End(
    MB_FRAMEDEC_RTU   *p_decoder,
    MB_ERROR          *p_error
);


/*
*********************************************************************************************************
*                                    MBFrameDecRTU_ToFrame()
*
* Description : Get the frame decoded by a Modbus RTU frame decoder.
*
* Argument(s) : (1) p_decoder     Pointer to the decoder.
*               (2) p_frame       Pointer to the variable that receives the frame.
*               (2) p_frameflags  Pointer to the variable that receives the frame flags (NULL if not wanted):
*
*                                     MB_FRAMEFLAGS_DROP                The frame should be dropped.
*                                     MB_FRAMEFLAGS_BUFFEROVERFLOW      The data buffer is too small to contain 
*                                                                       the frame data.
*                                     MB_FRAMEFLAGS_CHECKSUMMISMATCH    The CRC checksum mismatched.
*                                     MB_FRAMEFLAGS_TRUNCATED           The frame is truncated.
*                                     MB_FRAMEFLAGS_REDUNDANTBYTE       One or more byte(s) was/were written to 
*                                                                       the decoder after it ended.
*
*               (3) p_error       Pointer to the variable that receives error code from this function:
*
*                                     MB_ERROR_NONE                   No error occurred.
*                                     MB_ERROR_NULLREFERENCE          'p_decoder' is NULL.
*                                     MB_ERROR_FRAMEDEC_INVALIDSTATE  The decoder is in an invalid 
*                                                                     state, the decoder may not  
*                                                                     ended or not initialized yet.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBFrameDecRTU_ToFrame(
    MB_FRAMEDEC_RTU   *p_decoder,
    MB_FRAME          *p_frame,
    MB_FRAMEFLAGS     *p_frameflags,
    MB_ERROR          *p_error
);


#ifdef __cplusplus
}
#endif

#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */

#endif