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

#ifndef CLOCK_H_
#define CLOCK_H_

/** @file clock.h
 ** @brief Módulo para la gestión de un reloj digital con soporte de alarma, snooze y cancelación diaria.
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdbool.h>
#include <stdint.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/* === Public variable declarations ================================================================================ */

/**
 * @brief Representación de tiempo en formato BCD.
 */
typedef union{
    struct {
        uint8_t seconds[2];
        uint8_t minutes[2];
        uint8_t hours[2];
    } time;
    uint8_t bcd[6];
}clock_time_t;

/**
 * @brief Puntero a la instancia del reloj.
 */
typedef struct clock_s * clock_t;

/**
 * @brief Prototipo para la función callback de la alarma.
 *
 * Esta función se invoca cuando la alarma suena.
 *
 * @param clock Instancia del reloj que dispara la alarma.
 */
typedef void (*clock_alarm_callback_t)(clock_t clock);

/* === Public function declarations ================================================================================ */

/**
 * @brief Crea e inicializa una nueva instancia del reloj.
 *
 * @param ticks_per_second Cantidad de ticks necesarios para considerar un segundo completo.
 * @return clock_t Instancia del reloj creada.
 */

clock_t ClockCreate(uint16_t ticks_per_second, clock_alarm_callback_t callback);

/**
 * @brief Obtiene la hora actual del reloj.
 *
 * @param clock Instancia del reloj.
 * @param result Puntero donde se guardará la hora actual.
 * @return true Si la hora es válida.
 * @return false Si la hora aún no fue configurada o result es NULL.
 */
bool ClockGetTime(clock_t clock, clock_time_t * result);

/**
 * @brief Establece una nueva hora en el reloj.
 *
 * @param clock Instancia del reloj.
 * @param new_time Hora a establecer.
 * @return true Si la hora es válida y se configuró correctamente.
 * @return false Si la hora es inválida.
 */
bool ClockSetTime(clock_t clock, const clock_time_t * new_time);


/**
 * @brief Notifica al reloj que ocurrió un nuevo tick.
 *
 * Se deben acumular la cantidad de ticks por segundo para que el reloj avance un segundo.
 *
 * @param clock Instancia del reloj.
 */
void ClockNewTick(clock_t clock);


/**
 * @brief Configura una nueva hora de alarma.
 *
 * @param clock Instancia del reloj.
 * @param alarm_time Hora a la que debe sonar la alarma.
 * @return true Si la alarma se configuró correctamente.
 * @return false Si no se pudo configurar.
 */
bool ClockSetAlarm(clock_t clock, const clock_time_t * alarm_time);

/**
 * @brief Obtiene la hora configurada para la alarma.
 *
 * @param clock Instancia del reloj.
 * @param alarm_time Puntero donde se guardará la hora de la alarma.
 * @return true Si la alarma es válida.
 * @return false Si no hay alarma configurada.
 */
bool ClockGetAlarm(clock_t clock, clock_time_t * alarm_time);

/**
 * @brief Indica si la alarma está activa (sonando).
 *
 * @param clock Instancia del reloj.
 * @return true Si la alarma está activa.
 * @return false Si no lo está.
 */
bool ClockIsAlarmActive(clock_t clock);

/**
 * @brief Deshabilita la alarma completamente.
 *
 * @param clock Instancia del reloj.
 */
void ClockDisableAlarm(clock_t clock);

/**
 * @brief Pospone la alarma 5 minutos a partir de la hora actual.
 *
 * Resetea la bandera de alarma activa y recalcula la hora de la próxima alarma.
 *
 * @param clock Instancia del reloj.
 */
void ClockSnoozeAlarm(clock_t clock);

/**
 * @brief Pospone la alarma hasta el dia siguiente
 * 
 * @param clock Instancia del reloj.
 */
void ClockPostponeAlarmToNextDay(clock_t clock);

/**
 * @brief Habilita la alarma
 * 
 * @param clock Instancia del reloj
 */
bool ClockIsAlarmEnabled(clock_t clock);

/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_H_ */
