/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                              DEVICE DRIVER
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
*
* File      : MBDRV_TYPES.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MBDRV_TYPES_H__
#define MBDRV_TYPES_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mbdrv_cfg.h>

#include <mb_types.h>

#include <cpu.h>

#include <lib_def.h>


/*
*********************************************************************************************************
*                                          TYPE DEFINITIONS
*********************************************************************************************************
*/

/*  Device serial setup type.  */
typedef struct mb_serial_setup {
    MB_BAUDRATE  baudrate;
    MB_DATABITS  dataBits;
    MB_STOPBITS  stopBits;
    MB_PARITY    parity;
} MB_SERIAL_SETUP;

/*  Device driver type.  */
typedef struct mb_driver MB_DRIVER;

/*  Device driver callback type.  */
typedef struct mb_driver_callbacks {
#if (MB_CFG_CORE_RTUMODE == DEF_ENABLED)
    void (*halfCharacterTimeExceed)(MB_DRIVER *mbdrv, void *mbctx);
#endif
    void (*rxComplete)(MB_DRIVER *mbdrv, void *mbctx);
    void (*txComplete)(MB_DRIVER *mbdrv, void *mbctx);
} MB_DRIVER_CALLBACKS;

/*  Device driver descriptor type.  */
typedef struct mb_driver {
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


#endif