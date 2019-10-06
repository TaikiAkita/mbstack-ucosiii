/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_BUFFERFETCHER.C
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
#define MB_BUFFERFETCHER_SOURCE

#include <mb_bufferfetcher.h>
#include <mb_cfg.h>
#include <mb_constants.h>
#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void MBBufFetcher_EnsureRemaining(
    MB_BUFFERFETCHER  *p_fetcher,
    CPU_SIZE_T         min_length,
    MB_ERROR          *p_error
);


/*
*********************************************************************************************************
*                                    MBBufFetcher_Initialize()
*
* Description : Initialize a buffer fetcher.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) p_buffer        Pointer to the first element of the data buffer.
*               (3) buffer_length   Data buffer length.
*               (4) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE             No error occurred.
*                                       MB_ERROR_NULLREFERENCE    'p_fetcher' is NULL or 'p_buffer' is NULL 
*                                                                 while 'buffer_length' is not zero.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBBufFetcher_Initialize(
    MB_BUFFERFETCHER  *p_fetcher,
    CPU_INT08U        *p_buffer,
    CPU_SIZE_T         buffer_length,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
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

    /*  Reset the fetcher.  */
    p_fetcher->buffer = p_buffer;
    p_fetcher->bufferLength = buffer_length;
    p_fetcher->cursor = (CPU_SIZE_T)0U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


#if 0
/*
*********************************************************************************************************
*                                    MBBufFetcher_Reset()
*
* Description : Reset a buffer fetcher.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE             No error occurred.
*                                       MB_ERROR_NULLREFERENCE    'p_fetcher' is NULL.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) 'Reset' means to set the cursor of specific buffer fetcher to zero.
*********************************************************************************************************
*/

void MBBufFetcher_Reset(
    MB_BUFFERFETCHER  *p_fetcher,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Reset the cursor.  */
    p_fetcher->cursor = (CPU_SIZE_T)0U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
#endif


#if 0
/*
*********************************************************************************************************
*                                    MBBufFetcher_IsEnded()
*
* Description : Get whether the cursor of a buffer fetcher reaches the ending position.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE             No error occurred.
*                                       MB_ERROR_NULLREFERENCE    'p_fetcher' is NULL.
*
* Return(s)   : DEF_YES if so, DEF_NO if not.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_BOOLEAN MBBufFetcher_IsEnded(
    MB_BUFFERFETCHER  *p_fetcher,
    MB_ERROR          *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return DEF_NO;
    }
#endif

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    if (p_fetcher->cursor >= p_fetcher->bufferLength) {
        return DEF_YES;
    } else {
        return DEF_NO;
    }
}
#endif


#if 0
/*
*********************************************************************************************************
*                                    MBBufFetcher_GetCursor()
*
* Description : Get the cursor of a buffer fetcher.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE             No error occurred.
*                                       MB_ERROR_NULLREFERENCE    'p_fetcher' is NULL.
*
* Return(s)   : The cursor.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_SIZE_T MBBufFetcher_GetCursor(
    MB_BUFFERFETCHER  *p_fetcher,
    MB_ERROR          *p_error
) {

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_SIZE_T)0U;
    }
#endif

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return p_fetcher->cursor;
}
#endif


#if 0
/*
*********************************************************************************************************
*                                    MBBufFetcher_SetCursor()
*
* Description : Set the cursor of a buffer fetcher.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) cursor          New cursor.
*               (3) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE             No error occurred.
*                                       MB_ERROR_NULLREFERENCE    'p_fetcher' is NULL.
*                                       MB_ERROR_OVERFLOW         'cursor' is too large.
*
* Return(s)   : The cursor.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

void MBBufFetcher_SetCursor(
    MB_BUFFERFETCHER   *p_fetcher,
    CPU_SIZE_T          cursor,
    MB_ERROR           *p_error
) {

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Check the new cursor.  */
    if (p_fetcher->bufferLength == (CPU_SIZE_T)0U) {
        if (cursor != (CPU_SIZE_T)0U) {
            *p_error = MB_ERROR_OVERFLOW;
            return;
        }
    } else {
        if (cursor >= p_fetcher->bufferLength) {
            *p_error = MB_ERROR_OVERFLOW;
            return;
        }
    }

    /*  Set the cursor.  */
    p_fetcher->cursor = cursor;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
#endif


/*
*********************************************************************************************************
*                                    MBBufFetcher_ReadUInt8()
*
* Description : Read an unsigned 8-bit integer from current position of specific buffer fetcher.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                  No error occurred.
*                                       MB_ERROR_NULLREFERENCE         'p_fetcher' is NULL.
*                                       MB_ERROR_BUFFETCHER_BUFFEREND  The cursor is already at the ending position.
*
* Return(s)   : The integer.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_INT08U  MBBufFetcher_ReadUInt8(
    MB_BUFFERFETCHER  *p_fetcher,
    MB_ERROR          *p_error
) {
    CPU_INT08U  value;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT08U)0U;
    }
#endif

    /*  Ensure at least 1 byte remained.  */
    MBBufFetcher_EnsureRemaining(p_fetcher, (CPU_SIZE_T)1U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return (CPU_INT08U)0U;
    }

    /*  Read an unsigned 8-bit integer.  */
    value   = (CPU_INT08U)(p_fetcher->buffer[p_fetcher->cursor]);

    /*  Move the cursor.  */
    ++(p_fetcher->cursor);

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return value;
}


/*
*********************************************************************************************************
*                                    MBBufFetcher_ReadUInt16BE()
*
* Description : Read an unsigned big-endian 16-bit integer from current position of specific buffer fetcher.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                  No error occurred.
*                                       MB_ERROR_NULLREFERENCE         'p_fetcher' is NULL.
*                                       MB_ERROR_BUFFETCHER_BUFFEREND  Length of remaining data is shorter than 2.
*
* Return(s)   : The integer.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_INT16U  MBBufFetcher_ReadUInt16BE(
    MB_BUFFERFETCHER  *p_fetcher,
    MB_ERROR          *p_error
) {
    CPU_INT16U  value;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT16U)0U;
    }
#endif

    /*  Ensure at least 2 bytes remained.  */
    MBBufFetcher_EnsureRemaining(p_fetcher, (CPU_SIZE_T)2U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return (CPU_INT16U)0U;
    }

    /*  Read an unsigned big-endian 16-bit integer.  */
    value   = (CPU_INT16U)(p_fetcher->buffer[p_fetcher->cursor]);
    value <<= (CPU_INT16U)8;
    value  |= (CPU_INT16U)(p_fetcher->buffer[p_fetcher->cursor + (CPU_SIZE_T)1U]);

    /*  Move the cursor.  */
    p_fetcher->cursor += (CPU_SIZE_T)2U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return value;
}


#if 0
/*
*********************************************************************************************************
*                                    MBBufFetcher_ReadUInt16LE()
*
* Description : Read an unsigned little-endian 16-bit integer from current position of specific buffer fetcher.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                  No error occurred.
*                                       MB_ERROR_NULLREFERENCE         'p_fetcher' is NULL.
*                                       MB_ERROR_BUFFETCHER_BUFFEREND  Length of remaining data is shorter than 2.
*
* Return(s)   : The integer.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_INT16U  MBBufFetcher_ReadUInt16LE(
    MB_BUFFERFETCHER  *p_fetcher,
    MB_ERROR          *p_error
) {
    CPU_INT16U  value;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT16U)0U;
    }
#endif

    /*  Ensure at least 2 bytes remained.  */
    MBBufFetcher_EnsureRemaining(p_fetcher, (CPU_SIZE_T)2U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return (CPU_INT16U)0U;
    }

    /*  Read an unsigned little-endian 16-bit integer.  */
    value   = (CPU_INT16U)(p_fetcher->buffer[p_fetcher->cursor + (CPU_SIZE_T)1U]);
    value <<= (CPU_INT16U)8;
    value  |= (CPU_INT16U)(p_fetcher->buffer[p_fetcher->cursor]);

    /*  Move the cursor.  */
    p_fetcher->cursor += (CPU_SIZE_T)2U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return value;
}
#endif


#if 0
/*
*********************************************************************************************************
*                                    MBBufFetcher_ReadUInt32BE()
*
* Description : Read an unsigned big-endian 32-bit integer from current position of specific buffer fetcher.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                  No error occurred.
*                                       MB_ERROR_NULLREFERENCE         'p_fetcher' is NULL.
*                                       MB_ERROR_BUFFETCHER_BUFFEREND  Length of remaining data is shorter than 4.
*
* Return(s)   : The integer.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_INT32U  MBBufFetcher_ReadUInt32BE(
    MB_BUFFERFETCHER  *p_fetcher,
    MB_ERROR          *p_error
) {
    CPU_INT32U  value;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT32U)0U;
    }
#endif

    /*  Ensure at least 4 bytes remained.  */
    MBBufFetcher_EnsureRemaining(p_fetcher, (CPU_SIZE_T)4U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return (CPU_INT32U)0U;
    }

    /*  Read an unsigned big-endian 32-bit integer.  */
    value   = (CPU_INT32U)(p_fetcher->buffer[p_fetcher->cursor]);
    value <<= (CPU_INT32U)8;
    value  |= (CPU_INT32U)(p_fetcher->buffer[p_fetcher->cursor + (CPU_SIZE_T)1U]);
    value <<= (CPU_INT32U)8;
    value  |= (CPU_INT32U)(p_fetcher->buffer[p_fetcher->cursor + (CPU_SIZE_T)2U]);
    value <<= (CPU_INT32U)8;
    value  |= (CPU_INT32U)(p_fetcher->buffer[p_fetcher->cursor + (CPU_SIZE_T)3U]);

    /*  Move the cursor.  */
    p_fetcher->cursor += (CPU_SIZE_T)4U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return value;
}
#endif


#if 0
/*
*********************************************************************************************************
*                                    MBBufFetcher_ReadUInt32LE()
*
* Description : Read an unsigned little-endian 32-bit integer from current position of specific buffer fetcher.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                  No error occurred.
*                                       MB_ERROR_NULLREFERENCE         'p_fetcher' is NULL.
*                                       MB_ERROR_BUFFETCHER_BUFFEREND  Length of remaining data is shorter than 4.
*
* Return(s)   : The integer.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*********************************************************************************************************
*/

CPU_INT32U  MBBufFetcher_ReadUInt32LE(
    MB_BUFFERFETCHER  *p_fetcher,
    MB_ERROR          *p_error
) {
    CPU_INT32U  value;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_fetcher' parameter.  */
    if (p_fetcher == (MB_BUFFERFETCHER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT32U)0U;
    }
#endif

    /*  Ensure at least 4 bytes remained.  */
    MBBufFetcher_EnsureRemaining(p_fetcher, (CPU_SIZE_T)4U, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return (CPU_INT32U)0U;
    }

    /*  Read an unsigned little-endian 32-bit integer.  */
    value   = (CPU_INT32U)(p_fetcher->buffer[p_fetcher->cursor + (CPU_SIZE_T)3U]);
    value <<= (CPU_INT32U)8;
    value  |= (CPU_INT32U)(p_fetcher->buffer[p_fetcher->cursor + (CPU_SIZE_T)2U]);
    value <<= (CPU_INT32U)8;
    value  |= (CPU_INT32U)(p_fetcher->buffer[p_fetcher->cursor + (CPU_SIZE_T)1U]);
    value <<= (CPU_INT32U)8;
    value  |= (CPU_INT32U)(p_fetcher->buffer[p_fetcher->cursor]);

    /*  Move the cursor.  */
    p_fetcher->cursor += (CPU_SIZE_T)4U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return value;
}
#endif


/*
*********************************************************************************************************
*                                    MBBufFetcher_EnsureRemaining()
*
* Description : Ensure the length of remaining data of specific buffer fetcher is enough to be read.
*
* Argument(s) : (1) p_fetcher       Pointer to the fetcher.
*               (2) min_length      Minimum required remaining data length.
*               (2) p_error         Pointer to the variable that receives error code from this function:
*
*                                       MB_ERROR_NONE                  No error occurred.
*                                       MB_ERROR_BUFFETCHER_BUFFEREND  Length of remaining data is shorter than 'min_length'.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) 'p_fetcher' is assumed to be not NULL.
*********************************************************************************************************
*/

static void MBBufFetcher_EnsureRemaining(
    MB_BUFFERFETCHER  *p_fetcher,
    CPU_SIZE_T         min_length,
    MB_ERROR          *p_error
) {
    CPU_SIZE_T  remaining = (CPU_SIZE_T)(
        p_fetcher->bufferLength - p_fetcher->cursor
    );
    if (remaining < min_length) {
        *p_error = MB_ERROR_BUFFETCHER_BUFFEREND;
    } else {
        *p_error = MB_ERROR_NONE;
    }
}
