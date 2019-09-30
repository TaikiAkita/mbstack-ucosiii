/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                                 MODULE
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
* File      : MB_UTILITIES.C
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
#define MB_UTILITIES_SOURCE

#include <mb_utilities.h>
#include <mb_cfg.h>
#include <mb_constants.h>
#include <mb_types.h>

#include <mbdrv_types.h>

#include <cpu.h>

#include <lib_def.h>


/*
*********************************************************************************************************
*                                    MBUtil_GetSerialCharacterTime()
*
* Description : Get the time that each character would cost on the serial line (unit: microseconds).
*
* Argument(s) : (1) p_setup  Pointer to the variable that stores the serial port configuration.
*               (2) p_error  Pointer to the variable that receives the error code from this function:
*
*                                MB_ERROR_NONE              No error occurred.
*                                MB_ERROR_NULLREFERENCE     'p_setup' is NULL.
*                                MB_ERROR_INVALIDPARAMETER  Serial port configuration corrupted.
*
* Return(s)   : The character time.
*
* Note(s)     : (1) The return value is ensured to be zero if failed.
*               (2) The return value is ensured to be positive if succeed.
*********************************************************************************************************
*/

CPU_INT32U MBUtil_GetSerialCharacterTime(
    MB_SERIAL_SETUP *p_setup,
    MB_ERROR        *p_error
) {
    CPU_INT32U timeValue;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_setup' parameter.  */
    if (p_setup == (MB_SERIAL_SETUP*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT32U)0U;
    }
#endif

    /*******************************************
     *  Calculate the character time.          *
     *                                         *
     *           [Bits per character] * 10^6   *
     *  tc(us) = ---------------------------   *
     *                    BaudRate             *
     *                                         *
     *******************************************/

    timeValue          = (CPU_INT32U)1000000UL;
    switch (p_setup->dataBits) {
        case MB_SERIAL_DATABITS_8:
            timeValue += (CPU_INT32U)8000000UL;
            break;
        case MB_SERIAL_DATABITS_7:
            timeValue += (CPU_INT32U)7000000UL;
            break;
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return (CPU_INT32U)0U;
    }
    switch (p_setup->parity) {
        case MB_SERIAL_PARITY_NONE:
            break;
        case MB_SERIAL_PARITY_EVEN:
        case MB_SERIAL_PARITY_ODD:
        case MB_SERIAL_PARITY_MARK:
        case MB_SERIAL_PARITY_SPACE:
            timeValue += (CPU_INT32U)1000000UL;
            break;
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return (CPU_INT32U)0U;
    }
    switch (p_setup->stopBits) {
        case MB_SERIAL_STOPBITS_1:
            timeValue += (CPU_INT32U)1000000UL;
            break;
        case MB_SERIAL_STOPBITS_1D5:
            timeValue += (CPU_INT32U)1500000UL;
            break;
        case MB_SERIAL_STOPBITS_2:
            timeValue += (CPU_INT32U)2000000UL;
            break;
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return (CPU_INT32U)0U;
    }
    timeValue -= (CPU_INT32U)1U;
    timeValue /= (CPU_INT32U)(p_setup->baudrate);
    timeValue += (CPU_INT32U)1U;

    return timeValue;
}


/*
*********************************************************************************************************
*                                  MBUtil_GetHalfSerialCharacterTime()
*
* Description : Get the time that half character would cost on the serial line (unit: microseconds).
*
* Argument(s) : (1) p_setup  Pointer to the variable that stores the serial port configuration.
*               (2) p_error  Pointer to the variable that receives the error code from this function:
*
*                                MB_ERROR_NONE              No error occurred.
*                                MB_ERROR_NULLREFERENCE     'p_setup' is NULL.
*                                MB_ERROR_INVALIDPARAMETER  Serial port configuration corrupted.
*
* Return(s)   : The half character time.
*
* Note(s)     : (1) The return value is ensured to be zero if failed.
*               (2) The return value is ensured to be positive if succeed.
*********************************************************************************************************
*/

CPU_INT32U MBUtil_GetHalfSerialCharacterTime(
    MB_SERIAL_SETUP *p_setup,
    MB_ERROR        *p_error
) {
    CPU_INT32U timeValue;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_setup' parameter.  */
    if (p_setup == (MB_SERIAL_SETUP*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return (CPU_INT32U)0U;
    }
#endif

    /*  Calculate the character time.  */
    timeValue = MBUtil_GetSerialCharacterTime(
        p_setup,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        return (CPU_INT32U)0U;
    }

    /*  Divide the character time by 2.  */
    timeValue  -= 1U;
    timeValue >>= 1;
    timeValue  += 1U;

    return timeValue;
}
