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

screen_t ScreenCreate(uint8_t digits, screen_driver_t driver);

void ScreenWriteBCD(screen_t screen, uint8_t  value[], uint8_t size);

void ScreenRefresh(screen_t screen);

/**
 * @brief Función para hacer parpadear algunos digitos de la pantalla
 * 
 * @param display Puntero al descriptor de la pantalla con la que se quiere operar
 * @param from Posición del primer digito que se quiere hacer parpadear
 * @param to Posición del ultimo digito que se quiere hacer parpadear
 * @param frecuency Factor de división de la frecuencia de refresco para el parpadeo
 */
int DisplayFlashDigits(screen_t screen, uint8_t from, uint8_t to, uint16_t divisor);

/**
 * @brief Enciende los puntos centrales (dos puntos del reloj)
 * 
 * @param screen Descriptor de la pantalla
 */
void ScreenEnableDots(screen_t screen);

/**
 * @brief Apaga los puntos centrales
 * 
 * @param screen Descriptor de la pantalla
 */
void ScreenDisableDots(screen_t screen);

/**
 * @brief Alterna el estado de los puntos centrales
 * 
 * @param screen Descriptor de la pantalla
 */
void ScreenToggleDots(screen_t screen);


/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* SCREEN_H_ */
