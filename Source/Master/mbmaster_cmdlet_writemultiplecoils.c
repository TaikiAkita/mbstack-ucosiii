/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_WRITEMULTIPLECOILS.C
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
#define MBMASTER_SOURCE
#define MBMASTER_CMDLET_WRITECOILS_SOURCE

#include <mbmaster_cmdlet_writemultiplecoils.h>
#include <mbmaster_cmdlet_common.h>

#include <mbmaster_cfg.h>

#include <mb_constants.h>
#include <mb_types.h>

#include <mb_bufferemitter.h>
#include <mb_bufferfetcher.h>

#include <mb_constants.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLECOILS == DEF_ENABLED)

/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void MBMaster_CmdLet_FC0F_ReqHdl(
    CPU_INT08U   slave,
    void        *p_request,
    CPU_INT08U  *p_buffer,
    CPU_SIZE_T   buffer_size,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
);

static void MBMaster_CmdLet_FC0F_ResHdl(
    CPU_INT08U   slave,
    void        *p_request,
    void        *p_response,
    void        *p_responsearg,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
);


/*
*********************************************************************************************************
*                                       COMMAND-LET DESCRIPTOR
*********************************************************************************************************
*/

MBMASTER_CMDLET  g_MBMaster_CmdLet_FC0F = {
    .cbRequestHandler = MBMaster_CmdLet_FC0F_ReqHdl,
    .cbResponseHandler = MBMaster_CmdLet_FC0F_ResHdl
};


/*
*********************************************************************************************************
*                                  MBMaster_CmdLet_FC0F_ReqHdl()
*
* Description : Make request frame for "Write Multiple Coils (0x0F)" command.
*
* Argument(s) : (1) slave                 Slave address.
*               (2) p_request             Pointer to the request object.
*               (3) p_buffer              Pointer to the first element of the data buffer.
*               (4) buffer_size           Size of the data buffer.
*               (5) p_frame               Pointer to the variable that receives the request frame.
*               (6) p_error               Pointer to the variable that receives error code from this function:
*
*                                             MB_ERROR_NONE                    No error occurred.
*                                             MB_ERROR_NULLREFERENCE           One of following conditions occurred:
*
*                                                                                  (1) 'p_request' is NULL.
*                                                                                  (2) 'p_buffer' is NULL while 'buffer_size' is not zero.
*                                                                                  (3) 'p_frame' is NULL.
*
*                                             MB_ERROR_MASTER_TXBADREQUEST     Bad request parameter.
*                                             MB_ERROR_MASTER_TXBUFFERLOW      Data buffer is too small to contains the request data.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when calling this function.
*               (2) 'p_request' must points to a 'MBMASTER_CMDLET_WRITEMULTIPLECOILS_REQUEST' object.
*********************************************************************************************************
*/

static void MBMaster_CmdLet_FC0F_ReqHdl(
    CPU_INT08U   slave,
    void        *p_request,
    CPU_INT08U  *p_buffer,
    CPU_SIZE_T   buffer_size,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
) {
    MB_BUFFEREMITTER                               emitter;

    MBMASTER_CMDLET_WRITEMULTIPLECOILS_REQUEST    *request;

    CPU_BOOLEAN                                   *coilValuePtr;
    CPU_INT16U                                     coilQuantity;
    CPU_INT08U                                     coilStatusByte;

    CPU_INT08U                                     duffByte;
    CPU_INT08U                                     duffBitMask;

    CPU_INT16U                                     byteCount;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_request' parameter.  */
    if (p_request == (void*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_buffer' parameter.  */
    if (
        (p_buffer    == (CPU_INT08U*)0) && 
        (buffer_size != (CPU_SIZE_T)0U)
    ) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_frame' parameter.  */
    if (p_frame == (MB_FRAME*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Type cast.  */
    request = (MBMASTER_CMDLET_WRITEMULTIPLECOILS_REQUEST*)p_request;

    /*  Initialize the emitter.  */
    MBBufEmitter_Initialize(
        &(emitter),
        p_buffer,
        buffer_size,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Write the coil start address.  */
    MBBufEmitter_WriteUInt16BE(
        &(emitter),
        request->coilStartAddress,
        p_error
    );
    switch (*p_error) {
        case MB_ERROR_NONE:
            break;
        case MB_ERROR_BUFEMITTER_BUFFEREND:
            *p_error = MB_ERROR_MASTER_TXBUFFERLOW;
        default:
            return;
    }

    /*  Write the coil quantity.  */
    if (
        (request->coilQuantity == (CPU_INT16U)0x0000U) || 
        (request->coilQuantity  > (CPU_INT16U)0x07B0U)
    ) {
        *p_error = MB_ERROR_MASTER_TXBADREQUEST;
        return;
    }
    MBBufEmitter_WriteUInt16BE(
        &(emitter),
        request->coilQuantity,
        p_error
    );
    switch (*p_error) {
        case MB_ERROR_NONE:
            break;
        case MB_ERROR_BUFEMITTER_BUFFEREND:
            *p_error = MB_ERROR_MASTER_TXBUFFERLOW;
        default:
            return;
    }

    /*  Write the byte count.*/
    byteCount = request->coilQuantity;
    if (byteCount != (CPU_INT16U)0U) {
        --(byteCount);
        byteCount /= (CPU_INT16U)8U;
        ++(byteCount);
    }
    MBBufEmitter_WriteUInt16BE(
        &(emitter),
        byteCount,
        p_error
    );
    switch (*p_error) {
        case MB_ERROR_NONE:
            break;
        case MB_ERROR_BUFEMITTER_BUFFEREND:
            *p_error = MB_ERROR_MASTER_TXBUFFERLOW;
        default:
            return;
    }

    /*  Write coil status bytes.  */
    coilValuePtr = request->coilValues;
    coilQuantity = request->coilQuantity;
    while (coilQuantity != (CPU_INT16U)0U) {
        /*  Initialize the Duff's device.  */
        if (coilQuantity > (CPU_INT16U)8U) {
            duffByte = (CPU_INT08U)8U;
        } else {
            duffByte = (CPU_INT08U)coilQuantity;
        }
        duffBitMask = (CPU_INT08U)1U;

        /*  Reset the coil status byte.  */
        coilStatusByte = (CPU_INT08U)0U;

#define MBMASTER_WRCOILS_DUFFUNIT() {     \
    if (*(coilValuePtr)) {                \
        coilStatusByte |= duffBitMask;    \
    }                                     \
    ++(coilValuePtr);                     \
    --(coilQuantity);                     \
}

        /*  Body of the Duff's device.  */
        switch (duffByte) {
            case (CPU_INT08U)8U:
                MBMASTER_WRCOILS_DUFFUNIT();
            case (CPU_INT08U)7U:
                MBMASTER_WRCOILS_DUFFUNIT();
            case (CPU_INT08U)6U:
                MBMASTER_WRCOILS_DUFFUNIT();
            case (CPU_INT08U)5U:
                MBMASTER_WRCOILS_DUFFUNIT();
            case (CPU_INT08U)4U:
                MBMASTER_WRCOILS_DUFFUNIT();
            case (CPU_INT08U)3U:
                MBMASTER_WRCOILS_DUFFUNIT();
            case (CPU_INT08U)2U:
                MBMASTER_WRCOILS_DUFFUNIT();
            case (CPU_INT08U)1U:
                MBMASTER_WRCOILS_DUFFUNIT();
        }

#undef MBMASTER_WRCOILS_DUFFUNIT

        /*  Write the coil status byte.  */
        MBBufEmitter_WriteUInt8(
            &(emitter),
            coilStatusByte,
            p_error
        );
        switch (*p_error) {
            case MB_ERROR_NONE:
                break;
            case MB_ERROR_BUFEMITTER_BUFFEREND:
                *p_error = MB_ERROR_MASTER_TXBUFFERLOW;
            default:
                return;
        }
    }

    /*  Write the frame.  */
    p_frame->address = slave;
    p_frame->functionCode = MB_FNCODE_WRITEMULTIPLECOILS;
    p_frame->data = p_buffer;
    p_frame->dataLength = MBBufEmitter_GetWrittenLength(
        &(emitter),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        return;
    }
}


/*
*********************************************************************************************************
*                                  MBMaster_CmdLet_FC0F_ResHdl()
*
* Description : Handle response frame of "Write Multiple Coils (0x0F)" command.
*
* Argument(s) : (1) slave                 Slave address.
*               (2) p_request             Pointer to the request object.
*               (2) p_response            Pointer to the response object.
*               (3) p_responsearg         'p_arg' parameter passed to response callbacks.
*               (4) p_frame               Pointer to the variable that receives the request frame.
*               (5) p_error               Pointer to the variable that receives error code from this function:
*
*                                             MB_ERROR_NONE                    No error occurred.
*                                             MB_ERROR_NULLREFERENCE           One of following conditions occurred:
*
*                                                                                  (1) 'p_request' is NULL.
*                                                                                  (2) 'p_response' is NULL.
*                                                                                  (3) 'p_frame' is NULL.
*
*                                             MB_ERROR_MASTER_RXINVALIDSLAVE   Frame is not from the expected slave.
*                                             MB_ERROR_MASTER_RXTRUNCATED      Frame data is truncated.
*                                             MB_ERROR_MASTER_RXINVALIDFORMAT  Frame data contains invalid format (or value).
*                                             MB_ERROR_MASTER_CALLBACKFAILED   Error occurred while calling external callbacks.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when calling this function.
*               (2) 'p_request' must point to a 'MBMASTER_CMDLET_WRITEMULTIPLECOILS_REQUEST' object.
*               (3) 'p_response' must point to a 'MBMASTER_CMDLET_WRITEMULTIPLECOILS_RESPONSE' object.
*********************************************************************************************************
*/

static void MBMaster_CmdLet_FC0F_ResHdl(
    CPU_INT08U   slave,
    void        *p_request,
    void        *p_response,
    void        *p_responsearg,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
) {
    MB_BUFFERFETCHER                              fetcher;

    MBMASTER_CMDLET_WRITEMULTIPLECOILS_REQUEST   *request;
    MBMASTER_CMDLET_WRITEMULTIPLECOILS_RESPONSE  *response;

    CPU_INT16U                                    field16;

    CPU_INT08U                                    ec;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_request' parameter.  */
    if (p_request == (void*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_response' parameter.  */
    if (p_response == (void*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_frame' parameter.  */
    if (p_frame == (MB_FRAME*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Check whether the response frame is coming from the expected slave.  */
    if (slave != p_frame->address) {
        *p_error = MB_ERROR_MASTER_RXINVALIDSLAVE;
        return;
    }

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Type cast.  */
    request  = (MBMASTER_CMDLET_WRITEMULTIPLECOILS_REQUEST *)p_request;
    response = (MBMASTER_CMDLET_WRITEMULTIPLECOILS_RESPONSE*)p_response;

    /*  Initialize the fetcher.  */
    MBBufFetcher_Initialize(
        &(fetcher),
        p_frame->data,
        p_frame->dataLength,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    if (p_frame->functionCode == MB_FNCODE_WRITEMULTIPLECOILS) {
        /*  Read and check the coil start address.  */
        field16 = MBBufFetcher_ReadUInt16BE(
            &(fetcher),
            p_error
        );
        switch (*p_error) {
            case MB_ERROR_NONE:
                break;
            case MB_ERROR_BUFFETCHER_BUFFEREND:
                *p_error = MB_ERROR_MASTER_RXTRUNCATED;
            default:
                return;
        }
        if (field16 != request->coilStartAddress) {
            *p_error = MB_ERROR_MASTER_RXINVALIDFORMAT;
            return;
        }

        /*  Read and check the coil quantity.  */
        field16 = MBBufFetcher_ReadUInt16BE(
            &(fetcher),
            p_error
        );
        switch (*p_error) {
            case MB_ERROR_NONE:
                break;
            case MB_ERROR_BUFFETCHER_BUFFEREND:
                *p_error = MB_ERROR_MASTER_RXTRUNCATED;
            default:
                return;
        }
        if (field16 != request->coilQuantity) {
            *p_error = MB_ERROR_MASTER_RXINVALIDFORMAT;
            return;
        }

        /*  Notify the upper application that all coil status has been written.  */
        if (response->cbComplete != (MBMASTER_COILSWRITTEN_CB)0) {
            response->cbComplete(
                request->coilStartAddress,
                request->coilQuantity,
                p_responsearg, 
                p_error
            );
            if (*p_error != MB_ERROR_NONE) {
                *p_error = MB_ERROR_MASTER_CALLBACKFAILED;
                return;
            }
        }
    } else if (p_frame->functionCode == (CPU_INT08U)(MB_FNCODE_WRITEMULTIPLECOILS + (CPU_INT08U)0x80U)) {
        /*  Read the exception code.  */
        ec = MBBufFetcher_ReadUInt8(
            &(fetcher),
            p_error
        );
        switch (*p_error) {
            case MB_ERROR_NONE:
                break;
            case MB_ERROR_BUFFETCHER_BUFFEREND:
                *p_error = MB_ERROR_MASTER_RXTRUNCATED;
            default:
                return;
        }

        /*  Notify upper application the exception code.  */
        if (response->cbException != (MBMASTER_EXCEPTION_CB)0) {
            response->cbException(ec, p_responsearg, p_error);
            if (*p_error != MB_ERROR_NONE) {
                *p_error = MB_ERROR_MASTER_CALLBACKFAILED;
                return;
            }
        }
    } else {
        *p_error = MB_ERROR_MASTER_RXINVALIDFNCODE;
        return;
    }
}

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_WRITEMULTIPLECOILS == DEF_ENABLED)  */