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

#ifndef SCREEN_H_
#define SCREEN_H_

/** @file screen.h
 ** @brief Declaraciones del módulo para la gestión de una pantalla multiplexada de 7 segmentos
 **/

/* === Headers files inclusions ==================================================================================== */

#include <stdint.h>
#include <stdbool.h>

/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

#define SEGMENT_A (1 << 0)
#define SEGMENT_B (1 << 1)
#define SEGMENT_C (1 << 2)
#define SEGMENT_D (1 << 3)
#define SEGMENT_E (1 << 4)
#define SEGMENT_F (1 << 5)
#define SEGMENT_G (1 << 6)
#define SEGMENT_P (1 << 7)


/* === Public data type declarations =============================================================================== */

typedef struct screen_s * screen_t;

typedef void (*digits_turn_off_t)(void);
typedef void (*segments_update_t)(uint8_t);
typedef void (*digits_turn_on_t)(uint8_t);



typedef struct screen_driver_s {
    digits_turn_off_t DigitsTurnOff;
    segments_update_t SegmentsUpdate;
    digits_turn_on_t DigitTurnOn;
} const * screen_driver_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */

/**
 * @brief Función para crear una nueva instancia de la pantalla
 * 
 * @param digits numeros de digitos del display
 * @param driver puntero a la estructura que contiene las funciones del driver del display
 * @return screen_t Puntero a la nueva instancia de la pantalla
 */
screen_t ScreenCreate(uint8_t digits, screen_driver_t driver);

/**
 * @brief Función para escribir un valor en formato BCD en la pantalla.
 * 
 * @param screen Puntero a la instancia de la pantalla.
 * @param value Puntero al arreglo que contiene los valores BCD a escribir.
 * @param size Tamaño del arreglo de valores BCD.
 */
void ScreenWriteBCD(screen_t screen, uint8_t * value, uint8_t size);

/**
 * @brief  Función para refrescar la pantalla, actualizando el dígito actual.
 *
 * 
 * @param screen  Puntero a la instancia de la pantalla.
 */
void ScreenRefresh(screen_t screen);

/**
 * @brief Función para hacer parpadear los digitos del display
 * 
 * @param display Puntero al descriptor de la pantalla con la que se quiere operar
 * @param from Posición del primer digito que se quiere hacer parpadear
 * @param to Posición del ultimo digito que se quiere hacer parpadear
 * @param frecuency Factor de división de la frecuencia de refresco para el parpadeo
 */
int DisplayFlashDigits(screen_t screen, uint8_t from, uint8_t to, uint16_t divisor);

/**
 * @brief  Cambia el estado de un punto decimal en la pantalla
 * 
 * @param screen Puntero a la estructura de control de la pantalla
 * @param position Posición del punto decimal a cambiar
 */
void ScreenToggleDot(screen_t screen, uint8_t position);

/**
 * @brief Establece el estado de un punto decimal en la pantalla
 * 
 * @param screen Puntero a la estructura de control de la pantalla
 * @param position Posición del punto decimal a cambiar
 * @param on Estado deseado del punto
 */
void ScreenSetDot(screen_t screen, uint8_t position, bool on);
/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* SCREEN_H_ */
