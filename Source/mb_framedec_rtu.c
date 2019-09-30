/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_FRAMEDEC_RTU.C
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
#define MB_FRAMEDEC_RTU_SOURCE

#include <mb_framedec_rtu.h>

#include <mb_crc16.h>
#include <mb_cfg.h>
#include <mb_constants.h>
#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*  States of the Modbus RTU frame decoder.  */
#define MB_FRAMEDECRTU_STATE_ADDR          ((CPU_INT08U)0U)
#define MB_FRAMEDECRTU_STATE_FNCODE        ((CPU_INT08U)1U)
#define MB_FRAMEDECRTU_STATE_CRCHI         ((CPU_INT08U)2U)
#define MB_FRAMEDECRTU_STATE_CRCLO         ((CPU_INT08U)3U)
#define MB_FRAMEDECRTU_STATE_DATA          ((CPU_INT08U)4U)
#define MB_FRAMEDECRTU_STATE_END           ((CPU_INT08U)5U)


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
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_decoder' parameter.  */
    if (p_decoder == (MB_FRAMEDEC_RTU*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_buffer' parameter.  */
    if ((buffer_size != (CPU_SIZE_T)0U) && (p_buffer == (CPU_INT08U*)0)) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Initialize the members of the decoder.  */
    p_decoder->state                  = MB_FRAMEDECRTU_STATE_ADDR;
    p_decoder->address                = (CPU_INT08U)0U;
    p_decoder->functionCode           = (CPU_INT08U)0U;
    p_decoder->dataBuffer             = p_buffer;
    p_decoder->dataBufferSize         = buffer_size;
    p_decoder->dataBufferWritePtr     = p_buffer;
    p_decoder->dataBufferWrittenSize  = (CPU_SIZE_T)0U;
    p_decoder->crcLo                  = (CPU_INT08U)0U;
    p_decoder->crcHi                  = (CPU_INT08U)0U;
    p_decoder->flags                  = (MB_FRAMEFLAGS)0U;

    /*  Initialize the CRC-16 checksum context.  */
    MBCRC16_Initialize(&(p_decoder->crcContext));

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


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
) {
    CPU_INT08U  databyte;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_decoder' parameter.  */
    if (p_decoder == (MB_FRAMEDEC_RTU*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    switch (p_decoder->state) {
        case MB_FRAMEDECRTU_STATE_ADDR:
            /*
             *  Got one byte now, push the byte to 'Address' field, see following diagram:
             * 
             *                                                     +---------------------------+
             *                                                     |          CRC (2B)         |
             *  +--------------+--------------------+--------------+-------------+-------------+
             *  | Address (1B) | Function Code (1B) | ... Data ... | CRC Lo (1B) | CRC Hi (1B) |
             *  +-------^------+--------------------+--------------+-------------+-------------+
             *          |
             *          +-- Received byte (1B).
             */

            /*  Update the CRC.  */
            MBCRC16_Update(&(p_decoder->crcContext), datum);

            /*  Save the received byte to 'Address' field.  */
            p_decoder->address = datum;

            /*  Go to read the 'Function Code' field.  */
            p_decoder->state = MB_FRAMEDECRTU_STATE_FNCODE;

            break;
        case MB_FRAMEDECRTU_STATE_FNCODE:
            /*
             *  Got one byte now, push the byte to 'Function Code' field, see following 
             *  diagram:
             * 
             *                                                     +---------------------------+
             *                                                     |          CRC (2B)         |
             *  +--------------+--------------------+--------------+-------------+-------------+
             *  | Address (1B) | Function Code (1B) | ... Data ... | CRC Lo (1B) | CRC Hi (1B) |
             *  +--------------+---------^----------+--------------+-------------+-------------+
             *                           |
             *                           +-- Received byte (1B).
             */

            /*  Update the CRC.  */
            MBCRC16_Update(&(p_decoder->crcContext), datum);

            /*  Save the received byte to 'Function Code' field.  */
            p_decoder->functionCode = datum;

            /*  Go to read the 'CRC Hi' field.  */
            p_decoder->state = MB_FRAMEDECRTU_STATE_CRCHI;

            break;
        case MB_FRAMEDECRTU_STATE_CRCHI:
            /*
             *  Got one byte now, push the byte to 'CRC Hi' field, see following diagram:
             * 
             *                                                     +---------------------------+
             *                                                     |          CRC (2B)         |
             *  +--------------+--------------------+--------------+-------------+-------------+
             *  | Address (1B) | Function Code (1B) | ... Data ... | CRC Lo (1B) | CRC Hi (1B) |
             *  +--------------+--------------------+--------------+-------------+------^------+
             *                                                                          |
             *                                                     Received byte (1B) --+
             */

            /*  Save the received byte to 'CRC Hi' field.  */
            p_decoder->crcHi = datum;

            /*  Go to read the 'CRC Lo' field.  */
            p_decoder->state = MB_FRAMEDECRTU_STATE_CRCLO;

            break;
        case MB_FRAMEDECRTU_STATE_CRCLO:
            /*
             *  Got one byte now, do step (1) and (2), see following diagram:
             * 
             *                                                     +---------------------------+
             *                                                     |          CRC (2B)         |
             *  +--------------+--------------------+--------------+-------------+-------------+
             *  | Address (1B) | Function Code (1B) | ... Data ... | CRC Lo (1B) | CRC Hi (1B) |
             *  +--------------+--------------------+--------------+---------^---+---+-----^---+
             *                                                               |       |     |
             *                                                               +-------+     |
             *                                                                  (1)        |
             *                                                                             |
             *                                            Received byte (1B) --------------+
             *                                                                  (2)
             * 
             *  Step (1): Move the byte in 'CRC Hi' field to 'CRC Lo' field.
             *       (2): Write received byte to 'CRC Hi' field.
             */

            /*  Do step (1).  */
            p_decoder->crcLo = p_decoder->crcHi;

            /*  Do step (2).  */
            p_decoder->crcHi = datum;

            /*  Go to read 'Data' field.  */
            p_decoder->state = MB_FRAMEDECRTU_STATE_DATA;

            break;
        case MB_FRAMEDECRTU_STATE_DATA:
            /*
             *  Got one byte now, do step (1), (2) and (3), see following diagram:
             * 
             *                                                     +---------------------------+
             *                                                     |          CRC (2B)         |
             *  +--------------+--------------------+--------------+-------------+-------------+
             *  | Address (1B) | Function Code (1B) | ... Data ... | CRC Lo (1B) | CRC Hi (1B) |
             *  +--------------+--------------------+-------^------+---+-----^---+---+-----^---+
             *                                              |          |     |       |     |
             *                                              +----------+     +-------+     |
             *                                                   (1)            (2)        |
             *                                                                             |
             *                                            Received byte (1B) --------------+
             *                                                                  (3)
             * 
             *  Step (1): Append the byte in 'CRC Lo' field to the tail of the 'Data' field.
             *       (2): Move the byte in 'CRC Hi' field to 'CRC Lo' field.
             *       (3): Write received byte to 'CRC Hi' field.
             */

            /*  Get the byte to be written to the tail of the 'Data' field.  */
            databyte = p_decoder->crcLo;

            /*  Do step (1).  */
            if (p_decoder->dataBufferWrittenSize == p_decoder->dataBufferSize) {
                /*  Buffer is full, set the DROP and BUFFEROVERFLOW bits.  */
                p_decoder->flags |= (MB_FRAMEFLAGS)(
                    MB_FRAMEFLAGS_DROP | 
                    MB_FRAMEFLAGS_BUFFEROVERFLOW
                );
            } else {
                /*  Append to the tail of the 'Data' field.  */
                *(p_decoder->dataBufferWritePtr) = databyte;
                ++(p_decoder->dataBufferWritePtr);
                ++(p_decoder->dataBufferWrittenSize);
            }

            /*  Update the CRC.  */
            MBCRC16_Update(&(p_decoder->crcContext), databyte);

            /*  Do step (2).  */
            p_decoder->crcLo = p_decoder->crcHi;

            /*  Do step (3).  */
            p_decoder->crcHi = datum;

            break;
        case MB_FRAMEDECRTU_STATE_END:
            /*
             *  Caller tries to update an ended decoder, set the DROP and 
             *  REDUNDANTBYTE bits.
             */
            p_decoder->flags |= (MB_FRAMEFLAGS)(
                MB_FRAMEFLAGS_DROP | 
                MB_FRAMEFLAGS_REDUNDANTBYTE
            );

            break;
        default:
            /*  Error: Invalid decoder state.  */
            *p_error = MB_ERROR_FRAMEDEC_INVALIDSTATE;
            return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


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
) {
    CPU_INT16U    crcReal;
    CPU_INT08U    crcRealHi, crcRealLo;

    switch (p_decoder->state) {
        case MB_FRAMEDECRTU_STATE_ADDR:
        case MB_FRAMEDECRTU_STATE_FNCODE:
        case MB_FRAMEDECRTU_STATE_CRCHI:
        case MB_FRAMEDECRTU_STATE_CRCLO:
            /*
             *  If the decoder ends with these states, it means that the frame 
             *  is truncated and the checksum is unable to be verified.
             * 
             *  Mark DROP, TRUNCATED and CHECKSUMMISMATCH bit in these states.
             */
            p_decoder->flags |= (MB_FRAMEFLAGS)(
                MB_FRAMEFLAGS_DROP | 
                MB_FRAMEFLAGS_TRUNCATED | 
                MB_FRAMEFLAGS_CHECKSUMMISMATCH
            );

            break;
        case MB_FRAMEDECRTU_STATE_DATA:
            /*  Calculate the real CRC checksum.  */
            crcReal = MBCRC16_Final(&(p_decoder->crcContext));
            MBCRC16Util_HiLo(crcReal, &crcRealHi, &crcRealLo);

            /*  Check whether the CRC checksum matches.  */
            if (crcRealLo != p_decoder->crcLo || crcRealHi != p_decoder->crcHi) {
                /*  Mark the DROP and CHECKSUMMISMATCH bit.  */
                p_decoder->flags |= (MB_FRAMEFLAGS)(
                    MB_FRAMEFLAGS_DROP | 
                    MB_FRAMEFLAGS_CHECKSUMMISMATCH
                );
            }

            break;
        default:
            /*  Error: Invalid decoder state.  */
            *p_error = MB_ERROR_FRAMEDEC_INVALIDSTATE;
            return;
    }

    /*  Go to END state.  */
    p_decoder->state = MB_FRAMEDECRTU_STATE_END;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


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
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_decoder' parameter.  */
    if (p_decoder == (MB_FRAMEDEC_RTU*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_frame' parameter.  */
    if (p_frame == (MB_FRAME*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Check the decoder state.  */
    if (p_decoder->state != MB_FRAMEDECRTU_STATE_END) {
        *p_error = MB_ERROR_FRAMEDEC_INVALIDSTATE;
        return;
    }

    /*  Write frame information.  */
    p_frame->address = p_decoder->address;
    p_frame->functionCode = p_decoder->functionCode;
    p_frame->data = p_decoder->dataBuffer;
    p_frame->dataLength = p_decoder->dataBufferWrittenSize;

    /*  Write frame flags.  */
    if (p_frameflags != (MB_FRAMEFLAGS*)0) {
        *p_frameflags = p_decoder->flags;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}

#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */