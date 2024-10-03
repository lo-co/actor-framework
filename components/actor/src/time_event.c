/**
 * @file time_event.c
 * @author Matt Richardson (matt.richardson@msrconsults.com)
 * @brief Implementation for time events in the actor framework
 * @version 0.1
 * @date 2024-09-30
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

#include "time_event.h"

#include <assert.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** Maximum number of time event objects */
#define MAX_NUMBER_EVENTS 10

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/** Static array of time event objects */
static time_event_t *l_time_event[MAX_NUMBER_EVENTS];

/** Number of registered time event objects */
static uint8_t l_num_events = 0;

/*******************************************************************************
 * Function Definitions
 ******************************************************************************/

// Described in .h
void time_event_ctor(time_event_t * const me, signal_t sig, actor_t *actor)
{
    me->super.sig = sig;
    me->actor = actor;
    me->timeout = 0;
    me->interval = 0;

    assert(l_num_events < sizeof(l_time_event)/sizeof(l_time_event[0]));
    l_time_event[l_num_events] = me;
    l_num_events++;
}

// Described in .h
void time_event_arm(time_event_t *const me, uint32_t timeout, uint32_t interval)
{
    me->timeout = timeout;
    me->interval = interval;
}

// Described in .h
void time_event_disarm(time_event_t *const me)
{
    me->timeout = 0;
}

// Described in .h
void time_event_tick(void)
{
    // Loop through all registered timers
    for (uint8_t current_timer = 0; current_timer < l_num_events; current_timer++)
    {
        time_event_t * const t = l_time_event[current_timer];
        assert(t);

        if (t->timeout > 0)
        {
            // Timer is currently armed
            if (--t->timeout == 0)
            {
                // Timer expired this tick - fire event
                actor_post(t->actor, &t->super);

                // Rearm if configured for this
                t->timeout = t->interval;
            }
        }

    }

}