/*********************************************************************************************************************
Copyright (c) Año, Nombre y Apellido del autor <correo@ejemplo.com>

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

/** @file display.c
 ** @brief Implementacion de la tarea de pantalla
 **/

/* === Headers files inclusions ==================================================================================== */

#include "display.h"

/* === Macros definitions ========================================================================================== */

#define REFRESH 1

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void Refresh_Task(void * args) {
    refresh_args_t parameters = args;
    TickType_t duration;
    TickType_t last_time = xTaskGetTickCount();
    duration = pdMS_TO_TICKS(1);
    while (true) {
        if (xSemaphoreTake(parameters->screen_mutex, portMAX_DELAY)) {
            ScreenRefresh(parameters->board->screen);
        }
        xSemaphoreGive(parameters->screen_mutex);
        vTaskDelayUntil(&last_time, duration);
    }
}

/* === End of documentation ======================================================================================== */
