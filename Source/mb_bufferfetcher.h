/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_BUFFERFETCHER.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_BUFFERFETCHER_H__
#define MB_BUFFERFETCHER_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_types.h>

#include <cpu.h>


#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT08U    *buffer;
    CPU_SIZE_T     bufferLength;
    CPU_SIZE_T     cursor;
} MB_BUFFERFETCHER;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

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
);


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
);


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
);


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
*               (2) 'Reset' means to set the cursor of specific buffer fetcher to zero.
*********************************************************************************************************
*/

CPU_SIZE_T MBBufFetcher_GetCursor(
    MB_BUFFERFETCHER  *p_fetcher,
    MB_ERROR          *p_error
);


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
*               (2) 'Reset' means to set the cursor of specific buffer fetcher to zero.
*********************************************************************************************************
*/

void MBBufFetcher_SetCursor(
    MB_BUFFERFETCHER   *p_fetcher,
    CPU_SIZE_T          cursor,
    MB_ERROR           *p_error
);


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
);


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
);


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
);


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
);


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
);


#ifdef __cplusplus
}
#endif

#endif