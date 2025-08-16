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

/** @file time_task.c
 ** @brief Implementacion del tiempo del reloj
 **/

/* === Headers files inclusions ==================================================================================== */

#include "time_task.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */


void TimeTask(void * args){
    tick_args_t parameters = args;
    TickType_t lastTick = xTaskGetTickCount();
    while(1){
        ClockNewTick(parameters->clock);
        xSemaphoreTake(hourMutex, portMAX_DELAY);
        ClockGetTime(parameters->clock,&parameters->hour);
        xSemaphoreGive(hourMutex);
        vTaskDelayUntil(&lastTick,pdMS_TO_TICKS(1));
    }
}

/* === End of documentation ======================================================================================== */
