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

#ifndef EXTRAS_H_
#define EXTRAS_H_

/** @file extras.h
 ** @brief Modulo para declaraciones de funciones auxiliares
 **/

/* === Headers files inclusions ==================================================================================== */

#include "bsp.h"
#include "clock.h"
#include <stdint.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

typedef enum {
    UNCONFIGURED,       // No configurado
    SHOW_TIME,          // Modo normal
    SET_TIME_MINUTES,   // Seteo de minutos del reloj
    SET_TIME_HOURS,     // Seteo de horas del reloj
    SET_ALARM_MINUTES,  // Seteo de minutos de la alarma
    SET_ALARM_HOURS,    // Seteo de horas de la alarma
} clock_mode_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */


/**
 * @brief Convierte un tiempo de un arreglo BCD de 4 dígitos a clock_time_t
 * 
 * @param time Puntero a la estructura de tiempo  clock_time_t
 * @param digits Puntero a un arreglo de 4 elementos donde se almacena el tiempo en formato [decena_hora, unidad_hora,
 * decena_minuto, unidad_minuto]
 */
void clock_convert_time_to_bcd(clock_time_t * time, uint8_t digits[]);

/**
 * @brief Convierte un tiempo en formato clock_time_t a un arreglo BCD de 4 dígitos.
 * 
 * @param time Puntero a la estructura de tiempo  clock_time_t
 * @param digits Puntero a un arreglo de 4 elementos donde se almacena el tiempo en formato [decena_hora, unidad_hora,
 * decena_minuto, unidad_minuto
 */
void clock_convert_bcd_to_time(clock_time_t * time, uint8_t digits[]);

/**
 * @brief Obtiene el maximo valor de unidad
 * 
 * @param tens deceneas
 * @param max_tens maximo de decenas
 * @param max_units maximo de unidades
 * @return uint8_t 
 */
uint8_t GetMaxUnits(uint8_t tens, uint8_t max_tens, uint8_t max_units);

/**
 * @brief Incrementa el valor de un numero en BCD respetando un limite maximo
 * 
 * @param units Unidades
 * @param tens  Decenas
 * @param max_units Máximo de unidades
 * @param max_tens Máximo de decenas
 */
void clock_increment_bcd(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens);

/**
 * @brief Decrementa el valor de un numero en BCD respetando un limite maximo
 * 
 * @param units Unidades
 * @param tens Decenas
 * @param max_units Máximo de unidades
 * @param max_tens Máximo de decenas
 */
void clock_decrement_bcd(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens);
/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* EXTRAS_H_ */
