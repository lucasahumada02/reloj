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
static uint8_t digits[4];
static uint32_t inactivity_timer = 0;

/* botones largos */
static button_status_t btn_set_time_status = {0};
static button_status_t btn_set_alarm_status = {0};

/* === Private function declarations =========================================================== */

/**
 * @brief Alarma sonando
 * 
 * @param clock invocacion al reloj
 */
static void AlarmaRinging(clock_t clock);

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

/**
 * @brief Controla el tiempo de boton presionado
 * 
 * @param button boton presionado
 * @param status estado del boton
 * @param delay tiempo presionado
 * @return true presionado tiempo necesario
 * @return false no presionado el tiempo necesario
 */
static bool btn_check_long_press(digital_input_t button, button_status_t *status, uint32_t delay_ms);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

static void AlarmaRinging(clock_t clock){
    DigitalOutputActivate(board->led_red);
}

static bool btn_check_long_press(digital_input_t button, button_status_t *status, uint32_t delay_ms) {
    const uint32_t debounce = 50;
    if (DigitalInputGetIsActive(button)) {
        if (!status->is_pressed) {
            status->is_pressed = true;
            status->press_start_time = inactivity_timer;
            status->was_processed = false;
        } else if (!status->was_processed) {
            uint32_t held = inactivity_timer - status->press_start_time;
            if (held >= delay_ms + debounce) {
                status->was_processed = true;
                return true;
            }
        }
    } else {
        status->is_pressed = false;
        status->was_processed = false;
        status->press_start_time = 0;
    }
    return false;
}

static void clock_switch_mode(states_clock new_mode) {
    current_mode = new_mode;
    inactivity_timer = 0;
    switch (current_mode) {
    case UNCONFIGURED:
        DisplayFlashDigits(board->screen, 0, 3, DISPLAY_FLASH_FREQUENCY);
        ScreenToggleDot(board->screen, 1);
        break;
    case SHOW_TIME:
        DisplayFlashDigits(board->screen, 0, 0, 0);
        ScreenToggleDot(board->screen, 1);
        break;
    case SET_TIME_MINUTE:
        DisplayFlashDigits(board->screen, 2, 3, DISPLAY_FLASH_FREQUENCY);
        break;
    case SET_TIME_HOUR:
        DisplayFlashDigits(board->screen, 0, 1, DISPLAY_FLASH_FREQUENCY);
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
    if (*units > 9) {
        *units = 0;
        (*tens)++;
        if (*tens > max_tens) {
            *tens = 0;
            *units = 0;
        }
    }

    if (*tens == max_tens && *units > max_units)
    {
        *tens = 0;
        *units = 0;
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
}

/* === Public function implementation ========================================================= */

int main(void) {

    clock_time_t current_time_data, alarm_time_data; 

    clock = ClockCreate(CLOCK_TICKS_PER_SECOND, AlarmaRinging);
    board = BoardCreate();
    SysTickInit(CLOCK_TICKS_PER_SECOND);
    clock_switch_mode(UNCONFIGURED);

    while (true) {
        /* PRESION LARGA F1: entrar a set time minute */
        if (btn_check_long_press(board->set_time, &btn_set_time_status, BUTTON_SET_DELAY)) {
            if (ClockGetTime(clock, &current_time_data)) {
                clock_convert_time_to_bcd(&current_time_data, digits);
            } else {
                // si no está configurado, arrancar de 00:00
                digits[0] = digits[1] = digits[2] = digits[3] = 0;
            }
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
            clock_switch_mode(SET_TIME_MINUTE);
        }

        /* PRESION LARGA F2: entrar a set alarm minute */
        if (btn_check_long_press(board->set_alarm, &btn_set_alarm_status, BUTTON_SET_DELAY)) {
            if (ClockGetAlarm(clock, &alarm_time_data)) {
                clock_convert_time_to_bcd(&alarm_time_data, digits);
            } else {
                digits[0] = digits[1] = digits[2] = digits[3] = 0;
            }
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
            clock_switch_mode(SET_ALARM_MINUTE);
        } 

       if (DigitalInputWasDeactivated(board->accept)) {
            if (current_mode == SHOW_TIME) {
                if (ClockIsAlarmActive(clock)) {
                    ClockSnoozeAlarm(clock);
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
                    ClockPostponeAlarmToNextDay(clock);
                    ScreenSetDot(board->screen, 3, true);
                }else if (ClockIsAlarmEnabled(clock)) {
                    ClockDisableAlarm(clock);
                }
            } else if (current_mode == SET_TIME_MINUTE || current_mode == SET_TIME_HOUR) {
                if (ClockGetTime(clock, &current_time_data)) {
                    clock_switch_mode(SHOW_TIME);
                } else {
                    clock_switch_mode(UNCONFIGURED);
                }
            } else if (current_mode == SET_ALARM_MINUTE || current_mode == SET_ALARM_HOUR) {
                clock_switch_mode(SHOW_TIME);
            }
        }

        if (DigitalInputWasDeactivated(board->decrement)) {
            inactivity_timer = 0;
            if (current_mode == SET_TIME_MINUTE || current_mode == SET_ALARM_MINUTE) {
                clock_decrement_bcd(&digits[3], &digits[2], 9, 5);
            } else if (current_mode == SET_TIME_HOUR || current_mode == SET_ALARM_HOUR) {
                clock_decrement_bcd(&digits[1], &digits[0], 3, 2);
            }

            ScreenWriteBCD(board->screen, digits, sizeof(digits));

            if (current_mode == SET_ALARM_MINUTE || current_mode == SET_ALARM_HOUR) {
                ScreenToggleDot(board->screen, 0);
                ScreenToggleDot(board->screen, 1);
                ScreenToggleDot(board->screen, 2);
                ScreenToggleDot(board->screen, 3);
            }
        }

        if (DigitalInputWasDeactivated(board->increment)) {
            inactivity_timer = 0;
            if (current_mode == SET_TIME_MINUTE || current_mode == SET_ALARM_MINUTE) {
                clock_increment_bcd(&digits[3], &digits[2], 9, 5);
            } else if (current_mode == SET_TIME_HOUR || current_mode == SET_ALARM_HOUR) {
                clock_increment_bcd(&digits[1], &digits[0], 3, 2);
            }

            ScreenWriteBCD(board->screen, digits, sizeof(digits));

            if ( current_mode == SET_ALARM_MINUTE || current_mode == SET_ALARM_HOUR) {
                ScreenToggleDot(board->screen, 0);
                ScreenToggleDot(board->screen, 1);
                ScreenToggleDot(board->screen, 2);
                ScreenToggleDot(board->screen, 3);
            }
        }

        /* TIEMPO DE INACTIVIDAD */
        if ((current_mode == SET_TIME_MINUTE || current_mode == SET_TIME_HOUR ||
             current_mode == SET_ALARM_MINUTE || current_mode == SET_ALARM_HOUR) &&
            inactivity_timer >= INACTIVITY_TIMEOUT_MS) {
            if (current_mode == SET_TIME_MINUTE || current_mode == SET_TIME_HOUR) {
                if (ClockGetTime(clock, &current_time_data)) {
                    clock_switch_mode(SHOW_TIME);
                } else {
                    clock_switch_mode(UNCONFIGURED);
                }
            } else {
                clock_switch_mode(SHOW_TIME);
            }
        }

        for (volatile int delay_loop = 0; delay_loop < 25000; delay_loop++) {
            __asm("NOP");
        }
    }
 }

 void SysTick_Handler(void) {
    static uint16_t flash_counter = 0;
    clock_time_t time;

    ScreenRefresh(board->screen);
    ClockNewTick(clock);

    inactivity_timer++;

    if (current_mode == SHOW_TIME) {
        flash_counter = (flash_counter + 1) % 1000;

        if (ClockGetTime(clock, &time)) {
            clock_convert_time_to_bcd(&time, digits);
            ScreenWriteBCD(board->screen, digits, sizeof(digits));
        }

        if (flash_counter > 500) {
            ScreenToggleDot(board->screen, 1);
        }

        if (ClockIsAlarmEnabled(clock)) {
            ScreenSetDot(board->screen, 3, true);
        } else {
            ScreenSetDot(board->screen, 3, false);
        }

        if (ClockIsAlarmActive(clock)) {
            ScreenSetDot(board->screen, 0, true);
            DigitalOutputActivate(board->led_red);
        } else {
            ScreenSetDot(board->screen, 0, false);
            DigitalOutputDeactivate(board->led_red);
        }
    }else if (current_mode == UNCONFIGURED){
        digits[0] = digits[1] = digits[2] = digits[3] = 0;
        ScreenWriteBCD(board->screen, digits, sizeof(digits));
        ScreenSetDot(board->screen, 1, true);
    }
}
/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
