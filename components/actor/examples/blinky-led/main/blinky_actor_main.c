/**
 * @file blinky_actor_main.c
 * @author Matt Richardson (matt.richardson@msrconsults.com)
 * @brief
 * @version 0.1
 * @date 2024-09-30
 *
 * @copyright Copyright (c) 2024
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
 *
 */

#include <stdio.h>
#include <stdbool.h>

#include "actor.h"
#include "time_event.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp_log.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TAG "main_app"

#define ON 1

#define OFF 0

#define BLINK_GPIO GPIO_NUM_2

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/** Actor object that inherits from the base actor object */
typedef struct blinky_actor_s {
    actor_t super;          ///< Base class
    bool current_state;     ///< Current state of the led
} blinky_actor_t;

/** Signal events for the blinking LED */
enum blinky_signals {
    BLINK_SIG = USER_SIG,    ///< Blink event
};

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/**
 * @brief Dispatch handler for the blinky actor
 *
 * @param me Instance of blinky actor
 * @param msg Incoming message for the blinky actor to service
 */
void blinky_message_handler(actor_t * const me, actor_msg_t const * const msg);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/** Blinky actor object instance */
static blinky_actor_t blinky_actor = {0};

/** Tick event for blinking */
static time_event_t blink_time_event;

/*******************************************************************************
 * Function Definitions
 ******************************************************************************/

/**
 *
 * @brief Hook for FreeRTOS tick functionality
 *
 * This calls time_event_tick which will service each registered time event.
 */
void vApplicationTickHook(void) {
    time_event_tick();
}

/**
 * @brief Main application entry point used by the ESP IDF
 *
 */
void app_main(void)
{
    ESP_LOGI(TAG, "Starting main...");
    actor_ctor(&blinky_actor.super, blinky_message_handler);
    actor_start((actor_t *)&blinky_actor, 1, 10, 2048);

    time_event_ctor(&blink_time_event, BLINK_SIG, (actor_t *)&blinky_actor);
    time_event_arm(&blink_time_event, 100, 100);
}

// Described above
void blinky_message_handler(actor_t * const me, actor_msg_t const * const msg)
{
    switch (msg->sig)
    {
        case INIT_SIG:
        {
            ESP_LOGI(TAG, "Calling the init event...");
            gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
            gpio_set_level(BLINK_GPIO, OFF);
            blinky_actor.current_state = false;
            break;
        }
        case BLINK_SIG:
        {
            ESP_LOGI(TAG, "Calling the blink event...");
            uint8_t new_state = blinky_actor.current_state ? OFF : ON;
            gpio_set_level(BLINK_GPIO, new_state);
            blinky_actor.current_state = !blinky_actor.current_state;
            break;
        }
        default:
        {
            ESP_LOGW(TAG, "No signal to match current message!");
        }
    }
}