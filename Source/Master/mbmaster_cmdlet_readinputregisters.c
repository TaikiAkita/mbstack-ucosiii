/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                             MASTER MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBMASTER_CMDLET_READINPUTREGISTERS.C
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
#define MBMASTER_CMDLET_READIREGS_SOURCE

#include <mbmaster_cmdlet_readinputregisters.h>
#include <mbmaster_cmdlet_common.h>

#include <mbmaster_cfg.h>

#include <mb_constants.h>
#include <mb_types.h>

#include <mb_bufferemitter.h>
#include <mb_bufferfetcher.h>

#include <mb_constants.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_READINPUTREGS_EN == DEF_ENABLED)

/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void MBMaster_CmdLet_FC04_ReqHdl(
    CPU_INT08U   slave,
    void        *p_request,
    CPU_INT08U  *p_buffer,
    CPU_SIZE_T   buffer_size,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
);

static void MBMaster_CmdLet_FC04_ResHdl(
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

MBMASTER_CMDLET  g_MBMaster_CmdLet_FC04 = {
    .cbRequestHandler = MBMaster_CmdLet_FC04_ReqHdl,
    .cbResponseHandler = MBMaster_CmdLet_FC04_ResHdl
};


/*
*********************************************************************************************************
*                                  MBMaster_CmdLet_FC04_ReqHdl()
*
* Description : Make request frame for "Read Input Registers (0x04)" command.
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
*               (2) 'p_request' must points to a 'MBMASTER_CMDLET_READINPUTREGISTERS_REQUEST' object.
*********************************************************************************************************
*/

static void MBMaster_CmdLet_FC04_ReqHdl(
    CPU_INT08U   slave,
    void        *p_request,
    CPU_INT08U  *p_buffer,
    CPU_SIZE_T   buffer_size,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
) {
    MB_BUFFEREMITTER                                 emitter;

    MBMASTER_CMDLET_READINPUTREGISTERS_REQUEST      *request;

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
    request = (MBMASTER_CMDLET_READINPUTREGISTERS_REQUEST*)p_request;

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

    /*  Write the input register start address.  */
    MBBufEmitter_WriteUInt16BE(
        &(emitter),
        request->iregStartAddress,
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

    /*  Write the input register quantity.  */
    if (
        (request->iregQuantity == (CPU_INT16U)0x0000U) || 
        (request->iregQuantity  > (CPU_INT16U)0x007DU)
    ) {
        *p_error = MB_ERROR_MASTER_TXBADREQUEST;
        return;
    }
    MBBufEmitter_WriteUInt16BE(
        &(emitter),
        request->iregQuantity,
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
    p_frame->functionCode = MB_FNCODE_READINPUTREGISTERS;
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
*                                  MBMaster_CmdLet_FC04_ResHdl()
*
* Description : Handle response frame of "Read Input Registers (0x04)" command.
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
*               (2) 'p_request' must point to a 'MBMASTER_CMDLET_READINPUTREGISTERS_REQUEST' object.
*               (3) 'p_response' must point to a 'MBMASTER_CMDLET_READINPUTREGISTERS_RESPONSE' object.
*********************************************************************************************************
*/

static void MBMaster_CmdLet_FC04_ResHdl(
    CPU_INT08U   slave,
    void        *p_request,
    void        *p_response,
    void        *p_responsearg,
    MB_FRAME    *p_frame,
    MB_ERROR    *p_error
) {
    MB_BUFFERFETCHER                                fetcher;

    MBMASTER_CMDLET_READINPUTREGISTERS_REQUEST     *request;
    MBMASTER_CMDLET_READINPUTREGISTERS_RESPONSE    *response;

    CPU_INT32U                                      byteCountExpected;
    CPU_INT08U                                      byteCountActual;

    CPU_INT16U                                      iregCurrentAddress;
    CPU_INT16U                                      iregQuantity;
    CPU_INT16U                                      iregValue;

    CPU_INT08U                                      ec;

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
    request  = (MBMASTER_CMDLET_READINPUTREGISTERS_REQUEST *)p_request;
    response = (MBMASTER_CMDLET_READINPUTREGISTERS_RESPONSE*)p_response;

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

    if (p_frame->functionCode == MB_FNCODE_READINPUTREGISTERS) {
        /*  Get the expected byte count.  */
        byteCountExpected   = (CPU_INT32U)(request->iregQuantity);
        byteCountExpected <<= (CPU_INT32U)1U;

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
        if (byteCountExpected != (CPU_INT32U)byteCountActual) {
            *p_error = MB_ERROR_MASTER_RXINVALIDFORMAT;
            return;
        }

        /*  Notify the upper application that we're starting to transfer input register status.  */
        if (response->cbIRegValueStart != (MBMASTER_IREGVALUE_START_CB)0) {
            response->cbIRegValueStart(p_responsearg, p_error);
            if (*p_error != MB_ERROR_NONE) {
                *p_error = MB_ERROR_MASTER_CALLBACKFAILED;
                return;
            }
        }

        /*  Read all input registers status.  */
        iregCurrentAddress = request->iregStartAddress;
        iregQuantity = request->iregQuantity;
        while (iregQuantity != (CPU_INT16U)0U) {
            /*  Read one input register value.  */
            iregValue = MBBufFetcher_ReadUInt16BE(
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

            /*  Invoke the callback.  */
            if (response->cbIRegValueUpdate != (MBMASTER_IREGVALUE_UPDATE_CB)0) {
                response->cbIRegValueUpdate(
                    iregCurrentAddress,
                    iregValue,
                    p_responsearg,
                    p_error
                );
                if (*p_error != MB_ERROR_NONE) {
                    *p_error = MB_ERROR_MASTER_CALLBACKFAILED;
                    return;
                }
            }

            /*  Move to the next register.  */
            --(iregQuantity);
            ++(iregCurrentAddress);
        }

        /*  Notify the upper application that all input register status has been transfered.  */
        if (response->cbIRegValueEnd != (MBMASTER_IREGVALUE_END_CB)0) {
            response->cbIRegValueEnd(p_responsearg, p_error);
            if (*p_error != MB_ERROR_NONE) {
                *p_error = MB_ERROR_MASTER_CALLBACKFAILED;
                return;
            }
        }
    } else if (p_frame->functionCode == (CPU_INT08U)(MB_FNCODE_READINPUTREGISTERS + (CPU_INT08U)0x80U)) {
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

#endif  /*  #if (MB_CFG_MASTER_EN == DEF_ENABLED) && (MB_CFG_MASTER_BUILTIN_CMDLET_READINPUTREGS_EN == DEF_ENABLED)  */