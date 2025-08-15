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

#ifndef BUTTONS_TASK_H_
#define BUTTONS_TASK_H_

/** @file buttons_task.h
 ** @brief Modulo para declaraciones de tarea de los botones
 **/

/* === Headers files inclusions ==================================================================================== */
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "clock.h"
#include "bsp.h"
#include "digital.h"
#include "screen.h"
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define BUTTON_EVENT_KEY_0     (1 << 0)
#define BUTTON_EVENT_KEY_1     (1 << 1)
#define BUTTON_EVENT_KEY_2     (1 << 2)
#define BUTTON_EVENT_KEY_3     (1 << 3)
#define BUTTON_EVENT_KEY_4     (1 << 4)
#define BUTTON_EVENT_KEY_5     (1 << 5)
#define BUTTON_EVENT_LONG_0     (1 << 6)
#define BUTTON_EVENT_LONG_1     (1 << 7)

#define BUTTON_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)

/* === Public data type declarations =============================================================================== */

/* === Public variable declarations ================================================================================ */

typedef struct button_args_s {
    board_t board;                   // Objeto placa
    EventGroupHandle_t clock_events; // grupo de eventos
    uint8_t event_push;         // Bits asociados a evento de presionar un boton
    uint8_t event_hold;          // evento asociado presionar un boton durante x tiempo
    digital_input_t button;             // Entrada digital
} * button_args_t;

/* === Public function declarations ================================================================================ */

/**
 * @brief Funcion de presion del boton en un tiempo corto
 * 
 * @param args puntero a parametro
 */
void ButtonPushTask(void * args);

/**
 * @brief Funcion de presion del boton en un tiempo largp
 * 
 * @param args puntero a parametro
 */
void ButtonHoldTask(void * args);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* PLANTILLA_H_ */
