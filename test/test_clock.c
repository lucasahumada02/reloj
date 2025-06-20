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

/** @file test_clock.c
 ** @brief Plantilla para la creación de archivos de código fuente en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

// #include "plantilla.h"
#include "clock.h"
#include "unity.h"

/**
 * Al inicializar el reloj está en 00:00 y con hora invalida.
 * Al ajustar la hora el reloj queda en hora y es valida.
 * Después de n ciclos de reloj la hora avanza un segundo, diez
 * segundos, un minutos, diez minutos, una hora, diez horas y un día completo.
 * Tratar de ajustar la hora el reloj con valores invalidos y verificar que los rechaza.
 * Fijar la hora de la alarma y consultarla.
 * Fijar la alarma y avanzar el reloj para que suene.
 * Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
 * Hacer sonar la alarma y posponerla.
 * Hacer sonar la alarma y cancelarla hasta el otro dia.
 * Probar getTime con NULL como argumento.
 * Hacer una prueba con frecuencias diferentes.
 */
/* === Macros definitions ========================================================================================== */

#define CLOCK_TICKS_PER_SECOND 5

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */


void SimulateSeconds(clock_t clock, uint8_t seconds){
    for (uint8_t i = 0; i < CLOCK_TICKS_PER_SECOND * seconds; i++)
    {
        ClockNewTick(clock);
    }
    
}

/* === Public function implementation ============================================================================== */
// Al inicializar el reloj está en 00:00 y con hora invalida.
void test_set_up_with_invalid_time(void) {
    clock_time_t current_time = {
        .bcd = {1, 2, 3, 4, 5, 6},
    };

    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);
    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}

// Al ajustar la hora el reloj queda en hora y es valida.
void test_set_up_and_adjust_with_valid_time(void) {
    static const clock_time_t new_time = {.time = {
                                              .seconds = {4, 5},
                                              .minutes = {3, 0},
                                              .hours = {1, 4},
                                          }};
    clock_time_t current_time = {0};

    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);
    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));
    TEST_ASSERT_TRUE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(new_time.bcd, current_time.bcd, 6);
}

// Después de n ciclos de reloj la hora avanza un segundo
void test_clock_advance_one_second(void) {
    clock_time_t current_time = {0};
    static const clock_time_t espected_value = {.time = {
                                              .seconds = {1, 0},
                                              .minutes = {0, 0},
                                              .hours = {0, 0},
                                          }};
   

    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND);

    // Set initial time to 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 1);
    ClockGetTime(clock, &current_time);
    //TEST_ASSERT_EQUAL_UINT8_ARRAY(espected_value.bcd, current_time.bcd, 6);
    TEST_ASSERT_EQUAL_MEMORY(&espected_value, &current_time, sizeof(clock_time_t));

}
/* === End of documentation ======================================================================================== */
