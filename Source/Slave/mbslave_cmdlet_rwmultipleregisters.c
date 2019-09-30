/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              SLAVE MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MBSLAVE_CMDLET_RWMULTIPLEREGISTERS.C
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
#define MBSLAVE_SOURCE
#define MBSLAVE_CMDLET_RWHREG_SOURCE

#include <mbslave_cmdlet_rwmultipleregisters.h>
#include <mbslave_cfg.h>

#include <mb_bufferemitter.h>
#include <mb_bufferfetcher.h>
#include <mb_constants.h>

#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


#if (MB_CFG_SLAVE_EN == DEF_ENABLED) && (MB_CFG_SLAVE_BUILTIN_CMDLET_READWRITEMULTIPLEREGS == DEF_ENABLED)

/*
*********************************************************************************************************
*                                    MBSlave_CmdLet_ReadWriteMultipleRegisters()
*
* Description : Command implementation of "Read/Write Multiple Registers (0x17)".
*
* Argument(s) : (1) request_fncode        Function code of the request.
*               (2) p_request_data        Pointer to the first element of the request data.
*               (2) request_data_size     Size(length) of the request data.
*               (2) p_response_fncode     Pointer to the variable that receives the function code of the response.
*               (2) p_response_buffer     Pointer to the first element of the response data buffer.
*               (2) response_buffer_size  Size of the response data buffer.
*               (2) p_response_data_size  Pointer to the variable that receives the size of the response data.
*               (2) p_cmdlet_ctx          Pointer to the command-specific context.
*               (4) p_error               Pointer to the variable that receives error code from this function:
*
*                                             MB_ERROR_NONE                    No error occurred.
*                                             MB_ERROR_NULLREFERENCE           One of following conditions occurred:
*
*                                                                                  (1) 'p_request_data' is NULL while 'request_data_size' is not zero.
*                                                                                  (2) 'p_response_fncode' is NULL.
*                                                                                  (3) 'p_response_buffer' is NULL while 'response_buffer_size' is not zero.
*                                                                                  (4) 'p_response_data_size' is NULL.
*                                                                                  (5) 'p_cmdlet_ctx' is NULL.
*
*                                             MB_ERROR_SLAVE_REQUESTTRUNCATED  Request data is truncated.
*                                             MB_ERROR_SLAVE_RESPONSETRUNCATED Response data buffer is too small.
*                                             MB_ERROR_SLAVE_CALLBACKFAILED    Error occurred while calling callbacks.
*
* Return(s)   : None.
*
* Note(s)     : (1) This function is NOT thread(task)-safe.
*               (2) The 'p_cmdlet_ctx' pointer MUST point to a MBSLAVE_RWMULTIPLEREGISTERS_CTX object.
*********************************************************************************************************
*/

void MBSlave_CmdLet_ReadWriteMultipleRegisters(
    CPU_INT08U                        request_fncode,
    CPU_INT08U                       *p_request_data,
    CPU_SIZE_T                        request_data_size,
    CPU_INT08U                       *p_response_fncode,
    CPU_INT08U                       *p_response_buffer,
    CPU_SIZE_T                        response_buffer_size,
    CPU_SIZE_T                       *p_response_data_size,
    void                             *p_cmdlet_ctx,
    MB_ERROR                         *p_error
) {
    MBSLAVE_RWMULTIPLEREGISTERS_CTX  *cmdlet_ctx;

    MB_BUFFEREMITTER                  emitter;
    MB_BUFFERFETCHER                  fetcher;

    CPU_INT16U                        hregReadStartAddress;
    CPU_INT16U                        hregWriteStartAddress;
    CPU_INT16U                        hregEndAddress;
    CPU_INT16U                        hregCurrentAddress;
    CPU_INT16U                        hregReadQuantity;
    CPU_INT16U                        hregWriteQuantity;
    CPU_INT16U                        hregValue;

    CPU_INT08U                        inWriteByteCount;

    CPU_INT08U                        ec;

    CPU_BOOLEAN                       validity;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_request_data' parameter.  */
    if (
        (p_request_data == (CPU_INT08U*)0) && 
        (request_data_size != (CPU_SIZE_T)0U)
    ) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_response_fncode' parameter.  */
    if (p_response_fncode == (CPU_INT08U*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_response_buffer' parameter.  */
    if (
        (p_response_buffer == (CPU_INT08U*)0) && 
        (response_buffer_size != (CPU_SIZE_T)0U)
    ) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_response_data_size' parameter.  */
    if (p_response_data_size == (CPU_SIZE_T*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_cmdlet_ctx' parameter.  */
    if (p_cmdlet_ctx == (void*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Type cast.  */
    cmdlet_ctx = (MBSLAVE_RWMULTIPLEREGISTERS_CTX*)p_cmdlet_ctx;

    /*  Initialize the response data emitter.  */
    MBBufEmitter_Initialize(
        &(emitter),
        p_response_buffer,
        response_buffer_size,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Initialize the request data fetcher.  */
    MBBufFetcher_Initialize(
        &(fetcher),
        p_request_data,
        request_data_size,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Read the read start address from the request data.  */
    hregReadStartAddress = MBBufFetcher_ReadUInt16BE(
        &(fetcher),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        if (*p_error == MB_ERROR_BUFFETCHER_BUFFEREND) {
            *p_error = MB_ERROR_SLAVE_REQUESTTRUNCATED;
        }
        return;
    }

    /*  Read the register read quantity from the request data.  */
    hregReadQuantity = MBBufFetcher_ReadUInt16BE(
        &(fetcher),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        if (*p_error == MB_ERROR_BUFFETCHER_BUFFEREND) {
            *p_error = MB_ERROR_SLAVE_REQUESTTRUNCATED;
        }
        return;
    }

    /*  Read the write start address from the request data.  */
    hregWriteStartAddress = MBBufFetcher_ReadUInt16BE(
        &(fetcher),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        if (*p_error == MB_ERROR_BUFFETCHER_BUFFEREND) {
            *p_error = MB_ERROR_SLAVE_REQUESTTRUNCATED;
        }
        return;
    }

    /*  Read the register write quantity from the request data.  */
    hregWriteQuantity = MBBufFetcher_ReadUInt16BE(
        &(fetcher),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        if (*p_error == MB_ERROR_BUFFETCHER_BUFFEREND) {
            *p_error = MB_ERROR_SLAVE_REQUESTTRUNCATED;
        }
        return;
    }

    /*  Read the write byte count.  */
    inWriteByteCount = MBBufFetcher_ReadUInt8(
        &(fetcher),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        if (*p_error == MB_ERROR_BUFFETCHER_BUFFEREND) {
            *p_error = MB_ERROR_SLAVE_REQUESTTRUNCATED;
        }
        return;
    }

    /*  Validate the read register quantity.  */
    if (hregReadQuantity == (CPU_INT16U)0x0000U || hregReadQuantity > (CPU_INT16U)0x007DU) {
        goto MBSLAVE_RWHREG_CATCH_INVALIDVALUE;
    }

    /*  Validate the read start address.  */
    validity = cmdlet_ctx->cbValidateHoldingReg(
        hregReadStartAddress, 
        cmdlet_ctx->cbArg, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        *p_error = MB_ERROR_SLAVE_CALLBACKFAILED;
        return;
    }
    if (!validity) {
        goto MBSLAVE_RWHREG_CATCH_INVALIDADDR;
    }

    /*  Validate the read end address.  */
    hregEndAddress = (CPU_INT16U)(hregReadStartAddress + hregReadQuantity - (CPU_INT16U)1U);
    if (hregEndAddress < hregReadStartAddress) {
        /*  An overflow error detected.  */
        goto MBSLAVE_RWHREG_CATCH_INVALIDADDR;
    }
    validity = cmdlet_ctx->cbValidateHoldingReg(
        hregEndAddress, 
        cmdlet_ctx->cbArg, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        *p_error = MB_ERROR_SLAVE_CALLBACKFAILED;
        return;
    }
    if (!validity) {
        goto MBSLAVE_RWHREG_CATCH_INVALIDADDR;
    }


    /*  Validate the write register quantity.  */
    if (hregWriteQuantity == (CPU_INT16U)0x0000U || hregWriteQuantity > (CPU_INT16U)0x0079U) {
        goto MBSLAVE_RWHREG_CATCH_INVALIDVALUE;
    }

    /*  Validate the write start address.  */
    validity = cmdlet_ctx->cbValidateHoldingReg(
        hregWriteStartAddress, 
        cmdlet_ctx->cbArg, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        *p_error = MB_ERROR_SLAVE_CALLBACKFAILED;
        return;
    }
    if (!validity) {
        goto MBSLAVE_RWHREG_CATCH_INVALIDADDR;
    }

    /*  Validate the write end address.  */
    hregEndAddress = (CPU_INT16U)(hregWriteStartAddress + hregWriteQuantity - (CPU_INT16U)1U);
    if (hregEndAddress < hregWriteStartAddress) {
        /*  An overflow error detected.  */
        goto MBSLAVE_RWHREG_CATCH_INVALIDADDR;
    }
    validity = cmdlet_ctx->cbValidateHoldingReg(
        hregEndAddress, 
        cmdlet_ctx->cbArg, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        *p_error = MB_ERROR_SLAVE_CALLBACKFAILED;
        return;
    }
    if (!validity) {
        goto MBSLAVE_RWHREG_CATCH_INVALIDADDR;
    }

    /*  Validate the write byte count.  */
    if (inWriteByteCount != (CPU_INT08U)(hregWriteQuantity << ((CPU_INT16U)1U))) {
        goto MBSLAVE_RWHREG_CATCH_INVALIDVALUE;
    }

    /*  Write the count of output bytes.  */
    MBBufEmitter_WriteUInt8(
        &(emitter),
        (CPU_INT08U)(hregReadQuantity << (CPU_INT16U)1U),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        if (*p_error == MB_ERROR_BUFEMITTER_BUFFEREND) {
            *p_error = MB_ERROR_SLAVE_RESPONSETRUNCATED;
        }
        return;
    }

    /*  Process the write request.  */
    hregCurrentAddress = hregWriteStartAddress;
    while (hregWriteQuantity != (CPU_INT16U)0U) {
        /*  Fetch register value.  */
        hregValue = MBBufFetcher_ReadUInt16BE(
            &(fetcher),
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            if (*p_error == MB_ERROR_BUFFETCHER_BUFFEREND) {
                *p_error = MB_ERROR_SLAVE_REQUESTTRUNCATED;
            }
            return;
        }

        /*  Write register value.  */
        cmdlet_ctx->cbWriteHoldingReg(
            hregCurrentAddress,
            hregValue,
            cmdlet_ctx->cbArg,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            *p_error = MB_ERROR_SLAVE_CALLBACKFAILED;
            return;
        }

        /*  Decrease the write register quantity.  */              
        --(hregWriteQuantity);

        /*  Move to the next register.  */
        ++(hregCurrentAddress);
    }

    /*  Process the read request.  */
    hregCurrentAddress = hregReadStartAddress;
    while (hregReadQuantity != (CPU_INT16U)0U) {
        /*  Read register value.  */
        hregValue = cmdlet_ctx->cbReadHoldingReg(
            hregCurrentAddress,
            cmdlet_ctx->cbArg,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            *p_error = MB_ERROR_SLAVE_CALLBACKFAILED;
            return;
        }

        /*  Emit the register value.  */
        MBBufEmitter_WriteUInt16BE(
            &(emitter),
            hregValue,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            if (*p_error == MB_ERROR_BUFEMITTER_BUFFEREND) {
                *p_error = MB_ERROR_SLAVE_RESPONSETRUNCATED;
            }
            return;
        }

        /*  Decrease the register quantity.  */              
        --(hregReadQuantity);

        /*  Move to the next register.  */
        ++(hregCurrentAddress);
    }

    /*  Write the function code.  */
    *p_response_fncode = request_fncode;

    goto MBSLAVE_RWHREG_FINALLY;

MBSLAVE_RWHREG_CATCH_INVALIDVALUE:
    ec = MB_APUEC_ILLEGALDATAVALUE;
    goto MBSLAVE_RWHREG_CATCH_FINALLY;

MBSLAVE_RWHREG_CATCH_INVALIDADDR:
    ec = MB_APUEC_ILLEGALDATAADDRESS;

MBSLAVE_RWHREG_CATCH_FINALLY:
    /*  Write the function code.  */
    *p_response_fncode = (CPU_INT08U)(request_fncode + (CPU_INT08U)0x80U);

    /*  Discard emitted bytes.  */
    MBBufEmitter_Reset(
        &(emitter),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Write the exception code.  */
    MBBufEmitter_WriteUInt8(
        &(emitter),
        ec,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        if (*p_error == MB_ERROR_BUFEMITTER_BUFFEREND) {
            *p_error = MB_ERROR_SLAVE_RESPONSETRUNCATED;
        }
        return;
    }

MBSLAVE_RWHREG_FINALLY:
    /*  Get the length of the response data.  */
    *p_response_data_size = MBBufEmitter_GetWrittenLength(
        &(emitter),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        return;
    }
}

#endif  /*  #if (MB_CFG_SLAVE_EN == DEF_ENABLED) && (MB_CFG_SLAVE_BUILTIN_CMDLET_READWRITEMULTIPLEREGS == DEF_ENABLED)  */
