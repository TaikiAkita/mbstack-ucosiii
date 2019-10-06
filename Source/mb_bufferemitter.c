/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_BUFFEREMITTER.C
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
#define MB_BUFFEREMITTER_SOURCE

#include <mb_bufferemitter.h>
#include <mb_cfg.h>
#include <mb_constants.h>
#include <mb_types.h>

#include <cpu.h>


/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void MBBufEmitter_EnsureRemaining(
    MB_BUFFEREMITTER  *p_emitter,
    CPU_SIZE_T         min_length,
    MB_ERROR          *p_error
);


/*
*********************************************************************************************************
*                                    MBBufEmitter_Initialize()
*
* Description : Initialize a buffer emitter.
*
* Argument(s) : (1) p_emitter       Pointer to the emitter.
*               (2) p_buffer        Pointer to the first element of the data buffer.
*               (3) buffer_length   Data buffer length.
*               (4) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE             No error occurred.
*                                       MB_ERROR_NULLREFERENCE    'p_emitter' is NULL or 'p_buffer' is NULL 
*                                                                 while 'buffer_length' is not zero.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBBufEmitter_Initialize(
    MB_BUFFEREMITTER    *p_emitter,
    CPU_INT08U          *p_buffer,
    CPU_SIZE_T          buffer_length,
    MB_ERROR            *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_emitter' parameter.  */
    if (p_emitter == (MB_BUFFEREMITTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_buffer' parameter.  */
    if (
        (p_buffer == (CPU_INT08U*)0) && 
        (buffer_length != (CPU_SIZE_T)0U)
    ) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Reset the emitter.  */
    p_emitter->buffer = p_buffer;
    p_emitter->bufferLength = buffer_length;
    p_emitter->cursor = (CPU_SIZE_T)0U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBBufEmitter_Reset()
*
* Description : Reset a buffer emitter.
*
* Argument(s) : (1) p_emitter       Pointer to the emitter.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE             No error occurred.
*                                       MB_ERROR_NULLREFERENCE    'p_emitter' is NULL.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) 'Reset' means to set the cursor of specific buffer emitter to zero.
*********************************************************************************************************
*/

void MBBufEmitter_Reset(
    MB_BUFFEREMITTER  *p_emitter,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_emitter' parameter.  */
    if (p_emitter == (MB_BUFFEREMITTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Reset the cursor.  */
    p_emitter->cursor = (CPU_SIZE_T)0U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt8()
*
* Description : Write an unsigned 8-bit integer to current position of specific buffer emitter.
*
* Argument(s) : (1) p_emitter       Pointer to the emitter.
*               (2) value           Value to be written.
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                   No error occurred.
*                                       MB_ERROR_NULLREFERENCE          'p_emitter' is NULL.
*                                       MB_ERROR_BUFEMITTER_BUFFEREND   The cursor is already at the ending position.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBBufEmitter_WriteUInt8(
    MB_BUFFEREMITTER  *p_emitter,
    CPU_INT08U        value,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_emitter' parameter.  */
    if (p_emitter == (MB_BUFFEREMITTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Ensure buffer remaining size.  */
    MBBufEmitter_EnsureRemaining(p_emitter, (CPU_SIZE_T)1U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Write the value.  */
    p_emitter->buffer[p_emitter->cursor] = value;

    /*  Move the cursor.  */
    ++(p_emitter->cursor);

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt16BE()
*
* Description : Write an unsigned big-endian 16-bit integer to current position of specific buffer emitter.
*
* Argument(s) : (1) p_emitter       Pointer to the emitter.
*               (2) value           Value to be written.
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                   No error occurred.
*                                       MB_ERROR_NULLREFERENCE          'p_emitter' is NULL.
*                                       MB_ERROR_BUFEMITTER_BUFFEREND   Length of remaining buffer is shorter than 2.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBBufEmitter_WriteUInt16BE(
    MB_BUFFEREMITTER  *p_emitter,
    CPU_INT16U        value,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_emitter' parameter.  */
    if (p_emitter == (MB_BUFFEREMITTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Ensure buffer remaining size.  */
    MBBufEmitter_EnsureRemaining(p_emitter, (CPU_SIZE_T)2U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Write the value.  */
    p_emitter->buffer[p_emitter->cursor + (CPU_SIZE_T)1U] = (CPU_INT08U)(value & (CPU_INT16U)0xFFU);
    value >>= (CPU_INT16U)8;
    p_emitter->buffer[p_emitter->cursor] = (CPU_INT08U)value;

    /*  Move the cursor.  */
    p_emitter->cursor += (CPU_SIZE_T)2U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


#if 0
/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt16LE()
*
* Description : Write an unsigned little-endian 16-bit integer to current position of specific buffer emitter.
*
* Argument(s) : (1) p_emitter       Pointer to the emitter.
*               (2) value           Value to be written.
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                   No error occurred.
*                                       MB_ERROR_NULLREFERENCE          'p_emitter' is NULL.
*                                       MB_ERROR_BUFEMITTER_BUFFEREND   Length of remaining buffer is shorter than 2.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBBufEmitter_WriteUInt16LE(
    MB_BUFFEREMITTER  *p_emitter,
    CPU_INT16U        value,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_emitter' parameter.  */
    if (p_emitter == (MB_BUFFEREMITTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Ensure buffer remaining size.  */
    MBBufEmitter_EnsureRemaining(p_emitter, (CPU_SIZE_T)2U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Write the value.  */
    p_emitter->buffer[p_emitter->cursor] = (CPU_INT08U)(value & (CPU_INT16U)0xFFU);
    value >>= (CPU_INT16U)8;
    p_emitter->buffer[p_emitter->cursor + (CPU_SIZE_T)1U] = (CPU_INT08U)(value & (CPU_INT16U)0xFFU);

    /*  Move the cursor.  */
    p_emitter->cursor += (CPU_SIZE_T)2U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
#endif


#if 0
/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt32BE()
*
* Description : Write an unsigned big-endian 32-bit integer to current position of specific buffer emitter.
*
* Argument(s) : (1) p_emitter       Pointer to the emitter.
*               (2) value           Value to be written.
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                   No error occurred.
*                                       MB_ERROR_NULLREFERENCE          'p_emitter' is NULL.
*                                       MB_ERROR_BUFEMITTER_BUFFEREND   Length of remaining buffer is shorter than 4.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBBufEmitter_WriteUInt32BE(
    MB_BUFFEREMITTER  *p_emitter,
    CPU_INT32U        value,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_emitter' parameter.  */
    if (p_emitter == (MB_BUFFEREMITTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Ensure buffer remaining size.  */
    MBBufEmitter_EnsureRemaining(p_emitter, (CPU_SIZE_T)4U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Write the value.  */
    p_emitter->buffer[p_emitter->cursor + (CPU_SIZE_T)3U] = (CPU_INT08U)(value & (CPU_INT32U)0xFFU);
    value >>= (CPU_INT32U)8;
    p_emitter->buffer[p_emitter->cursor + (CPU_SIZE_T)2U] = (CPU_INT08U)(value & (CPU_INT32U)0xFFU);
    value >>= (CPU_INT32U)8;
    p_emitter->buffer[p_emitter->cursor + (CPU_SIZE_T)1U] = (CPU_INT08U)(value & (CPU_INT32U)0xFFU);
    value >>= (CPU_INT32U)8;
    p_emitter->buffer[p_emitter->cursor] = (CPU_INT08U)value;

    /*  Move the cursor.  */
    p_emitter->cursor += (CPU_SIZE_T)4U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
#endif


#if 0
/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt32LE()
*
* Description : Write an unsigned little-endian 32-bit integer to current position of specific buffer emitter.
*
* Argument(s) : (1) p_emitter       Pointer to the emitter.
*               (2) value           Value to be written.
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                   No error occurred.
*                                       MB_ERROR_NULLREFERENCE          'p_emitter' is NULL.
*                                       MB_ERROR_BUFEMITTER_BUFFEREND   Length of remaining buffer is shorter than 4.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBBufEmitter_WriteUInt32LE(
    MB_BUFFEREMITTER  *p_emitter,
    CPU_INT32U        value,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_emitter' parameter.  */
    if (p_emitter == (MB_BUFFEREMITTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Ensure buffer remaining size.  */
    MBBufEmitter_EnsureRemaining(p_emitter, (CPU_SIZE_T)4U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Write the value.  */
    p_emitter->buffer[p_emitter->cursor] = (CPU_INT08U)(value & (CPU_INT32U)0xFFU);
    value >>= (CPU_INT32U)8;
    p_emitter->buffer[p_emitter->cursor + (CPU_SIZE_T)1U] = (CPU_INT08U)(value & (CPU_INT32U)0xFFU);
    value >>= (CPU_INT32U)8;
    p_emitter->buffer[p_emitter->cursor + (CPU_SIZE_T)2U] = (CPU_INT08U)(value & (CPU_INT32U)0xFFU);
    value >>= (CPU_INT32U)8;
    p_emitter->buffer[p_emitter->cursor + (CPU_SIZE_T)3U] = (CPU_INT08U)value;

    /*  Move the cursor.  */
    p_emitter->cursor += (CPU_SIZE_T)4U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
#endif


/*
*********************************************************************************************************
*                                    MBBufEmitter_GetWrittenLength()
*
* Description : Get the length of written data of specific buffer emitter.
*
* Argument(s) : (1) p_emitter       Pointer to the emitter.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                   No error occurred.
*                                       MB_ERROR_NULLREFERENCE          'p_emitter' is NULL.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_SIZE_T MBBufEmitter_GetWrittenLength(
    MB_BUFFEREMITTER  *p_emitter,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_emitter' parameter.  */
    if (p_emitter == (MB_BUFFEREMITTER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_SIZE_T)0U;
    }
#endif

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return p_emitter->cursor;
}


/*
*********************************************************************************************************
*                                    MBBufEmitter_EnsureRemaining()
*
* Description : Ensure the length of remaining buffer of specific buffer emitter is enough to be written.
*
* Argument(s) : (1) p_emitter       Pointer to the emitter.
*               (2) min_length      Minimum required remaining buffer length.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                  No error occurred.
*                                       MB_ERROR_BUFEMITTER_BUFFEREND  Length of remaining buffer is shorter than 'min_length'.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) 'p_emitter' is assumed to be not NULL.
*********************************************************************************************************
*/

static void MBBufEmitter_EnsureRemaining(
    MB_BUFFEREMITTER  *p_emitter,
    CPU_SIZE_T         min_length,
    MB_ERROR          *p_error
) {
    CPU_SIZE_T  remaining = (CPU_SIZE_T)(
        p_emitter->bufferLength - p_emitter->cursor
    );
    if (remaining < min_length) {
        *p_error = MB_ERROR_BUFEMITTER_BUFFEREND;
    } else {
        *p_error = MB_ERROR_NONE;
    }
}
