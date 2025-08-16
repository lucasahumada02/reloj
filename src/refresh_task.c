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

/** @file refresh_task.c
 ** @brief Definicion de la tarea refrescar
 **/

/* === Headers files inclusions ==================================================================================== */

#include "refresh_task.h"
#include "time_task.h"
#include <stdbool.h>
#include <stdint.h>


/* === Macros definitions ========================================================================================== */

extern board_t board;
extern clock_t clock;
extern clock_mode_t mode;
extern uint8_t digits[4];
extern SemaphoreHandle_t hourMutex;
extern tick_args_t tick_param;

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

void RefreshTask(void * pointer) {
    const TickType_t kRefreshMs = 1;
    const TickType_t kBlinkMs = 500;
    TickType_t lastFrame = xTaskGetTickCount();
    TickType_t lastBlink = xTaskGetTickCount();
    bool blink = false;
    clock_time_t localHour = {0};

    while (true) {
        xSemaphoreTake(hourMutex, portMAX_DELAY);
        localHour = tick_param->hour;
        xSemaphoreGive(hourMutex);

        if (mode == SHOW_TIME || mode == UNCONFIGURED) {
            clock_convert_time_to_bcd(&localHour, digits);
        }

        TickType_t now = xTaskGetTickCount();
        if ((now - lastBlink) >= pdMS_TO_TICKS(kBlinkMs)) {
            blink = !blink;
            lastBlink = now;
        }

        ScreenWriteBCD(board->screen, digits, sizeof(digits));
        for (int i = 0; i < 4; i++) {
            ScreenSetDot(board->screen, i, false);
        }

        if (mode == SET_ALARM_MINUTES || mode == SET_ALARM_HOURS) {
            ScreenSetDot(board->screen, 0, true);
            ScreenSetDot(board->screen, 1, true);
            ScreenSetDot(board->screen, 2, true);
            ScreenSetDot(board->screen, 3, true);
        }
        if (mode == SHOW_TIME) {
            ScreenSetDot(board->screen, 1, blink);
        }
        if (ClockIsAlarmActive(clock)) {
            ScreenSetDot(board->screen, 0, true);
        }
        if (ClockIsAlarmEnabled(clock)) {
            ScreenSetDot(board->screen, 3, true);
        } else {
            ScreenSetDot(board->screen, 3, false);
        }

        ScreenRefresh(board->screen);
        vTaskDelayUntil(&lastFrame, pdMS_TO_TICKS(kRefreshMs));
    }
}

/* === End of documentation ======================================================================================== */
