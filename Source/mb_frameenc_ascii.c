/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_FRAMEENC_ASCII.C
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
#define MB_FRAMEENC_ASCII_SOURCE

#include <mb_frameenc_ascii.h>

#include <mb_cfg.h>
#include <mb_constants.h>
#include <mb_lrc.h>
#include <mb_types.h>

#include <cpu.h>

#include <lib_ascii.h>
#include <lib_def.h>


#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*  States of the Modbus ASCII frame encoder.  */
#define MB_FRAMEENCASCII_STATE_START         ((CPU_INT08U)0U)
#define MB_FRAMEENCASCII_STATE_ADDRHI        ((CPU_INT08U)1U)
#define MB_FRAMEENCASCII_STATE_ADDRLO        ((CPU_INT08U)2U)
#define MB_FRAMEENCASCII_STATE_FNCODEHI      ((CPU_INT08U)3U)
#define MB_FRAMEENCASCII_STATE_FNCODELO      ((CPU_INT08U)4U)
#define MB_FRAMEENCASCII_STATE_DATAHI        ((CPU_INT08U)5U)
#define MB_FRAMEENCASCII_STATE_DATALO        ((CPU_INT08U)6U)
#define MB_FRAMEENCASCII_STATE_LRCHI         ((CPU_INT08U)7U)
#define MB_FRAMEENCASCII_STATE_LRCLO         ((CPU_INT08U)8U)
#define MB_FRAMEENCASCII_STATE_CR            ((CPU_INT08U)9U)
#define MB_FRAMEENCASCII_STATE_LF           ((CPU_INT08U)10U)
#define MB_FRAMEENCASCII_STATE_END          ((CPU_INT08U)11U)


/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void MBFrameEncASCII_EncodeByte(
    CPU_INT08U   byteVal, 
    CPU_INT08U  *p_hi, 
    CPU_INT08U  *p_lo
);


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
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_encoder' parameter.  */
    if (p_encoder == (MB_FRAMEENC_ASCII*)0) {
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
    p_encoder->state         = MB_FRAMEENCASCII_STATE_START;
    p_encoder->dataReadPtr   = p_frame->data;
    p_encoder->dataReadSize  = (CPU_SIZE_T)0U;
    p_encoder->lf            = (CPU_INT08U)lf;
    p_encoder->partial       = (CPU_INT08U)0U;
    p_encoder->frame         = p_frame;

    /*  Initialize the LRC checksum context.  */
    MBLRC_Initialize(&(p_encoder->lrcCtx));

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


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
) {
    MB_FRAME   *frame;
    CPU_INT08U  frameLRC;

    CPU_INT08U  txByte;
    CPU_INT08U  txDatum;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_encoder' parameter.  */
    if (p_encoder == (MB_FRAMEENC_ASCII*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT08U)0U;
    }
#endif

    /*  Load the frame.  */
    frame = p_encoder->frame;

    switch (p_encoder->state) {
        case MB_FRAMEENCASCII_STATE_START:
            /*  Transmit the frame start character.  */
            txDatum = (CPU_INT08U)ASCII_CHAR_COLON;

            /*  Go to 'ADDRHI' state.  */
            p_encoder->state = MB_FRAMEENCASCII_STATE_ADDRHI;

            break;
        case MB_FRAMEENCASCII_STATE_ADDRHI:
            /*  Transmit the high-order part of the 'Address' field.  */
            txByte = frame->address;
            MBFrameEncASCII_EncodeByte(
                txByte,
                &(txDatum),
                &(p_encoder->partial)
            );

            /*  Update the LRC.  */
            MBLRC_Update(&(p_encoder->lrcCtx), txByte);

            /*  Go to 'ADDRLO' state.  */
            p_encoder->state = MB_FRAMEENCASCII_STATE_ADDRLO;

            break;
        case MB_FRAMEENCASCII_STATE_ADDRLO:
            /*  Transmit the low-order part of the 'Address' field.  */
            txDatum = p_encoder->partial;

            /*  Go to 'FNCODEHI' state.  */
            p_encoder->state = MB_FRAMEENCASCII_STATE_FNCODEHI;

            break;
        case MB_FRAMEENCASCII_STATE_FNCODEHI:
            /*  Transmit the high-order part of the 'FNCODE' field.  */
            txByte = frame->functionCode;
            MBFrameEncASCII_EncodeByte(
                frame->functionCode,
                &(txDatum),
                &(p_encoder->partial)
            );

            /*  Update the LRC.  */
            MBLRC_Update(&(p_encoder->lrcCtx), txByte);

            /*  Go to 'FNCODELO' state.  */
            p_encoder->state = MB_FRAMEENCASCII_STATE_FNCODELO;

            break;
        case MB_FRAMEENCASCII_STATE_FNCODELO:
            /*  Transmit the low-order part of the 'FNCODE' field.  */
            txDatum = p_encoder->partial;

            if (frame->dataLength == (CPU_SIZE_T)0U) {
                /*  Go to 'LRCHI' state directly since there is no data.  */
                p_encoder->state = MB_FRAMEENCASCII_STATE_LRCHI;
            } else {
                /*  Go to 'FNCODEHI' state.  */
                p_encoder->state = MB_FRAMEENCASCII_STATE_DATAHI;
            }

            break;
        case MB_FRAMEENCASCII_STATE_DATAHI:
            /*  Transmit the high-order part of current data byte.  */
            txByte = *(p_encoder->dataReadPtr);
            MBFrameEncASCII_EncodeByte(
                txByte,
                &(txDatum),
                &(p_encoder->partial)
            );

            /*  Update the LRC.  */
            MBLRC_Update(&(p_encoder->lrcCtx), txByte);

            /*  Go to 'DATALO' state.  */
            p_encoder->state = MB_FRAMEENCASCII_STATE_DATALO;

            break;
        case MB_FRAMEENCASCII_STATE_DATALO:
            /*  Transmit the low-order part of current data byte.  */
            txDatum = p_encoder->partial;

            if (++(p_encoder->dataReadSize) >= frame->dataLength) {
                /*  Go to 'LRCHI' state if no remaining data.  */
                p_encoder->state = MB_FRAMEENCASCII_STATE_LRCHI;
            } else {
                /*  Move the pointer to the next byte.  */
                ++(p_encoder->dataReadPtr);

                /*  Go to 'DATAHI' state.  */
                p_encoder->state = MB_FRAMEENCASCII_STATE_DATAHI;
            }

            break;
        case MB_FRAMEENCASCII_STATE_LRCHI:
            /*  Calculate the frame LRC.  */
            frameLRC = MBLRC_Final(&(p_encoder->lrcCtx));

            /*  Transmit the high-order part of the 'LRC' field.  */
            MBLRCUtil_HiLo(
                frameLRC,
                &(txDatum),
                &(p_encoder->partial)
            );

            /*  Go to 'LRCLO' state.  */
            p_encoder->state = MB_FRAMEENCASCII_STATE_LRCLO;

            break;
        case MB_FRAMEENCASCII_STATE_LRCLO:
            /*  Transmit the low-order part of the 'LRC' field.  */
            txDatum = p_encoder->partial;

            /*  Go to 'CR' state.  */
            p_encoder->state = MB_FRAMEENCASCII_STATE_CR;

            break;
        case MB_FRAMEENCASCII_STATE_CR:
            /*  Transmit a carriage-return character.  */
            txDatum = (CPU_INT08U)ASCII_CHAR_CARRIAGE_RETURN;

            /*  Go to 'LF' state.  */
            p_encoder->state = MB_FRAMEENCASCII_STATE_LF;

            break;
        case MB_FRAMEENCASCII_STATE_LF:
            /*  Transmit a line-feed character.  */
            txDatum = (CPU_INT08U)(p_encoder->lf);

            /*  Go to 'END' state.  */
            p_encoder->state = MB_FRAMEENCASCII_STATE_END;

            break;
        case MB_FRAMEENCASCII_STATE_END:
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
) {

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_encoder' parameter.  */
    if (p_encoder == (MB_FRAMEENC_ASCII*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return DEF_NO;
    }
#endif

    switch (p_encoder->state) {
        case MB_FRAMEENCASCII_STATE_START:
        case MB_FRAMEENCASCII_STATE_ADDRHI:
        case MB_FRAMEENCASCII_STATE_ADDRLO:
        case MB_FRAMEENCASCII_STATE_FNCODEHI:
        case MB_FRAMEENCASCII_STATE_FNCODELO:
        case MB_FRAMEENCASCII_STATE_DATAHI:
        case MB_FRAMEENCASCII_STATE_DATALO:
        case MB_FRAMEENCASCII_STATE_LRCHI:
        case MB_FRAMEENCASCII_STATE_LRCLO:
        case MB_FRAMEENCASCII_STATE_CR:
        case MB_FRAMEENCASCII_STATE_LF:
            /*  No error.  */
            *p_error = MB_ERROR_NONE;

            return DEF_YES;
        case MB_FRAMEENCASCII_STATE_END:
            /*  No error.  */
            *p_error = MB_ERROR_NONE;

            return DEF_NO;
        default:
            /*  Error: Invalid encoder state.  */
            *p_error = MB_ERROR_FRAMEENC_INVALIDSTATE;

            return DEF_NO;
    }
}


/*
*********************************************************************************************************
*                                    MBFrameEncASCII_EncodeByte()
*
* Description : Get whether there is remaining byte to be transmitted to the serial line.
*
* Argument(s) : (1) byteVal   The byte to be encoded.
*               (2) p_hi      Pointer to the variable that receives the high-order encoded character.
*               (3) p_lo      Pointer to the variable that receives the low-order encoded character.
*
* Return(s)   : None.
*
* Note(s)     : (1) 'p_hi' and 'p_lo' parameters are assumed to be NOT NULL.
*********************************************************************************************************
*/

static void MBFrameEncASCII_EncodeByte(
    CPU_INT08U   byteVal, 
    CPU_INT08U  *p_hi, 
    CPU_INT08U  *p_lo
) {
    CPU_INT08U  tmp;

    /*  Encode higher 4-bits.  */
    tmp = (CPU_INT08U)(byteVal >> 4);
    if (tmp <= (CPU_INT08U)9U) {
        *p_hi = (CPU_INT08U)(tmp + (CPU_INT08U)ASCII_CHAR_DIGIT_ZERO);
    } else {
        tmp -= (CPU_INT08U)10U;
        *p_hi = (CPU_INT08U)(tmp + (CPU_INT08U)ASCII_CHAR_LATIN_UPPER_A);
    }

    /*  Encode lower 4-bits.  */
    tmp = (CPU_INT08U)(byteVal & (CPU_INT08U)0x0FU);
    if (tmp <= (CPU_INT08U)9U) {
        *p_lo = (CPU_INT08U)(tmp + (CPU_INT08U)ASCII_CHAR_DIGIT_ZERO);
    } else {
        tmp -= (CPU_INT08U)10U;
        *p_lo = (CPU_INT08U)(tmp + (CPU_INT08U)ASCII_CHAR_LATIN_UPPER_A);
    }
}

#endif  /*  #if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)  */