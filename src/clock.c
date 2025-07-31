/*********************************************************************************************************************
Copyright (c) 2025, Lucas Ahumada Checa Casquero <lucasahum@gmial.com>

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
 ** @brief Implementación del módulo de reloj digital con funciones de alarma, snooze y cancelación diaria.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include <stddef.h>
#include <string.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/** @brief Estructura interna del reloj */
struct clock_s {
    uint16_t clock_ticks;            /**< Ticks acumulados desde el último segundo */
    uint16_t ticks_per_second;       /**< Cantidad de ticks necesarios para avanzar un segundo */
    clock_time_t current_time;       /**< Hora actual del reloj */
    clock_time_t alarm_time;         /**< Hora configurada de la alarma */
    bool valid_time;                 /**< Indica si la hora actual es válida */
    bool valid_alarm;                /**< Indica si hay una alarma válida configurada */
    bool alarm_enabled;              /**< Estado de habilitación de la alarma */
    bool alarm_cancelled_today;      /**< Indica si la alarma fue cancelada hoy */
    bool alarm_ringing;              /**< Indica si la alarma está sonando */
    clock_alarm_callback_t callback;
};
/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/**
 * @brief Verifica si un tiempo dado es válido en formato BCD.
 * @param time Puntero a la estructura de tiempo a validar.
 * @return true si el tiempo es válido, false en caso contrario.
 */
static bool IsValidTime(const clock_time_t * time) {
    bool is_valid = true;
    if (time->time.hours[1] > 2 || (time->time.hours[1] == 2 && time->time.hours[0] > 3) || time->time.minutes[1] > 5 ||
        time->time.minutes[0] > 9 || time->time.seconds[1] > 5 || time->time.seconds[0] > 9) {
        is_valid = false;
    }
    return is_valid;
}

static void SyncBcdFromTime(clock_time_t *t) {
    t->bcd[5] = t->time.hours[1];
    t->bcd[4] = t->time.hours[0];
    t->bcd[3] = t->time.minutes[1];
    t->bcd[2] = t->time.minutes[0];
    t->bcd[1] = t->time.seconds[1];
    t->bcd[0] = t->time.seconds[0];
}

static bool BcdIncrement(uint8_t * units, uint8_t * tens, uint8_t max_units, uint8_t max_tens) {
    (*units)++;
    if (*units > max_units) {
        *units = 0;
        (*tens)++;
        if (*tens > max_tens) {
            *tens = 0;
            return true;
        }
    }
    return false;
}

static void AdvanceTime(clock_t self) {
    clock_time_t *t = &self->current_time;

   bool was_end_of_day =
    (t->time.hours[0] == 3 && t->time.hours[1] == 2 &&
     t->time.minutes[0] == 9 && t->time.minutes[1] == 5 &&
     t->time.seconds[0] == 9 && t->time.seconds[1] == 5);

    if (BcdIncrement(&t->time.seconds[0], &t->time.seconds[1], 9, 5)) {
        if (BcdIncrement(&t->time.minutes[0], &t->time.minutes[1], 9, 5)) {
            if (BcdIncrement(&t->time.hours[0], &t->time.hours[1], 9, 2)) {
                t->time.hours[0] = 0;
                t->time.hours[1] = 0;
            } else {
                uint8_t hour_decimal = t->time.hours[1] * 10 + t->time.hours[0];
                if (hour_decimal >= 24) {
                    t->time.hours[0] = 0;
                    t->time.hours[1] = 0;
                }
            }

        }
    }

    if (was_end_of_day) {
        self->alarm_cancelled_today = false;
    }

    SyncBcdFromTime(t);

    if (self->alarm_enabled && self->valid_alarm && !self->alarm_cancelled_today) {
        if (memcmp(t->bcd, self->alarm_time.bcd, sizeof(t->bcd)) == 0) {
            if (!self->alarm_ringing) {
                self->alarm_ringing = true;
                if (self->callback) {
                    self->callback(self);
                }
            }
        }
    }
}

static void PosponeAlarm(clock_t self, uint8_t minutes) {
    uint8_t dec_min = self->alarm_time.time.minutes[1] * 10 + self->alarm_time.time.minutes[0];
    uint8_t dec_hour = self->alarm_time.time.hours[1] * 10 + self->alarm_time.time.hours[0];

    dec_min += minutes;
    dec_hour += dec_min / 60;
    dec_min = dec_min % 60;
    dec_hour = dec_hour % 24;

    self->alarm_time.time.minutes[1] = dec_min / 10;
    self->alarm_time.time.minutes[0] = dec_min % 10;
    self->alarm_time.time.hours[1] = dec_hour / 10;
    self->alarm_time.time.hours[0] = dec_hour % 10;

    SyncBcdFromTime(&self->alarm_time);
}
/* === Public function implementation ============================================================================== */
clock_t ClockCreate(uint16_t ticks_per_second, clock_alarm_callback_t callback){
    
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s));
    self->ticks_per_second = ticks_per_second;
    self->callback = callback;
    self->alarm_cancelled_today = false;
    self->valid_time = false;
    self->valid_alarm = false;

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
        SyncBcdFromTime(&self->current_time);
    }else
    {
        self->valid_time = false;
    }
    return self->valid_time;
}

void ClockNewTick(clock_t self){
    self->clock_ticks++;
    if (self->clock_ticks < self->ticks_per_second) {
        return;
    }
    self->clock_ticks = 0;

    AdvanceTime(self);
}

bool ClockSetAlarm(clock_t self, const clock_time_t * alarm_time){
     if (!IsValidTime(alarm_time)) {
        self->valid_alarm = false;
        return false;
    }
    memcpy(&self->alarm_time, alarm_time, sizeof(clock_time_t));
    SyncBcdFromTime(&self->alarm_time);
    self->valid_alarm = true;
    self->alarm_enabled = true;
    self->alarm_cancelled_today = false;
    self->alarm_ringing = false;
    return self->valid_alarm;
}

bool ClockGetAlarm(clock_t self, clock_time_t * alarm_time){
    memcpy(alarm_time, &self->alarm_time, sizeof(clock_time_t));
    return self->valid_alarm;
}

bool ClockIsAlarmActive(clock_t self) {
    return self && self->alarm_ringing;
}

void ClockDisableAlarm(clock_t self) {
    if (self) {
        self->alarm_enabled = false;
        self->alarm_ringing = false;
    }
}

void ClockSnoozeAlarm(clock_t self) {
   if (!self || !self->alarm_ringing) return;
    self->alarm_ringing = false;
    PosponeAlarm(self, 5);
}


void ClockPostponeAlarmToNextDay(clock_t self) {
    if (!self || !self->valid_alarm) return;
    
    uint8_t current_min = self->alarm_time.time.minutes[1] * 10 + self->alarm_time.time.minutes[0];
    uint8_t current_hour = self->alarm_time.time.hours[1] * 10 + self->alarm_time.time.hours[0];
    
    uint16_t total_min = current_hour * 60 + current_min + 1440;
    
    uint8_t new_hour = (total_min / 60) % 24;
    uint8_t new_min = total_min % 60;
    
    self->alarm_time.time.hours[1] = new_hour / 10;
    self->alarm_time.time.hours[0] = new_hour % 10;
    self->alarm_time.time.minutes[1] = new_min / 10;
    self->alarm_time.time.minutes[0] = new_min % 10;
    
    SyncBcdFromTime(&self->alarm_time);
    
    self->alarm_ringing = false;
    self->alarm_cancelled_today = true;
    self->alarm_enabled = true;
}

bool ClockIsAlarmEnabled(clock_t self) {
    return self && self->alarm_enabled && self->valid_alarm;
}
/* === End of documentation ======================================================================================== */
