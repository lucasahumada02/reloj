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
#include "time_task.h"
#include "display_task.h"
#include "buttons_task.h"


/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

static board_t board;
static clock_t clock;

/* === Private function declarations =========================================================== */

/**
 * @brief Funcion Baliza
 * 
 * @param parameter argumento de tarea
 */
static void Blinking(void * parameter);

/**
 * @brief Alarma sonando
 * 
 * @param clock invocacion al reloj
 */
static void AlarmaRinging(clock_t clock);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void Blinking(void * parameter){
    while (true) {
        DigitalOutputToggle(board->led_red);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void AlarmaRinging(clock_t clock){
   if (ClockIsAlarmActive(clock)) {
        DigitalOutputActivate(board->led_red);
    } else {
        DigitalOutputDeactivate(board->led_red);
    }
}

/* === Public function implementation ========================================================= */

int main(void){ 

    
    clock = ClockCreate(1000, AlarmaRinging);
    board = BoardCreate();
     SemaphoreHandle_t screen_mutex;
     EventGroupHandle_t clock_events;
     clock_events = xEventGroupCreate();
     screen_mutex = xSemaphoreCreateMutex();
     BaseType_t result;
         if (screen_mutex && clock_events)
    {
        tick_args_t tick_param = malloc(sizeof(*tick_param));
        tick_param->clock_events = clock_events;
        tick_param->tick_event = SECOND_EVENT;
        tick_param->board = board;
        result = xTaskCreate(Tick_Task, "Tick", TICK_TASK_STACK_SIZE, tick_param, tskIDLE_PRIORITY + 8, NULL);
    }
        if (result == pdPASS) {
        refresh_args_t display_param = malloc(sizeof(*display_param));
        display_param->board = board;
        display_param->clock_events = clock_events;
        display_param->screen_mutex = screen_mutex;
        result = xTaskCreate(Refresh_Task, "Refresh", REFRESH_TASK_SRACK_SIZE, display_param, tskIDLE_PRIORITY + 9, NULL);
    }
    
    if (result == pdPASS) {
        button_args_t btn_param = malloc(sizeof(*btn_param));
        btn_param->clock_events = clock_events;
        btn_param->event_hold = BUTTON_EVENT_LONG_0;
        btn_param->button = board->set_alarm;
        btn_param->screen = board->screen;
        result = xTaskCreate(ButtonHoldTask, "Set_Alarm", BUTTON_TASK_STACK_SIZE, btn_param, tskIDLE_PRIORITY + 1, NULL);
    }

    if (result == pdPASS) {
        button_args_t btn_param = malloc(sizeof(*btn_param));
        btn_param->clock_events = clock_events;
        btn_param->event_push = BUTTON_EVENT_KEY_4;
        btn_param->button = board->increment;
        btn_param->screen = board->screen;
        result = xTaskCreate(ButtonPushTask, "Increment", BUTTON_TASK_STACK_SIZE, btn_param, tskIDLE_PRIORITY + 1, NULL);
    }
    vTaskStartScheduler();


    while (true) {
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
