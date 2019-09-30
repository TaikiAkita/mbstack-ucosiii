# Driver Implementation Guide

This document describes how to implement drivers of different devices.

Device drivers are generally written in *C* and depends on *Board Support Package (BSP)* which is written in C and maybe some assembly language. In this document, we assumed that the reader has a good grasp of C programming language, assembly language, cross-compilation toolchain and the hardware platform (or "target board").

If there is already a device driver for the hardware platform to be used, it is unnecessary to read this document unless there is an interest in knowing how the driver really works.

This software can run on a hardware platform if it satisfies following general requirements:

 - The hardware platform has an ANSI C compiler.
 - The hardware platform supports high-precision on-board timer and can provide an interrupt at specific intervals (typically between 250us to 20ms). The timer would be used for monitoring 1.5 and 3.5 character time in RTU transmission mode.
 - The hardware platform has enough RAM and ROM space to contain the instructions, variables, buffer and data structures of this software.

Implementing a device driver is quite straightforward once the subtleties of the hardware platform and C compiler are understood. Depending on the complexity of the hardware platform, a device driver consists of writing or changing between 500 to 1000 lines of code, which takes a few hours to a few days to accomplish. The easiest thing to do, however, is to modify an existing device driver from a hardware platform that is similar to the one intended for use.

## Directories and Files

Following table shows the name of device driver files and where (the directory) they should be placed. The file names in **bold** are files you will need to create or modify for your own hardware platform.

| File                 | Directory                                           |
|----------------------|-----------------------------------------------------|
| mbdrv_cfg.h          | /Driver/                                            |
| mbdrv_types.h        | /Driver/                                            |
| **mbdrv_usart{X}.c** | /Driver/&lt;hardware-platform&gt;/&lt;compiler&gt;/ |
| **mbdrv_usart{X}.h** | /Driver/&lt;hardware-platform&gt;/&lt;compiler&gt;/ |

*&lt;hardware-platform&gt;* is the name of the hardware platform that the driver files apply to (e.g. "*MIMXRT1050-EVK*", "*Arduino/Mega2560*").

*&lt;compiler&gt;* is the name of the toolchain (compiler, assembler, linker/locator) used. Each has its own directory because they may have different features that makes them different from one another.

### (File) mbdrv_cfg.h

This is a wrapper file of *&lt;app_cfg.h&gt;*. This file defines a series of default value of the build options. However, this file should not require any changes.

### (File) mbdrv_types.h

This file defines following types and should not require any changes.

#### MB_SERIAL_SETUP

Here is the definition of the type:

```
/*  Device serial setup type.  */
typedef struct mb_serial_setup {
    MB_BAUDRATE  baudrate;
    MB_DATABITS  dataBits;
    MB_STOPBITS  stopBits;
    MB_PARITY    parity;
} MB_SERIAL_SETUP;
```

This type is a struct type that contains the USART configuration data (including baudrate, data bits, parity and stop bits).

The *baudrate* member is a 32-bit unsigned integer, stores the serial baudrate and contains value that defines in *&lt;mb_constants.h&gt;* and names *MB_SERIAL_BAUDRATE_\**

The *dataBits* member is a 8-bit unsigned integer, stores the count of the serial data bits and contains value that defines in *&lt;mb_constants.h&gt;* and names *MB_SERIAL_DATABITS_\**

The *stopBits* member is a 8-bit unsigned integer, stores the count of the serial stop bits and contains value that defines in *&lt;mb_constants.h&gt;* and names *MB_SERIAL_STOPBITS_\**

The *parity* member is a 8-bit unsigned integer, stores the type of the serial parity bit and contains value that defines in *&lt;mb_constants.h&gt;* and names *MB_SERIAL_PARITY_\**

#### MB_DRIVER_CALLBACKS

Here is the definition of the type:

```
/*  Device driver callback type.  */
typedef struct {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    void (*halfCharacterTimeExceed)(MB_DRIVER *mbdrv, void *mbctx);
#endif
    void (*rxComplete)(MB_DRIVER *mbdrv, void *mbctx);
    void (*txComplete)(MB_DRIVER *mbdrv, void *mbctx);
} MB_DRIVER_CALLBACKS;
```

This type is a struct type that contains callback functions which are passed to the device driver on initialization and should be called by the device driver to notify Modbus core module that specified interrupt/event occurred.

The *halfCharacterTimeExceed* callback is used to notify that the half-character timer exceeds.  The *mbdrv* parameter should stores the pointer to the *MB_DRIVER* object of current driver. The *mbctx* parameter should stores the pointer to the Modbus context object that passed on initialization.

The *rxComplete* callback is used to notify that the receive buffer is not empty (a byte has been received from the serial line). Two parameters are the same as *halfCharacterTimeExceed* callback.

The *txComplete* callback is used to notify that all bytes in the transmit buffer has been transmitted to the serial line. Two parameters are the same as *halfCharacterTimeExceed* callback.

#### MB_DRIVER

Here is the definition of the type:

```
/*  Device driver descriptor type.  */
typedef struct {
    void        (*initialize)(
        void                *p_mbctx,
        MB_DRIVER_CALLBACKS *p_drvcb,
        MB_ERROR            *p_error
    );
    void        (*deviceOpen)(
        MB_SERIAL_SETUP *p_setup,
        MB_ERROR        *p_error
    );
    void        (*deviceClose)(
        MB_ERROR *p_error
    );
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    void        (*halfCharacterTimerStart)(
        MB_ERROR *p_error
    );
    void        (*halfCharacterTimerStop)(
        MB_ERROR *p_error
    );
#endif
    void        (*rxStart)(
        MB_ERROR *p_error
    );
    void        (*rxStop)(
        MB_ERROR *p_error
    );
    CPU_INT08U  (*rxRead)(
        MB_ERROR *p_error
    );
    void        (*txStart)(
        MB_ERROR *p_error
    );
    void        (*txStop)(
        MB_ERROR *p_error
    );
    void        (*txTransmit)(
        CPU_INT08U datum, 
        MB_ERROR  *p_error
    );
    void        (*halfDuplexModeSetup)(
        MB_DUPLEXMODE  duplexMode,
        MB_ERROR      *p_error
    );
    CPU_BOOLEAN (*hasParityError)();
    void        (*clearParityError)();
    CPU_BOOLEAN (*hasDataOverRunError)();
    void        (*clearDataOverRunError)();
    CPU_BOOLEAN (*hasFrameError)();
    void        (*clearFrameError)();
} MB_DRIVER;
```

This type is a struct type that contains all interfaces that a device driver should provide.

### mbdrv_usart{X}.c

This file contains the implementation of the driver. But instead of exporting a lot of functions, this file only export one *MB_DRIVER* object, like following (the function name may be different from your driver implementation):

```
/*  Driver descriptor.  */
MB_DRIVER MBDRV_USART_DRIVERDESC = {
    .initialize              = MBDrv_USART_Initialize,
    .deviceOpen              = MBDrv_USART_DeviceOpen,
    .deviceClose             = MBDrv_USART_DeviceClose,
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    .halfCharacterTimerStart = MBDrv_USART_HalfCharacterTimerStart,
    .halfCharacterTimerStop  = MBDrv_USART_HalfCharacterTimerStop,
#endif
    .rxStart                 = MBDrv_USART_RxStart,
    .rxStop                  = MBDrv_USART_RxStop,
    .rxRead                  = MBDrv_USART_RxRead,
    .txStart                 = MBDrv_USART_TxStart,
    .txStop                  = MBDrv_USART_TxStop,
    .txTransmit              = MBDrv_USART_TxTransmit,
    .halfDuplexModeSetup     = MBDrv_USART_HalfDuplex_SetMode,
    .hasParityError          = MBDrv_USART_HasParityError,
    .clearParityError        = MBDrv_USART_ClearParityError,
    .hasDataOverRunError     = MBDrv_USART_HasDataOverRunError,
    .clearDataOverRunError   = MBDrv_USART_ClearDataOverRunError,
    .hasFrameError           = MBDrv_USART_HasFrameError,
    .clearFrameError         = MBDrv_USART_ClearFrameError
};
```

#### MBDrv_USART_Initialize(p_mbctx, p_drvcb, p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                   MBDrv_USART_Initialize()
*
* Description : Initialize the driver.
*
* Argument(s) : (1) p_mbctx     Pointer to the Modbus context.
*               (2) p_drvcb     Pointer to the driver callbacks.
*               (3) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Driver initialized successfully.
*                                   MB_ERROR_NULLREFERENCE        'p_mbctx' or 'p_drvcb' is NULL.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_Initialize(
    void                *p_mbctx,
    MB_DRIVER_CALLBACKS *p_drvcb,
    MB_ERROR            *p_error
) {
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_mbctx' parameter.  */
    if (p_mbctx == (void*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_drvcb' parameter.  */
    if (p_drvcb == (MB_DRIVER_CALLBACKS*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    Save 'p_mbctx' to a global variable.
    Save 'p_drvcb' to a global variable.

    Mark the hardware serial as not opened.

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    Clear the context variable used by half-character timer.
#endif

    Clear all counters.

    Reset the hardware serial.

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
```

#### MBDrv_USART_DeviceOpen(p_setup, p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                   MBDrv_USART_DeviceOpen()
*
* Description : Open the Modbus link-layer device.
*
* Argument(s) : (1) p_setup     Pointer to the serial port configuration.
*               (2) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Device opened successfully.
*                                   MB_ERROR_NULLREFERENCE        'p_setup' is NULL.
*                                   MB_ERROR_INVALIDPARAMETER     Serial port configuration corrupted.
*                                   MB_ERROR_DEVICEOPENED         Device was already opened.
*                                   MB_ERROR_DEVICEFAIL           Device failed to be opened.
*
* Return(s)   : None.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_DeviceOpen(
    MB_SERIAL_SETUP *p_setup,
    MB_ERROR        *p_error
) {
    CPU_SR_ALLOC();

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_setup' parameter.  */
    if (p_setup == (MB_SERIAL_SETUP*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Initialize local variables.  */
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    clrHalfCharTimer = DEF_NO;
#endif

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Check whether the device is already opened.  */
    if (The hardware serial is already opened) {
        *p_error = MB_ERROR_DEVICEOPENED;
        goto MBDRV_DEVICEOPEN_EXIT;
    }

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Calculate half character time.  */
    if (p_setup->baudrate > (MB_BAUDRATE)19200U) {
        /*
         *  According to the Modbus specification, for baud rates greater than 
         *  19200 Bps, fixed values for 2 timers should be used:
         * 
         *    750us for the inter-character time-out (t1.5)
         *    1.750ms for inter-frame delay (t3.5)
         */
        halfCharTime = (CPU_INT32U)250U;
    } else {
        halfCharTime = MBUtil_GetHalfSerialCharacterTime(
            p_setup,
            p_error
        );
        if (*p_error != MB_ERROR_NONE) {
            *p_error = MB_ERROR_INVALIDPARAMETER;
            goto MBDRV_DEVICEOPEN_EXIT;
        }
    }

    Create a on-board hardware timer working in periodic triggering mode and set its interval to 'halfCharTime' milliseconds. Note that the timer should stay in stopped mode after created.
    if (Failed to create the timer) {
        *p_error = MB_ERROR_DEVICEFAIL;
        goto MBDRV_DEVICEOPEN_EXIT;
    }
    clrHalfCharTimer = DEF_YES;
#endif

    /*  Open serial device.  */
    Open the hardware serial with configurations from 'p_setup'.
    Enable the receiver/transmitter of the hardware serial with RX/TX complete interrupts disabled.
    Let the hardware serial invoke MBDrv_USART_ISR_RxComplete() when received one byte.
    Let the hardware serial invoke MBDrv_USART_ISR_TxComplete() when transmitted one byte completely.

    Mark the device as opened.

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Do NOT clear the half character timer when exit this function.  */
    clrHalfCharTimer = DEF_NO;
#endif

    Clear internal error counter.

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

MBDRV_DEVICEOPEN_EXIT:
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    /*  Cleanup the half character timer (only on failure).  */
    if (clrHalfCharTimer) {
        Dispose the half character timer.
        clrHalfCharTimer = DEF_NO;
    }
#endif

    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
}
```

#### MBDrv_USART_DeviceClose(p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                   MBDrv_USART_DeviceClose()
*
* Description : Close the Modbus link-layer device.
*
* Argument(s) : (1) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Device closed successfully.
*                                   MB_ERROR_DEVICENOTOPENED      Device is not opened yet.
*                                   MB_ERROR_DEVICEBUSY           Device is still busy.
*                                   MB_ERROR_DEVICEFAIL           Other device failure occurred.
*
* Return(s)   : None.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_DeviceClose(
    MB_ERROR *p_error
) {
    CPU_SR_ALLOC();

    /*  Enter critical section.  */
    CPU_CRITICAL_ENTER();

    /*  Check whether the device is opened.  */
    if (The device is not opened yet) {
        *p_error = MB_ERROR_DEVICENOTOPENED;
        goto MBDRV_DEVICECLOSE_EXIT;
    }

#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    Dispose the half character timer.
    if (Failed to dispose the half character timer) {
        Set *p_error to MB_ERROR_DEVICEBUSY or MB_ERROR_DEVICEFAIL.
        goto MBDRV_DEVICECLOSE_EXIT;
    }
#endif

    Close the hardware serial.

    Mark the device as not opened.

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

MBDRV_DEVICECLOSE_EXIT:
    /*  Exit critical section.  */
    CPU_CRITICAL_EXIT();
}
```

#### MBDrv_USART_HalfCharacterTimerStart(p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                             MBDrv_USART_HalfCharacterTimerStart()
*
* Description : Start the half-character timer.
*
* Argument(s) : (1) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Timer started successfully.
*                                   MB_ERROR_DEVICEFAIL           Timer failed to be started.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_HalfCharacterTimerStart(
    MB_ERROR *p_error
) {
    Start the half character timer.
    if (Failed to start the half character timer) {
        *p_error = MB_ERROR_DEVICEFAIL;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
```

#### MBDrv_USART_HalfCharacterTimerStop(p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                             MBDrv_USART_HalfCharacterTimerStop()
*
* Description : Stop the half-character timer.
*
* Argument(s) : (1) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Timer stopped successfully.
*                                   MB_ERROR_DEVICEFAIL           Timer failed to be stopped.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_HalfCharacterTimerStop(
    MB_ERROR *p_error
) {
    Stop the half character timer.
    if (Failed to stop the half character timer) {
        *p_error = MB_ERROR_DEVICEFAIL;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
```

#### MBDrv_USART_RxStart(p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                     MBDrv_USART_RxStart()
*
* Description : Start the receiver.
*
* Argument(s) : (1) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Receiver started successfully.
*                                   MB_ERROR_DEVICENOTOPENED      Device is not opened yet.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_RxStart(
    MB_ERROR *p_error
) {
    if (The device is not opened) {
        *p_error = MB_ERROR_DEVICENOTOPENED;
        return;
    }

    Enable the RX complete interrupt.

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
```

#### MBDrv_USART_RxStop(p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                     MBDrv_USART_RxStop()
*
* Description : Stop the receiver.
*
* Argument(s) : (1) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Receiver stopped successfully.
*                                   MB_ERROR_DEVICENOTOPENED      Device is not opened yet.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_RxStop(
    MB_ERROR *p_error
) {
    if (The device is not opened) {
        *p_error = MB_ERROR_DEVICENOTOPENED;
        return;
    }

    Disable the RX complete interrupt.

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
```

#### MBDrv_USART_RxRead(p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                     MBDrv_USART_RxRead()
*
* Description : Read one byte from the receiver.
*
* Argument(s) : (1) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Read from the receiver successfully.
*                                   MB_ERROR_DEVICENOTOPENED      Device is not opened yet.
*
* Return(s)   : The byte received.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*               (2) If the receiver buffer is empty, the function would be blocked.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static CPU_INT08U MBDrv_USART_RxRead(
    MB_ERROR *p_error
) {
    if (The device is not opened) {
        *p_error = MB_ERROR_DEVICENOTOPENED;
        return (CPU_INT08U)0x00U;
    }

    while (The receive buffer is empty) {
    }

    Read one byte from the receive buffer of the hardware serial.
    if (Parity error occurred while receiving the byte) {
        Mark the internal parity error flag.
    }
    if (Data overrun error occurred while receiving the byte) {
        Mark the internal data overrun error flag.
    }
    if (Frame error occurred while receiving the byte) {
		Mark the internal frame error flag.
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    Return the received byte.
}
```

#### MBDrv_USART_RxStart(p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                     MBDrv_USART_TxStart()
*
* Description : Start the transmitter.
*
* Argument(s) : (1) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Transmitter started successfully.
*                                   MB_ERROR_DEVICENOTOPENED      Device is not opened yet.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_TxStart(
    MB_ERROR *p_error
) {
    if (The device is not opened) {
        *p_error = MB_ERROR_DEVICENOTOPENED;
        return;
    }

    Enable the TX complete interrupt.

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
```

#### MBDrv_USART_TxStop(p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                     MBDrv_USART_TxStop()
*
* Description : Stop the transmitter.
*
* Argument(s) : (1) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Transmitter stopped successfully.
*                                   MB_ERROR_DEVICENOTOPENED      Device is not opened yet.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_TxStop(
    MB_ERROR *p_error
) {
    if (The device is not opened) {
        *p_error = MB_ERROR_DEVICENOTOPENED;
        return;
    }

    Disable the TX complete interrupt.

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
```

#### MBDrv_USART_TxTransmit(datum, p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                   MBDrv_USART_TxTransmit()
*
* Description : Transmit one byte.
*
* Argument(s) : (1) datum       Byte to be written.
*               (2) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Transmit successfully.
*                                   MB_ERROR_DEVICENOTOPENED      Device is not opened yet.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*               (2) If the transmitter buffer is not empty, the function would be blocked.
*               (3) The returning of this function (with no error) only means that the byte has been 
*                   written to the transmit buffer.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_TxTransmit(
    CPU_INT08U datum, 
    MB_ERROR *p_error
) {
    if (The device is not opened) {
        *p_error = MB_ERROR_DEVICENOTOPENED;
        return;
    }

    while (Transmit buffer is not empty) {
    }

    Transmit the byte through the hardware serial.

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
```

#### MBDrv_USART_HalfDuplex_SetMode(duplexMode, p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                 MBDrv_USART_HalfDuplex_SetMode()
*
* Description : Set half-duplex transaction mode.
*
* Argument(s) : (1) duplexMode  The duplex mode, one of following:
*
*                                   MB_HALFDUPLEX_RECEIVE         Receive mode.
*                                   MB_HALFDUPLEX_TRANSMIT        Transmit mode.
*
*               (2) p_error     Pointer to variable that will receive the return error code from this function:
*
*                                   MB_ERROR_NONE                 Transmitter stopped successfully.
*                                   MB_ERROR_INVALIDPARAMETER     'duplexMode' is not valid.
*                                   MB_ERROR_DEVICENOTOPENED      Device is not opened yet.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_HalfDuplex_SetMode(
    MB_DUPLEXMODE  duplexMode,
    MB_ERROR      *p_error
) {
    /*  Switch half-duplex direction.  */
    switch (duplexMode) {
        case MB_HALFDUPLEX_RECEIVE:
            Switch the hardware serial to receive mode.
            break;
        case MB_HALFDUPLEX_TRANSMIT:
	        Switch the hardware serial to transmit mode.
            break;
#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return;
#endif
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}
```

#### MBDrv_USART_HasParityError()

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                 MBDrv_USART_HasParityError()
*
* Description : Check whether parity error occurred.
*
* Argument(s) : None.
*
* Return(s)   : DEF_YES if so, DEF_NO if not.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*               (2) It's recommended to call this function right after calling MBDrv_USART_RxRead()
*                   to check whether transmission error occurred.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static CPU_BOOLEAN MBDrv_USART_HasParityError() {
    if (Internal parity error flag is marked) {
        return DEF_YES;
    } else {
	    return DEF_NO;
    }
}
```

#### MBDrv_USART_ClearParityError()

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                 MBDrv_USART_ClearParityError()
*
* Description : Clear the parity error occurred flag.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_ClearParityError() {
    Unmark the internal parity error flag.
}
```

#### MBDrv_USART_HasDataOverRunError()

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                 MBDrv_USART_HasDataOverRunError()
*
* Description : Check whether data overrun error occurred.
*
* Argument(s) : None.
*
* Return(s)   : DEF_YES if so, DEF_NO if not.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*               (2) It's recommended to call this function right after calling MBDrv_USART_RxRead()
*                   to check whether transmission error occurred.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static CPU_BOOLEAN MBDrv_USART_HasDataOverRunError() {
    if (Internal data overrun error flag is marked) {
	    return DEF_YES;
    } else {
	    return DEF_NO;
    }
}
```

#### MBDrv_USART_ClearDataOverRunError()

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                 MBDrv_USART_ClearDataOverRunError()
*
* Description : Clear the data overrun error occurred flag.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_ClearDataOverRunError() {
    Unmark the internal data overrun error flag.
}
```

#### MBDrv_USART_HasFrameError(p_error)

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                 MBDrv_USART_HasFrameError()
*
* Description : Check whether frame error occurred.
*
* Argument(s) : None.
*
* Return(s)   : DEF_YES if so, DEF_NO if not.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*               (2) It's recommended to call this function right after calling MBDrv_USART_RxRead()
*                   to check whether transmission error occurred.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static CPU_BOOLEAN MBDrv_USART_HasFrameError() {
    if (Internal frame error flag is marked) {
	    return DEF_YES;
    } else {
	    return DEF_NO;
    }
}
```

#### MBDrv_USART_ClearFrameError()

Here is the document comment block of this function:

```
/*
*********************************************************************************************************
*                                 MBDrv_USART_ClearFrameError()
*
* Description : Clear the frame error occurred flag.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/
```

Here is the pseudo-code of this function:

```
static void MBDrv_USART_ClearFrameError() {
    Unmark the internal frame error flag.
}
```

#### (ISR) MBDrv_USART_ISR_HalfCharacterTimeExceed()

This function should be called when half-character timer exceeds.

Here is the pseudo-code of the ISR handler:

```
if (Device is opened) {
    Let cb = Saved 'p_drvcb'.
    Let ctx = Saved 'p_mbctx'.
    cb->halfCharacterTimeExceed(
	    &(MBDRV_USART_DRIVERDESC),
	    ctx
    );
}
```

#### (ISR) MBDrv_USART_ISR_RxComplete()

This function should be called by the RX complete ISR.

Here is the pseudo-code of the ISR handler:

```
if (Device is opened) {
    Let cb = Saved 'p_drvcb'.
    Let ctx = Saved 'p_mbctx'.
    cb->rxComplete(
	    &(MBDRV_USART_DRIVERDESC),
	    ctx
    );
}
```

#### (ISR) MBDrv_USART_ISR_TxComplete()

This function should be called by the TX complete ISR.

Here is the pseudo-code of the ISR handler:

```
if (Device is opened) {
    Let cb = Saved 'p_drvcb'.
    Let ctx = Saved 'p_mbctx'.
    cb->txComplete(
	    &(MBDRV_USART_DRIVERDESC),
	    ctx
    );
}
```

