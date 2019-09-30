/*
*********************************************************************************************************
*                                          MODBUS COMMUNICATION
*                                            OS ABSTRACT LAYER
*
*
*                           (c) Copyright 2019; XiaoJSoft Studio.;
*                    All rights reserved.  Protected by international copyright laws.
*
*                                             uC/OS-III Port
*
* File      : MB_OS.C
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

#define MBOS_SOURCE
#include <bsp.h>
#include <util/delay.h>

#include <mb_os.h>
#include <mb_constants.h>
#include <mb_os_basetypes.h>
#include <mb_os_cfg.h>
#include <mb_types.h>

#include <mbport_limits.h>

#include <cpu.h>

#include <lib_def.h>

#include <os.h>


/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void MBOS_TimerExpireCallback(void *p_tmr, void *p_arg);


/*
*********************************************************************************************************
*                                    MBOS_GetMaxTimeValue()
*
* Description : Get the maximum allowed value of a timespan variable.
*
* Argument(s) : None.
*
* Return(s)   : The maximum value.
*********************************************************************************************************
*/

MB_TIMESPAN MBOS_GetMaxTimeValue() {
    MB_TIMESPAN r;

    /*
     *  Maximum timespan (tsmax, unit: millisecond) is calculated through 
     *  following formula:
     *
     *                           MB_TIMESPAN_MAX
     *    tsmax = Math.floor (---------------------)
     *                         OS_CFG_TICK_RATE_HZ
     * 
     */
    r  = MB_TIMESPAN_MAX;
    r /= (MB_TIMESPAN)OS_CFG_TICK_RATE_HZ;

    return r;
}


/*
*********************************************************************************************************
*                                    MBOS_TimeToTickCount()
*
* Description : Convert timespan to system tick count.
*
* Argument(s) : (1) time      The timespan (unit: millisecond).
*               (2) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE              No error occurred.
*                                 MB_ERROR_OVERFLOW          'time' parameter exceeds.
*
* Return(s)   : The tick count.
*
* Note(s)     : (1) The value of 'time' parameter must not be larger than the value returned by 
*                   MBOS_GetMaxTimeValue().
*********************************************************************************************************
*/

MB_SYSTICK  MBOS_TimeToTickCount(
    MB_TIMESPAN  time,
    MB_ERROR    *p_error
) {
    MB_TIMESPAN  r;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (time > MBOS_GetMaxTimeValue()) {
        *p_error = MB_ERROR_OVERFLOW;
        return (MB_SYSTICK)0;
    }
#endif

    /*
     *  Tick count is calculated through following formula:
     *
     *                        time * OS_CFG_TICK_RATE_HZ
     *    ticks = Math.ceil (----------------------------)
     *                                  10^3
     * 
     */
    r  = time;
    r *= (MB_TIMESPAN)OS_CFG_TICK_RATE_HZ;
    r -= (MB_TIMESPAN)1U;
    r /= (MB_TIMESPAN)1000U;
    r += (MB_TIMESPAN)1U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return (MB_SYSTICK)r;
}


/*
*********************************************************************************************************
*                                    MBOS_TickCountToTime()
*
* Description : Convert system tick count to time.
*
* Argument(s) : (1) ticks     The system tick count.
*               (2) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE              No error occurred.
*
* Return(s)   : The time (unit: milliseconds).
*
* Note(s)     : (1) No overflow check is applied in this function.
*               (2) Generally, if the value of 'ticks' comes from the return value of MBOS_TimeToTickCount(), 
*                   there would be no overflow.
*********************************************************************************************************
*/

MB_TIMESPAN  MBOS_TickCountToTime(
    MB_SYSTICK    ticks,
    MB_ERROR     *p_error
) {
    MB_TIMESPAN   r;

    /*
     *  Time is calculated through following formula:
     *
     *                          ticks * 10^3
     *    time = Math.ceil (---------------------)
     *                       OS_CFG_TICK_RATE_HZ
     * 
     */
    r  = (MB_TIMESPAN)ticks;
    r *= (MB_TIMESPAN)1000U;
    r -= (MB_TIMESPAN)1U;
    r /= (MB_TIMESPAN)OS_CFG_TICK_RATE_HZ;
    r += (MB_TIMESPAN)1U;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return r;
}


/*
*********************************************************************************************************
*                                    MBOS_GetTickCount()
*
* Description : Get current system tick count.
*
* Argument(s) : (1) p_error  Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_OS_TIME_FAILEDGET       Failed to get the tick count.
*
* Return(s)   : The tick count.
*********************************************************************************************************
*/

MB_SYSTICK  MBOS_GetTickCount(
    MB_ERROR    *p_error
) {
    OS_ERR       errOS;

    OS_TICK      ticks;

    /*  Get the tick count.  */
    ticks = OSTimeGet(&errOS);
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_TIME_FAILEDGET;
        return (MB_SYSTICK)0U;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;

    return ticks;
}


/*
*********************************************************************************************************
*                                    MBOS_MutexCreate()
*
* Description : Create a mutex object.
*
* Argument(s) : (1) p_mutex  Pointer to the mutex object.
*               (2) p_error  Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_mutex' points to NULL.
*                                 MB_ERROR_OS_MUTEX_FAILEDCREATE   Failed to create the mutex object.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBOS_MutexCreate(
    MB_MUTEX *p_mutex,
    MB_ERROR *p_error
) {
    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_mutex' parameter.  */
    if (p_mutex == (MB_MUTEX*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Create the mutex object.  */
    OSMutexCreate(
        &(p_mutex->mutex),
        (CPU_CHAR*)0,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_MUTEX_FAILEDCREATE;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_MutexDispose()
*
* Description : Dispose a mutex object.
*
* Argument(s) : (1) p_mutex  Pointer to the mutex object.
*               (2) p_error  Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_mutex' points to NULL.
*                                 MB_ERROR_OS_MUTEX_FAILEDDISPOSE  Failed to dispose the mutex object.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBOS_MutexDispose(
    MB_MUTEX *p_mutex,
    MB_ERROR *p_error
) {
    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_mutex' parameter.  */
    if (p_mutex == (MB_MUTEX*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Dispose the mutex object.  */
    OSMutexDel(
        &(p_mutex->mutex),
        OS_OPT_DEL_ALWAYS,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_MUTEX_FAILEDDISPOSE;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_MutexPend()
*
* Description : Acquire specified mutex.
*
* Argument(s) : (1) p_mutex  Pointer to the mutex object.
*               (2) timeout  The timeout value (unit: millisecond).
*               (3) p_error  Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_mutex' points to NULL.
*                                 MB_ERROR_OVERFLOW                'timeout' parameter exceeds.
*                                 MB_ERROR_TIMEOUT                 Failed to acquire the mutex object due 
*                                                                  to the expiration of the timeout limit.
*                                 MB_ERROR_OS_MUTEX_FAILEDPEND     Failed to acquire the mutex object due 
*                                                                  to other reasons.
*
* Return(s)   : None.
*
* Note(s)     : (1) The value of 'timeout' parameter must not be larger than the value returned by 
*                   MBOS_GetMaxTimeValue().
*               (2) If caller want to wait infinitely, set 'timeout' to 0U.
*********************************************************************************************************
*/

void MBOS_MutexPend(
    MB_MUTEX      *p_mutex,
    MB_TIMESPAN    timeout,
    MB_ERROR      *p_error
) {
    MB_SYSTICK  timeoutTicks;
    OS_ERR    errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_mutex' parameter.  */
    if (p_mutex == (MB_MUTEX*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Convert timespan to system tick count.  */
    if (timeout == (MB_TIMESPAN)0) {
        timeoutTicks = (MB_SYSTICK)0;
    } else {
        timeoutTicks = MBOS_TimeToTickCount(timeout, p_error);
        if (*p_error != MB_ERROR_NONE) {
            return;
        }
    }

    /*  Acquire the mutex object.  */
    OSMutexPend(
        &(p_mutex->mutex),
        (OS_TICK)timeoutTicks,
        OS_OPT_PEND_BLOCKING,
        (CPU_TS*)0,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        if (errOS == OS_ERR_TIMEOUT) {
            *p_error = MB_ERROR_TIMEOUT;
        } else {
            *p_error = MB_ERROR_OS_MUTEX_FAILEDPEND;
        }
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_MutexPost()
*
* Description : Release specified mutex.
*
* Argument(s) : (1) p_mutex  Pointer to the mutex object.
*               (2) p_error  Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_mutex' points to NULL.
*                                 MB_ERROR_OS_MUTEX_FAILEDPOST     Failed to release the mutex object.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBOS_MutexPost(
    MB_MUTEX  *p_mutex,
    MB_ERROR  *p_error
) {
    OS_ERR    errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_mutex' parameter.  */
    if (p_mutex == (MB_MUTEX*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Release the mutex object.  */
    OSMutexPost(
        &(p_mutex->mutex),
        OS_OPT_POST_NONE,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_MUTEX_FAILEDPOST;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_FlagGroupCreate()
*
* Description : Create a flag group object.
*
* Argument(s) : (1) p_grp    Pointer to the flag group object.
*               (2) initial  The initial value of the flag group.
*               (3) p_error  Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_grp' points to NULL.
*                                 MB_ERROR_OS_FGRP_FAILEDCREATE    Failed to create the flag group object.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBOS_FlagGroupCreate(
    MB_FLAGGROUP  *p_grp,
    MB_FLAGS       initial,
    MB_ERROR      *p_error
) {
    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_grp' parameter.  */
    if (p_grp == (MB_FLAGGROUP*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Create a flag group.  */
    OSFlagCreate(
        &(p_grp->fgrp),
        (CPU_CHAR*)0,
        (OS_FLAGS)initial,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_FGRP_FAILEDCREATE;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_FlagGroupDispose()
*
* Description : Delete a flag group object.
*
* Argument(s) : (1) p_grp    Pointer to the flag group object.
*               (2) p_error  Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_grp' points to NULL.
*                                 MB_ERROR_OS_FGRP_FAILEDDISPOSE   Failed to dispose the flag group object.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBOS_FlagGroupDispose(
    MB_FLAGGROUP  *p_grp,
    MB_ERROR      *p_error
) {
    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_grp' parameter.  */
    if (p_grp == (MB_FLAGGROUP*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Delete the flag group.  */
    OSFlagDel(
        &(p_grp->fgrp),
        OS_OPT_DEL_ALWAYS,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_FGRP_FAILEDDISPOSE;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_FlagGroupPend()
*
* Description : Wait for certain bits in a flag group to be set (or cleared).
*
* Argument(s) : (1) p_grp    Pointer to the flag group object.
*               (2) p_flags  Pointer to the variable that stores the bit pattern indicating which 
*                            bit(s) to check and receives the flags that caused current task to be 
*                            ready-to-run.
*               (3) timeout  The timeout value (unit: millisecond).
*               (4) opt      The options:
*
*                                 MB_FLAGGROUP_OPT_CLR_ALL         Check all bits in flags to be cleared (0).
*                                 MB_FLAGGROUP_OPT_CLR_ANY         Check any bit in flags to be cleared (0).
*                                 MB_FLAGGROUP_OPT_SET_ALL         Check all bits in flags to be set (1).
*                                 MB_FLAGGROUP_OPT_SET_ANY         Check any bit in flags to be set (1).
*
*                            The caller may also specify whether the flags are comsumed by "adding"
*                            following option to the 'opt' parameter:
*
*                                 MB_FLAGGROUP_OPT_CONSUME
*
*               (5) p_error  Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_grp' or 'p_flags' points to NULL.
*                                 MB_ERROR_INVALIDPARAMETER        'opt' parameter contains invalid option(s).
*                                 MB_ERROR_OVERFLOW                'timeout' parameter exceeds.
*                                 MB_ERROR_OS_FGRP_FAILEDPEND      Failed to pend on the flag group object.
*                                 MB_ERROR_OS_FGRP_FAILEDRDYFLAGS  Failed to get the flags that make current task 
*                                                                  ready-to-run.
*
* Return(s)   : None.
*
* Note(s)     : (1) The value of 'timeout' parameter must not be larger than the value returned by 
*                   MBOS_GetMaxTimeValue().
*               (2) If caller want to wait infinitely, set 'timeout' to 0U.
*********************************************************************************************************
*/

void MBOS_FlagGroupPend(
    MB_FLAGGROUP  *p_grp,
    MB_FLAGS      *p_flags,
    MB_TIMESPAN    timeout,
    MB_OPT         opt,
    MB_ERROR      *p_error
) {
    OS_FLAGS    rdyFlags;
    MB_SYSTICK  timeoutTicks;

    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_grp' parameter.  */
    if (p_grp == (MB_FLAGGROUP*)0) {
        /*  Error: Null reference.  */
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'p_flags' parameter.  */
    if (p_flags == (MB_FLAGS*)0) {
        /*  Error: Null reference.  */
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'opt' parameter.  */
    switch (opt) {
        case MB_FLAGGROUP_OPT_CLR_ALL:
        case MB_FLAGGROUP_OPT_CLR_ANY:
        case MB_FLAGGROUP_OPT_SET_ALL:
        case MB_FLAGGROUP_OPT_SET_ANY:
        case (MB_FLAGGROUP_OPT_CONSUME | MB_FLAGGROUP_OPT_CLR_ALL):
        case (MB_FLAGGROUP_OPT_CONSUME | MB_FLAGGROUP_OPT_CLR_ANY):
        case (MB_FLAGGROUP_OPT_CONSUME | MB_FLAGGROUP_OPT_SET_ALL):
        case (MB_FLAGGROUP_OPT_CONSUME | MB_FLAGGROUP_OPT_SET_ANY):
            break;
        default:
            /*  Error: Invalid 'opt' parameter.  */
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return;
    }
#endif

    /*  Convert timespan to system tick count.  */
    if (timeout == (MB_TIMESPAN)0) {
        timeoutTicks = (MB_SYSTICK)0;
    } else {
        timeoutTicks = MBOS_TimeToTickCount(timeout, p_error);
        if (*p_error != MB_ERROR_NONE) {
            return;
        }
    }

    /*  Wait for the flag group object.  */
    OSFlagPend(
        &(p_grp->fgrp),
        (OS_FLAGS)(*p_flags),
        (OS_TICK)timeoutTicks,
        (OS_OPT)opt,
        (CPU_TS*)0,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_FGRP_FAILEDPEND;
        return;
    }

    /*  Read the flags that makes the task ready.  */
    rdyFlags = OSFlagPendGetFlagsRdy(&errOS);
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_FGRP_FAILEDRDYFLAGS;
        return;
    }
    *p_flags = (MB_FLAGS)rdyFlags;

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_FlagGroupPost()
*
* Description : Set (or clear) certain bits in a flag group.
*
* Argument(s) : (1) p_grp    Pointer to the flag group object.
*               (2) flags    Flags that specifies which bit(s) to be set or cleared.
*               (3) opt      The options:
*
*                                 MB_FLAGGROUP_OPT_SET             Set selected bit(s).
*                                 MB_FLAGGROUP_OPT_CLR             Clear selected bit(s).
*
*               (4) p_error  Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_grp' points to NULL.
*                                 MB_ERROR_INVALIDPARAMETER        'opt' parameter contains invalid option(s).
*                                 MB_ERROR_OS_FGRP_FAILEDPOST      Failed to post to the flag group object.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void MBOS_FlagGroupPost(
    MB_FLAGGROUP  *p_grp,
    MB_FLAGS       flags,
    MB_OPT         opt,
    MB_ERROR      *p_error
) {
    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_grp' parameter.  */
    if (p_grp == (MB_FLAGGROUP*)0) {
        /*  Error: Null reference.  */
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'opt' parameter.  */
    switch (opt) {
        case MB_FLAGGROUP_OPT_SET:
        case MB_FLAGGROUP_OPT_CLR:
            break;
        default:
            /*  Error: Invalid 'opt' parameter.  */
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return;
    }
#endif

    /*  Post to the flag group object.  */
    OSFlagPost(
        &(p_grp->fgrp),
        (OS_FLAGS)flags,
        (OS_OPT)opt,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_FGRP_FAILEDPOST;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_TimerCreate()
*
* Description : Create a timer object.
*
* Argument(s) : (1) p_tmr     Pointer to the timer object.
*               (2) interval  Interval of the timer (must be larger than 0, unit: millisecond).
*               (3) mode      Timer mode, with one of following variables:
*
*                                 MB_TIMER_MODE_ONESHOT             One-shot mode.
*                                 MB_TIMER_MODE_PERIODIC            Periodic mode.
*
*               (4) cb        Timer callback.
*               (5) p_cbarg   Timer callback argument that would be passed to the 'p_arg' parameter of the 
*                             timer callback.
*
*               (6) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_tmr' or 'cb' points to NULL.
*                                 MB_ERROR_UNDERFLOW               'interval' equals to zero.
*                                 MB_ERROR_OVERFLOW                'interval' exceeds.
*                                 MB_ERROR_INVALIDPARAMETER        'mode' contains invalid value.
*                                 MB_ERROR_OS_TIMER_FAILEDCREATE   Failed to create the timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) The value of 'interval' parameter must not be larger than the value returned by 
*                   MBOS_GetMaxTimeValue().
*               (2) The value of 'interval' parameter should at least be 1.
*********************************************************************************************************
*/

void MBOS_TimerCreate(
    MB_TIMER          *p_tmr,
    MB_TIMESPAN        interval,
    MB_TIMERMODE       mode,
    MB_TIMERCALLBACK   cb,
    void              *p_cbarg,
    MB_ERROR          *p_error
) {
    OS_OPT   tmrOpt;

    OS_TICK  tmrDelay;
    OS_TICK  tmrPeriod;

    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_tmr' parameter.  */
    if (p_tmr == (MB_TIMER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }

    /*  Check 'interval' parameter.  */
    if (interval == (MB_TIMESPAN)0) {
        *p_error = MB_ERROR_UNDERFLOW;
        return;
    }

    /*  Check 'mode' parameter.  */
    switch (mode) {
        case MB_TIMER_MODE_ONESHOT:
        case MB_TIMER_MODE_PERIODIC:
            break;
        default:
            *p_error = MB_ERROR_INVALIDPARAMETER;
            return;
    }

    /*  Check 'cb' parameter.  */
    if (cb == (MB_TIMERCALLBACK)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Convert timespan to system tick count.  */
    tmrDelay = (OS_TICK)MBOS_TimeToTickCount(interval, p_error);
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Save the timer context.  */
    p_tmr->cb = cb;
    p_tmr->cb_arg = p_cbarg;

    /*  Get other several timer creation parameters.  */
    if (mode == MB_TIMER_MODE_PERIODIC) {
        tmrOpt = OS_OPT_TMR_PERIODIC;
        tmrPeriod = tmrDelay;
    } else {
        tmrOpt = OS_OPT_TMR_ONE_SHOT;
        tmrPeriod = (OS_TICK)0;
    }

    /*  Create a timer.  */
    OSTmrCreate(
        &(p_tmr->tmr),
        (CPU_CHAR*)0,
        tmrDelay,
        tmrPeriod,
        tmrOpt,
        MBOS_TimerExpireCallback,
        (void*)p_tmr,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_TIMER_FAILEDCREATE;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_TimerDispose()
*
* Description : Dispose a timer object.
*
* Argument(s) : (1) p_tmr     Pointer to the timer object.
*               (2) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_tmr' points to NULL.
*                                 MB_ERROR_OS_TIMER_FAILEDDISPOSE  Failed to dispose the timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) The timer to be disposed would be stopped immediately.
*********************************************************************************************************
*/

void MBOS_TimerDispose(
    MB_TIMER  *p_tmr,
    MB_ERROR  *p_error
) {
    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_tmr' parameter.  */
    if (p_tmr == (MB_TIMER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Delete the timer.  */
    OSTmrDel(
        &(p_tmr->tmr),
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_TIMER_FAILEDDISPOSE;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_TimerStart()
*
* Description : Start a timer object.
*
* Argument(s) : (1) p_tmr     Pointer to the timer object.
*               (2) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_tmr' points to NULL.
*                                 MB_ERROR_OS_TIMER_FAILEDSTART    Failed to start the timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) If the timer is already started, the timer would be restarted immediately.
*********************************************************************************************************
*/

void MBOS_TimerStart(
    MB_TIMER  *p_tmr,
    MB_ERROR  *p_error
) {
    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_tmr' parameter.  */
    if (p_tmr == (MB_TIMER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Start the timer.  */
    OSTmrStart(
        &(p_tmr->tmr),
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_TIMER_FAILEDSTART;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_TimerStop()
*
* Description : Stop a timer object.
*
* Argument(s) : (1) p_tmr     Pointer to the timer object.
*               (2) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                    No error occurred.
*                                 MB_ERROR_NULLREFERENCE           'p_tmr' points to NULL.
*                                 MB_ERROR_OS_TIMER_FAILEDSTOP     Failed to stop the timer object.
*
* Return(s)   : None.
*
* Note(s)     : (1) If the timer is not started yet, the function would return without throwing any error.
*********************************************************************************************************
*/

void MBOS_TimerStop(
    MB_TIMER  *p_tmr,
    MB_ERROR  *p_error
) {
    OS_ERR errOS;

#if (MB_CFG_ARG_CHK_EN == DEF_ENABLED)
    /*  Check 'p_tmr' parameter.  */
    if (p_tmr == (MB_TIMER*)0) {
        *p_error = MB_ERROR_NULLREFERENCE;
        return;
    }
#endif

    /*  Stop the timer.  */
    OSTmrStop(
        &(p_tmr->tmr),
        OS_OPT_TMR_NONE,
        (void*)0,
        &errOS
    );
    switch (errOS) {
        case OS_ERR_NONE:
        case OS_ERR_TMR_STOPPED:
            break;
        default:
            *p_error = MB_ERROR_OS_TIMER_FAILEDSTOP;
            return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                    MBOS_Delay()
*
* Description : Delay specific time.
*
* Argument(s) : (1) time      The timespan (unit: millisecond).
*               (2) p_error   Pointer to the variable that receives error code from this function:
*
*                                 MB_ERROR_NONE                 No error occurred.
*                                 MB_ERROR_OVERFLOW             'time' parameter exceeds.
*                                 MB_ERROR_OS_TIME_FAILEDDELAY  OS failed to delay.
*
* Return(s)   : None.
*
* Note(s)     : (1) The value of 'time' parameter must not be larger than the value returned by 
*                   MBOS_GetMaxTimeValue().
*               (2) Zero delay time is allowed. The function would return (without throwing any 
*                   error) if the value of 'time' parameter is zero.
*********************************************************************************************************
*/

void MBOS_Delay(
    MB_TIMESPAN   time,
    MB_ERROR     *p_error
) {
    OS_ERR        errOS;
    OS_TICK       ticks;

    /*  Fast path for no delay.  */
    if (time == (MB_TIMESPAN)0U) {
        return;
    }

    /*  Convert the timespan to tick count.  */
    ticks = (OS_TICK)MBOS_TimeToTickCount(
        time,
        p_error
    );
    if (*p_error != MB_ERROR_NONE) {
        return;
    }

    /*  Delay.  */
    OSTimeDly(
        ticks,
        OS_OPT_TIME_DLY,
        &errOS
    );
    if (errOS != OS_ERR_NONE) {
        *p_error = MB_ERROR_OS_TIME_FAILEDDELAY;
        return;
    }

    /*  No error.  */
    *p_error = MB_ERROR_NONE;
}


/*
*********************************************************************************************************
*                                MBOS_TimerExpireCallback()
*
* Description : Timer expiration callback.
*
* Argument(s) : (1) p_tmr     Pointer to the timer object (an OS_TMR object).
*               (2) p_arg     Callback argument.
*
* Return(s)   : None.
*
* Note(s)     : (1) Interrupts are assumed to be disabled when calling this function.
*********************************************************************************************************
*/

static void MBOS_TimerExpireCallback(void *p_tmr, void *p_arg) {
    MB_TIMER *mbtmr = (MB_TIMER*)p_arg;
    mbtmr->cb(p_arg, mbtmr->cb_arg);
}
