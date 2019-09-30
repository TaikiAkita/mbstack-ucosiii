/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_CORE.C
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
#define MB_CORE_SOURCE

#include <mb_core.h>
#include <mb_cfg.h>
#include <mb_constants.h>
#include <mb_framedec_ascii.h>
#include <mb_framedec_rtu.h>
#include <mb_frameenc_ascii.h>
#include <mb_frameenc_rtu.h>
#include <mb_types.h>

#include <mbdrv_types.h>

#include <mb_os_types.h>
#include <mb_os.h>

#include <mbport_limits.h>
#include <mbport_crc16.h>

#include <cpu.h>

#include <lib_ascii.h>
#include <lib_def.h>


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*  Modbus context event flags.  */
#define MBCTX_EVENT_RXCOMPLETE            ((MB_FLAGS)(1U))
#define MBCTX_EVENT_TXCOMPLETE            ((MB_FLAGS)(2U))
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
#define MBCTX_EVENT_1D5CTIMEEXCEED        ((MB_FLAGS)(4U))
#define MBCTX_EVENT_2D0CTIMEEXCEED        ((MB_FLAGS)(8U))
#define MBCTX_EVENT_3D5CTIMEEXCEED       ((MB_FLAGS)(16U))
#endif
#define MBCTX_EVENT_RXTIMEOUT            ((MB_FLAGS)(32U))

#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
/*  Modbus ASII-mode receiver states.  */
#define MBASCIIRXSTATE_WAITCOLON        ((CPU_INT08U)(0U))
#define MBASCIIRXSTATE_WAITLF           ((CPU_INT08U)(1U))
#define MBASCIIRXSTATE_WAITCR           ((CPU_INT08U)(2U))
#endif

/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

typedef struct {
    CPU_INT08U     rxInProgressCounter;
    CPU_INT08U     txInProgressCounter;

    CPU_INT08U     rxDatum;
    CPU_BOOLEAN    rxDatumEaten;

    MB_MUTEX       ioLock;

    MB_FLAGGROUP   evFlags;

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    CPU_INT32U     halfCharCounter;

    CPU_INT16U     halfCharTimerPrescaler;
    CPU_INT32U     halfCharCountCache_1D5;
    CPU_INT32U     halfCharCountCache_2D0;
    CPU_INT32U     halfCharCountCache_3D5;
#endif

    CPU_BOOLEAN    rxParityError;
    CPU_BOOLEAN    rxDataOverRunError;
    CPU_BOOLEAN    rxFrameError;

#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED)
    MB_COUNTERVALUE     cntParityError;
#endif
#if (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED)
    MB_COUNTERVALUE     cntDataOverRunError;
#endif
#if (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)
    MB_COUNTERVALUE     cntFrameError;
#endif

#if (MB_CFG_CORE_GETLASTTXADDRESS == DEF_ENABLED)
    CPU_INT08U     lastTxAddr;
#endif
#if (MB_CFG_CORE_GETLASTTXFUNCTIONCODE == DEF_ENABLED)
    CPU_INT08U     lastTxFnCode;
#endif
#if (MB_CFG_CORE_GETLASTTXEXCEPTIONCODE == DEF_ENABLED)
    CPU_INT08U     lastTxExCode;
#endif
} MB_CONTEXT;

typedef struct {
    CPU_BOOLEAN   initialized;
    MB_DRIVER    *driver;
    MB_CONTEXT    context;

    CPU_BOOLEAN   opened;

    MB_TRMODE     mode;

#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
    CPU_INT08U    asciiModeLF;
#endif
} MB_DEVICE;

typedef struct {
    MB_DEVICE  *device;
} MB_RXTIMEOUT_CBINFO;


/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static MB_DEVICE* MB_GetDevice(
    MB_IFINDEX   ifnbr,
    CPU_BOOLEAN  chk_init,
    CPU_BOOLEAN  chk_opened,
    CPU_BOOLEAN  chk_notopened,
    MB_ERROR    *p_error
);
static void MB_Context_Initialize(
    MB_CONTEXT  *ctx, 
    MB_ERROR    *p_error
);
static void MB_ISR_RxTimeoutExceed(
    void        *p_tmr, 
    void        *p_arg
);
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
static void MB_ISR_HalfCharacterTimeExceed(
    MB_DRIVER   *mbdrv, 
    void        *mbctx_
);
#endif
static void MB_ISR_RxComplete(
    MB_DRIVER   *mbdrv, 
    void        *mbctx_
);
static void MB_ISR_TxComplete(
    MB_DRIVER   *mbdrv, 
    void        *mbctx_
);


/*
*********************************************************************************************************
*                                      LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/*  Modbus driver callbacks.  */
static MB_DRIVER_CALLBACKS g_ModbusDriverCallbacks = {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    .halfCharacterTimeExceed  = MB_ISR_HalfCharacterTimeExceed,
#endif
    .rxComplete               = MB_ISR_RxComplete,
    .txComplete               = MB_ISR_TxComplete
};

/*  Modbus devices.  */
static MB_DEVICE g_ModbusDevices[(CPU_SIZE_T)MB_CFG_MAX_NBR_IF];


/*
*********************************************************************************************************
*                                    MB_Initialize()
*
* Description : Initialize the Modbus module.
*
* Argument(s) : (1) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE       No error occurred.
*
* Return(s)   : None.
*
* Note(s)     : (1) You must call this function before using all other functions of the Modbus communication 
*                   module.
*               (2) You must call this function after OS and CPU initialized.
*********************************************************************************************************
*/

void MB_Initialize(
    MB_ERROR  *p_error
) {
    MB_IFINDEX ifidx;
    MB_DEVICE *ifdev;

    CPU_SR_ALLOC();

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Clear the device table.  */
    for (ifidx = (MB_IFINDEX)0U; ifidx < (MB_IFINDEX)MB_CFG_MAX_NBR_IF; ++ifidx) {
        ifdev               = &(g_ModbusDevices[(CPU_SIZE_T)ifidx]);
        ifdev->driver       = (MB_DRIVER*)0;
        ifdev->initialized  = DEF_NO;
        ifdev->opened       = DEF_NO;
        ifdev->mode         = (MB_TRMODE) 0U;
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
        ifdev->asciiModeLF  = (CPU_INT08U)ASCII_CHAR_LINE_FEED;
#endif
    }

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Initialize port module.  */
    MBPort_CRC16_InitializeTable();
#endif

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MB_RegisterDevice()
*
* Description : Register the Modbus device.
*
* Argument(s) : (1) p_drv     Pointer to the device driver.
*               (2) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_drv' is NULL.
*                                 MB_ERROR_DEVICEEXISTED           Device already exists.
*                                 MB_ERROR_DEVICENOFREE            No free device entry that can be used.
*                                 MB_ERROR_OS_MUTEX_FAILEDCREATE   Unable to create a mutex object.
*                                 MB_ERROR_OS_FGRP_FAILEDCREATE    Unable to create a flag group object.
*
* Return(s)   : The Modbus device interface ID.
*********************************************************************************************************
*/

MB_IFINDEX MB_RegisterDevice(
    MB_DRIVER *p_drv,
    MB_ERROR  *p_error
) {
    MB_IFINDEX   ifidx;
    CPU_BOOLEAN  ifalloc;
    MB_IFINDEX   ifallocidx;
    MB_DEVICE   *ifdev;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_drv' parameter.  */
    if (p_drv == (MB_DRIVER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT08U)0U;
    }
#endif

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Allocate a device ID.  */
    ifalloc = DEF_NO;
    ifallocidx = (MB_IFINDEX)0U;
    for (ifidx = (MB_IFINDEX)0U; ifidx < (MB_IFINDEX)MB_CFG_MAX_NBR_IF; ++ifidx) {
        ifdev = &(g_ModbusDevices[(CPU_SIZE_T)ifidx]);
        if (ifdev->initialized) {
            if (ifdev->driver == p_drv) {
                *p_error = MB_ERROR_DEVICEEXISTED;
                goto MBREGDEV_FAIL;
            }
        } else {
            if (!ifalloc) {
                ifalloc = DEF_YES;
                ifallocidx = ifidx;
            }
        }
    }

    /*  Throw an error if unable to allocate.  */
    if (!ifalloc) {
        *p_error = MB_ERROR_DEVICENOFREE;
        goto MBREGDEV_FAIL;
    }

    /*  Get the pointer of the device.  */
    ifdev = &(g_ModbusDevices[ifallocidx]);

    /*  Initialize the driver.  */
    p_drv->initialize(
        &(ifdev->context),
        &g_ModbusDriverCallbacks,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        /*  Error: Driver initialization failed.  */
        goto MBREGDEV_FAIL;
    }

    /*  Initialize half-duplex direction.  */
    p_drv->halfDuplexModeSetup(MB_HALFDUPLEX_RECEIVE, p_error);
    if (*p_error != MB_ERROR_NONE) {
        /*  Error: Driver initialization failed.  */
        goto MBREGDEV_FAIL;
    }

    /*  Save the driver.  */
    ifdev->driver = p_drv;

    /*  Initialize the Modbus context.  */
    MB_Context_Initialize(
        &(ifdev->context),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        /*  Error: Context initialization failed.  */
        goto MBREGDEV_FAIL;
    }

    /*  Mark the device as initialized.  */
    ifdev->initialized = DEF_YES;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    return ifallocidx;

MBREGDEV_FAIL:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    return (MB_IFINDEX)0U;
}


/*
*********************************************************************************************************
*                                    MB_OpenDevice()
*
* Description : Open a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) mode           Modbus transmission mode, one of following:
*
*                                      MB_TRMODE_RTU                RTU transmission mode.
*                                      MB_TRMODE_ASCI               ASCII transmission mode.
*
*               (3) p_serialsetup  Pointer to the Modbus serial configuration.
*               (4) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_NULLREFERENCE       'p_serialsetup' is NULL.
*                                      MB_ERROR_INVALIDPARAMETER    'mode' or 'p_serialsetup' contains invalid device configuration.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICEOPENED        Device is already opened.
*                                      MB_ERROR_DEVICEFAIL          Device failed to be opened.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void  MB_OpenDevice(
    MB_IFINDEX        ifnbr,
    MB_TRMODE         mode,
    MB_SERIAL_SETUP  *p_serialsetup,
    MB_ERROR         *p_error
) {
    MB_DEVICE   *ifdev;
    MB_DRIVER   *ifdrv;
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    MB_CONTEXT  *ctx;
#endif

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'mode' parameter.  */
    switch (mode) {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
        case MB_TRMODE_RTU:
            break;
#endif
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
        case MB_TRMODE_ASCII:
            break;
#endif
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return;
    }

    /*  Check 'p_serialsetup'.  */
    if (p_serialsetup == (MB_SERIAL_SETUP*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Check 'p_serialsetup->dataBits'.  */
    if (mode == MB_TRMODE_RTU) {
        if (p_serialsetup->dataBits != MB_SERIAL_DATABITS_8) {
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return;
        }
    }
#endif
#endif

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and not opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_NO, 
        DEF_YES, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBOPENDEV_EXIT;
    }

    /*  Get the driver.  */
    ifdrv = ifdev->driver;

    /*  Open the device.  */
    ifdrv->deviceOpen(p_serialsetup, p_error);
    if (*p_error != MB_ERROR_NONE) {
        goto MBOPENDEV_EXIT;
    }

    /*  Clear RX errors.  */
    ifdrv->clearParityError();
    ifdrv->clearDataOverRunError();
    ifdrv->clearFrameError();

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Reset half character timer prescale.  */
    ctx->halfCharTimerPrescaler = (CPU_INT16U)1U;

    /*  Reset half character counter.  */
    ctx->halfCharCounter        = (CPU_INT32U)0U;

    /*  Reset half character counter caches.  */
    ctx->halfCharCountCache_1D5 = (CPU_INT32U)3U;
    ctx->halfCharCountCache_2D0 = (CPU_INT32U)4U;
    ctx->halfCharCountCache_3D5 = (CPU_INT32U)7U;
#endif

#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
    /*  Reset the ASCII-mode line feed character.  */
    ifdev->asciiModeLF = (CPU_INT08U)ASCII_CHAR_LINE_FEED;
#endif

    /*  Save the transmission mode.  */
    ifdev->mode = mode;

    /*  Mark the device as opened.  */
    ifdev->opened = DEF_YES;

MBOPENDEV_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
}


/*
*********************************************************************************************************
*                                    MB_CloseDevice()
*
* Description : Close a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEBUSY          Device is still busy.
*                                      MB_ERROR_DEVICEFAIL          Device failed to be closed.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MB_CloseDevice(
    MB_IFINDEX  ifnbr,
    MB_ERROR   *p_error
) {
    MB_DEVICE  *ifdev;
    MB_DRIVER  *ifdrv;
    MB_CONTEXT *ctx;

    CPU_SR_ALLOC();

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBCLOSEDEV_EXIT;
    }

    /*  Get the driver.  */
    ifdrv = ifdev->driver;

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Throw an error if the device is still in use.  */
    if (
        (ctx->rxInProgressCounter != (CPU_INT08U)0U) || 
        (ctx->txInProgressCounter != (CPU_INT08U)0U)
    ) {
        *p_error = MB_ERROR_DEVICEBUSY;
        goto MBCLOSEDEV_EXIT;
    }

    /*  Close the device.  */
    ifdrv->deviceClose(p_error);
    if (*p_error != MB_ERROR_NONE) {
        goto MBCLOSEDEV_EXIT;
    }

    /*  Mark the device as not opened.  */
    ifdev->opened = DEF_NO;

MBCLOSEDEV_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
}


#if (MB_CFG_CORE_SETMODE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_SetMode()
*
* Description : Set the transmission of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) mode           New transmission mode.
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_INVALIDPARAMETER    'mode' is not valid.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEBUSY          Device is still busy.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MB_SetMode(
    MB_IFINDEX   ifnbr,
    MB_TRMODE    mode,
    MB_ERROR    *p_error
) {
    MB_DEVICE   *ifdev;

    MB_CONTEXT  *ctx;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'mode' parameter.  */
    switch (mode) {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
        case MB_TRMODE_RTU:
            break;
#endif
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
        case MB_TRMODE_ASCII:
            break;
#endif
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return;
    }
#endif

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES,
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBSETMODE_EXIT;
    }

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Check whether there is unfinished I/O operation.  */
    if (
        (ctx->rxInProgressCounter != (CPU_INT08U)0U) || 
        (ctx->txInProgressCounter != (CPU_INT08U)0U)
    ) {
        *p_error = MB_ERROR_DEVICEBUSY;
        goto MBSETMODE_EXIT;
    }

    /*  Set the transmission mode.  */
    ifdev->mode = mode;

MBSETMODE_EXIT:
    /*  Exit critical section.*/
    CPU_CRITICAL_EXIT();
}
#endif  /*  #if (MB_CFG_CORE_SETMODE_EN == DEF_ENABLED)  */


#if (MB_CFG_CORE_GETMODE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetMode()
*
* Description : Get the transmission mode of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*
* Return(s)   : The transmission mode.
*********************************************************************************************************
*/

MB_TRMODE MB_GetMode(
    MB_IFINDEX   ifnbr,
    MB_ERROR    *p_error
) {
    MB_DEVICE   *ifdev;

    MB_TRMODE    mode;

    CPU_SR_ALLOC();

    /*  Initialize local variables.  */
    mode = (MB_TRMODE)0U;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBGETMODE_EXIT;
    }

    /*  Get the transmission mode.  */
    mode = ifdev->mode;

MBGETMODE_EXIT:
    /*  Exit critical section.*/
    CPU_CRITICAL_EXIT();

    return mode;
}
#endif  /*  #if (MB_CFG_CORE_GETMODE_EN == DEF_ENABLED)  */


#if (MB_CFG_CORE_SETLINEFEED_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_SetLineFeed()
*
* Description : Set the line-feed character of a Modbus device (for ASCII transmission mode only).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) lf             Line-feed character.
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_INVALIDPARAMETER    'lf' is not an ASCII character.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEMODEMISMATCH  Device is not in ASCII transmission mode.
*                                      MB_ERROR_DEVICEBUSY          Device is still busy.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MB_SetLineFeed(
    MB_IFINDEX   ifnbr,
    CPU_CHAR     lf,
    MB_ERROR    *p_error
) {
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
    MB_DEVICE   *ifdev;

    MB_CONTEXT  *ctx;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'lf' parameter.  */
    if (lf < (CPU_CHAR)0) {
        *p_error = MB_ERROR_INVALIDPARAMETER;
        return;
    }
#endif

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES,
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBSETLF_EXIT;
    }

    /*  The line-feed character is only used in ASCII mode.  */
    if (ifdev->mode != MB_TRMODE_ASCII) {
        *p_error = MB_ERROR_DEVICEMODEMISMATCH;
        goto MBSETLF_EXIT;
    }

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Check whether there is unfinished I/O operation.  */
    if (
        (ctx->rxInProgressCounter != (CPU_INT08U)0U) || 
        (ctx->txInProgressCounter != (CPU_INT08U)0U)
    ) {
        *p_error = MB_ERROR_DEVICEBUSY;
        goto MBSETLF_EXIT;
    }

    /*  Set the line-feed character.  */
    ifdev->asciiModeLF = (CPU_INT08U)lf;

MBSETLF_EXIT:
    /*  Exit critical section.*/
    CPU_CRITICAL_EXIT();
#else
    /*  Unused parameters.  */
    (void)ifnbr;
    (void)lf;

    /*  Error: Device transmission mode mismatched.*/
    *p_error = MB_ERROR_DEVICEMODEMISMATCH;
#endif
}
#endif  /*  #if (MB_CFG_CORE_SETLINEFEED_EN == DEF_ENABLED)  */


#if (MB_CFG_CORE_GETLINEFEED_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetLineFeed()
*
* Description : Get the line-feed character of a Modbus device (for ASCII transmission mode only).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEMODEMISMATCH  Device is not in ASCII transmission mode.
*
* Return(s)   : The line-feed character.
*********************************************************************************************************
*/

CPU_CHAR MB_GetLineFeed(
    MB_IFINDEX   ifnbr,
    MB_ERROR    *p_error
) {
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
    MB_DEVICE   *ifdev;

    CPU_INT08U   lf;

    CPU_SR_ALLOC();

    /*  Initialize local variables.  */
    lf = (CPU_INT08U)0U;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBGETLF_EXIT;
    }

    /*  The line-feed character is only used in ASCII mode.  */
    if (ifdev->mode != MB_TRMODE_ASCII) {
        *p_error = MB_ERROR_DEVICEMODEMISMATCH;
        goto MBGETLF_EXIT;
    }

    /*  Get the line-feed character.  */
    lf = ifdev->asciiModeLF;

MBGETLF_EXIT:
    /*  Exit critical section.*/
    CPU_CRITICAL_EXIT();

    return (CPU_CHAR)lf;
#else
    /*  Unused parameters.  */
    (void)ifnbr;

    /*  Error: Device transmission mode mismatched.  */
    *p_error = MB_ERROR_DEVICEMODEMISMATCH;

    return (CPU_CHAR)0;
#endif
}
#endif  /*  #if (MB_CFG_CORE_GETLINEFEED_EN == DEF_ENABLED)  */


#if (MB_CFG_CORE_SETCHARTIMEPRESCALE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_SetCharTimePrescale()
*
* Description : Set the prescale of character time of a Modbus device (for RTU transmission mode only).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) prescale       Time prescale.
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEMODEMISMATCH  Device is not in RTU transmission mode.
*                                      MB_ERROR_DEVICEBUSY          Device is still busy.
*                                      MB_ERROR_UNDERFLOW           'prescale' equals to zero.
*
* Return(s)   : None.
*
* Note(s)     : (1) The new 1.5/3.5 character time can be calculated with following formula:
*
*                      (t1.5)new = (t1.5)default * prescale
*                      (t3.5)new = (t3.5)default * prescale
*
*               (2) The prescale is set to 1 by default.
*********************************************************************************************************
*/

void MB_SetCharTimePrescale(
    MB_IFINDEX   ifnbr,
    CPU_INT16U   prescale,
    MB_ERROR    *p_error
) {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    MB_DEVICE   *ifdev;

    MB_CONTEXT  *ctx;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'prescale'.  */
    if (prescale == (CPU_INT16U)0U) {
        *p_error = MB_ERROR_UNDERFLOW;
        return;
    }
#endif

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES,
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBSETHCTPRESCALE_EXIT;
    }

    /*  The line-feed character is only used in RTU mode.  */
    if (ifdev->mode != MB_TRMODE_RTU) {
        *p_error = MB_ERROR_DEVICEMODEMISMATCH;
        goto MBSETHCTPRESCALE_EXIT;
    }

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Check whether there is unfinished I/O operation.  */
    if (
        (ctx->rxInProgressCounter != (CPU_INT08U)0U) || 
        (ctx->txInProgressCounter != (CPU_INT08U)0U)
    ) {
        *p_error = MB_ERROR_DEVICEBUSY;
        goto MBSETHCTPRESCALE_EXIT;
    }

    /*  Set the prescale.  */
    ctx->halfCharTimerPrescaler = prescale;
    ctx->halfCharCountCache_1D5 = (CPU_INT32U)(((CPU_INT32U)3U) * ((CPU_INT32U)prescale));
    ctx->halfCharCountCache_2D0 = (CPU_INT32U)(((CPU_INT32U)4U) * ((CPU_INT32U)prescale));
    ctx->halfCharCountCache_3D5 = (CPU_INT32U)(((CPU_INT32U)7U) * ((CPU_INT32U)prescale));

MBSETHCTPRESCALE_EXIT:
    /*  Exit critical section.*/
    CPU_CRITICAL_EXIT();
#else
    /*  Error: Device transmission mode mismatched.  */
    *p_error = MB_ERROR_DEVICEMODEMISMATCH;
#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */
}
#endif  /*  #if (MB_CFG_CORE_SETCHARTIMEPRESCALE_EN == DEF_ENABLED)  */


#if (MB_CFG_CORE_GETCHARTIMEPRESCALE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetCharTimePrescale()
*
* Description : Get the prescale of character time of a Modbus device (for RTU transmission mode only).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST      'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER   Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED     Device is not opened.
*                                      MB_ERROR_DEVICEMODEMISMATCH  Device is not in RTU transmission mode.
*
* Return(s)   : The prescale.
*********************************************************************************************************
*/

CPU_INT16U MB_GetCharTimePrescale(
    MB_IFINDEX   ifnbr,
    MB_ERROR    *p_error
) {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    MB_DEVICE   *ifdev;

    CPU_INT16U   prescale;

    CPU_SR_ALLOC();

    /*  Initialize local variables.  */
    prescale = (CPU_INT16U)0U;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBGETHCTPRESCALE_EXIT;
    }

    /*  The line-feed character is only used in RTU mode.  */
    if (ifdev->mode != MB_TRMODE_RTU) {
        *p_error = MB_ERROR_DEVICEMODEMISMATCH;
        goto MBGETHCTPRESCALE_EXIT;
    }

    /*  Get the prescale.  */
    prescale = ifdev->context.halfCharTimerPrescaler;

MBGETHCTPRESCALE_EXIT:
    /*  Exit critical section.*/
    CPU_CRITICAL_EXIT();

    return prescale;
#else
    /*  Error: Device transmission mode mismatched.  */
    *p_error = MB_ERROR_DEVICEMODEMISMATCH;

    return (CPU_INT16U)0U;
#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */
}
#endif  /*  #if (MB_CFG_CORE_GETCHARTIMEPRESCALE_EN == DEF_ENABLED)  */


/*
*********************************************************************************************************
*                                    MB_WaitStartup()
*
* Description : Do startup wait.
*
*               [RTU transmission mode]:
*                   Wait for the serial line to be idle for 3.5 character time.
*
*               [ASCII transmission mode]:
*                   No delay (return immediately).
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) timeout        Timeout value (unit: milliseconds, set to 0 to wait infinitely).
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                   No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST         'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER      Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED        Device is not opened.
*                                      MB_ERROR_DEVICEFAIL             Device operation failed.
*                                      MB_ERROR_OVERFLOW               'timeout' parameter exceeds maximum allowed value.
*                                      MB_ERROR_TIMEOUT                Timeout limit exceeds.
*                                      MB_ERROR_RXTOOMANY              Too many receive requests.
*                                      MB_ERROR_OS_FGRP_FAILEDPEND     Failed to pend on a flag group object.
*                                      MB_ERROR_OS_FGRP_FAILEDPOST     Failed to post to a flag group object.
*                                      MB_ERROR_OS_MUTEX_FAILEDPEND    Failed to pend on a mutex object.
*                                      MB_ERROR_OS_TIMER_FAILEDCREATE  Failed to create timer object.
*                                      MB_ERROR_OS_TIMER_FAILEDSTART   Failed to start a timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) The value of 'timeout' parameter should be within following range:
*
*                       (0, MBOS_GetMaxTimeValue()]
*
*               (2) The function would be blocked (without respecting the 'timeout' parameter) until previous I/O finished.
*********************************************************************************************************
*/

void MB_WaitStartup(
    MB_IFINDEX             ifnbr,
    MB_TIMESPAN            timeout,
    MB_ERROR              *p_error
) {
    MB_ERROR               error;

    MB_DEVICE             *ifdev;
    MB_DRIVER             *ifdrv;

    MB_CONTEXT            *ctx;

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    MB_RXTIMEOUT_CBINFO    rxTimeoutCbInfo;
#endif

    MB_TIMER               timeouter;

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    MB_FLAGS               fgrpFlags;
#endif

    struct {
        CPU_BOOLEAN        clrIoLock:1;
        CPU_BOOLEAN        clrRxReceiver:1;
        CPU_BOOLEAN        clrRxHalfCharTmr:1;
        CPU_BOOLEAN        clrRxTimeoutTmr:1;
        CPU_BOOLEAN        clrRxRequestCnt:1;
        CPU_BOOLEAN        clrCriticalSect:1;
        CPU_INT08U         __padding:2;
    } gc;

    CPU_SR_ALLOC();

    /*  Avoid 'unused-but-set-variable' warning.  */
    (void)ifdev;

    /*  Initialize local variables.  */
    ctx                  = (MB_CONTEXT*)0;
    ifdev                = (MB_DEVICE*)0;
    ifdrv                = (MB_DRIVER*)0;
    gc.clrIoLock         = DEF_NO;
    gc.clrRxReceiver     = DEF_NO;
    gc.clrRxHalfCharTmr  = DEF_NO;
    gc.clrRxTimeoutTmr   = DEF_NO;
    gc.clrRxRequestCnt   = DEF_NO;
    gc.clrCriticalSect   = DEF_NO;
    error                = MB_ERROR_NONE;

    /*  No error by default.  */
    *p_error             = MB_ERROR_NONE;

    /*
     *  Stage 1: Get the device, its driver and acquire the I/O lock.
     */

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();
    gc.clrCriticalSect   = DEF_YES;

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBWAITSTARTUP_EXIT;
    }

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  The startup delay (t3.5) is only needed in RTU mode.  */
    if (ifdev->mode == MB_TRMODE_RTU) {
        /*  Get the driver.  */
        ifdrv = ifdev->driver;

        /*  Get the Modbus context.  */
        ctx = &(ifdev->context);

        /*  Increase the RX inprogress request counter.  */
        if (ctx->rxInProgressCounter == MBPORT_UINT08_MAX) {
            /*  Error: Too many RX requests.  */
            *p_error = MB_ERROR_RXTOOMANY;

            goto MBWAITSTARTUP_EXIT;
        }
        ++(ctx->rxInProgressCounter);
        gc.clrRxRequestCnt = DEF_YES;

        /*  Exit critical section.  */
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;

        /*  Acquire the I/O lock.  */
        MBOS_MutexPend(
            &(ctx->ioLock),
            (MB_TIMESPAN)0U,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            goto MBWAITSTARTUP_EXIT;
        }
        gc.clrIoLock = DEF_YES;

        /*  Enter critical section.  */
        CPU_CRITICAL_ENTER();
        gc.clrCriticalSect = DEF_YES;

        /*
        *  Stage 2: Wait for a 3.5 character idle time.
        */

        /*  Clear the timeout exceeds bit.  */
        MBOS_FlagGroupPost(
            &(ctx->evFlags),
            MBCTX_EVENT_RXTIMEOUT,
            MB_FLAGGROUP_OPT_CLR,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            goto MBWAITSTARTUP_EXIT;
        }

        /*  Create the timeout timer.  */
        if (timeout != (MB_TIMESPAN)0U) {
            rxTimeoutCbInfo.device = ifdev;
            MBOS_TimerCreate(
                &timeouter,
                timeout,
                MB_TIMER_MODE_ONESHOT,
                MB_ISR_RxTimeoutExceed,
                (void*)(&rxTimeoutCbInfo),
                p_error
            );
            if (*p_error != MB_ERROR_NONE) {
                goto MBWAITSTARTUP_EXIT;
            }
            gc.clrRxTimeoutTmr = DEF_YES;
            MBOS_TimerStart(
                &timeouter,
                p_error
            );
            if (*p_error != MB_ERROR_NONE) {
                goto MBWAITSTARTUP_EXIT;
            }
        }

        /*  Clear RX datum.  */
        ctx->rxDatum = (CPU_INT08U)0U;
        ctx->rxDatumEaten = DEF_YES;

        /*  Start the receiver.  */
        ifdrv->rxStart(p_error);
        if (*p_error != MB_ERROR_NONE) {
            goto MBWAITSTARTUP_EXIT;
        }
        gc.clrRxReceiver = DEF_YES;

        while (DEF_YES) {
            /*  Reset the events.  */
            MBOS_FlagGroupPost(
                &(ctx->evFlags),
                (
                    MBCTX_EVENT_1D5CTIMEEXCEED | 
                    MBCTX_EVENT_2D0CTIMEEXCEED | 
                    MBCTX_EVENT_3D5CTIMEEXCEED | 
                    MBCTX_EVENT_RXCOMPLETE
                ),
                MB_FLAGGROUP_OPT_CLR,
                p_error
            );
            if (*p_error != MB_ERROR_NONE) {
                goto MBWAITSTARTUP_EXIT;
            }

            /*  Reset the half-character counter.  */
            ctx->halfCharCounter = (CPU_INT32U)0U;

            /*  Start the half-character timer.  */
            ifdrv->halfCharacterTimerStart(p_error);
            if (*p_error != MB_ERROR_NONE) {
                goto MBWAITSTARTUP_EXIT;
            }
            gc.clrRxHalfCharTmr = DEF_YES;

            /*  Exit critical section.  */
            CPU_CRITICAL_EXIT();
            gc.clrCriticalSect = DEF_NO;

            /*  Wait for events.  */
            fgrpFlags = (MB_FLAGS)(
                MBCTX_EVENT_3D5CTIMEEXCEED | 
                MBCTX_EVENT_RXCOMPLETE | 
                MBCTX_EVENT_RXTIMEOUT
            );
            MBOS_FlagGroupPend(
                &(ctx->evFlags),
                &fgrpFlags,
                (MB_TIMESPAN)0U,
                MB_FLAGGROUP_OPT_SET_ANY,
                p_error
            );
            if (*p_error != MB_ERROR_NONE) {
                goto MBWAITSTARTUP_EXIT;
            }

            /*  Enter critical section.  */
            CPU_CRITICAL_ENTER();
            gc.clrCriticalSect = DEF_YES;

            /*  Stop the half-character timer.  */
            ifdrv->halfCharacterTimerStop(p_error);
            if (*p_error != MB_ERROR_NONE) {
                goto MBWAITSTARTUP_EXIT;
            }
            gc.clrRxHalfCharTmr = DEF_NO;

            /*  Check events.  */
            if ((fgrpFlags & MBCTX_EVENT_RXCOMPLETE) != (MB_FLAGS)0) {
                /*  Received a character, keep waiting.  */
                continue;
            } else if ((fgrpFlags & MBCTX_EVENT_3D5CTIMEEXCEED) != (MB_FLAGS)0) {
                /*  3.5 character time exceeds without receiving any character, OK now.  */
                break;
            } else if ((fgrpFlags & MBCTX_EVENT_RXTIMEOUT) != (MB_FLAGS)0) {
                /*  RX timeout exceeds.  */
                *p_error = MB_ERROR_TIMEOUT;

                goto MBWAITSTARTUP_EXIT;
            } else {
                /*  Unknown event.  */
                continue;
            }
        }

        /*  Stop the receiver.  */
        ifdrv->rxStop(p_error);
        if (*p_error != MB_ERROR_NONE) {
            goto MBWAITSTARTUP_EXIT;
        }
        gc.clrRxReceiver = DEF_NO;
    }
#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */

MBWAITSTARTUP_EXIT:
    /*
     *  Stage 3: Release used resources.
     */

    /*  Stop the receiver (if needed).  */
    if (gc.clrRxReceiver) {
        if (ifdrv != (MB_DRIVER*)0) {
            ifdrv->rxStop(&error);
        }
        gc.clrRxReceiver = DEF_NO;
    }

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Stop the half-character timer (if needed).  */
    if (gc.clrRxHalfCharTmr) {
        if (ifdrv != (MB_DRIVER*)0) {
            ifdrv->halfCharacterTimerStop(&error);
        }
        gc.clrRxHalfCharTmr = DEF_NO;
    }
#endif

    /*  Dispose the RX timeout timer (if needed).  */
    if (gc.clrRxTimeoutTmr) {
        MBOS_TimerDispose(
            &timeouter,
            &error
        );
        gc.clrRxTimeoutTmr = DEF_NO;
    }

    /*  Decrease the RX request counter (if needed).  */
    if (gc.clrRxRequestCnt) {
        if (ctx != (MB_CONTEXT*)0) {
            --(ctx->rxInProgressCounter);
        }
        gc.clrRxRequestCnt = DEF_NO;
    }

    /*  Release the I/O lock (if needed).  */
    if (gc.clrIoLock) {
        if (ctx != (MB_CONTEXT*)0) {
            MBOS_MutexPost(
                &(ctx->ioLock),
                &error
            );
        }
        gc.clrIoLock = DEF_NO;
    }

    /*  Exit critical section (if needed).  */
    if (gc.clrCriticalSect) {
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;
    }
}


/*
*********************************************************************************************************
*                                    MB_ReceiveFrame()
*
* Description : Receive a Modbus frame from a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_buffer       Pointer to the first element of the data buffer.
*               (3) buffer_size    Size of the data buffer.
*               (4) p_frame        Pointer to the variable that receives the frame.
*               (5) p_frameflags   Pointer to the variable that receives the frame flags (optional, NULL if not needed):
*
*                                      MB_FRAMEFLAGS_DROP                Frame should be dropped.
*                                      MB_FRAMEFLAGS_BUFFEROVERFLOW      Data buffer is too small to contain 
*                                                                        the frame data.
*                                      MB_FRAMEFLAGS_CHECKSUMMISMATCH    CRC/LRC checksum mismatched.
*                                      MB_FRAMEFLAGS_TRUNCATED           Frame is truncated.
*                                      MB_FRAMEFLAGS_PARITYERROR         Parity error occurred.
*                                      MB_FRAMEFLAGS_OVERRUNERROR        Data overrun error occurred.
*                                      MB_FRAMEFLAGS_FRAMEERROR          Frame error occurred.
*                                      MB_FRAMEFLAGS_REDUNDANTBYTE       (RTU) One or more byte received after 1.5 character time exceeds.
*                                      MB_FRAMEFLAGS_INVALIDBYTE         (ASCII) One or more non-ASCII or invalid ASCII 
*                                                                        character(s) was/were received.
*
*               (6) timeout        Timeout value (unit: milliseconds, set to 0 to wait infinitely).
*               (7) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                   No error occurred.
*                                      MB_ERROR_NULLREFERENCE          'p_frame' is NULL or 'p_buffer' is NULL while buffer_size is not zero.
*                                      MB_ERROR_INVALIDMODE            Device transmission mode is not supported.
*                                      MB_ERROR_DEVICENOTEXIST         'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER      Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED        Device is not opened.
*                                      MB_ERROR_DEVICEFAIL             Device operation failed.
*                                      MB_ERROR_OVERFLOW               'timeout' parameter exceeds maximum allowed value.
*                                      MB_ERROR_TIMEOUT                Timeout limit exceeds.
*                                      MB_ERROR_RXTOOMANY              Too many receive requests.
*                                      MB_ERROR_OS_FGRP_FAILEDPEND     Failed to pend on a flag group object.
*                                      MB_ERROR_OS_FGRP_FAILEDPOST     Failed to post to a flag group object.
*                                      MB_ERROR_OS_MUTEX_FAILEDPEND    Failed to pend on a mutex object.
*                                      MB_ERROR_OS_TIMER_FAILEDCREATE  Failed to create timer object.
*                                      MB_ERROR_OS_TIMER_FAILEDSTART   Failed to start a timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) The value of 'timeout' parameter should be within following range:
*
*                       (0, MBOS_GetMaxTimeValue()]
*
*               (2) The 'timeout' parameter only affects the time of waiting a frame on the serial line.
*               (3) The function would be blocked (without respecting the 'timeout' parameter) until previous I/O finished.
*********************************************************************************************************
*/

void MB_ReceiveFrame(
    MB_IFINDEX             ifnbr,
    CPU_INT08U            *p_buffer,
    CPU_SIZE_T             buffer_size,
    MB_FRAME              *p_frame,
    MB_FRAMEFLAGS         *p_frameflags,
    MB_TIMESPAN            timeout,
    MB_ERROR              *p_error
) {
    MB_ERROR               error;

    MB_DEVICE             *ifdev;
    MB_DRIVER             *ifdrv;

    MB_CONTEXT            *ctx;

    MB_RXTIMEOUT_CBINFO    rxTimeoutCbInfo;

    MB_TIMER               timeouter;

    MB_FLAGS               fgrpFlags;

    struct {
        CPU_BOOLEAN        clrIoLock:1;
        CPU_BOOLEAN        clrRxReceiver:1;
        CPU_BOOLEAN        clrRxHalfCharTmr:1;
        CPU_BOOLEAN        clrRxTimeoutTmr:1;
        CPU_BOOLEAN        clrRxRequestCnt:1;
        CPU_BOOLEAN        clrCriticalSect:1;
        CPU_INT08U         __padding:2;
    } gc;

    union {
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
        struct {
            CPU_INT08U     rxState;
            CPU_INT08U     rxDatumTmp;
            CPU_BOOLEAN    rxResetDecoder;
        } asciiMode;
#endif
        struct {
            CPU_BOOLEAN    rtuFirstChar;
        } rtuMode;
    } msv  /*  MSV. = Mode Specific Variables.  */;

    union {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
        MB_FRAMEDEC_RTU    rtuDecoder;
#endif
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
        MB_FRAMEDEC_ASCII  asciiDecoder;
#endif
    } decoder;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_frame' parameter.  */
    if (p_frame == (MB_FRAME*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_buffer' parameter.  */
    if ((p_buffer == (CPU_INT08U*)0) && (buffer_size != (CPU_SIZE_T)0U)) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Avoid 'unused-variable' warning.  */
    (void)decoder;
    (void)msv;
    (void)fgrpFlags;

    /*  Initialize local variables.  */
    ifdev                = (MB_DEVICE*)0;
    ifdrv                = (MB_DRIVER*)0;
    ctx                  = (MB_CONTEXT*)0;
    gc.clrIoLock         = DEF_NO;
    gc.clrRxReceiver     = DEF_NO;
    gc.clrRxHalfCharTmr  = DEF_NO;
    gc.clrRxTimeoutTmr   = DEF_NO;
    gc.clrRxRequestCnt   = DEF_NO;
    gc.clrCriticalSect   = DEF_NO;
    error                = MB_ERROR_NONE;

    /*  No error by default.  */
    *p_error             = MB_ERROR_NONE;

    /*  Clear frame flags.  */
    *p_frameflags        = (MB_FRAMEFLAGS)0U;

    /*
     *  Stage 1: Get the device, its driver and acquire the I/O lock.
     */

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();
    gc.clrCriticalSect = DEF_YES;

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(ifnbr, DEF_YES, DEF_YES, DEF_NO, p_error);
    if (*p_error != MB_ERROR_NONE) {
        goto MBRXFRAME_EXIT;
    }

    /*  Get the driver.  */
    ifdrv = ifdev->driver;

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Increase the RX inprogress request counter.  */
    if (ctx->rxInProgressCounter == MBPORT_UINT08_MAX) {
        /*  Error: Too many RX requests.  */
        *p_error = MB_ERROR_RXTOOMANY;

        goto MBRXFRAME_EXIT;
    }
    ++(ctx->rxInProgressCounter);
    gc.clrRxRequestCnt = DEF_YES;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
    gc.clrCriticalSect = DEF_NO;

    /*  Acquire the I/O lock.  */
    MBOS_MutexPend(
        &(ctx->ioLock),
        (MB_TIMESPAN)0U,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBRXFRAME_EXIT;
    }
    gc.clrIoLock = DEF_YES;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();
    gc.clrCriticalSect = DEF_YES;

    /*
     *  Stage 2: Read the frame.
     */

    /*  Clear RX errors.  */
    ctx->rxParityError = DEF_NO;
    ctx->rxDataOverRunError = DEF_NO;
    ctx->rxFrameError = DEF_NO;

    /*  Clear the RX timeout bit.  */
    MBOS_FlagGroupPost(
        &(ctx->evFlags),
        MBCTX_EVENT_RXTIMEOUT,
        MB_FLAGGROUP_OPT_CLR,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBRXFRAME_EXIT;
    }

    /*  Create the RX timeout timer.  */
    if (timeout != (MB_TIMESPAN)0U) {
        rxTimeoutCbInfo.device = ifdev;
        MBOS_TimerCreate(
            &timeouter,
            timeout,
            MB_TIMER_MODE_ONESHOT,
            MB_ISR_RxTimeoutExceed,
            (void*)(&rxTimeoutCbInfo),
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            goto MBRXFRAME_EXIT;
        }
        gc.clrRxTimeoutTmr = DEF_YES;
        MBOS_TimerStart(
            &timeouter,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            goto MBRXFRAME_EXIT;
        }
    }

    /*  Clear RX datum.  */
    ctx->rxDatum = (CPU_INT08U)0U;
    ctx->rxDatumEaten = DEF_YES;

    /*  Read the frame.  */
    switch (ifdev->mode) {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
        case MB_TRMODE_RTU:
            {
                /*  Initialize MSV.  */
                msv.rtuMode.rtuFirstChar = DEF_YES;

                /*  Initialize the frame decoder.  */
                MBFrameDecRTU_Initialize(
                    &(decoder.rtuDecoder), 
                    p_buffer, 
                    buffer_size, 
                    p_error
                );
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }

                /*  Start the receiver.  */
                ifdrv->rxStart(p_error);
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }
                gc.clrRxReceiver = DEF_YES;

                /*
                *  Here we are going to the "Reception" state.
                */
                while (DEF_YES) {
                    /*  Reset the events.  */
                    MBOS_FlagGroupPost(
                        &(ctx->evFlags),
                        (
                            MBCTX_EVENT_1D5CTIMEEXCEED | 
                            MBCTX_EVENT_2D0CTIMEEXCEED | 
                            MBCTX_EVENT_3D5CTIMEEXCEED | 
                            MBCTX_EVENT_RXCOMPLETE
                        ),
                        MB_FLAGGROUP_OPT_CLR,
                        p_error
                    );

                    /*  Reset the half-character counter.  */
                    ctx->halfCharCounter = (CPU_INT32U)0U;

                    /*  Start the half-character timer.  */
                    if (!(msv.rtuMode.rtuFirstChar)) {
                        ifdrv->halfCharacterTimerStart(p_error);
                        if (*p_error != MB_ERROR_NONE) {
                            goto MBRXFRAME_EXIT;
                        }
                        gc.clrRxHalfCharTmr = DEF_YES;
                    }

                    /*  Exit critical section.  */
                    CPU_CRITICAL_EXIT();
                    gc.clrCriticalSect = DEF_NO;

                    /*  Wait for events.  */
                    fgrpFlags = MBCTX_EVENT_RXCOMPLETE;
                    if (msv.rtuMode.rtuFirstChar) {
                        /*  Wait for RX timeout event only when receives the first character.  */
                        fgrpFlags |= MBCTX_EVENT_RXTIMEOUT;
                    } else {
                        /*  Wait for 1.5 character time exceed event only for non-first characters.  */
                        fgrpFlags |= MBCTX_EVENT_1D5CTIMEEXCEED;
                    }
                    MBOS_FlagGroupPend(
                        &(ctx->evFlags),
                        &fgrpFlags,
                        (MB_TIMESPAN)0U,
                        MB_FLAGGROUP_OPT_SET_ANY,
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBRXFRAME_EXIT;
                    }

                    /*  Enter critical section.  */
                    CPU_CRITICAL_ENTER();
                    gc.clrCriticalSect = DEF_YES;

                    /*  Stop the half-character timer.  */
                    if (!(msv.rtuMode.rtuFirstChar)) {
                        ifdrv->halfCharacterTimerStop(p_error);
                        if (*p_error != MB_ERROR_NONE) {
                            goto MBRXFRAME_EXIT;
                        }
                        gc.clrRxHalfCharTmr = DEF_NO;
                    }

                    /*  Handle RX complete event.  */
                    if ((fgrpFlags & MBCTX_EVENT_RXCOMPLETE) != (MB_FLAGS)0) {
                        /*  Received one characcter, unmark the first character flag.  */
                        if (msv.rtuMode.rtuFirstChar) {
                            msv.rtuMode.rtuFirstChar = DEF_NO;
                        }

                        /*  Push the character to the frame decoder.  */
                        ctx->rxDatumEaten = DEF_YES;
                        MBFrameDecRTU_Update(&(decoder.rtuDecoder), ctx->rxDatum, p_error);
                        if (*p_error != MB_ERROR_NONE) {
                            goto MBRXFRAME_EXIT;
                        }
                    }

                    /*  Handle 1.5 character time exceeds event.  */
                    if ((fgrpFlags & MBCTX_EVENT_1D5CTIMEEXCEED) != (MB_FLAGS)0) {
                        break;
                    }

                    /*  Handle RX timeout event.  */
                    if ((fgrpFlags & MBCTX_EVENT_RXTIMEOUT) != (MB_FLAGS)0) {
                        /*  Error: RX timeout exceeds.  */
                        *p_error = MB_ERROR_TIMEOUT;

                        goto MBRXFRAME_EXIT;
                    }
                }

                /*  End the frame decoder.  */
                MBFrameDecRTU_End(
                    &(decoder.rtuDecoder),
                    p_error
                );
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }

                /*
                *  Here we are going to the "Control and Waiting" state.
                */

                /*  Reset character time bits.  */
                MBOS_FlagGroupPost(
                    &(ctx->evFlags),
                    (MB_FLAGS)(
                        MBCTX_EVENT_1D5CTIMEEXCEED | 
                        MBCTX_EVENT_2D0CTIMEEXCEED | 
                        MBCTX_EVENT_3D5CTIMEEXCEED
                    ),
                    MB_FLAGGROUP_OPT_CLR,
                    p_error
                );

                /*  Reset the half-character counter.  */
                ctx->halfCharCounter = (CPU_INT32U)0U;

                /*  Start the half-character timer.  */
                ifdrv->halfCharacterTimerStart(p_error);
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }
                gc.clrRxHalfCharTmr = DEF_YES;

                /*  Wait for another 2 character time exceeds.  */
                while (DEF_YES) {
                    /*  Reset the RX complete bit.  */
                    MBOS_FlagGroupPost(
                        &(ctx->evFlags),
                        MBCTX_EVENT_RXCOMPLETE,
                        MB_FLAGGROUP_OPT_CLR,
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBRXFRAME_EXIT;
                    }

                    /*  Exit critical section.  */
                    CPU_CRITICAL_EXIT();
                    gc.clrCriticalSect = DEF_NO;

                    /*  Wait for events.  */
                    fgrpFlags = (MB_FLAGS)(
                        MBCTX_EVENT_2D0CTIMEEXCEED | 
                        MBCTX_EVENT_RXCOMPLETE
                    );
                    MBOS_FlagGroupPend(
                        &(ctx->evFlags),
                        &fgrpFlags,
                        (MB_TIMESPAN)0U,
                        MB_FLAGGROUP_OPT_SET_ANY,
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBRXFRAME_EXIT;
                    }

                    /*  Enter critical section.  */
                    CPU_CRITICAL_ENTER();
                    gc.clrCriticalSect = DEF_YES;

                    /*  Handle RX complete event.  */
                    if ((fgrpFlags & MBCTX_EVENT_RXCOMPLETE) != (MB_FLAGS)0) {
                        ctx->rxDatumEaten = DEF_YES;
                        MBFrameDecRTU_Update(
                            &(decoder.rtuDecoder),
                            ctx->rxDatum,
                            p_error
                        );
                        if (*p_error != MB_ERROR_NONE) {
                            goto MBRXFRAME_EXIT;
                        }
                    }

                    /*  Handle 2 character time exceeds event.  */
                    if ((fgrpFlags & MBCTX_EVENT_2D0CTIMEEXCEED) != (MB_FLAGS)0) {
                        break;
                    }
                }

                /*  Stop the receiver.  */
                ifdrv->rxStop(p_error);
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }
                gc.clrRxReceiver = DEF_NO;

                /*  Stop the half-character timer.  */
                ifdrv->halfCharacterTimerStop(p_error);
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }
                gc.clrRxHalfCharTmr = DEF_NO;

                /*  Extract the frame from the decoder.  */
                MBFrameDecRTU_ToFrame(
                    &(decoder.rtuDecoder),
                    p_frame,
                    p_frameflags,
                    p_error
                );
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }
            }
            break;
#endif
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
        case MB_TRMODE_ASCII:
            {
                /*  Initialize MSV.  */
                msv.asciiMode.rxState = MBASCIIRXSTATE_WAITCOLON;

                /*  Start the receiver.  */
                ifdrv->rxStart(p_error);
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }
                gc.clrRxReceiver = DEF_YES;

                while (DEF_YES) {
                    /*  Reset events.  */
                    MBOS_FlagGroupPost(
                        &(ctx->evFlags),
                        (MB_FLAGS)(
                            MBCTX_EVENT_RXCOMPLETE |
                            MBCTX_EVENT_RXTIMEOUT
                        ),
                        MB_FLAGGROUP_OPT_CLR,
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBRXFRAME_EXIT;
                    }

                    /*  Exit critical section.  */
                    CPU_CRITICAL_EXIT();
                    gc.clrCriticalSect = DEF_NO;

                    /*  Wait for events.  */
                    fgrpFlags = (MB_FLAGS)(
                        MBCTX_EVENT_RXCOMPLETE | 
                        MBCTX_EVENT_RXTIMEOUT
                    );
                    MBOS_FlagGroupPend(
                        &(ctx->evFlags),
                        &fgrpFlags,
                        (MB_TIMESPAN)0,
                        MB_FLAGGROUP_OPT_SET_ANY,
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBRXFRAME_EXIT;
                    }

                    /*  Enter critical section.  */
                    CPU_CRITICAL_ENTER();
                    gc.clrCriticalSect = DEF_YES;

                    /*  Handle RX complete event.  */
                    if ((fgrpFlags & MBCTX_EVENT_RXCOMPLETE) != (MB_FLAGS)0) {
                        ctx->rxDatumEaten = DEF_YES;
                        msv.asciiMode.rxDatumTmp = ctx->rxDatum;
                        switch (msv.asciiMode.rxState) {
                            case MBASCIIRXSTATE_WAITCOLON:
                                if (msv.asciiMode.rxDatumTmp == (CPU_INT08U)ASCII_CHAR_COLON) {
                                    msv.asciiMode.rxResetDecoder = DEF_YES;
                                    msv.asciiMode.rxState = MBASCIIRXSTATE_WAITCR;
                                }
                                break;
                            case MBASCIIRXSTATE_WAITCR:
                                /*  Reset the decoder if needed.  */
                                if (msv.asciiMode.rxResetDecoder) {
                                    /*  Initialize (or reset) the frame decoder.  */
                                    MBFrameDecASCII_Initialize(
                                        &(decoder.asciiDecoder),
                                        p_buffer,
                                        buffer_size,
                                        p_error
                                    );
                                    if (*p_error != MB_ERROR_NONE) {
                                        goto MBRXFRAME_EXIT;
                                    }

                                    /*  Unmark the reset flag.  */
                                    msv.asciiMode.rxResetDecoder = DEF_NO;
                                }

                                /*  Handle the character.  */
                                switch (msv.asciiMode.rxDatumTmp) {
                                    case ((CPU_INT08U)ASCII_CHAR_CARRIAGE_RETURN):
                                        /*  End the frame decoder.  */
                                        MBFrameDecASCII_End(
                                            &(decoder.asciiDecoder),
                                            p_error
                                        );
                                        if (*p_error != MB_ERROR_NONE) {
                                            goto MBRXFRAME_EXIT;
                                        }

                                        /*  Go to "WAITLF" state.  */
                                        msv.asciiMode.rxState = MBASCIIRXSTATE_WAITLF;

                                        break;
                                    case ((CPU_INT08U)ASCII_CHAR_COLON):
                                        /*  Reset the frame decoder when received the next character.  */
                                        msv.asciiMode.rxResetDecoder = DEF_YES;

                                        break;
                                    default:
                                        /*  Push the character to the frame decoder.  */
                                        MBFrameDecASCII_Update(
                                            &(decoder.asciiDecoder),
                                            msv.asciiMode.rxDatumTmp,
                                            p_error
                                        );
                                        if (*p_error != MB_ERROR_NONE) {
                                            goto MBRXFRAME_EXIT;
                                        }

                                        break;
                                }

                                break;
                            case MBASCIIRXSTATE_WAITLF:
                                if (msv.asciiMode.rxDatumTmp == ifdev->asciiModeLF) {
                                    /*  Now a frame has been read, go to the next stage.  */
                                    goto MBRXFRAME_ASCII_FRAMEFINISH;
                                } else if (msv.asciiMode.rxDatumTmp == (CPU_INT08U)ASCII_CHAR_COLON) {
                                    /*  Reset the frame decoder when received the next character.  */
                                    msv.asciiMode.rxResetDecoder = DEF_YES;

                                    /*  Go to "WAITCR" mode.  */
                                    msv.asciiMode.rxState = MBASCIIRXSTATE_WAITCR;
                                } else {
                                    /*  Go to "WAITCOLON" mode.  */
                                    msv.asciiMode.rxState = MBASCIIRXSTATE_WAITCOLON;
                                }

                                break;
                            default:
                                /*
                                *  Never reach here.
                                *  For failsafe, go to "WAITCOLON" mode.
                                */
                                msv.asciiMode.rxState = MBASCIIRXSTATE_WAITCOLON;

                                break;
                        }
                    }

                    /*  Handle RX timeout event.  */
                    if ((fgrpFlags & MBCTX_EVENT_RXTIMEOUT) != (MB_FLAGS)0) {
                        /*  Error: RX timeout exceeds.  */
                        *p_error = MB_ERROR_TIMEOUT;

                        goto MBRXFRAME_EXIT;
                    }
                }

MBRXFRAME_ASCII_FRAMEFINISH:
                /*  Stop the receiver.  */
                ifdrv->rxStop(p_error);
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }
                gc.clrRxReceiver = DEF_NO;

                /*  Extract the frame from the decoder.  */
                MBFrameDecASCII_ToFrame(
                    &(decoder.asciiDecoder),
                    p_frame,
                    p_frameflags,
                    p_error
                );
                if (*p_error != MB_ERROR_NONE) {
                    goto MBRXFRAME_EXIT;
                }
            }
            break;
#endif  /*  #if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)  */
        default:
            /*  Error: Invalid transmission mode.  */
            *p_error = MB_ERROR_INVALIDMODE;
            goto MBRXFRAME_EXIT;
    }

    /*  Drop the frame if any transmission error occurred during receiving.  */
    if (p_frameflags != (MB_FRAMEFLAGS*)0) {
        if (ctx->rxParityError) {
            *(p_frameflags) |= (MB_FLAGS)(
                MB_FRAMEFLAGS_DROP | 
                MB_FRAMEFLAGS_PARITYERROR
            );
        }
        if (ctx->rxDataOverRunError) {
            *(p_frameflags) |= (MB_FLAGS)(
                MB_FRAMEFLAGS_DROP | 
                MB_FRAMEFLAGS_OVERRUNERROR
            );
        }
        if (ctx->rxFrameError) {
            *(p_frameflags) |= (MB_FLAGS)(
                MB_FRAMEFLAGS_DROP | 
                MB_FRAMEFLAGS_FRAMEERROR
            );
        }
    }

MBRXFRAME_EXIT:
    /*
     *  State 3: Release used resources.
     */

    /*  Stop the receiver (if needed).  */
    if (gc.clrRxReceiver) {
        if (ifdrv != (MB_DRIVER*)0) {
            ifdrv->rxStop(&error);
        }
        gc.clrRxReceiver = DEF_NO;
    }

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Stop the half-character timer (if needed).  */
    if (gc.clrRxHalfCharTmr) {
        if (ifdrv != (MB_DRIVER*)0) {
            ifdrv->halfCharacterTimerStop(&error);
        }
        gc.clrRxHalfCharTmr = DEF_NO;
    }
#endif

    /*  Dispose the RX timeout timer.  */
    if (gc.clrRxTimeoutTmr) {
        MBOS_TimerDispose(
            &timeouter,
            &error
        );
        gc.clrRxTimeoutTmr = DEF_NO;
    }

    /*  Decrease the RX request counter (if needed).  */
    if (gc.clrRxRequestCnt) {
        if (ctx != (MB_CONTEXT*)0) {
            --(ctx->rxInProgressCounter);
        }
        gc.clrRxRequestCnt = DEF_NO;
    }

    /*  Release the I/O lock (if needed).  */
    if (gc.clrIoLock) {
        if (ctx != (MB_CONTEXT*)0) {
            MBOS_MutexPost(
                &(ctx->ioLock),
                &error
            );
        }
        gc.clrIoLock = DEF_NO;
    }

    /*  Exit critical section (if needed).*/
    if (gc.clrCriticalSect) {
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;
    }
}


/*
*********************************************************************************************************
*                                    MB_TransmitFrame()
*
* Description : Transmit a Modbus frame through a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_frame        Pointer to the variable that receives the frame.
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                   No error occurred.
*                                      MB_ERROR_NULLREFERENCE          'p_frame' is NULL or 'pframe->data' is NULL while 'pframe->dataLength' is not zero.
*                                      MB_ERROR_INVALIDMODE            Device transmission mode is not supported.
*                                      MB_ERROR_DEVICENOTEXIST         'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER      Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED        Device is not opened.
*                                      MB_ERROR_DEVICEFAIL             Device operation failed.
*                                      MB_ERROR_TXTOOMANY              Too many transmit requests.
*                                      MB_ERROR_OS_FGRP_FAILEDPEND     Failed to pend on a flag group object.
*                                      MB_ERROR_OS_FGRP_FAILEDPOST     Failed to post to a flag group object.
*                                      MB_ERROR_OS_MUTEX_FAILEDPEND    Failed to pend on a mutex object.
*                                      MB_ERROR_OS_TIMER_FAILEDCREATE  Failed to create timer object.
*                                      MB_ERROR_OS_TIMER_FAILEDSTART   Failed to start a timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) The function would be blocked until previous I/O finished.
*********************************************************************************************************
*/

void MB_TransmitFrame(
    MB_IFINDEX             ifnbr,
    MB_FRAME              *p_frame,
    MB_ERROR              *p_error
) {
    MB_ERROR               error;

    MB_DEVICE             *ifdev;
    MB_DRIVER             *ifdrv;

    MB_CONTEXT            *ctx;

    MB_FLAGS               fgrpFlags;

    struct {
        CPU_BOOLEAN        clrIoLock:1;
        CPU_BOOLEAN        clrHalfCharTmr:1;
        CPU_BOOLEAN        clrTxDirection:1;
        CPU_BOOLEAN        clrTxTransmit:1;
        CPU_BOOLEAN        clrTxRequestCnt:1;
        CPU_BOOLEAN        clrCriticalSect:1;
        CPU_INT08U         __padding:2;
    } gc;

    union {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
        MB_FRAMEENC_RTU    rtuEncoder;
#endif
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
        MB_FRAMEENC_ASCII  asciiEncoder;
#endif
    } encoder;

    CPU_BOOLEAN  encoderHasNext;
    CPU_INT08U   encoderDatum;

    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_frame' parameter.  */
    if (p_frame == (MB_FRAME*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_buffer' parameter.  */
    if ((p_frame->data == (CPU_INT08U*)0) && (p_frame->dataLength != (CPU_SIZE_T)0U)) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Avoid 'unused-variable' warning.  */
    (void)encoderDatum;
    (void)encoderHasNext;
    (void)encoder;
    (void)fgrpFlags;

    /*  Initialize local variables.  */
    ifdev                = (MB_DEVICE*)0;
    ifdrv                = (MB_DRIVER*)0;
    ctx                  = (MB_CONTEXT*)0;
    gc.clrIoLock         = DEF_NO;
    gc.clrHalfCharTmr    = DEF_NO;
    gc.clrTxDirection    = DEF_NO;
    gc.clrTxTransmit     = DEF_NO;
    gc.clrTxRequestCnt   = DEF_NO;
    gc.clrCriticalSect   = DEF_NO;
    error                = MB_ERROR_NONE;

    /*  No error by default.  */
    *p_error             = MB_ERROR_NONE;

    /*
     *  Stage 1: Get the device, its driver and acquire the I/O lock.
     */

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();
    gc.clrCriticalSect   = DEF_YES;

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBTXFRAME_EXIT;
    }

    /*  Get the driver.  */
    ifdrv = ifdev->driver;

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Increase the TX inprogress request counter.  */
    if (ctx->txInProgressCounter == MBPORT_UINT08_MAX) {
        /*  Error: Too many TX requests.  */
        *p_error = MB_ERROR_TXTOOMANY;

        goto MBTXFRAME_EXIT;
    }
    ++(ctx->txInProgressCounter);
    gc.clrTxRequestCnt = DEF_YES;

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
    gc.clrCriticalSect = DEF_NO;

    /*  Acquire the I/O lock.  */
    MBOS_MutexPend(
        &(ctx->ioLock),
        (MB_TIMESPAN)0U,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBTXFRAME_EXIT;
    }
    gc.clrIoLock = DEF_YES;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();
    gc.clrCriticalSect = DEF_YES;

    /*
     *  Stage 2: Transmit the frame.
     */

    /*  Switch to transmission mode.  */
    ifdrv->halfDuplexModeSetup(MB_HALFDUPLEX_TRANSMIT, p_error);
    if (*p_error != MB_ERROR_NONE) {
        goto MBTXFRAME_EXIT;
    }
    gc.clrTxDirection = DEF_YES;

    /*  Start transmission.  */
    ifdrv->txStart(p_error);
    if (*p_error != MB_ERROR_NONE) {
        goto MBTXFRAME_EXIT;
    }
    gc.clrTxTransmit = DEF_YES;

    /*  Transmit the frame.  */
    switch (ifdev->mode) {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
        case MB_TRMODE_RTU:
            {
                /*  Initialize the encoder.  */
                MBFrameEncRTU_Initialize(
                    &(encoder.rtuEncoder),
                    p_frame,
                    p_error
                );
                if (*p_error != MB_ERROR_NONE) {
                    goto MBTXFRAME_EXIT;
                }

                while (DEF_YES) {
                    /*  Check whether we have more byte(s) to be transmitted.  */
                    encoderHasNext = MBFrameEncRTU_HasNext(
                        &(encoder.rtuEncoder),
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Stop if no more byte.  */
                    if (!encoderHasNext) {
                        break;
                    }

                    /*  Get the next byte to be transmitted.  */
                    encoderDatum = MBFrameEncRTU_Next(
                        &(encoder.rtuEncoder),
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Reset the events.  */
                    MBOS_FlagGroupPost(
                        &(ctx->evFlags),
                        MBCTX_EVENT_TXCOMPLETE,
                        MB_FLAGGROUP_OPT_CLR,
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Transmit the byte.  */
                    ifdrv->txTransmit(encoderDatum, p_error);
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Exit critical section.  */
                    CPU_CRITICAL_EXIT();
                    gc.clrCriticalSect = DEF_NO;

                    /*  Wait for TX complete.  */
                    fgrpFlags = MBCTX_EVENT_TXCOMPLETE;
                    MBOS_FlagGroupPend(
                        &(ctx->evFlags),
                        &fgrpFlags,
                        (MB_TIMESPAN)0U,
                        MB_FLAGGROUP_OPT_SET_ANY,
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Enter critical section.  */
                    CPU_CRITICAL_ENTER();
                    gc.clrCriticalSect = DEF_YES;
                }
            }
            break;
#endif
#if (MB_CFG_CORE_ASCIIMODE == DEF_ENABLED)
        case MB_TRMODE_ASCII:
            {
                /*  Initialize the encoder.  */
                MBFrameEncASCII_Initialize(
                    &(encoder.asciiEncoder), 
                    p_frame, 
                    (CPU_CHAR)(ifdev->asciiModeLF), 
                    p_error
                );
                if (*p_error != MB_ERROR_NONE) {
                    goto MBTXFRAME_EXIT;
                }

                while (DEF_YES) {
                    /*  Check whether we have more byte(s) to be transmitted.  */
                    encoderHasNext = MBFrameEncASCII_HasNext(
                        &(encoder.asciiEncoder),
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Stop if no more byte.  */
                    if (!encoderHasNext) {
                        break;
                    }

                    /*  Get the next byte to be transmitted.  */
                    encoderDatum = MBFrameEncASCII_Next(
                        &(encoder.asciiEncoder),
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Reset the events.  */
                    MBOS_FlagGroupPost(
                        &(ctx->evFlags),
                        MBCTX_EVENT_TXCOMPLETE,
                        MB_FLAGGROUP_OPT_CLR,
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Transmit the byte.  */
                    ifdrv->txTransmit(encoderDatum, p_error);
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Exit critical section.  */
                    CPU_CRITICAL_EXIT();
                    gc.clrCriticalSect = DEF_NO;

                    /*  Wait for TX complete.  */
                    fgrpFlags = MBCTX_EVENT_TXCOMPLETE;
                    MBOS_FlagGroupPend(
                        &(ctx->evFlags),
                        &fgrpFlags,
                        (MB_TIMESPAN)0U,
                        MB_FLAGGROUP_OPT_SET_ANY,
                        p_error
                    );
                    if (*p_error != MB_ERROR_NONE) {
                        goto MBTXFRAME_EXIT;
                    }

                    /*  Enter critical section.  */
                    CPU_CRITICAL_ENTER();
                    gc.clrCriticalSect = DEF_YES;
                }
            }
            break;
#endif
        default:
            /*  Error: Invalid transmission mode.  */
            *p_error = MB_ERROR_INVALIDMODE;
            goto MBTXFRAME_EXIT;
    }

    /*  Stop transmission.  */
    ifdrv->txStop(p_error);
    if (*p_error != MB_ERROR_NONE) {
        goto MBTXFRAME_EXIT;
    }
    gc.clrTxTransmit = DEF_NO;

    /*  Switch back to receive mode.  */
    ifdrv->halfDuplexModeSetup(MB_HALFDUPLEX_RECEIVE, p_error);
    if (*p_error != MB_ERROR_NONE) {
        goto MBTXFRAME_EXIT;
    }
    gc.clrTxDirection = DEF_NO;

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Delay 3.5 character time in RTU mode.  */
    if (ifdev->mode == MB_TRMODE_RTU) {
        /*  Reset events.  */
        MBOS_FlagGroupPost(
            &(ctx->evFlags),
            (MB_FLAGS)(
                MBCTX_EVENT_1D5CTIMEEXCEED | 
                MBCTX_EVENT_2D0CTIMEEXCEED | 
                MBCTX_EVENT_3D5CTIMEEXCEED
            ),
            MB_FLAGGROUP_OPT_CLR,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            goto MBTXFRAME_EXIT;
        }

        /*  Reset the half character counter.  */
        ctx->halfCharCounter = (CPU_INT32U)0U;

        /*  Start the half character timer.  */
        ifdrv->halfCharacterTimerStart(p_error);
        if (*p_error != MB_ERROR_NONE) {
            goto MBTXFRAME_EXIT;
        }
        gc.clrHalfCharTmr = DEF_YES;

        /*  Exit critical section.  */
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;

        /*  Wait for 3.5 character time.  */
        fgrpFlags = MBCTX_EVENT_3D5CTIMEEXCEED;
        MBOS_FlagGroupPend(
            &(ctx->evFlags),
            &fgrpFlags,
            (MB_TIMESPAN)0U,
            MB_FLAGGROUP_OPT_SET_ANY,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            goto MBTXFRAME_EXIT;
        }

        /*  Enter critical section.  */
        CPU_CRITICAL_ENTER();
        gc.clrCriticalSect = DEF_YES;

        /*  Stop the half character timer.  */
        ifdrv->halfCharacterTimerStop(p_error);
        if (*p_error != MB_ERROR_NONE) {
            goto MBTXFRAME_EXIT;
        }
        gc.clrHalfCharTmr = DEF_NO;
    }
#endif  /*  #if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)  */

#if (MB_CFG_CORE_GETLASTTXADDRESS == DEF_ENABLED)
    /*  Record the address of the transmitted frame.  */
    ctx->lastTxAddr = p_frame->address;
#endif

#if (MB_CFG_CORE_GETLASTTXFUNCTIONCODE == DEF_ENABLED)
    /*  Record the function code of the transmitted frame.  */
    ctx->lastTxFnCode = p_frame->functionCode;
#endif

#if (MB_CFG_CORE_GETLASTTXEXCEPTIONCODE == DEF_ENABLED)
    /*  Record the exception code (if occurred) of the transmitted frame.  */
    if (p_frame->functionCode > (CPU_INT08U)0x80U) {
        if (p_frame->dataLength != (CPU_SIZE_T)0U) {
            ctx->lastTxExCode = p_frame->data[0];
        }
    }
#endif

MBTXFRAME_EXIT:
    /*
     *  Stage 3: Release used resources.
     */

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Stop the half character timer (if needed).  */
    if (gc.clrHalfCharTmr) {
        if (ifdrv != (MB_DRIVER*)0) {
            ifdrv->halfCharacterTimerStop(&error);
        }
        gc.clrHalfCharTmr = DEF_NO;
    }
#endif

    /*  Stop the transmitter (if needed).  */
    if (gc.clrTxTransmit) {
        if (ifdrv != (MB_DRIVER*)0) {
            ifdrv->txStop(&error);
        }
        gc.clrTxTransmit = DEF_NO;
    }

    /*  Reset half-duplex direction to receive mode (if needed).  */
    if (gc.clrTxDirection) {
        if (ifdrv != (MB_DRIVER*)0) {
            ifdrv->halfDuplexModeSetup(MB_HALFDUPLEX_RECEIVE, &error);
        }
        gc.clrTxDirection = DEF_NO;
    }

    /*  Decrease the TX request counter (if needed).  */
    if (gc.clrTxRequestCnt) {
        if (ctx != (MB_CONTEXT*)0) {
            --(ctx->txInProgressCounter);
        }
        gc.clrTxRequestCnt = DEF_NO;
    }

    /*  Release the I/O lock (if needed).  */
    if (gc.clrIoLock) {
        if (ctx != (MB_CONTEXT*)0) {
            MBOS_MutexPost(
                &(ctx->ioLock),
                &error
            );
        }
        gc.clrIoLock = DEF_NO;
    }

    /*  Exit critical section (if needed).*/
    if (gc.clrCriticalSect) {
        CPU_CRITICAL_EXIT();
        gc.clrCriticalSect = DEF_NO;
    }
}


#if (MB_CFG_CORE_GETCOUNTERVALUE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetCounterValue()
*
* Description : Get the value of the specific counter of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) type           Counter type, one of following values:
*
*                                      MB_COUNTERTYPE_PARITYERROR        Parity error counter.
*                                      MB_COUNTERTYPE_DATAOVERRUNERROR   Data overrun error counter.
*                                      MB_COUNTERTYPE_FRAMEERROR         Frame error counter.
*
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*                                      MB_ERROR_INVALIDCOUNTER           'type' parameter contains invalid value.
*
* Return(s)   : The counter value.
*********************************************************************************************************
*/

MB_COUNTERVALUE  MB_GetCounterValue(
    MB_IFINDEX             ifnbr,
    MB_COUNTERTYPE         type,
    MB_ERROR              *p_error
) {
#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED) || \
    (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED) || \
    (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)

    MB_COUNTERVALUE  counter;

    MB_DEVICE  *ifdev;
    MB_CONTEXT *ctx;

    CPU_SR_ALLOC();

    /*  Initialize local variables.  */
    counter = (MB_COUNTERVALUE)0U;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBGETCNTVALUE_EXIT;
    }

#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED) || \
    (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED) || \
    (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)
    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);
#endif

    /*  Read the counter.  */
    switch (type) {
#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED)
        case MB_COUNTERTYPE_PARITYERROR:
            counter = ctx->cntParityError;
            break;
#endif
#if (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED)
        case MB_COUNTERTYPE_DATAOVERRUNERROR:
            counter = ctx->cntDataOverRunError;
            break;
#endif
#if (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)
        case MB_COUNTERTYPE_FRAMEERROR:
            counter = ctx->cntFrameError;
            break;
#endif
        default:
            *p_error = MB_ERROR_INVALIDCOUNTER;
            goto MBGETCNTVALUE_EXIT;
    }

MBGETCNTVALUE_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    return counter;
#else
    /*  Unused parameters.  */
    (void)ifnbr;
    (void)type;

    /*  Error: Invalid counter.  */
    *p_error = MB_ERROR_INVALIDCOUNTER;

    return (MB_COUNTERVALUE)0U;
#endif
}
#endif  /*  #if (MB_CFG_CORE_GETCOUNTERVALUE_EN == DEF_ENABLED)  */


#if (MB_CFG_CORE_CLEARCOUNTERVALUE_EN == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_ClearCounterValue()
*
* Description : Clear the value of the specific counter of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) type           Counter type, one of following values:
*
*                                      MB_COUNTERTYPE_PARITYERROR        Parity error counter.
*                                      MB_COUNTERTYPE_DATAOVERRUNERROR   Data overrun error counter.
*                                      MB_COUNTERTYPE_FRAMEERROR         Frame error counter.
*
*               (3) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*                                      MB_ERROR_INVALIDCOUNTER           'type' parameter contains invalid value.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void  MB_ClearCounterValue(
    MB_IFINDEX             ifnbr,
    MB_COUNTERTYPE         type,
    MB_ERROR              *p_error
) {
#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED) || \
    (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED) || \
    (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)

    MB_DEVICE  *ifdev;
    MB_CONTEXT *ctx;

    CPU_SR_ALLOC();

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBCLRCNTVALUE_EXIT;
    }

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Clear the counter.  */
    switch (type) {
#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED)
        case MB_COUNTERTYPE_PARITYERROR:
            ctx->cntParityError = (MB_COUNTERVALUE)0U;
            break;
#endif
#if (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED)
        case MB_COUNTERTYPE_DATAOVERRUNERROR:
            ctx->cntDataOverRunError = (MB_COUNTERVALUE)0U;
            break;
#endif
#if (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)
        case MB_COUNTERTYPE_FRAMEERROR:
            ctx->cntFrameError = (MB_COUNTERVALUE)0U;
            break;
#endif
        default:
            *p_error = MB_ERROR_INVALIDCOUNTER;
            goto MBCLRCNTVALUE_EXIT;
    }

MBCLRCNTVALUE_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
#else
    /*  Unused parameters.  */
    (void)ifnbr;
    (void)type;

    /*  Error: Invalid counter type.  */
    *p_error = MB_ERROR_INVALIDCOUNTER;
#endif
}
#endif  /*  #if (MB_CFG_CORE_CLEARCOUNTERVALUE_EN == DEF_ENABLED)  */


#if (MB_CFG_CORE_GETLASTTXADDRESS == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetLastTxAddress()
*
* Description : Get the address of the last transmitted frame of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*
* Return(s)   : The address.
*
* Note(s)     : (1) If no frame transmitted yet, 0 would be returned.
*********************************************************************************************************
*/

CPU_INT08U  MB_GetLastTxAddress(
    MB_IFINDEX             ifnbr,
    MB_ERROR              *p_error
) {
    CPU_INT08U  lastTxAddr;

    MB_DEVICE  *ifdev;
    MB_CONTEXT *ctx;

    CPU_SR_ALLOC();

    /*  Initialize local variables.  */
    lastTxAddr = (CPU_INT08U)0U;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBGETLASTTXADDR_EXIT;
    }

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Get the address of last TX frame.  */
    lastTxAddr = ctx->lastTxAddr;

MBGETLASTTXADDR_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    return lastTxAddr;
}
#endif  /*  #if (MB_CFG_CORE_GETLASTTXADDRESS == DEF_ENABLED)  */


#if (MB_CFG_CORE_GETLASTTXFUNCTIONCODE == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetLastTxFunctionCode()
*
* Description : Get the function code of the last transmitted frame of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*
* Return(s)   : The function code.
*
* Note(s)     : (1) If no frame transmitted yet, 0 would be returned.
*********************************************************************************************************
*/

CPU_INT08U  MB_GetLastTxFunctionCode(
    MB_IFINDEX             ifnbr,
    MB_ERROR              *p_error
) {
    CPU_INT08U  lastTxFnCode;

    MB_DEVICE  *ifdev;
    MB_CONTEXT *ctx;

    CPU_SR_ALLOC();

    /*  Initialize local variables.  */
    lastTxFnCode = (CPU_INT08U)0U;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBGETLASTTXFNCODE_EXIT;
    }

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Get the function code of last TX frame.  */
    lastTxFnCode = ctx->lastTxFnCode;

MBGETLASTTXFNCODE_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    return lastTxFnCode;
}
#endif  /*  #if (MB_CFG_CORE_GETLASTTXFUNCTIONCODE == DEF_ENABLED)  */


#if (MB_CFG_CORE_GETLASTTXEXCEPTIONCODE == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_GetLastTxExceptionCode()
*
* Description : Get the exception code of the last transmitted exception frame of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*
* Return(s)   : The exception code.
*
* Note(s)     : (1) If no exception frame transmitted yet, 0 would be returned.
*********************************************************************************************************
*/

CPU_INT08U  MB_GetLastTxExceptionCode(
    MB_IFINDEX             ifnbr,
    MB_ERROR              *p_error
) {
    CPU_INT08U  lastTxExCode;

    MB_DEVICE  *ifdev;
    MB_CONTEXT *ctx;

    CPU_SR_ALLOC();

    /*  Initialize local variables.  */
    lastTxExCode = (CPU_INT08U)0U;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBGETLASTTXEXCODE_EXIT;
    }

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Get the exception code of last TX exception frame.  */
    lastTxExCode = ctx->lastTxExCode;

MBGETLASTTXEXCODE_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();

    return lastTxExCode;
}
#endif  /*  #if (MB_CFG_CORE_GETLASTTXEXCEPTIONCODE == DEF_ENABLED)  */


#if (MB_CFG_CORE_CLEARLASTTXEXCEPTIONCODE == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_ClearLastTxExceptionCode()
*
* Description : Clear the exception code of the last transmitted exception frame of a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                     No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST           'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER        Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED          Device is not opened.
*
* Return(s)   : None.
*
* Note(s)     : (1) After calling this function, it will become as if no exception frame was sent on the Modbus 
*                   device.
*********************************************************************************************************
*/

void  MB_ClearLastTxExceptionCode(
    MB_IFINDEX             ifnbr,
    MB_ERROR              *p_error
) {
#if (MB_CFG_CORE_GETLASTTXEXCEPTIONCODE == DEF_ENABLED)
    MB_DEVICE  *ifdev;
    MB_CONTEXT *ctx;

    CPU_SR_ALLOC();

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*
     *  Get (and check) the device.
     * 
     *  Note(s):
     *    (1) In this procedure, we would also check the 'ifnbr' parameter.
     *    (2) The device must be initialized and opened. Otherwise, it won't
     *        pass the check.
     */
    ifdev = MB_GetDevice(
        ifnbr, 
        DEF_YES, 
        DEF_YES, 
        DEF_NO, 
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBCLRLASTTXEXCODE_EXIT;
    }

    /*  Get the Modbus context.  */
    ctx = &(ifdev->context);

    /*  Get the exception code of last TX exception frame.  */
    ctx->lastTxExCode = (CPU_INT08U)0U;

MBCLRLASTTXEXCODE_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
#else
    (void)ifnbr;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
#endif
}
#endif  /*  #if (MB_CFG_CORE_CLEARLASTTXEXCEPTIONCODE == DEF_ENABLED)  */


/*
*********************************************************************************************************
*                                    MB_GetDevice()
*
* Description : Get a Modbus device.
*
* Argument(s) : (1) ifnbr          Modbus device interface ID.
*               (2) chk_init       DEF_YES if to check whether the device is initialized (DEF_NO if not).
*               (3) chk_opened     DEF_YES if to check whether the device is opened (DEF_NO if not).
*               (4) chk_notopened  DEF_YES if to check whether the device is not opened (DEF_NO if not).
*               (5) p_error        Pointer to the variable that receives error code from this function:
*
*                                      MB_ERROR_NONE                    No error occurred.
*                                      MB_ERROR_DEVICENOTEXIST          'ifnbr' is not valid.
*                                      MB_ERROR_DEVICENOTREGISTER       Device is not registered (initialized) yet.
*                                      MB_ERROR_DEVICENOTOPENED         Device is not opened.
*                                      MB_ERROR_DEVICEOPENED            Device is already opened.
*
* Return(s)   : The device.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when calling this function.
*********************************************************************************************************
*/

static MB_DEVICE* MB_GetDevice(
    MB_IFINDEX   ifnbr,
    CPU_BOOLEAN  chk_init,
    CPU_BOOLEAN  chk_opened,
    CPU_BOOLEAN  chk_notopened,
    MB_ERROR    *p_error
) {
    MB_DEVICE *ifdev;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
#if (MB_CFG_MAX_NBR_IF < 255U)
    /*  Check 'ifnbr' parameter.  */
    if (ifnbr >= (MB_IFINDEX)MB_CFG_MAX_NBR_IF) {
        *p_error = MB_ERROR_DEVICENOTEXIST;
        return (MB_DEVICE*)0;
    }
#endif
#endif

    /*  Get the device.  */
    ifdev = &(g_ModbusDevices[(CPU_SIZE_T)ifnbr]);

    /*  Check whether the device is initialized.  */
    if (chk_init) {
        if (!(ifdev->initialized)) {
            *p_error = MB_ERROR_DEVICENOTREGISTER;
            return (MB_DEVICE*)0;
        }
    }

    /*  Check whether the device is opened.  */
    if (chk_opened) {
        if (!(ifdev->opened)) {
            *p_error = MB_ERROR_DEVICENOTOPENED;
            return (MB_DEVICE*)0;
        }
    }
    if (chk_notopened) {
        if (ifdev->opened) {
            *p_error = MB_ERROR_DEVICEOPENED;
            return (MB_DEVICE*)0;
        }
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return ifdev;
}


/*
*********************************************************************************************************
*                                    MB_Context_Initialize()
*
* Description : Initialize a Modbus context.
*
* Argument(s) : (1) ctx       Pointer to the Modbus context.
*               (1) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'ctx' is NULL.
*                                 MB_ERROR_OS_MUTEX_FAILEDCREATE   Unable to create a mutex object.
*                                 MB_ERROR_OS_FGRP_FAILEDCREATE    Unable to create a flag group object.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when calling this function.
*********************************************************************************************************
*/

static void MB_Context_Initialize(
    MB_CONTEXT  *ctx, 
    MB_ERROR    *p_error
) {
    MB_ERROR disposeError;

    CPU_BOOLEAN  clrIoLock;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'ctx' parameter.  */
    if (ctx == (MB_CONTEXT*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Initialize local variables.  */
    clrIoLock = DEF_NO;

    /*  No error by default.  */
    *p_error = MB_ERROR_NONE;

    /*  Initialize 'rxInProgressCounter' member.  */
    ctx->rxInProgressCounter = DEF_NO;

    /*  Initialize 'txInProgressCounter' member.  */
    ctx->txInProgressCounter = DEF_NO;

    /*  Initialize 'ioLock' member.  */
    MBOS_MutexCreate(
        &(ctx->ioLock),
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBCTXINIT_EXIT;
    }
    clrIoLock = DEF_YES;

    /*  Initialize 'evFlags' member.  */
    MBOS_FlagGroupCreate(
        &(ctx->evFlags),
        (MB_FLAGS)0U,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        goto MBCTXINIT_EXIT;
    }

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Initialize 'halfCharCounter' member.  */
    ctx->halfCharCounter = (CPU_INT32U)0U;

    /*  Initialize half character timer prescaler.  */
    ctx->halfCharTimerPrescaler = (CPU_INT16U)1U;

    /*  Initialize half character counter caches.  */
    ctx->halfCharCountCache_1D5 = (CPU_INT32U)3U;
    ctx->halfCharCountCache_2D0 = (CPU_INT32U)4U;
    ctx->halfCharCountCache_3D5 = (CPU_INT32U)7U;
#endif

    /*  Clear RX datum.  */
    ctx->rxDatum = (CPU_INT08U)0U;
    ctx->rxDatumEaten = DEF_YES;

    /*  Clear RX errors.  */
    ctx->rxParityError = DEF_NO;
    ctx->rxDataOverRunError = DEF_NO;
    ctx->rxFrameError = DEF_NO;
#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED)
    ctx->cntParityError = (MB_COUNTERVALUE)0U;
#endif
#if (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED)
    ctx->cntDataOverRunError = (MB_COUNTERVALUE)0U;
#endif
#if (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)
    ctx->cntFrameError = (MB_COUNTERVALUE)0U;
#endif

    /*  Clear lastest TX frame information.  */
#if (MB_CFG_CORE_GETLASTTXADDRESS == DEF_ENABLED)
    ctx->lastTxAddr = (CPU_INT08U)0U;
#endif
#if (MB_CFG_CORE_GETLASTTXFUNCTIONCODE == DEF_ENABLED)
    ctx->lastTxFnCode = (CPU_INT08U)0U;
#endif
#if (MB_CFG_CORE_GETLASTTXEXCEPTIONCODE == DEF_ENABLED)
    ctx->lastTxExCode = (CPU_INT08U)0U;
#endif

    /*  Now all members are initialized successfully, unmark cleanup flags.  */
    clrIoLock = DEF_NO;

MBCTXINIT_EXIT:
    /*  Cleanup the I/O lock (if error occurred).  */
    if (clrIoLock) {
        MBOS_MutexDispose(
            &(ctx->ioLock),
            &disposeError
        );
        clrIoLock = DEF_NO;
    }
}


/*
*********************************************************************************************************
*                                    MB_ISR_RxTimeoutExceed()
*
* Description : (ISR) Handle RX timeout exceed event.
*
* Argument(s) : (1) p_tmr   The timer object.
*               (1) p_arg   The callback argument.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when calling this function.
*********************************************************************************************************
*/

static void MB_ISR_RxTimeoutExceed(void *p_tmr, void *p_arg) {
    MB_ERROR     error;
    MB_RXTIMEOUT_CBINFO   *cbinfo;
    MB_CONTEXT *mbctx;

    /*  Get the Modbus context.  */
    cbinfo = (MB_RXTIMEOUT_CBINFO*)p_arg;
    mbctx = &(cbinfo->device->context);

    /*  Set the RX timeout bit.  */
    MBOS_FlagGroupPost(
        &(mbctx->evFlags),
        MBCTX_EVENT_RXTIMEOUT,
        MB_FLAGGROUP_OPT_SET,
        &error
    );
}


#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
/*
*********************************************************************************************************
*                                    MB_ISR_HalfCharacterTimeExceed()
*
* Description : (ISR) Handle half character time exceed event.
*
* Argument(s) : (1) mbdrv   The Modbus device driver.
*               (2) mbctx_  The Modbus context.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when calling this function.
*********************************************************************************************************
*/

static void MB_ISR_HalfCharacterTimeExceed(MB_DRIVER *mbdrv, void *mbctx_) {
    MB_CONTEXT  *mbctx;
    MB_ERROR     error;

    /*  Type cast.  */
    mbctx = (MB_CONTEXT*)mbctx_;

    /*  Don't do anything if the half character counter reaches 7 (3.5 character time).  */
    if (mbctx->halfCharCounter >= mbctx->halfCharCountCache_3D5) {
        mbctx->halfCharCounter  = mbctx->halfCharCountCache_3D5;
        return;
    }

    /*  Increase the half character counter.  */
    ++(mbctx->halfCharCounter);

    if (mbctx->halfCharCounter >= mbctx->halfCharCountCache_3D5) {
        /*  Set the 3.5 character time excess bit.  */
        MBOS_FlagGroupPost(
            &(mbctx->evFlags),
            MBCTX_EVENT_3D5CTIMEEXCEED,
            MB_FLAGGROUP_OPT_SET,
            &error
        );
    } else if (mbctx->halfCharCounter >= mbctx->halfCharCountCache_2D0) {
        /*  Set the 2 character time excess bit.  */
        MBOS_FlagGroupPost(
            &(mbctx->evFlags),
            MBCTX_EVENT_2D0CTIMEEXCEED,
            MB_FLAGGROUP_OPT_SET,
            &error
        );
    } else {
        if (mbctx->halfCharCounter >= mbctx->halfCharCountCache_1D5) {
            /*  Set the 1.5 character time excess bit.  */
            MBOS_FlagGroupPost(
                &(mbctx->evFlags),
                MBCTX_EVENT_1D5CTIMEEXCEED,
                MB_FLAGGROUP_OPT_SET,
                &error
            );
        }
    }
}
#endif


/*
*********************************************************************************************************
*                                    MB_ISR_RxComplete()
*
* Description : (ISR) Handle RX complete event.
*
* Argument(s) : (1) mbdrv   The Modbus device driver.
*               (2) mbctx_  The Modbus context.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when calling this function.
*********************************************************************************************************
*/

static void MB_ISR_RxComplete(MB_DRIVER *mbdrv, void *mbctx_) {
    MB_CONTEXT  *mbctx;
    MB_ERROR     error;
#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED) || \
    (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED) || \
    (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)

    MB_COUNTERVALUE   cnt;

#endif

    /*  Type cast.  */
    mbctx = (MB_CONTEXT*)mbctx_;

    /*  Read the character data.  */
    if (mbctx->rxDatumEaten) {
        mbctx->rxDatum = mbdrv->rxRead(&error);
        if (error != MB_ERROR_NONE) {
            return;
        }
        mbctx->rxDatumEaten = DEF_NO;
    } else {
        /*  Read the byte and drop.  */
        mbdrv->rxRead(&error);
        if (error != MB_ERROR_NONE) {
            return;
        }

        /*  Soft overrun deteted.  */
        mbctx->rxDataOverRunError = DEF_YES;
#if (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED)
        cnt = mbctx->cntDataOverRunError;
        if (cnt != MB_COUNTERVALUE_MAX) {
            ++cnt;
            mbctx->cntDataOverRunError = cnt;
        }
#endif
    }

    /*  Update error status.  */
    if (mbdrv->hasParityError()) {
        mbdrv->clearParityError();
        mbctx->rxParityError = DEF_YES;
#if (MB_CFG_CORE_PARITYERRORCOUNTER_EN == DEF_ENABLED)
        cnt = mbctx->cntParityError;
        if (cnt != MB_COUNTERVALUE_MAX) {
            ++cnt;
            mbctx->cntParityError = cnt;
        }
#endif
    }
    if (mbdrv->hasDataOverRunError()) {
        mbdrv->clearDataOverRunError();
        mbctx->rxDataOverRunError = DEF_YES;
#if (MB_CFG_CORE_DATAOVERRUNERRORCOUNTER_EN == DEF_ENABLED)
        cnt = mbctx->cntDataOverRunError;
        if (cnt != MB_COUNTERVALUE_MAX) {
            ++cnt;
            mbctx->cntDataOverRunError = cnt;
        }
#endif
    }
    if (mbdrv->hasFrameError()) {
        mbdrv->clearFrameError();
        mbctx->rxFrameError = DEF_YES;
#if (MB_CFG_CORE_FRAMEERRORCOUNTER_EN == DEF_ENABLED)
        cnt = mbctx->cntFrameError;
        if (cnt != MB_COUNTERVALUE_MAX) {
            ++cnt;
            mbctx->cntFrameError = cnt;
        }
#endif
    }

    /*  Set the RX complete bit.  */
    MBOS_FlagGroupPost(
        &(mbctx->evFlags),
        MBCTX_EVENT_RXCOMPLETE,
        MB_FLAGGROUP_OPT_SET,
        &error
    );
}


/*
*********************************************************************************************************
*                                    MB_ISR_TxComplete()
*
* Description : (ISR) Handle TX complete event.
*
* Argument(s) : (1) mbdrv   The Modbus device driver.
*               (2) mbctx_  The Modbus context.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when calling this function.
*********************************************************************************************************
*/

static void MB_ISR_TxComplete(MB_DRIVER *mbdrv, void *mbctx_) {
    MB_CONTEXT *mbctx;
    MB_ERROR    error;

    /*  Type cast.  */
    mbctx = (MB_CONTEXT*)mbctx_;

    /*  Set the TX complete bit.  */
    MBOS_FlagGroupPost(
        &(mbctx->evFlags),
        MBCTX_EVENT_TXCOMPLETE,
        MB_FLAGGROUP_OPT_SET,
        &error
    );
}
