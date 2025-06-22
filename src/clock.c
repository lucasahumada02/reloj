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

/** @file clock.c
 ** @brief Plantilla para la creación de archivos de código fuente en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include <stddef.h>
#include <string.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

struct clock_s {
    uint16_t clock_ticks;
    clock_time_t current_time;
    clock_time_t alarm_time;
    bool valid_time;
    bool valid_alarm;
    bool alarm_enabled;
    bool alarm_triggered;
};


/* === Public variable definitions ================================================================================= */


/* === Private function definitions ================================================================================ */

static bool IsValidTime(const clock_time_t * time) {
    bool is_valid = true;
    if (time->time.hours[1] > 2 || (time->time.hours[1] == 2 && time->time.hours[0] > 3) || time->time.minutes[1] > 5 ||
        time->time.minutes[0] > 9 || time->time.seconds[1] > 5 || time->time.seconds[0] > 9) {
        is_valid = false;
    }
    return is_valid;
}

/* === Public function implementation ============================================================================== */
clock_t ClockCreate(uint16_t ticks_per_second){
    (void) ticks_per_second;
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s));
    self->valid_time = false;
    self->valid_alarm = false;
    self->alarm_enabled = false;
    self->alarm_triggered = false;
    return self;
}

bool ClockGetTime(clock_t self, clock_time_t * result){
    if (result != NULL)
    {
        memcpy(result, &self->current_time, sizeof(clock_time_t));
    } else
    {
        return false;
    }

    return self->valid_time;
}

bool ClockSetTime(clock_t self, const clock_time_t * new_time){
    if (IsValidTime(new_time))
    {
        self->valid_time = true;
        memcpy(&self->current_time, new_time, sizeof(clock_time_t));
    }else
    {
        self->valid_time = false;
    }
    return self->valid_time;
}

void ClockNewTick(clock_t self){
    self->clock_ticks++;
    if (self->clock_ticks < 5) {
        return;
    }
    self->clock_ticks = 0;

    //Segundos
    self->current_time.time.seconds[0]++;
    if (self->current_time.time.seconds[0] > 9) {
        self->current_time.time.seconds[0] = 0;
        self->current_time.time.seconds[1]++;

        if (self->current_time.time.seconds[1] > 5) {
            self->current_time.time.seconds[1] = 0;

            //Minutos
            self->current_time.time.minutes[0]++;

            if (self->current_time.time.minutes[0] > 9) {
                self->current_time.time.minutes[0] = 0;
                self->current_time.time.minutes[1]++;

                if (self->current_time.time.minutes[1] > 5) {
                    self->current_time.time.minutes[1] = 0;

                    //Horas
                    self->current_time.time.hours[0]++;

                    if ((self->current_time.time.hours[1] < 2 && self->current_time.time.hours[0] > 9) ||
                        (self->current_time.time.hours[1] == 2 && self->current_time.time.hours[0] > 3)) {
                        self->current_time.time.hours[0] = 0;
                        self->current_time.time.hours[1]++;

                        // Si supera 23, reiniciar a 00:00:00
                        if (self->current_time.time.hours[1] > 2) {
                            self->current_time.time.hours[0] = 0;
                            self->current_time.time.hours[1] = 0;
                        }
                    }
                }
            }
        }
    }
    if (self->alarm_enabled && self->valid_alarm) {
    bool equal = true;
    for (int i = 0; i < 6; i++) {
        if (self->current_time.bcd[i] != self->alarm_time.bcd[i]) {
            equal = false;
            break;
        }
    }
    if (equal) {
        self->alarm_triggered = true;
    }
}

}

bool ClockSetAlarm(clock_t self, const clock_time_t * alarm_time){
    memcpy(&self->alarm_time, alarm_time, sizeof(clock_time_t));
    self->valid_alarm = true;
    self->alarm_enabled = true;
    self->alarm_triggered = false;
    return self->valid_alarm;
}

bool ClockGetAlarm(clock_t self, clock_time_t * alarm_time){
    memcpy(alarm_time, &self->alarm_time, sizeof(clock_time_t));
    return self->valid_alarm;
}

bool ClockIsAlarmActive(clock_t self) {
    return self->alarm_enabled && self->alarm_triggered;
}

void ClockDisableAlarm(clock_t self) {
    self->alarm_enabled = false;
    self->alarm_triggered = false;
}


/* === End of documentation ======================================================================================== */
