/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_FRAMEENC_RTU.C
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
#define MB_FRAMEENC_RTU_SOURCE

#include <mb_frameenc_rtu.h>

#include <mb_cfg.h>
#include <mb_constants.h>
#include <mb_crc16.h>
#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*  States of the Modbus RTU frame encoder.  */
#define MB_FRAMEENCRTU_STATE_ADDR     ((CPU_INT08U)0U)
#define MB_FRAMEENCRTU_STATE_FNCODE   ((CPU_INT08U)1U)
#define MB_FRAMEENCRTU_STATE_DATA     ((CPU_INT08U)2U)
#define MB_FRAMEENCRTU_STATE_CRCLO    ((CPU_INT08U)3U)
#define MB_FRAMEENCRTU_STATE_CRCHI    ((CPU_INT08U)4U)
#define MB_FRAMEENCRTU_STATE_END      ((CPU_INT08U)5U)


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
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_encoder' parameter.  */
    if (p_encoder == (MB_FRAMEENC_RTU*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_frame' parameter.  */
    if (p_frame == (MB_FRAME*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Initialize the members of the encoder.  */
    p_encoder->state         = MB_FRAMEENCRTU_STATE_ADDR;
    p_encoder->dataReadPtr   = p_frame->data;
    p_encoder->dataReadSize  = (CPU_SIZE_T)0U;
    p_encoder->frame         = p_frame;
    p_encoder->crcHi         = (CPU_INT08U)0U;
    p_encoder->crcLo         = (CPU_INT08U)0U;

    /*  Initialize the CRC-16 checksum context.  */
    MBCRC16_Initialize(&(p_encoder->crcCtx));

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


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
) {
    MB_FRAME   *frame;
    CPU_INT16U  frameCRC;

    CPU_INT08U  txDatum;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_encoder' parameter.  */
    if (p_encoder == (MB_FRAMEENC_RTU*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT08U)0U;
    }
#endif

    /*  Load the frame.  */
    frame = p_encoder->frame;

    switch (p_encoder->state) {
        case MB_FRAMEENCRTU_STATE_ADDR:
            /*  Transmit the 'Address' field.  */
            txDatum = frame->address;

            /*  Update the CRC.  */
            MBCRC16_Update(&(p_encoder->crcCtx), txDatum);

            /*  Go to 'FNCODE' state.  */
            p_encoder->state = MB_FRAMEENCRTU_STATE_FNCODE;

            break;
        case MB_FRAMEENCRTU_STATE_FNCODE:
            /*  Transmit the 'Function Code' field.  */
            txDatum = frame->functionCode;

            /*  Update the CRC.  */
            MBCRC16_Update(&(p_encoder->crcCtx), txDatum);

            if (frame->dataLength == (CPU_SIZE_T)0U) {
                /*  Go to 'CRCLO' state directly since there is no data.  */
                p_encoder->state = MB_FRAMEENCRTU_STATE_CRCLO;
            } else {
                /*  Go to 'DATA' state.  */
                p_encoder->state = MB_FRAMEENCRTU_STATE_DATA;
            }

            break;
        case MB_FRAMEENCRTU_STATE_DATA:
            /*  Transmit the data byte.  */
            txDatum = *(p_encoder->dataReadPtr);

            if (++(p_encoder->dataReadSize) >= frame->dataLength) {
                /*  Go to 'CRCLO' state if no remaining data.  */
                p_encoder->state = MB_FRAMEENCRTU_STATE_CRCLO;
            } else {
                /*  Move the pointer to the next byte.  */
                ++(p_encoder->dataReadPtr);
            }

            /*  Update the CRC.  */
            MBCRC16_Update(&(p_encoder->crcCtx), txDatum);

            break;
        case MB_FRAMEENCRTU_STATE_CRCLO:
            /*  Calculate the final CRC and transmit the low-order byte of the CRC.  */
            frameCRC = MBCRC16_Final(&(p_encoder->crcCtx));
            MBCRC16Util_HiLo(
                frameCRC,
                &(p_encoder->crcHi),
                &(txDatum)
            );
            p_encoder->crcLo = txDatum;

            /*  Go to 'CRCHI' state.  */
            p_encoder->state = MB_FRAMEENCRTU_STATE_CRCHI;

            break;
        case MB_FRAMEENCRTU_STATE_CRCHI:
            /*  Transmit the high-order byte of the CRC.  */
            txDatum = p_encoder->crcHi;

            /*  Go to 'END' state.  */
            p_encoder->state = MB_FRAMEENCRTU_STATE_END;

            break;
        case MB_FRAMEENCRTU_STATE_END:
            /*  Error: Frame already ended.  */
            *p_error = MB_ERROR_FRAMEENC_FRAMEEND;

            return (CPU_INT08U)0U;
        default:
            /*  Error: Invalid encoder state.  */
            *p_error = MB_ERROR_FRAMEENC_INVALIDSTATE;

            return (CPU_INT08U)0U;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return txDatum;
}


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
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_encoder' parameter.  */
    if (p_encoder == (MB_FRAMEENC_RTU*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return DEF_NO;
    }
#endif

    switch (p_encoder->state) {
        case MB_FRAMEENCRTU_STATE_ADDR:
        case MB_FRAMEENCRTU_STATE_FNCODE:
        case MB_FRAMEENCRTU_STATE_DATA:
        case MB_FRAMEENCRTU_STATE_CRCLO:
        case MB_FRAMEENCRTU_STATE_CRCHI:
            /*  No error.  */
            *p_error = MB_ERROR_NONE;

            return DEF_YES;
        case MB_FRAMEENCRTU_STATE_END:
            /*  No error.  */
            *p_error = MB_ERROR_NONE;

            return DEF_NO;
        default:
            /*  Error: Invalid encoder state.  */
            *p_error = MB_ERROR_FRAMEENC_INVALIDSTATE;

            return DEF_NO;
    }
}

#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */