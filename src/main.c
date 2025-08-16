/* Copyright 2025, Lucas Ahumada Checa Casquerp
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "bsp.h"
#include <stdint.h>
#include <stdbool.h>
#include "clock.h"
#include <stdlib.h>
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "semphr.h"
#include "buttons_task.h"
#include <string.h>
#include "time_task.h"
#include "extras.h"
#include "refresh_task.h"
#include "state_task.h"


/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

board_t board;
clock_t clock;
clock_mode_t mode;
clock_time_t hour = {0};
clock_time_t alarm = {0};
uint8_t digits[4] = {0};
SemaphoreHandle_t hourMutex;
EventGroupHandle_t keyEventGroup;
tick_args_t tick_param;

/* === Private function declarations =========================================================== */

/**
 * @brief Alarma sonando
 * 
 * @param clock invocacion al reloj
 */
static void AlarmaRinging(clock_t clock);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void AlarmaRinging(clock_t clock){
   if (ClockIsAlarmActive(clock)) {
        DigitalOutputActivate(board->led_red);
    } else {
        DigitalOutputDeactivate(board->led_red);
    }
}

/* === Public function implementation ========================================================= */

int main(void) { 
    board = BoardCreate();
    clock = ClockCreate(1000, AlarmaRinging);
    SysTickInit(1000);
    tick_param = malloc(sizeof(*tick_param));
    hourMutex = xSemaphoreCreateMutex();
    keyEventGroup = xEventGroupCreate();
    ClockGetTime(clock, &tick_param->hour);
    clock_convert_time_to_bcd(&tick_param->hour, digits);
    ScreenWriteBCD(board->screen, digits, sizeof(digits));
    
    tick_param->clock = clock;
    tick_param->hour = hour;
    xTaskCreate(TimeTask, "TickTask", TICK_TASK_STACK_SIZE, tick_param, tskIDLE_PRIORITY + 3, NULL);

    xTaskCreate(RefreshTask,"RefreshTask",REFRESH_TASK_STACK_SIZE,NULL,tskIDLE_PRIORITY+4,NULL);

    xTaskCreate(StateTask,"StateTask",STATE_TASK_STACK_SIZE,NULL,tskIDLE_PRIORITY+2,NULL);

    button_args_t btnAccept = malloc(sizeof(struct button_args_s));
    btnAccept->clock_events = keyEventGroup; btnAccept->event_push = ACCEPT; btnAccept->button = board->accept;
    xTaskCreate(ButtonPushTask,"BtnAccept",BUTTON_TASK_STACK_SIZE,btnAccept,tskIDLE_PRIORITY+1,NULL);

    button_args_t btnCancel = malloc(sizeof(struct button_args_s));
    btnCancel->clock_events = keyEventGroup; btnCancel->event_push = CANCEL; btnCancel->button = board->cancel;
    xTaskCreate(ButtonPushTask,"BtnCancel",BUTTON_TASK_STACK_SIZE,btnCancel,tskIDLE_PRIORITY+1,NULL);

    button_args_t btnInc = malloc(sizeof(struct button_args_s));
    btnInc->clock_events = keyEventGroup; btnInc->event_push = INCREMENT; btnInc->button = board->increment;
    xTaskCreate(ButtonPushTask,"BtnInc",BUTTON_TASK_STACK_SIZE,btnInc,tskIDLE_PRIORITY+1,NULL);

    button_args_t btnDec = malloc(sizeof(struct button_args_s));
    btnDec->clock_events = keyEventGroup; btnDec->event_push = DECREMENT; btnDec->button = board->decrement;
    xTaskCreate(ButtonPushTask,"BtnDec",BUTTON_TASK_STACK_SIZE,btnDec,tskIDLE_PRIORITY+1,NULL);

    button_args_t btnSetTime = malloc(sizeof(struct button_args_s));
    btnSetTime->clock_events = keyEventGroup; btnSetTime->event_hold = SET_TIME; btnSetTime->button = board->set_time;
    xTaskCreate(ButtonHoldTask,"BtnSetTime",BUTTON_TASK_STACK_SIZE,btnSetTime,tskIDLE_PRIORITY+1,NULL);

    button_args_t btnSetAlarm = malloc(sizeof(struct button_args_s));
    btnSetAlarm->clock_events = keyEventGroup; btnSetAlarm->event_hold = SET_ALARM; btnSetAlarm->button = board->set_alarm;
    xTaskCreate(ButtonHoldTask,"BtnSetAlarm",BUTTON_TASK_STACK_SIZE,btnSetAlarm,tskIDLE_PRIORITY+1,NULL);

    vTaskStartScheduler();
    while(1){}
}
/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
