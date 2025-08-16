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

/** @file state_task.c
 ** @brief Definicion de la tarea de estados
 **/

/* === Headers files inclusions ==================================================================================== */

#include "state_task.h"

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */


extern board_t board;
extern clock_t clock;
extern clock_mode_t mode;
extern uint8_t digits[4];
extern clock_time_t hour;
extern clock_time_t alarm;
extern SemaphoreHandle_t hourMutex;
extern tick_args_t tick_param;
extern EventGroupHandle_t keyEventGroup;

/* === Private function definitions ================================================================================ */

static void ChangeMode(clock_mode_t value) {
    mode = value;
    switch (mode) {
    case UNCONFIGURED:
        DisplayFlashDigits(board->screen, 0, 3, 150);
        break;
    case SHOW_TIME:
        DisplayFlashDigits(board->screen, 0, 0, 0);
        break;
    case SET_TIME_MINUTES:
        DisplayFlashDigits(board->screen, 2, 3, 150);
        break;
    case SET_TIME_HOURS:
        DisplayFlashDigits(board->screen, 0, 1, 150);
        break;
    case SET_ALARM_MINUTES:
        DisplayFlashDigits(board->screen, 2, 3, 150);
        break;
    case SET_ALARM_HOURS:
        DisplayFlashDigits(board->screen, 0, 1, 150);
        break;
    }
}

/* === Public function implementation ============================================================================== */



void StateTask(void * pointer) {
    (void)pointer;
    EventBits_t events;
    TickType_t last_inactivity = xTaskGetTickCount();
    ChangeMode(UNCONFIGURED);
    while(1) {
        events = xEventGroupWaitBits(
            keyEventGroup, 
            ACCEPT | CANCEL | SET_TIME | SET_ALARM | DECREMENT | INCREMENT,
            pdTRUE,  
            pdFALSE, 
            pdMS_TO_TICKS(100));

        TickType_t now_ticks = xTaskGetTickCount();

        if(events){ 
        if (events & (ACCEPT | CANCEL | DECREMENT | INCREMENT)) {
            if (mode == SET_TIME_MINUTES || mode == SET_ALARM_MINUTES ||
                mode == SET_TIME_HOURS   || mode == SET_ALARM_HOURS) {
                last_inactivity = now_ticks;
            }
        }
        if (events & ACCEPT) {
            if (mode == SHOW_TIME) {
                if (ClockIsAlarmActive(clock)) {
                    ClockSnoozeAlarm(clock);
                    DigitalOutputDeactivate(board->led_red);
                } else if (!ClockEnableAlarm(clock)) {
                    ClockEnableAlarm(clock);
                }
            } else if (mode == SET_TIME_MINUTES) {
                ChangeMode(SET_TIME_HOURS);
            } else if (mode == SET_TIME_HOURS) {
                clock_convert_bcd_to_time(&hour, digits);
                ClockSetTime(clock, &hour);
                ChangeMode(SHOW_TIME);
            } else if (mode == SET_ALARM_MINUTES) {
                ChangeMode(SET_ALARM_HOURS);
            } else if (mode == SET_ALARM_HOURS) {
                clock_convert_bcd_to_time(&alarm, digits);
                ClockSetAlarm(clock, &alarm);
                ChangeMode(SHOW_TIME);
            }
        }

        if (events & CANCEL) {
            if (mode == SHOW_TIME) {
                if (ClockIsAlarmActive(clock)) {
                    DigitalOutputDeactivate(board->led_red);
                    ClockPostponeAlarmToNextDay(clock);
                } else if (ClockEnableAlarm(clock)) {
                    ClockDisableAlarm(clock);
                }
            } else if (mode == SET_TIME_MINUTES || mode == SET_TIME_HOURS) {
                if (ClockGetTime(clock, &hour)) {
                    ChangeMode(SHOW_TIME);
                } else {
                    ChangeMode(UNCONFIGURED);
                }
            } else if (mode == SET_ALARM_MINUTES || mode == SET_ALARM_HOURS) {
                ChangeMode(SHOW_TIME);
            }
        }

        if (events & SET_TIME) {
            ChangeMode(SET_TIME_MINUTES);
            ClockGetTime(clock, &hour);
            clock_convert_time_to_bcd(&hour, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (events & SET_ALARM) {
            ChangeMode(SET_ALARM_MINUTES);
            ClockGetAlarm(clock, &alarm);
            clock_convert_time_to_bcd(&alarm, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (events & DECREMENT) {
            if (mode == SET_TIME_MINUTES || mode == SET_ALARM_MINUTES) {
                clock_decrement_bcd(&digits[3], &digits[2], 9, 5);
            } else if (mode == SET_TIME_HOURS || mode == SET_ALARM_HOURS) {
                clock_decrement_bcd(&digits[1], &digits[0], 3, 2);
            }
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (events & INCREMENT) {
            if (mode == SET_TIME_MINUTES || mode == SET_ALARM_MINUTES) {
                clock_increment_bcd(&digits[3], &digits[2], 9, 5);
            } else if (mode == SET_TIME_HOURS || mode == SET_ALARM_HOURS) {
                clock_increment_bcd(&digits[1], &digits[0], 3, 2);
            }
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }
         }else{        
            if((now_ticks - last_inactivity) > pdMS_TO_TICKS(30000)) {
            if (mode == SET_TIME_MINUTES || mode == SET_ALARM_MINUTES ||
                mode == SET_TIME_HOURS   || mode == SET_ALARM_HOURS) {
                ChangeMode(SHOW_TIME);
                last_inactivity = now_ticks;
            }
        }
    }
    }
}


/* === End of documentation ======================================================================================== */
