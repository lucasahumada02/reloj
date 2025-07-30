/* Copyright 2025, Lucas Ahumada Checa Casquerp
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "bsp.h"
#include <stdint.h>
#include <stdbool.h>
#include "clock.h"

/* === Macros definitions ====================================================================== */

#define CLOCK_TICKS_PER_SECOND 1000  ///< Cantidad de ticks por segundo
#define BUTTON_SET_DELAY 3000        ///< Tiempo de presión para entrar en modo ajuste (ms)
#define DISPLAY_FLASH_FREQUENCY 200  ///< Frecuencia de parpadeo de dígitos
#define INACTIVITY_TIMEOUT_MS 30000  ///< Tiempo máximo de inactividad (ms)

/* === Private data type declarations ========================================================== */

/**
 * @brief Estados de funcionamiento del reloj
 *
 */
typedef enum {
    UNCONFIGURED,     //!< Hora no configurada
    SHOW_TIME,        //!< Visualización de hora actual
    SET_TIME_MINUTE,  //!< Configuración de los minutos 
    SET_TIME_HOUR,    //!< Configuración de las horas
    SET_ALARM_MINUTE, //!< Configuración de los minutos de la alarma
    SET_ALARM_HOUR,   //!< Configuración de las horas de la alarma
} states_clock;

/**
 * @brief Estados de boton
 * 
 */
typedef struct {
    bool is_pressed;            //!< Boton presionado
    bool was_processed;         //!< Ya fue presionado el boton
    uint32_t press_start_time;  //!< Empieza a presionar el boton
} button_status_t;

/* === Private variable declarations =========================================================== */

static board_t board;
static clock_t clock;
static states_clock current_mode;
//static uint32_t system_tick_count = 0;
static uint32_t inactivity_timer = 0;
//static uint8_t display_digits[4] = {0};
static uint8_t digits[4];
// static button_status_t btn_set_time_status = {false, false, 0};
// static button_status_t btn_set_alarm_status = {false, false, 0};
//static bool alarm_active = false;  ///< Estado de la alarma

/* === Private function declarations =========================================================== */

static void AlarmaRinging(clock_t clock);

// /**
//  * @brief Activa la alamrma
//  * 
//  * @param clock Instancia del reloj
//  */
// static void clock_activate_alarm(clock_t clock);

// /**
//  * @brief Desactiva la alarma
//  *  
//  */
// static void clock_deactivate_alarm(void);

/**
 * @brief Controla el tiempo de boton presionado
 * 
 * @param button boton presionado
 * @param status estado del boton
 * @param delay tiempo presionado
 * @return true presionado tiempo necesario
 * @return false no presionado el tiempo necesario
 */
//static bool btn_check_long_press(digital_input_t button, button_status_t *status, uint32_t delay);

/**
 * @brief Cambia el estado del reloj
 * 
 * @param new_mode Nuevo estado a trabajar
 */
static void clock_switch_mode(states_clock new_mode);

/**
 * @brief Incrementa el valor de un numero en BCD respetando un limite maximo
 * 
 * @param units Unidades
 * @param tens Decenas
 * @param max_units Máximo de unidades
 * @param max_tens Máximo de decenas
 */
static void clock_increment_bcd(uint8_t *units, uint8_t *tens, uint8_t max_units, uint8_t max_tens);

/**
 * @brief Decrementa el valor de un numero en BCD respetando un limite maximo
 * 
 * @param units Unidades
 * @param tens Decenas
 * @param max_units Máximo de unidades
 * @param max_tens Máximo de decenas
 */
static void clock_decrement_bcd(uint8_t *units, uint8_t *tens, uint8_t max_units, uint8_t max_tens);

/**
 * @brief Convierte un tiempo de un arreglo BCD de 4 dígitos a clock_time_t
 * 
 * @param time Puntero a la estructura de tiempo  clock_time_t
 * @param bcd Puntero a un arreglo de 4 elementos donde se almacena el tiempo en formato [decena_hora, unidad_hora,
 * decena_minuto, unidad_minuto]
 */
static void clock_convert_time_to_bcd(clock_time_t *time, uint8_t *bcd);

/**
 * @brief Convierte un tiempo en formato clock_time_t a un arreglo BCD de 4 dígitos.
 * 
 * @param time Puntero a la estructura de tiempo  clock_time_t
 * @param bcd Puntero a un arreglo de 4 elementos donde se almacena el tiempo en formato [decena_hora, unidad_hora,
 * decena_minuto, unidad_minuto]
 */
static void clock_convert_bcd_to_time(clock_time_t *time, uint8_t *bcd);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void AlarmaRinging(clock_t clock){
    DigitalOutputActivate(board->led_red);
}

// static void clock_activate_alarm(clock_t clock) {
//     DigitalOutputActivate(board->buzzer);
//     DigitalOutputActivate(board->led_red);
// }

// static void clock_deactivate_alarm(void) {
//     DigitalOutputDeactivate(board->buzzer);
//     DigitalOutputDeactivate(board->led_red);
// }

// static bool btn_check_long_press(digital_input_t button, button_status_t *status, uint32_t delay) {
//     if (DigitalInputGetIsActive(button)) {
//         if (!status->is_pressed) {
//             status->is_pressed = true;
//             status->press_start_time = system_tick_count;
//         } else if ((system_tick_count - status->press_start_time) >= delay && !status->was_processed) {
//             status->was_processed = true;
//             return true;
//         }
//     } else {
//         status->is_pressed = false;
//         status->was_processed = false;
//         status->press_start_time = 0;
//     }
//     return false;
// }

static void clock_switch_mode(states_clock new_mode) {
    current_mode = new_mode;
    inactivity_timer = 0;
    switch (current_mode) {
    case UNCONFIGURED:
        DisplayFlashDigits(board->screen, 0, 3, DISPLAY_FLASH_FREQUENCY);
       // ScreenToggleDot(board->screen, 0);
        ScreenToggleDot(board->screen, 1);
       // ScreenToggleDot(board->screen, 2);
       // ScreenToggleDot(board->screen, 3);
        break;
    case SHOW_TIME:
        DisplayFlashDigits(board->screen, 0, 0, 0);
        // ScreenToggleDot(board->screen, 0);
        //ScreenToggleDot(board->screen, 1);
        // ScreenToggleDot(board->screen, 2);
        // ScreenToggleDot(board->screen, 3);
        break;
    case SET_TIME_MINUTE:
        DisplayFlashDigits(board->screen, 2, 3, DISPLAY_FLASH_FREQUENCY);
        // ScreenToggleDot(board->screen, 0);
        // ScreenToggleDot(board->screen, 1);
        // ScreenToggleDot(board->screen, 2);
        // ScreenToggleDot(board->screen, 3);
        break;
    case SET_TIME_HOUR:
        DisplayFlashDigits(board->screen, 0, 1, DISPLAY_FLASH_FREQUENCY);
        // ScreenToggleDot(board->screen, 0);
        // ScreenToggleDot(board->screen, 1);
        // ScreenToggleDot(board->screen, 2);
        // ScreenToggleDot(board->screen, 3);
        break;
    case SET_ALARM_MINUTE:
        DisplayFlashDigits(board->screen, 2, 3, DISPLAY_FLASH_FREQUENCY);
        ScreenToggleDot(board->screen, 0);
        ScreenToggleDot(board->screen, 1);
        ScreenToggleDot(board->screen, 2);
        ScreenToggleDot(board->screen, 3);
        break;
    case SET_ALARM_HOUR:
        DisplayFlashDigits(board->screen, 0, 1, DISPLAY_FLASH_FREQUENCY);
        ScreenToggleDot(board->screen, 0);
        ScreenToggleDot(board->screen, 1);
        ScreenToggleDot(board->screen, 2);
        ScreenToggleDot(board->screen, 3);
        break;
    }
}

static void clock_increment_bcd(uint8_t *units, uint8_t *tens, uint8_t max_units, uint8_t max_tens) {
    (*units)++;
    if (*units > max_units) {
        *units = 0;
        (*tens)++;
        if (*tens > max_tens) {
            *tens = 0;
            *units = 0;
        }
    }
}

static void clock_decrement_bcd(uint8_t *units, uint8_t *tens, uint8_t max_units, uint8_t max_tens) {
    if (*units > 0) {
        (*units)--;
    } else {
        if (*tens > 0) {
            (*tens)--;
            *units = 9;
        } else {
            *tens = max_tens;
            *units = max_units;
        }
    }
}

static void clock_convert_time_to_bcd(clock_time_t *time, uint8_t digits[]) {
     if (time && digits) {
        digits[0] = time->bcd[5]; // Hora de decenas
        digits[1] = time->bcd[4]; // Hora de unidades
        digits[2] = time->bcd[3]; // Minuto de decenas
        digits[3] = time->bcd[2]; // Minuto de unidades
//     bcd[0] = time->time.hours[1];
//     bcd[1] = time->time.hours[0];
//     bcd[2] = time->time.minutes[1];
//     bcd[3] = time->time.minutes[0];
    }
 }

static void clock_convert_bcd_to_time(clock_time_t *time, uint8_t digits[]) {
     if (time && digits) {
        time->bcd[5] = digits[0]; // Hora de decenas
        time->bcd[4] = digits[1]; // Hora de unidades
        time->bcd[3] = digits[2]; // Minuto de decenas
        time->bcd[2] = digits[3]; // Minuto de unidades
        time->bcd[1] = 0;
        time->bcd[0] = 0;
    }
    // time->time.hours[1] = bcd[0];
    // time->time.hours[0] = bcd[1];
    // time->time.minutes[1] = bcd[2];
    // time->time.minutes[0] = bcd[3];
    // time->time.seconds[1] = 0;
    // time->time.seconds[0] = 0;
}

/* === Public function implementation ========================================================= */


int main(void) {

    clock_time_t current_time_data, alarm_time_data;
    

    clock = ClockCreate(CLOCK_TICKS_PER_SECOND, AlarmaRinging);
    board = BoardCreate();
    SysTickInit(CLOCK_TICKS_PER_SECOND);
    clock_switch_mode(UNCONFIGURED);

    while (true) {

       if (DigitalInputWasDeactivated(board->accept)) {
            if (current_mode == SHOW_TIME) {
                if (ClockIsAlarmActive(clock)) {
                    ClockSnoozeAlarm(clock); // Posponer alarma 5 minutos
                }
            } else if (current_mode == SET_TIME_MINUTE) {
                clock_switch_mode(SET_TIME_HOUR);
            } else if (current_mode == SET_TIME_HOUR) {
                clock_convert_bcd_to_time(&current_time_data, digits);
                ClockSetTime(clock, &current_time_data);
                clock_switch_mode(SHOW_TIME);
            } else if (current_mode == SET_ALARM_MINUTE) {
                clock_switch_mode(SET_ALARM_HOUR);
            } else if (current_mode == SET_ALARM_HOUR) {
                clock_convert_bcd_to_time(&alarm_time_data, digits);
                ClockSetAlarm(clock, &alarm_time_data);
                clock_switch_mode(SHOW_TIME);
            }
        }

        if (DigitalInputWasDeactivated(board->cancel)) {
            if (current_mode == SHOW_TIME) {
                if (ClockIsAlarmActive(clock)) {
                    ClockCancelAlarmToday(clock);
                }
            } else if (current_mode == SET_TIME_MINUTE || current_mode == SET_TIME_HOUR) {
                if (ClockGetTime(clock, &current_time_data)) {
                    clock_switch_mode(SHOW_TIME);
                } else {
                    clock_switch_mode(UNCONFIGURED); // Si la hora no es válida, volver a UNCONFIGURED
                }
            } else if (current_mode == SET_ALARM_MINUTE || current_mode == SET_ALARM_HOUR) {
                clock_switch_mode(SHOW_TIME); // Cancelar configuración de alarma
            }
        }

        if (DigitalInputWasDeactivated(board->set_time)) {
            clock_switch_mode(SET_TIME_MINUTE);
            ClockGetTime(clock, &current_time_data);
            clock_convert_time_to_bcd(&current_time_data, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (DigitalInputWasDeactivated(board->set_alarm)) {
            //dotsOn = false; // Aseguramos que los puntos estén apagados al iniciar la configuración de alarma
            clock_switch_mode(SET_ALARM_MINUTE);
            ClockGetAlarm(clock, &alarm_time_data);
            clock_convert_time_to_bcd(&alarm_time_data, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (DigitalInputWasDeactivated(board->decrement)) {
            if (current_mode == SET_TIME_MINUTE || current_mode == SET_ALARM_MINUTE) {
                clock_decrement_bcd(&digits[3], &digits[2], 9, 5);
            } else if (current_mode == SET_TIME_HOUR || current_mode == SET_ALARM_HOUR) {
                clock_decrement_bcd(&digits[1], &digits[0], 3, 2);
            }

            //hour_digits[0] = display_digits[0]; hour_digits[1] = display_digits[1];
            //minute_digits[0] = display_digits[2]; minute_digits[1] = display_digits[3];
            ScreenWriteBCD(board->screen, digits, sizeof(digits));

            if (current_mode == SET_ALARM_MINUTE || current_mode == SET_ALARM_HOUR) {
                ScreenToggleDot(board->screen, 0);
                ScreenToggleDot(board->screen, 1);
                ScreenToggleDot(board->screen, 2);
                ScreenToggleDot(board->screen, 3);
                //dotsOn = true;
            }
        }

        if (DigitalInputWasDeactivated(board->increment)) {
            if (current_mode == SET_TIME_MINUTE || current_mode == SET_ALARM_MINUTE) {
                clock_increment_bcd(&digits[3], &digits[2], 9, 5);
            } else if (current_mode == SET_TIME_HOUR || current_mode == SET_ALARM_HOUR) {
                clock_increment_bcd(&digits[1], &digits[0], 3, 2);
            }

            //hour_digits[0] = display_digits[0]; hour_digits[1] = display_digits[1];
            //minute_digits[0] = display_digits[2]; minute_digits[1] = display_digits[3];
            ScreenWriteBCD(board->screen, digits, sizeof(digits));

            if ( current_mode == SET_ALARM_MINUTE || current_mode == SET_ALARM_HOUR) {
                ScreenToggleDot(board->screen, 0);
                ScreenToggleDot(board->screen, 1);
                ScreenToggleDot(board->screen, 2);
                ScreenToggleDot(board->screen, 3);
                //dotsOn = true; // nos aseguramos que sigue en true
            }
        }


        
        // switch (current_mode) {
        // case UNCONFIGURED:
        //     if (ClockGetTime(clock, &current_time_data)) {
        //         clock_switch_mode(SET_TIME_MINUTE);
        //     } else {
        //         clock_convert_time_to_bcd(&current_time_data, display_digits);
        //         ScreenWriteBCD(board->screen, display_digits, 4);
        //         ScreenToggleDot(board->screen, 1);
        //         if (btn_check_long_press(board->set_time, &btn_set_time_status, BUTTON_SET_DELAY)) {
        //             clock_switch_mode(SET_TIME_MINUTE);
        //         }
        //     }
        //     break;

        // case SHOW_TIME:
        //     if (ClockGetTime(clock, &current_time_data)) {
        //         clock_convert_time_to_bcd(&current_time_data, display_digits);
        //         ScreenWriteBCD(board->screen, display_digits, 4);
        //         if (btn_check_long_press(board->set_time, &btn_set_time_status, BUTTON_SET_DELAY)) {
        //             ClockGetTime(clock, &current_time_data);
        //             clock_convert_time_to_bcd(&current_time_data, display_digits);
        //             hour_digits[0] = display_digits[0]; hour_digits[1] = display_digits[1];
        //             minute_digits[0] = display_digits[2]; minute_digits[1] = display_digits[3];
        //             clock_switch_mode(SET_TIME_MINUTE);
        //         }
        //         if (btn_check_long_press(board->set_alarm, &btn_set_alarm_status, BUTTON_SET_DELAY)) {
        //             if (ClockGetAlarm(clock, &alarm_time_data)) {
        //                 clock_convert_time_to_bcd(&alarm_time_data, display_digits);
        //                 hour_digits[0] = display_digits[0]; hour_digits[1] = display_digits[1];
        //                 minute_digits[0] = display_digits[2]; minute_digits[1] = display_digits[3];
        //             }
        //             clock_switch_mode(SET_ALARM_MINUTE);
        //         }
        //         if (DigitalInputWasActivated(board->accept)) {
        //             ClockSetAlarm(clock, &alarm_time_data);
        //         }
        //         if (DigitalInputWasActivated(board->cancel)) {
        //             ClockDisableAlarm(clock);
        //             clock_deactivate_alarm();

        //         }
        //         if (ClockIsAlarmActive(clock) && !alarm_active) {
        //             alarm_active = true;
        //             clock_activate_alarm(clock);
        //         } else if (!ClockIsAlarmActive(clock) && alarm_active) {
        //             alarm_active = false;
        //             clock_deactivate_alarm();
        //         }
        //         if (alarm_active) {
        //             if (DigitalInputWasActivated(board->accept)) {
        //                 ClockSnoozeAlarm(clock);
        //                 clock_deactivate_alarm();
        //             }
        //             if (DigitalInputWasActivated(board->cancel)) {
        //                 ClockCancelAlarmToday(clock);
        //                 clock_deactivate_alarm();
        //             }
        //         }
        //     }
        //     break;

        // case SET_TIME_MINUTE:
        //     if (DigitalInputWasActivated(board->increment)) {
        //         clock_increment_bcd(&minute_digits[1], &minute_digits[0], 9, 5);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->decrement)) {
        //         clock_decrement_bcd(&minute_digits[1], &minute_digits[0], 9, 5);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->accept)) {
        //         clock_switch_mode(SET_TIME_HOUR);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->cancel) || inactivity_timer >= INACTIVITY_TIMEOUT_MS) {
        //         if (ClockGetTime(clock, &current_time_data)) {
        //             clock_switch_mode(SHOW_TIME);
        //         } else {
        //             clock_switch_mode(UNCONFIGURED);
        //         }
        //         inactivity_timer = 0;
        //     }
        //     hour_digits[0] = display_digits[0]; hour_digits[1] = display_digits[1];
        //     minute_digits[0] = display_digits[2]; minute_digits[1] = display_digits[3];
        //     ScreenWriteBCD(board->screen, display_digits, 4);
        //     DisplayFlashDigits(board->screen, 2, 3, DISPLAY_FLASH_FREQUENCY);
        //     break;

        // case SET_TIME_HOUR:
        //     if (DigitalInputWasActivated(board->increment)) {
        //         clock_increment_bcd(&hour_digits[1], &hour_digits[0], 3, 2);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->decrement)) {
        //         clock_decrement_bcd(&hour_digits[1], &hour_digits[0], 3, 2);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->accept)) {
        //         display_digits[0] = hour_digits[0]; display_digits[1] = hour_digits[1];
        //         display_digits[2] = minute_digits[0]; display_digits[3] = minute_digits[1];
        //         clock_convert_bcd_to_time(&current_time_data, display_digits);
        //         ClockSetTime(clock, &current_time_data);
        //         clock_switch_mode(SHOW_TIME);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->cancel) || inactivity_timer >= INACTIVITY_TIMEOUT_MS) {
        //         if (ClockGetTime(clock, &current_time_data)) {
        //             clock_switch_mode(SHOW_TIME);
        //         } else {
        //             clock_switch_mode(UNCONFIGURED);
        //         }
        //         inactivity_timer = 0;
        //     }
        //     display_digits[0] = hour_digits[0]; display_digits[1] = hour_digits[1];
        //     display_digits[2] = minute_digits[0]; display_digits[3] = minute_digits[1];
        //     ScreenWriteBCD(board->screen, display_digits, 4);
        //     DisplayFlashDigits(board->screen, 0, 1, DISPLAY_FLASH_FREQUENCY);
        //     break;

        // case SET_ALARM_MINUTE:
        //     if (DigitalInputWasActivated(board->increment)) {
        //         clock_increment_bcd(&minute_digits[1], &minute_digits[0], 9, 5);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->decrement)) {
        //         clock_decrement_bcd(&minute_digits[1], &minute_digits[0], 9, 5);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->accept)) {
        //         clock_switch_mode(SET_ALARM_HOUR);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->cancel) || inactivity_timer >= INACTIVITY_TIMEOUT_MS) {
        //         clock_switch_mode(SHOW_TIME);
        //         inactivity_timer = 0;
        //     }
        //     display_digits[0] = hour_digits[0]; display_digits[1] = hour_digits[1];
        //     display_digits[2] = minute_digits[0]; display_digits[3] = minute_digits[1];
        //     ScreenWriteBCD(board->screen, display_digits, 4);
        //     DisplayFlashDigits(board->screen, 2, 3, DISPLAY_FLASH_FREQUENCY);
        //     ScreenToggleDot(board->screen, 0);
        //     ScreenToggleDot(board->screen, 1);
        //     ScreenToggleDot(board->screen, 2);
        //     ScreenToggleDot(board->screen, 3);
        //     break;

        // case SET_ALARM_HOUR:
        //     if (DigitalInputWasActivated(board->increment)) {
        //         clock_increment_bcd(&hour_digits[1], &hour_digits[0], 3, 2);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->decrement)) {
        //         clock_decrement_bcd(&hour_digits[1], &hour_digits[0], 3, 2);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->accept)) {
        //         display_digits[0] = hour_digits[0]; display_digits[1] = hour_digits[1];
        //         display_digits[2] = minute_digits[0]; display_digits[3] = minute_digits[1];
        //         clock_convert_bcd_to_time(&alarm_time_data, display_digits);
        //         ClockSetAlarm(clock, &alarm_time_data);
        //         clock_switch_mode(SHOW_TIME);
        //         inactivity_timer = 0;
        //     }
        //     if (DigitalInputWasActivated(board->cancel) || inactivity_timer >= INACTIVITY_TIMEOUT_MS) {
        //         clock_switch_mode(SHOW_TIME);
        //         inactivity_timer = 0;
        //     }
        //     display_digits[0] = hour_digits[0]; display_digits[1] = hour_digits[1];
        //     display_digits[2] = minute_digits[0]; display_digits[3] = minute_digits[1];
        //     ScreenWriteBCD(board->screen, display_digits, 4);
        //     DisplayFlashDigits(board->screen, 0, 1, DISPLAY_FLASH_FREQUENCY);
        //     ScreenToggleDot(board->screen, 0);
        //     ScreenToggleDot(board->screen, 1);
        //     ScreenToggleDot(board->screen, 2);
        //     ScreenToggleDot(board->screen, 3);
        //     break;
        // }

        for (volatile int delay_loop = 0; delay_loop < 25000; delay_loop++) {
            __asm("NOP");
        }
    }
 }

 void SysTick_Handler(void) {
    static uint16_t count = 0;
    clock_time_t time;

    ScreenRefresh(board->screen);
    ClockNewTick(clock);

    count = (count + 1) % 1000;
    if (current_mode <= SHOW_TIME) {
        ClockGetTime(clock, &time);
        clock_convert_time_to_bcd(&time, digits);
        ScreenWriteBCD(board->screen, digits, sizeof(digits));
        if (count > 500 && current_mode == SHOW_TIME) {
            ScreenToggleDot(board->screen, 1);
            DigitalOutputActivate(board->led_red);
        }
        if (ClockIsAlarmActive(clock)) {
            ScreenToggleDot(board->screen, 3);
            DigitalOutputDeactivate(board->led_red);
        }
        if (!ClockIsAlarmActive(clock)) {
            //DigitalOutputToggle(board->led_red);
            DigitalOutputDeactivate(board->led_red);
            ScreenToggleDot(board->screen, 1);
        }
    }
}
// void SysTick_Handler(void) {
//     static uint32_t tick_counter = 0;
//     clock_time_t time_data;

//     ScreenRefresh(board->screen);
//     ClockNewTick(clock);
//     system_tick_count++;
//     inactivity_timer++;

//     if (current_mode == SHOW_TIME) {
//         if (ClockGetTime(clock, &time_data)) {
//             clock_convert_time_to_bcd(&time_data, display_digits);
//             ScreenWriteBCD(board->screen, display_digits, 4);
//             if (tick_counter > 500) {
//                 ScreenToggleDot(board->screen, 1); // Parpadea el punto del segundo dígito
//             }
//             if (ClockIsAlarmActive(clock)) {
//                 ScreenToggleDot(board->screen, 0); // Punto del último dígito encendido si alarma activa
//             }
//         }
//     }
//     tick_counter = (tick_counter + 1) % 1000;
// }

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
