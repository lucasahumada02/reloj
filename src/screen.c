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

/** @file screen.c
 ** @brief Implementación del módulo para la gestión de una pantalla multiplexada de 7 segmentos
 **/

/* === Headers files inclusions ==================================================================================== */

#include "screen.h"
#include "poncho.h"
#include <stdlib.h>
#include <string.h>

/* === Macros definitions ========================================================================================== */

#ifndef SCREEN_MAX_DIGITS
#define SCREEN_MAX_DIGITS 8
#endif


/* === Private data type declarations ============================================================================== */

struct screen_s{
    uint8_t digits;
    uint8_t value[SCREEN_MAX_DIGITS];
    uint8_t dots[SCREEN_MAX_DIGITS];
    uint8_t current_digit;

    uint8_t flashing_Digits_from;
    uint8_t flashing_Digits_to;
    uint8_t flashing_Digits_count;
    uint16_t flashing_Digits_frecuency;


    uint8_t flashing_Dots_from;
    uint8_t flashing_Dots_to;
    uint8_t flashing_Dots_count;
    uint16_t flashing_Dots_frecuency;

    screen_driver_t driver;
    
};

/* === Private function declarations =============================================================================== */
static const uint8_t IMAGES[] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E |SEGMENT_F, //0
    SEGMENT_B | SEGMENT_C, //1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,//2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,//3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,//4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,//5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,//6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,//7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,//8
    DIGIT_1_MASK|DIGIT_2_MASK|DIGIT_3_MASK|DIGIT_4_MASK|SEGMENTS_MASK //9
};

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */


/* === Public function implementation ============================================================================== */
screen_t ScreenCreate(uint8_t digits, screen_driver_t driver){
    screen_t self = malloc(sizeof(struct screen_s));
    if (digits > SCREEN_MAX_DIGITS){
        digits = SCREEN_MAX_DIGITS;
    }
    if (self != NULL){
        self ->digits = digits;
        self->driver = driver;
        self->current_digit = 0;
        self->flashing_Digits_count = 0;
        self->flashing_Digits_frecuency = 0;
        self->flashing_Dots_count = 0;
        self->flashing_Dots_frecuency = 0;
    }
    return self;
}
void ScreenWriteBCD(screen_t self, uint8_t  value[], uint8_t size,uint8_t  dots[]){
    memset(self->value,0,sizeof(self->value));
    memset(self->dots,0,sizeof(self->dots));
    if (size > self->digits){
        size = self->digits;
    }
    for (uint8_t i = 0; i < size; i++){
        self->value[i]=IMAGES[value[i]];
        self->dots[i]= dots[i] ? SEGMENT_P : 0;
    }
}
void ScreenRefresh(screen_t self){
    uint8_t segments, dots;

    self->driver->DigitsTurnOff();
    self->current_digit = (self->current_digit + 1) % self->digits;
    
    segments = self->value[self->current_digit];
    if (self->flashing_Digits_frecuency != 0){
        if (self->current_digit == 0){
            self->flashing_Digits_count = (self->flashing_Digits_count + 1) % (self->flashing_Digits_frecuency);
        }
        if (self->flashing_Digits_count < (self->flashing_Digits_frecuency / 2)){
            if (self->current_digit >= self->flashing_Digits_from){
                if (self->current_digit <= self->flashing_Digits_to)
                {
                   segments = 0;
                }
            } 
        }  
    }

    dots = self->dots[self->current_digit];
    if (self->flashing_Dots_frecuency != 0){
        if (self->current_digit == 0){
            self->flashing_Dots_count = (self->flashing_Dots_count + 1) % (self->flashing_Dots_frecuency);
        }
        if (self->flashing_Dots_count < (self->flashing_Dots_frecuency / 2)){
            if (self->current_digit >= self->flashing_Dots_from){
                if (self->current_digit <= self->flashing_Dots_to)
                {
                   dots = 0;
                }
            } 
        }  
    }
    self->driver->SegmentsUpdate(segments | dots);
    self->driver->DigitTurnOn(self->current_digit);
    
}

int DisplayFlashDigits(screen_t self, uint8_t from, uint8_t to, uint16_t divisor){
    int result = 0;

    if ((from > to) || (from >= SCREEN_MAX_DIGITS) || (to >= SCREEN_MAX_DIGITS)){
        result = -1;
    } else if (!self){
        result = -1;
    }else{
            self->flashing_Digits_from = from;
            self->flashing_Digits_to = to;
            self->flashing_Digits_frecuency = 2 * divisor;
            self->flashing_Digits_count = 0;
    }

    return result;
}

int DisplayFlashDots(screen_t self, uint8_t from, uint8_t to, uint16_t divisor){
    int result = 0;

    if ((from > to) || (from >= SCREEN_MAX_DIGITS) || (to >= SCREEN_MAX_DIGITS)){
        result = -1;
    } else if (!self){
        result = -1;
    }else{
            self->flashing_Dots_from = from;
            self->flashing_Dots_to = to;
            self->flashing_Dots_frecuency = 2 * divisor;
            self->flashing_Dots_count = 0;
    }

    return result;
}

/* === End of documentation ======================================================================================== */
