/**
 * @file actor.h
 * @author Matt Richardson (matt.richardson@msrconsults.com)
 * @brief API for actor implementation in the actor framework
 * @version 0.1
 * @date 2024-09-27
 *
 * @copyright Copyright (c) 2024
 *
 *
 * MIT License
 *
 * Copyright (c) 2024 MSR Consulting, LLC
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

#include <stdint.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#pragma once

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

typedef uint16_t signal_t;

/** Define reserved signals for use by the framework */
enum ReservedSignals {
    INIT_SIG,   ///< Initialization signal to be used by all actors
    QUIT_SIG,
    USER_SIG,   ///< User signals begin here
};

/** Definition of the base message class */
typedef struct actor_msg_s {
    signal_t sig;
} actor_msg_t;

/** Forward declaration of the actor class */
typedef struct actor_s actor_t;

/** Definition of the dispatch handler for each class */
typedef void (*DispatchHandler)(actor_t * const me, actor_msg_t const * const msg);

/** Definition of actor object */
struct actor_s {
    TaskHandle_t main_task;     ///< Main actor task that handles incoming messages
    QueueHandle_t msg_queue;    ///< Message queue to send messages
    DispatchHandler dispatch;   ///< Dispatch function for handling messages
    // Private actor parameters after this
};

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/**
 * @brief Constructor for the actor object
 *
 * @param me
 * @param dispatch
 */
void actor_ctor(actor_t *const me, DispatchHandler dispatch);

/**
 * @brief Start the actor processes
 *
 *
 * @param me
 * @param prio
 * @param queue_length
 * @param stack_size
 */
void actor_start(actor_t *const me, uint8_t prio, uint32_t queue_length, uint32_t stack_size);

/**
 * @brief
 *
 * @param me
 * @param msg
 */
void actor_post(actor_t *const me, actor_msg_t const * const msg);
