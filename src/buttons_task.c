/*********************************************************************************************************************
Copyright (c) 2025, Lucas Ahumada Checa Casquero <lucasahum@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

/** @file buttons_task.c
 ** @brief Implementacion de la tarea de los botones
 **/

/* === Headers files inclusions ==================================================================================== */

#include "buttons_task.h"

/* === Macros definitions ========================================================================================== */

#define DELAY_BUTTON 100
#define HOLS_PRESS_TIME 3000 // [ms]

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void ButtonPushTask(void * args) {

    button_args_t parameters = args;

    while (1) {
        if (DigitalInputWasActivated(parameters->button)) {
            xEventGroupSetBits(parameters->clock_events, parameters->event_push);

            // Prueba visual: escribir "01" en pantalla al detectar pulsación corta
            uint8_t short_press_val[2] = {0, 1};
            ScreenWriteBCD(parameters->screen, short_press_val, 2);
        }
        vTaskDelay(pdMS_TO_TICKS(DELAY_BUTTON));
    }
}

void ButtonHoldTask(void * args) {
    button_args_t parameters = args;
    bool is_pressed  = false;
    bool event_triggered  = false;
    TickType_t press_start  = 0;
    while (1) {
        if (DigitalInputGetIsActive(parameters->button)) {
            if (!is_pressed) {
                is_pressed = true;
                press_start = xTaskGetTickCount();
                event_triggered = false;
            } else if (!event_triggered && (xTaskGetTickCount() - press_start >= pdMS_TO_TICKS(HOLS_PRESS_TIME))) {
                xEventGroupSetBits(parameters->clock_events, parameters->event_hold);
                event_triggered = true;

                // Prueba visual: escribir "99" en pantalla al detectar pulsación larga
                uint8_t long_press_val[2] = {9, 9};
                ScreenWriteBCD(parameters->screen, long_press_val, 2);
            }
        } else {
            is_pressed = false;
        }
        vTaskDelay(pdMS_TO_TICKS(DELAY_BUTTON));
    }
}

/* === End of documentation ======================================================================================== */
