/********************************************************************************************************************
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

/** @file digital.c
 ** @brief Codigo fuente del modulo para la gestión entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digital.h"
#include <stdbool.h>
#include <stdio.h>
#include "chip.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"
/* === Macros definitions ========================================================================================== */

/*#ifdef USAR_MEMORIA_ESTATICA
    #ifndef ALUMNOS_MAX_INSTANCIAS
        #define ALUMNOS_MAX_INSTANCIAS 2 //!<Cantidad maxima de alumnos
    #endif
#endif*/

/* === Private data type declarations ============================================================================== */

/*! Estructura que representa una salida digital */
struct digital_output_s {
    uint8_t port; /*!< Puerto al que pertenece la salida */
    uint8_t pin;  /*!< Pin al que pertenece la salida */
    bool state;
};

/* === Private function declarations =============================================================================== */

/**
 * @brief Crea una nueva instancia de la estructura alumno_s.
 *
 * @return Puntero a la nueva instancia de alumno_s, o NULL si no hay espacio disponible.
 */
/*#ifdef USAR_MEMORIA_ESTATICA
static alumno_t CrearInstancia(void);
#endif*/

/* === Private variable definitions ================================================================================ */
// #ifdef USAR_MEMORIA_ESTATICA
//     static struct alumno_s instancias[ALUMNOS_MAX_INSTANCIAS]={0};
// #endif
/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */


/* === Public function implementation ============================================================================== */

digital_output_t DigitalOutputCreate(uint8_t port, uint8_t pin){
    digital_output_t self = malloc(sizeof(struct digital_output_s));
    if (self != NULL)
    {
        self ->port = port;
        self ->pin = pin;
    }
    return self;
} 

void DigitalOutputActivate(digital_output_t self){
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,self->port,self->pin,!self->state);
}

void DigitalOutputDeactivate(digital_output_t self){
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,self->port,self->pin,!self->state);
}

void DigitalOutputToggle(digital_output_t self){
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->port, self->pin);
}

/* === End of documentation ======================================================================================== */
