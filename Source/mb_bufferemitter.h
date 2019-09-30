/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_BUFFEREMITTER.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_BUFFEREMITTER_H__
#define MB_BUFFEREMITTER_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_types.h>

#include <cpu.h>


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT08U  *buffer;
    CPU_SIZE_T   bufferLength;
    CPU_SIZE_T   cursor;
} MB_BUFFEREMITTER;


#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

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
);



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
);



/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt8()
*
* Description : Write an unsigned 8-bit integer to current position of specific buffer fetcher.
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
);



/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt16BE()
*
* Description : Write an unsigned big-endian 16-bit integer to current position of specific buffer fetcher.
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
);



/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt16LE()
*
* Description : Write an unsigned little-endian 16-bit integer to current position of specific buffer fetcher.
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
);



/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt32BE()
*
* Description : Write an unsigned big-endian 32-bit integer to current position of specific buffer fetcher.
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
);



/*
*********************************************************************************************************
*                                    MBBufEmitter_WriteUInt32LE()
*
* Description : Write an unsigned little-endian 32-bit integer to current position of specific buffer fetcher.
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
);



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
);


#ifdef __cplusplus
}
#endif

#endif