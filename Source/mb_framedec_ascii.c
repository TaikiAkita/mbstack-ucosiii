/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_FRAMEDEC_ASCII.C
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
#define MB_FRAMEDEC_ASCII_SOURCE

#include <mb_framedec_ascii.h>

#include <mb_lrc.h>
#include <mb_cfg.h>
#include <mb_constants.h>
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

/*  States of the Modbus ASCII frame decoder.  */
#define MB_FRAMEDECASCII_STATE_ADDR_HI          ((CPU_INT08U)0U)
#define MB_FRAMEDECASCII_STATE_ADDR_LO          ((CPU_INT08U)1U)
#define MB_FRAMEDECASCII_STATE_FNCODE_HI        ((CPU_INT08U)2U)
#define MB_FRAMEDECASCII_STATE_FNCODE_LO        ((CPU_INT08U)3U)
#define MB_FRAMEDECASCII_STATE_LRC_HI           ((CPU_INT08U)4U)
#define MB_FRAMEDECASCII_STATE_LRC_LO           ((CPU_INT08U)5U)
#define MB_FRAMEDECASCII_STATE_DATA_HI          ((CPU_INT08U)6U)
#define MB_FRAMEDECASCII_STATE_DATA_LO          ((CPU_INT08U)7U)
#define MB_FRAMEDECASCII_STATE_END              ((CPU_INT08U)8U)


/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static CPU_INT08U  MBFrameDecASCII_DecodeCharacter(
    CPU_INT08U   ch,
    MB_ERROR    *p_error
);
static CPU_INT08U  MBFrameDecASCII_DecodeByte(
    CPU_INT08U   hi,
    CPU_INT08U   lo,
    MB_ERROR    *p_error
);


/*
*********************************************************************************************************
*                                    MBFrameDecASCII_Initialize()
*
* Description : Initialize a Modbus ASCII frame decoder.
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
*               (2) Caller is allowed to reinitialize a Modbus ASCII frame decoder at anytime by calling this 
*                   function.
*********************************************************************************************************
*/

void MBFrameDecASCII_Initialize(
    MB_FRAMEDEC_ASCII   *p_decoder,
    CPU_INT08U          *p_buffer,
    CPU_SIZE_T           buffer_size,
    MB_ERROR            *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_decoder' parameter.  */
    if (p_decoder == (MB_FRAMEDEC_ASCII*)0) {
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
    p_decoder->state                  = MB_FRAMEDECASCII_STATE_ADDR_HI;
    p_decoder->partial                = (CPU_INT08U)0U;
    p_decoder->address                = (CPU_INT08U)0U;
    p_decoder->functionCode           = (CPU_INT08U)0U;
    p_decoder->dataBuffer             = p_buffer;
    p_decoder->dataBufferSize         = buffer_size;
    p_decoder->dataBufferWritePtr     = p_buffer;
    p_decoder->dataBufferWrittenSize  = (CPU_SIZE_T)0U;
    p_decoder->lrc                    = (CPU_INT08U)0U;
    p_decoder->flags                  = (MB_FRAMEFLAGS)0U;

    /*  Initialize the LRC checksum context.  */
    MBLRC_Initialize(&(p_decoder->lrcContext));

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBFrameDecASCII_Update()
*
* Description : Update a Modbus ASCII frame decoder with character received from the serial port.
*
* Argument(s) : (1) p_decoder     Pointer to the decoder.
*               (2) datum         The received character.
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

void MBFrameDecASCII_Update(
    MB_FRAMEDEC_ASCII   *p_decoder,
    CPU_INT08U           datum,
    MB_ERROR            *p_error
) {
    MB_ERROR   error;

    CPU_INT08U  rdbyte;
    CPU_INT08U  databyte;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_decoder' parameter.  */
    if (p_decoder == (MB_FRAMEDEC_ASCII*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    switch (p_decoder->state) {
        case MB_FRAMEDECASCII_STATE_ADDR_HI:
            /*  Save the received character to the partial register.  */
            p_decoder->partial = datum;

            /*  Go to read the low-order character of the 'Address' field.  */
            p_decoder->state = MB_FRAMEDECASCII_STATE_ADDR_LO;

            break;
        case MB_FRAMEDECASCII_STATE_ADDR_LO:
            /*
             *  Got one byte (two characters) now, push the byte to 'Address' field, see following 
             *  diagram:
             * 
             *  +--------------+
             *  | Partial (1C) |  (This register contains the character received previously.)
             *  +--------------+
             *          |
             *          |      +--------------+--------------------+--------------+----------+
             *          |      | Address (1B) | Function Code (1B) | ... Data ... | LRC (1B) |
             *          |      +-------^------+--------------------+--------------+----------+
             *          |              |
             *          |         +----+----+
             *          |         | Hi | Lo |
             *          |         +-^--+--^-+
             *          |           |     |
             *          +-----------+     +--- Received character (1C).
             */

            /*  Convert two characters to one byte.  */
            rdbyte = MBFrameDecASCII_DecodeByte(
                p_decoder->partial,
                datum,
                &error
            );
            if (error != MB_ERROR_NONE) {
                /*
                 *  Received character(s) contain(s) error(s), set the
                 *  DROP and INVALIDBYTE bits.
                 */
                p_decoder->flags |= (MB_FRAMEFLAGS)(
                    MB_FRAMEFLAGS_DROP | 
                    MB_FRAMEFLAGS_INVALIDBYTE
                );

                /*  Also set the byte value to 0x00.  */
                rdbyte = (CPU_INT08U)0U;
            }

            /*  Update the LRC checksum.  */
            MBLRC_Update(&(p_decoder->lrcContext), rdbyte);

            /*  Save the byte to the 'Address' field.  */
            p_decoder->address = rdbyte;

            /*  Go to read the high-order character of the 'Function Code' field.  */
            p_decoder->state = MB_FRAMEDECASCII_STATE_FNCODE_HI;

            break;
        case MB_FRAMEDECASCII_STATE_FNCODE_HI:
            /*  Save the received character to the partial register.  */
            p_decoder->partial = datum;

            /*  Go to read the low-order character of the 'Function Code' field.  */
            p_decoder->state = MB_FRAMEDECASCII_STATE_FNCODE_LO;

            break;
        case MB_FRAMEDECASCII_STATE_FNCODE_LO:
            /*
             *  Got one byte (two characters) now, push the byte to 'Function Code' field, see 
             *  following diagram:
             * 
             *  +--------------+
             *  | Partial (1C) |  (This register contains the character received previously.)
             *  +--------------+
             *          |
             *          |      +--------------+--------------------+--------------+----------+
             *          |      | Address (1B) | Function Code (1B) | ... Data ... | LRC (1B) |
             *          |      +--------------+---------^----------+--------------+----------+
             *          |                               |
             *          |                          +----+----+
             *          |                          | Hi | Lo |
             *          |                          +-^--+--^-+
             *          |                            |     |
             *          +----------------------------+     +--- Received character (1C).
             */

            /*  Convert two characters to one byte.  */
            rdbyte = MBFrameDecASCII_DecodeByte(
                p_decoder->partial,
                datum,
                &error
            );
            if (error != MB_ERROR_NONE) {
                /*
                 *  Received character(s) contain(s) error(s), set the
                 *  DROP and INVALIDBYTE bits.
                 */
                p_decoder->flags |= (MB_FRAMEFLAGS)(
                    MB_FRAMEFLAGS_DROP | 
                    MB_FRAMEFLAGS_INVALIDBYTE
                );

                /*  Also set the byte value to 0x00.  */
                rdbyte = (CPU_INT08U)0U;
            }

            /*  Update the LRC checksum.  */
            MBLRC_Update(&(p_decoder->lrcContext), rdbyte);

            /*  Save the byte to the 'Function Code' field.  */
            p_decoder->functionCode = rdbyte;

            /*  Go to read the high-order character of the 'LRC' field.  */
            p_decoder->state = MB_FRAMEDECASCII_STATE_LRC_HI;

            break;
        case MB_FRAMEDECASCII_STATE_LRC_HI:
            /*  Save the received character to the partial register.  */
            p_decoder->partial = datum;

            /*  Go to read the low-order character of the 'LRC' field.  */
            p_decoder->state = MB_FRAMEDECASCII_STATE_LRC_LO;

            break;
        case MB_FRAMEDECASCII_STATE_LRC_LO:
            /*
             *  Got one byte (two characters) now, push the byte to 'LRC' field, see following diagram:
             * 
             *  +--------------+
             *  | Partial (1C) |  (This register contains the character received previously.)
             *  +--------------+
             *          |
             *          |      +--------------+--------------------+--------------+----------+
             *          |      | Address (1B) | Function Code (1B) | ... Data ... | LRC (1B) |
             *          |      +--------------+--------------------+--------------+----^-----+
             *          |                                                              |
             *          |                                                         +----+----+
             *          |                                                         | Hi | Lo |
             *          |                                                         +-^--+--^-+
             *          |                                                           |     |
             *          +-----------------------------------------------------------+     +--- Received character (1C).
             */

            /*  Convert two characters to one byte.  */
            rdbyte = MBFrameDecASCII_DecodeByte(
                p_decoder->partial,
                datum,
                &error
            );
            if (error != MB_ERROR_NONE) {
                /*
                 *  Received character(s) contain(s) error(s), set the
                 *  DROP and INVALIDBYTE bits.
                 */
                p_decoder->flags |= (MB_FRAMEFLAGS)(
                    MB_FRAMEFLAGS_DROP | 
                    MB_FRAMEFLAGS_INVALIDBYTE
                );

                /*  Also set the byte value to 0x00.  */
                rdbyte = (CPU_INT08U)0U;
            }

            /*  Save the byte to the 'LRC' field.  */
            p_decoder->lrc = rdbyte;

            /*  Go to read the high-order character of the 'LRC' field.  */
            p_decoder->state = MB_FRAMEDECASCII_STATE_DATA_HI;

            break;
        case MB_FRAMEDECASCII_STATE_DATA_HI:
            /*  Save the received character to the partial register.  */
            p_decoder->partial = datum;

            /*  Go to read the low-order character of the 'LRC' field.  */
            p_decoder->state = MB_FRAMEDECASCII_STATE_DATA_LO;

            break;
        case MB_FRAMEDECASCII_STATE_DATA_LO:
            /*
             *  Got one byte (two characters) now, do step (1) and (2), see following diagram:
             * 
             *  +--------------+
             *  | Partial (1C) |  (This register contains the character received previously.)
             *  +--------------+
             *          |
             *          |      +--------------+--------------------+--------------+----------+   (2)
             *          |      | Address (1B) | Function Code (1B) | ... Data ... | LRC (1B) |<-------+
             *          |      +--------------+--------------------+-------^------+----+-----+        |
             *          |                                                  |           |              |
             *          |                                                  +-----------+         +----+----+
             *          |                                                       (1)              | Hi | Lo |
             *          |                                                                        +-^--+--^-+
             *          |                                                                          |     |
             *          +--------------------------------------------------------------------------+     |
             *                                                                                           |
             *                                                                 Received character (1C) --+
             * 
             *  Step (1): Append the byte stores in 'LRC' field to the tail of the data field.
             *       (2): Save received byte (2 characters) to the 'LRC' field.
             */

            /*  Convert two characters to one byte.  */
            rdbyte = MBFrameDecASCII_DecodeByte(
                p_decoder->partial,
                datum,
                &error
            );
            if (error != MB_ERROR_NONE) {
                /*
                 *  Received character(s) contain(s) error(s), set the
                 *  DROP and INVALIDBYTE bits.
                 */
                p_decoder->flags |= (MB_FRAMEFLAGS)(
                    MB_FRAMEFLAGS_DROP | 
                    MB_FRAMEFLAGS_INVALIDBYTE
                );

                /*  Also set the byte value to 0x00.  */
                rdbyte = (CPU_INT08U)0U;
            }

            /*  Get the byte that would be appended to 'Data' field.  */
            databyte = p_decoder->lrc;

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

            /*  Update the LRC.  */
            MBLRC_Update(&(p_decoder->lrcContext), databyte);

            /*  Save the received byte to the 'LRC' field.  */
            p_decoder->lrc = rdbyte;

            /*  Go to read the next character.  */
            p_decoder->state = MB_FRAMEDECASCII_STATE_DATA_HI;

            break;
        case MB_FRAMEDECASCII_STATE_END:
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
*                                    MBFrameDecASCII_End()
*
* Description : End a Modbus ASCII frame decoder.
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
*               (2) Caller is allowed to reinitialize the frame decoder by calling MBFrameDecASCII_Initialize().
*********************************************************************************************************
*/

void MBFrameDecASCII_End(
    MB_FRAMEDEC_ASCII   *p_decoder,
    MB_ERROR            *p_error
) {
    CPU_INT08U   lrcReal;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_decoder' parameter.  */
    if (p_decoder == (MB_FRAMEDEC_ASCII*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    switch (p_decoder->state) {
        case MB_FRAMEDECASCII_STATE_ADDR_HI:
        case MB_FRAMEDECASCII_STATE_ADDR_LO:
        case MB_FRAMEDECASCII_STATE_FNCODE_HI:
        case MB_FRAMEDECASCII_STATE_FNCODE_LO:
        case MB_FRAMEDECASCII_STATE_LRC_HI:
        case MB_FRAMEDECASCII_STATE_LRC_LO:
        case MB_FRAMEDECASCII_STATE_DATA_LO:
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
        case MB_FRAMEDECASCII_STATE_DATA_HI:
            /*  Calculate the real LRC checksum.  */
            lrcReal = MBLRC_Final(&(p_decoder->lrcContext));

            /*  Check whether the LRC checksum matches.  */
            if (lrcReal != p_decoder->lrc) {
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
    p_decoder->state = MB_FRAMEDECASCII_STATE_END;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBFrameDecASCII_ToFrame()
*
* Description : Get the frame decoded by a Modbus ASCII frame decoder.
*
* Argument(s) : (1) p_decoder     Pointer to the decoder.
*               (2) p_frame       Pointer to the variable that receives the frame.
*               (2) p_frameflags  Pointer to the variable that receives the frame flags (NULL if not wanted):
*
*                                     MB_FRAMEFLAGS_DROP                The frame should be dropped.
*                                     MB_FRAMEFLAGS_BUFFEROVERFLOW      The data buffer is too small to contain 
*                                                                       the frame data.
*                                     MB_FRAMEFLAGS_CHECKSUMMISMATCH    The LRC checksum mismatched.
*                                     MB_FRAMEFLAGS_TRUNCATED           The frame is truncated.
*                                     MB_FRAMEFLAGS_REDUNDANTBYTE       One or more byte(s) was/were written to 
*                                                                       the decoder after it ended.
*                                     MB_FRAMEFLAGS_INVALIDBYTE         One or more non-ASCII or invalid ASCII 
*                                                                       character(s) was/were received.
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

void MBFrameDecASCII_ToFrame(
    MB_FRAMEDEC_ASCII   *p_decoder,
    MB_FRAME            *p_frame,
    MB_FRAMEFLAGS       *p_frameflags,
    MB_ERROR            *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_decoder' parameter.  */
    if (p_decoder == (MB_FRAMEDEC_ASCII*)0) {
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
    if (p_decoder->state != MB_FRAMEDECASCII_STATE_END) {
        *p_error = MB_ERROR_FRAMEDEC_INVALIDSTATE;
        return;
    }

    /*  Write frame information.  */
    p_frame->address       = p_decoder->address;
    p_frame->functionCode  = p_decoder->functionCode;
    p_frame->data          = p_decoder->dataBuffer;
    p_frame->dataLength    = p_decoder->dataBufferWrittenSize;

    /*  Write frame flags.  */
    if (p_frameflags != (MB_FRAMEFLAGS*)0) {
        *p_frameflags     = p_decoder->flags;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBFrameDecASCII_DecodeCharacter()
*
* Description : Decode a hex character (0-9, A-F) to a digit (0-15).
*
* Argument(s) : (1) ch        The hex character.
*               (3) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                   No error occurred.
*                                 MB_ERROR_FRAMEDEC_INVALIDCHAR   'ch' contains an invalid character.
*
* Return(s)   : The decoded digit (0-15).
*********************************************************************************************************
*/

static CPU_INT08U  MBFrameDecASCII_DecodeCharacter(
    CPU_INT08U   ch,
    MB_ERROR    *p_error
) {
    CPU_INT08U  ret;

    /*  Check whether the character is a valid ASCII character.  */
    if ((ch >> 7) != (CPU_INT08U)0) {
        *p_error = MB_ERROR_FRAMEDEC_INVALIDCHAR;
        return (CPU_INT08U)0U;
    }

    if (
        /*  Check whether the character is a digit character.  */
        (ch >= (CPU_INT08U)ASCII_CHAR_DIG_ZERO) && 
        (ch <= (CPU_INT08U)ASCII_CHAR_DIG_NINE)
    ) {
        /*  Convert the digit character to digit (0-9).  */
        ret = (CPU_INT08U)(
            ch - (CPU_INT08U)ASCII_CHAR_DIG_ZERO
        );
    } else if (
        /*  Check whether the character is a hex character (A-F).  */
        (ch >= (CPU_INT08U)ASCII_CHAR_LATIN_UPPER_A) && 
        (ch <= (CPU_INT08U)ASCII_CHAR_LATIN_UPPER_F)
    ) {
        /*  Convert the hex character to digit (10-15).  */
        ret = (CPU_INT08U)(
            (ch - (CPU_INT08U)ASCII_CHAR_LATIN_UPPER_A) + (CPU_INT08U)10U
        );
    } else {
        /*  Error: Invalid character.  */
        *p_error = MB_ERROR_FRAMEDEC_INVALIDCHAR;

        return (CPU_INT08U)0U;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return ret;
}


/*
*********************************************************************************************************
*                                    MBFrameDecASCII_DecodeByte()
*
* Description : Decode two hex characters to one byte.
*
* Argument(s) : (1) hi        The high-order hex character.
*               (2) lo        The low-order hex character.
*               (3) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                   No error occurred.
*                                 MB_ERROR_FRAMEDEC_INVALIDCHAR   'hi' or 'lo' contains invalid character(s).
*
* Return(s)   : The decoded byte.
*********************************************************************************************************
*/

static CPU_INT08U  MBFrameDecASCII_DecodeByte(
    CPU_INT08U   hi,
    CPU_INT08U   lo,
    MB_ERROR    *p_error
) {
    CPU_INT08U  byteHi;
    CPU_INT08U  byteLo;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Decode the high-order character.  */
    byteHi = MBFrameDecASCII_DecodeCharacter(hi, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return (CPU_INT08U)0U;
    }
    byteHi <<= 4;

    /*  Decode the low-order character.  */
    byteLo = MBFrameDecASCII_DecodeCharacter(lo, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return (CPU_INT08U)0U;
    }

    return (CPU_INT08U)(byteHi | byteLo);
}

#endif  /*  #if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)  */