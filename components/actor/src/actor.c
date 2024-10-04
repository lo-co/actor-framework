/**
 * @file actor.c
 * @author Matt Richardson (matt.richardson@msrconsults.com)
 * @brief Implementation for actor code in the actor framework.
 * @version 0.1
 * @date 2024-09-27
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

#include <string.h>

#include <esp_err.h>
#include <esp_log.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TAG "actor"

#define MAX_CHILDREN 10
/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

typedef uint16_t actor_id_t;

typedef struct actor_child_s
{
    actor_id_t child_id;
    actor_t *child;
} actor_child_t;

/**
 * @brief Definition of actor object.
 *
 * Contents are hidden.  Only contents available to the actor are the ones defined
 * on inheritance of the struct.
 *
 */
struct actor_s {
    actor_id_t actor_id;
    TaskHandle_t main_task;     ///< Main actor task that handles incoming messages
    QueueHandle_t msg_queue;    ///< Message queue to send messages
    DispatchHandler dispatch;   ///< Dispatch function for handling messages
    const actor_t *parent;            ///< Reference to the parent; can be NULL
    actor_child_t *children;
    uint8_t child_count;
    // Private actor parameters after this
};

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/**
 * @brief This is the main message loop for the actor object
 *
 * @param pdata Private task data; it will be the instance of the actor object
 */
static void actor_msgloop(void *pdata);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Function Definitions
 ******************************************************************************/

// Described in .h
void actor_ctor(actor_t const * parent, actor_t **me, DispatchHandler dispatch)
{
    if (*me ==NULL)
    {
        *me = (actor_t *)malloc(sizeof(actor_t));
        if (*me == NULL)
        {
            ESP_LOGE(TAG, "Unable to create actor");
            return;
        }
        // Clear allocated memory to avoid garbage values
        memset(*me, 0, sizeof(actor_t));
    }
    (*me)->parent = parent;
    (*me)->dispatch = dispatch;
    (*me)->child_count = 0;
}

// Described in .h
void actor_start(actor_t *const me, uint8_t prio, uint32_t queue_length, uint32_t stack_size)
{
    ESP_LOGI(TAG, "Starting actor at %p with dispatch function %p", me, me->dispatch);
    me->msg_queue = xQueueCreate(queue_length, sizeof(actor_msg_t *));
    ESP_LOGI(TAG, "Queue assigned");

    xTaskCreate(actor_msgloop, NULL, stack_size, me, prio, &me->main_task);
    ESP_LOGI(TAG, "Task created");
}

// Dsecribed in .h
void actor_post(actor_t *const me, actor_msg_t const * const msg)
{
    xQueueSend(me->msg_queue, msg, 0);
}

void actor_add_child(actor_t * const me, actor_t type, DispatchHandler dispatch)
{
    if (me->child_count >= MAX_CHILDREN)
    {
        ESP_LOGE(TAG, "Unable to add child.");
        return;
    }

    actor_ctor(me, &me->children[me->child_count].child, dispatch);
}


// Described above
static void actor_msgloop(void *pdata)
{
    actor_t *me = (actor_t *)pdata;

    ESP_LOGI(TAG, "Starting message loop with actor at %p with dispatch at %p ", me, me->dispatch);

    // Initialize the actor object
    static actor_msg_t const initMsg = {INIT_SIG};

    (me->dispatch)(me, &initMsg);
    ESP_LOGI(TAG, "Message dispatched");

    while (1)
    {
        // Begin receiving and handling messages
        // Wait forever for a message.
        actor_msg_t msg;
        esp_err_t err = xQueueReceive(me->msg_queue, &msg, portMAX_DELAY);

        (me->dispatch)(me, &msg);
    }
}

