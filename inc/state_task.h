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

#ifndef STATE_TASK_H_
#define STATE_TASK_H_

/** @file state_task.h
 ** @brief Modulo para declaracion de la tarea de estados del reloj
 **/

/* === Headers files inclusions ==================================================================================== */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "bsp.h"
#include "clock.h"
#include "extras.h"
#include "refresh_task.h"
#include "buttons_task.h"
#include "time_task.h"

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define ACCEPT BUTTON_EVENT_KEY_0    // Boton "Aceptar"
#define CANCEL BUTTON_EVENT_KEY_1    // Boton "Cancelar"
#define SET_TIME BUTTON_EVENT_LONG_0  // Boton "Setear Tiempo"
#define SET_ALARM BUTTON_EVENT_LONG_1 // Boton "Setear Alarma"
#define DECREMENT BUTTON_EVENT_KEY_4 // Boton "Decrementar valor"
#define INCREMENT BUTTON_EVENT_KEY_5 // Boton "Incrementar valor"

#define STATE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE*2

/* === Public data type declarations =============================================================================== */

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Tarea para los estados del reloj
 * 
 * @param pointer 
 */
void StateTask(void * pointer);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* STATE_TASK_H_ */
