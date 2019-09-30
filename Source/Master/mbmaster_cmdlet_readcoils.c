/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_READCOILS.C
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
#define MBMASTER_CMDLET_READCOILS_SOURCE

#include <mbmaster_cmdlet_readcoils.h>
#include <mbmaster_cmdlet_common.h>

#include <mbmaster_cfg.h>

#include <mb_constants.h>
#include <mb_types.h>

#include <mb_bufferemitter.h>
#include <mb_bufferfetcher.h>

#include <mb_constants.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_READCOILS_EN == DEF_ENABLED)

/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void MBMaster_CmdLet_FC01_ReqHdl(
    CPU_INT08U   slave,
    void        *p_request,
    CPU_INT08U  *p_buffer,
    CPU_SIZE_T   buffer_size,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
);

static void MBMaster_CmdLet_FC01_ResHdl(
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

MBMASTER_CMDLET  g_MBMaster_CmdLet_FC01 = {
    .cbRequestHandler = MBMaster_CmdLet_FC01_ReqHdl,
    .cbResponseHandler = MBMaster_CmdLet_FC01_ResHdl
};


/*
*********************************************************************************************************
*                                  MBMaster_CmdLet_FC01_ReqHdl()
*
* Description : Make request frame for "Read Coils (0x01)" command.
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
*               (2) 'p_request' must points to a 'MBMASTER_CMDLET_READCOILS_REQUEST' object.
*********************************************************************************************************
*/

static void MBMaster_CmdLet_FC01_ReqHdl(
    CPU_INT08U   slave,
    void        *p_request,
    CPU_INT08U  *p_buffer,
    CPU_SIZE_T   buffer_size,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
) {
    MB_BUFFEREMITTER                      emitter;

    MBMASTER_CMDLET_READCOILS_REQUEST    *request;

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
    request = (MBMASTER_CMDLET_READCOILS_REQUEST*)p_request;

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
        (request->coilQuantity  > (CPU_INT16U)0x07D0U)
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

    /*  Write the frame.  */
    p_frame->address = slave;
    p_frame->functionCode = MB_FNCODE_READCOILS;
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
*                                  MBMaster_CmdLet_FC01_ResHdl()
*
* Description : Handle response frame of "Read Coils (0x01)" command.
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
*               (2) 'p_request' must point to a 'MBMASTER_CMDLET_READCOILS_REQUEST' object.
*               (3) 'p_response' must point to a 'MBMASTER_CMDLET_READCOILS_RESPONSE' object.
*********************************************************************************************************
*/

static void MBMaster_CmdLet_FC01_ResHdl(
    CPU_INT08U   slave,
    void        *p_request,
    void        *p_response,
    void        *p_responsearg,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
) {
    MB_BUFFERFETCHER                     fetcher;

    MBMASTER_CMDLET_READCOILS_REQUEST   *request;
    MBMASTER_CMDLET_READCOILS_RESPONSE  *response;

    CPU_INT16U                           byteCountExpected;
    CPU_INT08U                           byteCountActual;
    CPU_INT08U                           byteOffset;

    CPU_INT16U                           coilCurrent;
    CPU_INT16U                           coilRemainingCount;

    CPU_INT08U                           coilStatusByte;
    CPU_BOOLEAN                          coilStatusCurrent;

    CPU_INT08U                           duffBitMask;
    CPU_INT08U                           duffByte;

    CPU_INT08U                           ec;

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
    request  = (MBMASTER_CMDLET_READCOILS_REQUEST *)p_request;
    response = (MBMASTER_CMDLET_READCOILS_RESPONSE*)p_response;

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

    if (p_frame->functionCode == MB_FNCODE_READCOILS) {
        /*  Get the expected byte count.  */
        byteCountExpected = request->coilQuantity;
        --(byteCountExpected);
        byteCountExpected /= (CPU_INT16U)8U;
        ++(byteCountExpected);

        /*  Read the actual byte count.  */
        byteCountActual = MBBufFetcher_ReadUInt8(
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

        /*  Check whether the actual byte count is equal to the expected value.  */
        if (byteCountExpected != (CPU_INT16U)byteCountActual) {
            *p_error = MB_ERROR_MASTER_RXINVALIDFORMAT;
            return;
        }

        /*  Notify the upper application that we're starting to transfer coil status.  */
        if (response->cbCoilValueStart != (MBMASTER_COILVALUE_START_CB)0) {
            response->cbCoilValueStart(p_responsearg, p_error);
            if (*p_error != MB_ERROR_NONE) {
                *p_error = MB_ERROR_MASTER_CALLBACKFAILED;
                return;
            }
        }

        /*  Read all coil status.  */
        coilCurrent = request->coilStartAddress;
        coilRemainingCount = request->coilQuantity;
        for (byteOffset = (CPU_INT08U)0U; byteOffset < byteCountActual; ++byteOffset) {
            /*  Read one coil status byte.  */
            coilStatusByte = MBBufFetcher_ReadUInt8(
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

            /*  Initialize the Duff's device.  */
            if (coilRemainingCount > (CPU_INT16U)8U) {
                duffByte = (CPU_INT08U)8U;
            } else {
                duffByte = (CPU_INT08U)coilRemainingCount;
            }
            duffBitMask = (CPU_INT08U)1U;

#define MBMASTER_RDCOILS_DUFFUNIT() {                                      \
    /*  Get the coil status.  */                                           \
    if ((coilStatusByte & duffBitMask) != (CPU_INT08U)0) {                 \
        coilStatusCurrent = DEF_YES;                                       \
    } else {                                                               \
        coilStatusCurrent = DEF_NO;                                        \
    }                                                                      \
                                                                           \
    /*  Invoke the callback.  */                                           \
    if (response->cbCoilValueUpdate != (MBMASTER_COILVALUE_UPDATE_CB)0) {  \
        response->cbCoilValueUpdate(                                       \
            coilCurrent,                                                   \
            coilStatusCurrent,                                             \
            p_responsearg,                                                 \
            p_error                                                        \
        );                                                                 \
        if (*p_error != MB_ERROR_NONE) {                                   \
            *p_error = MB_ERROR_MASTER_CALLBACKFAILED;                     \
            return;                                                        \
        }                                                                  \
    }                                                                      \
                                                                           \
    /*  Move to the next coil.  */                                         \
    duffBitMask <<= (CPU_INT08U)1U;                                        \
    ++(coilCurrent);                                                       \
    --(coilRemainingCount);                                                \
}

            /*  Body of the Duff's device.  */
            switch (duffByte) {
                case (CPU_INT08U)8U:
                    MBMASTER_RDCOILS_DUFFUNIT();
                case (CPU_INT08U)7U:
                    MBMASTER_RDCOILS_DUFFUNIT();
                case (CPU_INT08U)6U:
                    MBMASTER_RDCOILS_DUFFUNIT();
                case (CPU_INT08U)5U:
                    MBMASTER_RDCOILS_DUFFUNIT();
                case (CPU_INT08U)4U:
                    MBMASTER_RDCOILS_DUFFUNIT();
                case (CPU_INT08U)3U:
                    MBMASTER_RDCOILS_DUFFUNIT();
                case (CPU_INT08U)2U:
                    MBMASTER_RDCOILS_DUFFUNIT();
                case (CPU_INT08U)1U:
                    MBMASTER_RDCOILS_DUFFUNIT();
            }

#undef MBMASTER_RDCOILS_DUFFUNIT
        }

        /*  Notify the upper application that all coil status has been transfered.  */
        if (response->cbCoilValueEnd != (MBMASTER_COILVALUE_END_CB)0) {
            response->cbCoilValueEnd(p_responsearg, p_error);
            if (*p_error != MB_ERROR_NONE) {
                *p_error = MB_ERROR_MASTER_CALLBACKFAILED;
                return;
            }
        }
    } else if (p_frame->functionCode == (CPU_INT08U)(MB_FNCODE_READCOILS + (CPU_INT08U)0x80U)) {
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

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_READCOILS_EN == DEF_ENABLED)  */