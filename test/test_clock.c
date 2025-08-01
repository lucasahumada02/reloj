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
 ** @brief Pruebas unitarias del módulo de reloj digital usando Unity y Ceedling.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include "unity.h"

/**
 * -Al inicializar el reloj está en 00:00 y con hora invalida.
 * -Al ajustar la hora el reloj queda en hora y es valida.
 * -Después de n ciclos de reloj la hora avanza un segundo, diez
 *  segundos, un minutos, diez minutos, una hora, diez horas y un día completo.
 * -Tratar de ajustar la hora el reloj con valores invalidos y verificar que los rechaza.
 * -Fijar la hora de la alarma y consultarla.
 * -Fijar la alarma y avanzar el reloj para que suene.
 * -Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
 * -Hacer sonar la alarma y posponerla.
 * -Hacer sonar la alarma y cancelarla hasta el otro dia.
 * -Probar getTime con NULL como argumento.
 * -Hacer una prueba con frecuencias diferentes.
 */
/* === Macros definitions ========================================================================================== */

/**
 * @def CLOCK_TICKS_PER_SECOND
 * @brief Frecuencia base del reloj simulada para las pruebas.
 */
#define CLOCK_TICKS_PER_SECOND 5

/**
 * @brief Macro para validar el tiempo actual del reloj en formato BCD.
 */
#define TEST_ASSERT_TIME(hours_tens, hours_units, minutes_tens, minutes_units,seconds_tens, seconds_units, current_time) \
    clock_time_t current_time = {0};  \
    TEST_ASSERT_TRUE_MESSAGE(ClockGetTime(clock, &current_time), "Clock has invalid time."); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_units, current_time.bcd[0], "Diference in unit seconds."); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_tens, current_time.bcd[1],"Diference in tens seconds."); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_units, current_time.bcd[2],"Diference in unit minutes."); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_tens, current_time.bcd[3],"Diference in tens minutes."); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_units, current_time.bcd[4],"Diference in unit hours."); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_tens, current_time.bcd[5],"Diference in tens hours.");

/**
 * @brief Macro para validar la hora configurada de la alarma.
 */
#define TEST_ASSERT_ALARM(hour_tens, hour_units, minutes_tens, minutes_units, seconds_tens, seconds_units) \
    clock_time_t alarm_time = {0}; \
    TEST_ASSERT_TRUE_MESSAGE(ClockGetAlarm(clock, &alarm_time), "Clock has invalid time"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_units, alarm_time.bcd[0], "Diference in unit seconds"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_tens, alarm_time.bcd[1], "Diference in tens seconds"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_units, alarm_time.bcd[2], "Diference in unit minutes"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_tens, alarm_time.bcd[3], "Diference in tens minutes"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hour_units, alarm_time.bcd[4], "Diference in unit hours"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hour_tens, alarm_time.bcd[5], "Diference in tens hours");

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */
/**
 * @brief Instancia global del reloj utilizada en las pruebas.
 */
clock_t clock;

/* === Private function definitions ================================================================================ */

/**
 * @brief Simula el avance del reloj en segundos.
 * 
 * @param clock Instancia del reloj.
 * @param seconds Cantidad de segundos a simular.
 */
static void SimulateSeconds(clock_t clock, uint32_t seconds){
    for (uint32_t i = 0; i < CLOCK_TICKS_PER_SECOND * seconds; i++)
    {
        ClockNewTick(clock);
    } 
}

static void DummyAlarmCallback(clock_t clock) {
    // No hace nada, solo evita errores en las pruebas
    (void)clock;
}

/**
 * @brief Setup que se ejecuta antes de cada test. Crea una nueva instancia del reloj.
 */
void setUp(void){
    clock = ClockCreate(CLOCK_TICKS_PER_SECOND, DummyAlarmCallback);
    ClockSetTime(clock, &(clock_time_t){0}); 
}

/* === Public function implementation ============================================================================== */
// Al inicializar el reloj está en 00:00 y con hora invalida.
void test_set_up_with_invalid_time(void) {
    clock_time_t current_time = {
        .bcd = {1, 2, 3, 4, 5, 6},
    };

    clock_t clock = ClockCreate(CLOCK_TICKS_PER_SECOND, DummyAlarmCallback);
    TEST_ASSERT_FALSE(ClockGetTime(clock, &current_time));
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, 6);
}

// Al ajustar la hora el reloj queda en hora y es valida.
void test_set_up_and_adjust_with_valid_time(void) {
    static const clock_time_t new_time = {.time = {
        .hours = {4, 1},.minutes = {3, 0}, .seconds = {4, 5},
    }};

    
    TEST_ASSERT_TRUE(ClockSetTime(clock, &new_time));
    TEST_ASSERT_TIME(1,4,0,3,5,4, current_time);
}

// Después de n ciclos de reloj la hora avanza un segundo
void test_clock_advance_one_second(void) {
    // Set initial time to 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 1);
    TEST_ASSERT_TIME(0,0,0,0,0,1, current_time);
}

// Después de diez ciclos de reloj la hora avanza diez segundo
void test_clock_advance_ten_seconds(void) {
    // Set initial time to 00:00:00
    ClockSetTime(clock, &(clock_time_t){0});
    SimulateSeconds(clock, 10);
    TEST_ASSERT_TIME(0,0,0,0,1,0, current_time);
}

// Despues de n ciclos de reloj la hora avanza un minuto
void test_clock_advance_one_minute(void) {

    ClockSetTime(clock, &(clock_time_t){.time = {
                                          .hours = {0, 0}, .minutes = {0, 0}, .seconds = {0, 3}
                                      }});
    SimulateSeconds(clock, 30);
    TEST_ASSERT_TIME(0, 0, 0, 1, 0, 0, current_time);
}

// Despues de n ciclos de reloj la hora avanza diez minutos
void test_clock_advance_ten_minutes(void) {

    ClockSetTime(clock, &(clock_time_t){.time = {
                                          .hours = {0, 0}, .minutes = {9, 0}, .seconds = {0, 3}
                                      }});
    SimulateSeconds(clock, 30);
    TEST_ASSERT_TIME(0, 0, 1, 0, 0, 0, current_time);
}

// Despues de n ciclos de reloj la hora avanza una hora
void test_clock_advance_one_hour(void) {

    ClockSetTime(clock, &(clock_time_t){.time = {
                                          .hours = {0, 0}, .minutes = {9, 5}, .seconds = {0, 3}
                                      }});
    SimulateSeconds(clock, 30);
    TEST_ASSERT_TIME(0, 1, 0, 0, 0, 0, current_time);
}

// Despues de n ciclos de reloj la hora avanza diez horas
void test_clock_advance_ten_hours(void) {

    ClockSetTime(clock, &(clock_time_t){.time = {
                                          .hours = {9, 0}, .minutes = {9, 5}, .seconds = {0, 3} 
                                      }});
    SimulateSeconds(clock, 30);
    TEST_ASSERT_TIME(1, 0, 0, 0, 0, 0, current_time);
}

// Despues de n ciclos de reloj la hora avanza un dia completo
void test_clock_advance_one_day(void) {
    ClockSetTime(clock, &(clock_time_t){.time = {
        .hours = {3, 2}, .minutes = {9, 5}, .seconds = {9, 5} // 23:59:59
    }});

    SimulateSeconds(clock, 1); // Avanza a 00:00:00

    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0, current_time);
}


// Tratar de ajustar la hora el reloj con valores invalidos y verificar que los rechaza.
void test_set_up_and_adjust_with_invalid_time(void) {
    static const clock_time_t newTime = {.bcd = {9, 9, 9, 9, 9, 9}};
    TEST_ASSERT_FALSE(ClockSetTime(clock, &newTime));
}

//Fijar la hora de la alarma y consultarla.
void test_clock_set_and_get_alarm_time(void) {
    static const clock_time_t alarm_t = {.time = {
                                             .hours = {9, 1}, .minutes = {0, 0}, .seconds = {0, 0}
                                         }};

    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_t));
    TEST_ASSERT_ALARM(1, 9, 0, 0, 0, 0); 
}

//Fijar la alarma y avanzar el reloj para que suene.
void test_clock_set_alarm_and_trigger(void) {
     static const clock_time_t alarm_t = {.time = {
        .hours = {0, 1}, .minutes = {0, 0}, .seconds = {0, 0} //10:00:00
    }};
    ClockSetTime(clock, &(clock_time_t){
        .time = {.hours = {9, 0}, .minutes = {9, 5}, .seconds = {5, 4}} //09:59:45
    });
    ClockSetAlarm(clock, &alarm_t);
    SimulateSeconds(clock, 15);
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
}

//Fijar la alarma, deshabilitarla y avanzar el reloj para no suene.
void test_clock_set_alarm_and_disable(void) {
   ClockSetTime(clock, &(clock_time_t){
        .time = {.hours = {9, 0}, .minutes = {9, 5}, .seconds = {5, 4}} //09:59:45
    });
    static const clock_time_t alarm_t = {.time = {
        .hours = {0, 1}, .minutes = {0, 0}, .seconds = {0, 0} //10:00:00
    }};
    TEST_ASSERT_TRUE(ClockSetAlarm(clock, &alarm_t));
    ClockDisableAlarm(clock);
    SimulateSeconds(clock, 15);
    TEST_ASSERT_FALSE(ClockIsAlarmActive(clock));
}

//Hacer sonar la alarma y posponerla.
void test_clock_ring_and_snooze(void) {
    ClockSetTime(clock, &(clock_time_t){
        .time = {.hours = {9, 0}, .minutes = {9, 5}, .seconds = {5, 4}} // 09:59:45
    });

    static const clock_time_t alarm_t = {.time = {
        .hours = {0, 1}, .minutes = {0, 0}, .seconds = {0, 0} // 10:00:00
    }};

    ClockSetAlarm(clock, &alarm_t);
    SimulateSeconds(clock, 15); 
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
    ClockSnoozeAlarm(clock);
    TEST_ASSERT_ALARM(1, 0, 0, 5, 0, 0);
    SimulateSeconds(clock, 300);
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
}

// Hacer sonar la alarma y cancelarla hasta el otro día
void test_clock_ring_and_cancel_alarm_today(void) {
    ClockSetTime(clock, &(clock_time_t){
        .time = {.hours = {9, 0}, .minutes = {9, 5}, .seconds = {5, 4}} // 09:59:45
    });
    static const clock_time_t alarm = {.time = {
        .hours = {0, 1}, .minutes = {0, 0}, .seconds = {0, 0} // 10:00:00
    }};
    ClockSetAlarm(clock, &alarm);
    SimulateSeconds(clock, 15); // ahora son 10:00:00
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));

    ClockCancelAlarmToday(clock);
    TEST_ASSERT_FALSE(ClockIsAlarmActive(clock));
    //SimulateSeconds(clock, 3600);
    TEST_ASSERT_FALSE(ClockIsAlarmActive(clock));

    ClockSetTime(clock, &(clock_time_t){
        .time = {.hours = {9, 0}, .minutes = {9, 5}, .seconds = {5, 4}} // 09:59:45
    });
    ClockCancelAlarmToday(clock);
    ClockSetTime(clock, &(clock_time_t){
        .time = {.hours = {3, 2}, .minutes = {9, 5}, .seconds = {9, 5}}
    });
    SimulateSeconds(clock, 1); 
        
    SimulateSeconds(clock, 86400); // avanza 1 día completo, se activa alarma nuevamente


    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));

    /* 
    ClockSetTime(clock, &(clock_time_t){
        .time = {.hours = {3, 2}, .minutes = {9, 5}, .seconds = {9, 5}} // 23:59:59
    });
    SimulateSeconds(clock, 1);  // 00:00:00 se resetea alarm_cancelled_today
*/
    ClockSetTime(clock, &(clock_time_t){
        .time = {.hours = {9, 0}, .minutes = {9, 5}, .seconds = {5, 4}} // 09:59:45
    });
    SimulateSeconds(clock, 15); // ahora son 10:00:00 otra vez
    TEST_ASSERT_TRUE(ClockIsAlarmActive(clock));
}

//Probar getTime con NULL como argumento.
void test_clock_get_time_with_null_argument(void) {
    clock_time_t *null_time = NULL;
    TEST_ASSERT_FALSE(ClockGetTime(clock, null_time));
}

//Hacer una prueba con frecuencias diferentes.
void test_clock_with_different_tick_frequency(void) {
    clock = ClockCreate(10, DummyAlarmCallback);
    ClockSetTime(clock, &(clock_time_t){0});
    for (int i = 0; i < 10; i++) {
        ClockNewTick(clock);
    }
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1, current_time);
}

//Rechaza alarmas con horarios fuera del rango válido (como 99:99:99).
void test_set_alarm_with_invalid_time_should_fail(void) {
    static const clock_time_t invalid_alarm = {.bcd = {9, 9, 9, 9, 9, 9}};
    TEST_ASSERT_FALSE(ClockSetAlarm(clock, &invalid_alarm));
}


/* === End of documentation ======================================================================================== */
