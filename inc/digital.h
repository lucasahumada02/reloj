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

#ifndef DIGITAL_H_
#define DIGITAL_H_

/** @file digital.h
 ** @brief Declaraciones del módulo para la gestion de entradas y salidas digitales
 **
 **Esta biblioteca puede usar memoria dinamica o estatica
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */
#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */

/**
 * @brief Estados posibles de cambio en una entrada digital.
 * 
 */
typedef enum digital_states_e {
    DIGITAL_INPUT_WAS_DEACTIVATED = -1,
    DIGITAL_INPUT_NO_CHANGE = 0,
    DIGITAL_INPUT_WAS_ACTIVATED = 1,
}digital_states_t;

/**
 * @brief Representa una salida digital.
 * 
 */
typedef struct digital_output_s * digital_output_t;

/**
 * @brief Representa una entrada digital.
 * 
 */
typedef struct digital_input_s * digital_input_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/**
 * @brief Funcion para crear una salida digital
 * 
 * Esta funcion crea un objeto de la clase digital. La alocacion de memoria depende de la
 * @ref configuracion de la biblioteca "config"
 * 
 * @param gpio Puerto GPIO a utilizar
 * @param bit  Bit a utilizar dentro del puerto GPIO
 * @return digital_output_t Puntero a la instancia de la salida digital creada
 */
digital_output_t DigitalOutputCreate(uint8_t gpio, uint8_t bit);

/**
 * @brief Función para activar una salida digital
 * 
 * @param output Puntero a la instancia de la salida digital devuelta por la funcion DigitalOutputCreate()
 */
void DigitalOutputActivate(digital_output_t output);

/**
 * @brief Función para desactivar una salida digital
 * 
 * @param output Puntero a la instancia de la salida digital devuelta por la función DigitalOutputCreate()
 */
void DigitalOutputDeactivate(digital_output_t output);
/**
 * @brief Función para cambiar el estado de una salida digital
 * 
 * Esta función cambia el estado de la salida digital, si estaba activa la desactiva y viceversa
 * 
 * @param output Puntero a la instancia de la salida digital devuelta por la función DigitalOutputCreate()
 */
void DigitalOutputToggle(digital_output_t output);

/**
 * @brief Función para crear una entrada digital
 * 
 * @param gpio Puerto GPIO a utilizar
 * @param bit Bit a utilizar dentro del puerto GPIO
 * @param inverted Indica si la entrada digital es invertida o no
 * @return digital_input_t Puntero a la instancia de la entrada digital creada
 */
digital_input_t DigitalInputCreate(uint8_t gpio, uint8_t bit, bool inverted);

/**
 * @brief Función para leer el estado de una entrada digital
 * 
 * @param input Puntero a la instancia de la entrada digital devuelto por la función DigitalInputCreate()
 * @return bool true si la entrada digital está activa, false en caso contrario
 */
bool DigitalInputGetIsActive(digital_input_t input);

/**
 * @brief Indica si una entrada digital fue activada
 * 
 * Devuelve true si la entrada digital pasó de inactiva a activa desde la última consulta
 * 
 * @param input Puntero a la instancia de la entrada digital devuelto por la función DigitalInputCreate()
 * @return true si la entrada fue activada, false en caso contrario
 */
bool DigitalInputWasActivated(digital_input_t input);

/**
 * @brief Indica si una entrada digital fue desactivada
 * 
 * Devuelve true si la entrada digital pasó de activa a inactiva desde la última consulta
 * 
 * @param input Puntero a la instancia de la entrada digital devuelto por la función DigitalInputCreate()
 * @return true si la entrada fue desactivada, false en caso contrario
 */
bool DigitalInputWasDeactivated(digital_input_t input);

/**
 * @brief Indica si el estado de una entrada digital ha cambiado
 * 
 * Esta función verifica si el estado de la entrada digital ha cambiado desde la última consulta
 * 
 * @param input Puntero a la instancia de la entrada digital devuelto por la función DigitalInputCreate()
 * @return enum digital_states_e Estado del cambio: activada, desactivada o sin cambios
 */
enum digital_states_e DigitalInputWasChanged(digital_input_t input);

/* === End of conditional blocks =================================================================================== */
#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_H_ */
