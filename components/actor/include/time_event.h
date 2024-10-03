/**
 * @file time_event.h
 * @author Matt Richardson (matt.richardson@msrconsults.com)
 * @brief API for time events in the actor framework
 * @version 0.1
 * @date 2024-09-30
 *
 * @copyright Copyright (c) 2024
 *
 *
 * MIT License
 *
 * Copyright (c) [YEAR] [YOUR NAME OR ORGANIZATION]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "actor.h"

#include <stdint.h>

#pragma once

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

typedef struct time_event_s {
    actor_msg_t super;
    actor_t *actor;
    uint32_t timeout;
    uint32_t interval;
} time_event_t;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/**
 * @brief Constructor for a timer event.
 *
 * This will initialize the current timeout and interval to 0.
 *
 * @param me Timer to register
 * @param sig Signal to send to actor upon timer expiration
 * @param actor Actor to send signal to.
 */
void time_event_ctor(time_event_t * const me, signal_t sig, actor_t *actor);

/**
 * @brief Arm the current timer event
 *
 * @param me Timer to arm
 * @param timeout Unsigned word integer representing initial timeout value
 * @param interval Unsigned word integer representing the following timeout values
 *                 on timer expiry.
 */
void time_event_arm(time_event_t *const me, uint32_t timeout, uint32_t interval);

/**
 * @brief Disarm the specified timer
 *
 * Disarms the timer by settign the timeout value to 0.
 *
 * @param me Specified timer to disarm
 */
void time_event_disarm(time_event_t *const me);

/**
 * @brief Global function to be called on sys tick interrupt
 *
 * In FreeRTOS, this function would be called in `vApplicationTickHook` so
 * that the registered timers are called every system tick.
 */
void time_event_tick(void);