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
* File      : MB_OS.H
* Version   : V1.0.320
* By        : Ji WenCong
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef MB_OS_H__
#define MB_OS_H__


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <mb_types.h>
#include <mb_os_types.h>
#include <mbport_limits.h>

#include <cpu.h>

#include <os.h>


#ifdef __cplusplus
extern  "C" {
#endif


/*
**************************************************************************************************************************
*                                                      CONSTANTS
**************************************************************************************************************************
*/

/*  Max value of MB_TIMESPAN type variables.  */
#define  MB_TIMESPAN_MAX              ((MB_TIMESPAN)MBPORT_UINT32_MAX)

/*  Options of MBOS_FlagGroupPend().  */
#define MB_FLAGGROUP_OPT_CLR_ALL    ((MB_OPT)OS_OPT_PEND_FLAG_CLR_ALL)
#define MB_FLAGGROUP_OPT_CLR_ANY    ((MB_OPT)OS_OPT_PEND_FLAG_CLR_ANY)
#define MB_FLAGGROUP_OPT_SET_ALL    ((MB_OPT)OS_OPT_PEND_FLAG_SET_ALL)
#define MB_FLAGGROUP_OPT_SET_ANY    ((MB_OPT)OS_OPT_PEND_FLAG_SET_ANY)
#define MB_FLAGGROUP_OPT_CONSUME    ((MB_OPT)OS_OPT_PEND_FLAG_CONSUME)

/*  Options of MBOS_FlagGroupPost().  */
#define MB_FLAGGROUP_OPT_SET            ((MB_OPT)OS_OPT_POST_FLAG_SET)
#define MB_FLAGGROUP_OPT_CLR            ((MB_OPT)OS_OPT_POST_FLAG_CLR)


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

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

MB_TIMESPAN MBOS_GetMaxTimeValue();


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
);


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
);


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
);


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
);


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
);


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
* Note(s)     : (1) The value of 'tiemout' parameter must not be larger than the value returned by 
*                   MBOS_GetMaxTimeValue().
*********************************************************************************************************
*/

void MBOS_MutexPend(
    MB_MUTEX      *p_mutex,
    MB_TIMESPAN    timeout,
    MB_ERROR      *p_error
);


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
);


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
);


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
);


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
*********************************************************************************************************
*/

void MBOS_FlagGroupPend(
    MB_FLAGGROUP  *p_grp,
    MB_FLAGS      *p_flags,
    MB_TIMESPAN    timeout,
    MB_OPT         opt,
    MB_ERROR      *p_error
);


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
);


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
);


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
);


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
);


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
);


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
);


#ifdef __cplusplus
}
#endif

#endif