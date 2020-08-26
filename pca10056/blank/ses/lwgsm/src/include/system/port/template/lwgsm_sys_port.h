/**
 * \file            lwgsm_sys_port.h
 * \brief           Template file for system functions
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwGSM - Lightweight GSM-AT library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#ifndef LWGSM_HDR_SYSTEM_PORT_H
#define LWGSM_HDR_SYSTEM_PORT_H

#include <stdint.h>
#include <stdlib.h>
#include "lwgsm/lwgsm_opt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \addtogroup      LWGSM_SYS
 * \{
 */

#if LWGSM_CFG_OS || __DOXYGEN__

/* Include any OS specific features */
#include "cmsis_os.h"



/// \details Thread ID identifies the thread.
typedef void *osThreadId_t;

/// \details Timer ID identifies the timer.
typedef void *osTimerId_t;

/// \details Event Flags ID identifies the event flags.
typedef void *osEventFlagsId_t;

/// \details Mutex ID identifies the mutex.
typedef void *osMutexId_t;

/// \details Semaphore ID identifies the semaphore.
typedef void *osSemaphoreId_t;

/// \details Memory Pool ID identifies the memory pool.
typedef void *osMemoryPoolId_t;

/// \details Message Queue ID identifies the message queue.
typedef void *osMessageQueueId_t;

/**
 * \brief           System mutex type
 *
 * It is used by middleware as base type of mutex.
 */
typedef osMutexId_t         lwgsm_sys_mutex_t;

/**
 * \brief           System semaphore type
 *
 * It is used by middleware as base type of mutex.
 */
typedef osSemaphoreId_t     lwgsm_sys_sem_t;

/**
 * \brief           System message queue type
 *
 * It is used by middleware as base type of mutex.
 */
typedef osMessageQueueId_t  lwgsm_sys_mbox_t;

/**
 * \brief           System thread ID type
 */
typedef osThreadId_t        lwgsm_sys_thread_t;

/**
 * \brief           System thread priority type
 *
 * It is used as priority type for system function,
 * to start new threads by middleware.
 */
typedef osPriority          lwgsm_sys_thread_prio_t;

/**
 * \brief           Mutex invalid value
 *
 * Value assigned to \ref lwgsm_sys_mutex_t type when it is not valid.
 */
#define LWGSM_SYS_MUTEX_NULL          ((lwgsm_sys_mutex_t)0)

/**
 * \brief           Semaphore invalid value
 *
 * Value assigned to \ref lwgsm_sys_sem_t type when it is not valid.
 */
#define LWGSM_SYS_SEM_NULL            ((lwgsm_sys_sem_t)0)

/**
 * \brief           Message box invalid value
 *
 * Value assigned to \ref lwgsm_sys_mbox_t type when it is not valid.
 */
#define LWGSM_SYS_MBOX_NULL           ((lwgsm_sys_mbox_t)0)

/**
 * \brief           OS timeout value
 *
 * Value returned by operating system functions (mutex wait, sem wait, mbox wait)
 * when it returns timeout and does not give valid value to application
 */
#define LWGSM_SYS_TIMEOUT             ((uint32_t)osWaitForever)

/**
 * \brief           Default thread priority value used by middleware to start built-in threads
 *
 * Threads can well operate with normal (default) priority and do not require
 * any special feature in terms of priority for prioer operation.
 */
#define LWGSM_SYS_THREAD_PRIO         (osPriorityNormal)

/**
 * \brief           Stack size in units of bytes for system threads
 *
 * It is used as default stack size for all built-in threads.
 */
#define LWGSM_SYS_THREAD_SS           (1024)

#endif /* LWGSM_CFG_OS || __DOXYGEN__ */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWGSM_HDR_SYSTEM_PORT_H */
