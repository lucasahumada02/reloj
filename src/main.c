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

int main(void) {
    board_t board = BoardCreate();

    // Mostrar "----" al arrancar
    uint8_t sin_tecla[4] = {10, 10, 10, 10};  // 10 = "-"
    ScreenWriteBCD(board->screen, sin_tecla, 4);

    while (true) {
        uint8_t valor[4] = {10, 10, 10, 10};  // Default "-"

        if (DigitalInputGetIsActive(board->accept)) {
            valor[3] = 1;  // Mostrar "0001"
        } else if (DigitalInputGetIsActive(board->cancel)) {
            valor[3] = 2;
        } else if (DigitalInputGetIsActive(board->set_time)) {
            valor[3] = 3;
        } else if (DigitalInputGetIsActive(board->set_alarm)) {
            valor[3] = 4;
        } else if (DigitalInputGetIsActive(board->increment)) {
            valor[3] = 5;
        } else if (DigitalInputGetIsActive(board->decrement)) {
            valor[3] = 6;
        }

        ScreenWriteBCD(board->screen, valor, 4);
        ScreenRefresh(board->screen);

        // Pequeño retardo para evitar parpadeos
        for (volatile int i = 0; i < 10000; i++) {
            __asm("NOP");
        }
    }
}


// #include <stdbool.h>
// #include "bsp.h"
// #include "clock.h"

// /* === Macros definitions ====================================================================== */

// /* === Private data type declarations ========================================================== */

// /* === Private variable declarations =========================================================== */

// /* === Private function declarations =========================================================== */

// /* === Public variable definitions ============================================================= */

// /* === Private variable definitions ============================================================ */

// /* === Private function implementation ========================================================= */

// /* === Public function implementation ========================================================= */

// int main(void) { 
//     int divisor  = 0;
//     uint8_t value[4]= {1, 2, 3, 4};
//     //uint8_t dots[4]={1,1,1,1};

//     board_t board = BoardCreate();

//     ScreenWriteBCD(board->screen, value, sizeof(value));
//     DisplayFlashDigits(board->screen, 0, 3, 50);

//     while (true) {
//         if (DigitalInputGetIsActive(board->accept)) {
//             DigitalOutputActivate(board->led_red);
//         } else {
//             DigitalOutputDeactivate(board->led_red);
//         }

//         //   if (DigitalInputWasActivated(board->accept)) {
//         //     DigitalOutputToggle(board->led_red);  // LED rojo = ACEPTAR
//         // }

//         // if (DigitalInputWasActivated(board->cancel)) {
//         //     DigitalOutputToggle(board->led_green); // LED verde = CANCELAR
//         // }

//         // if (DigitalInputWasActivated(board->set_time)) {
//         //     DigitalOutputToggle(board->led_blue);  // LED azul = F1
//         // }

//         // if (DigitalInputWasActivated(board->set_alarm)) {
//         //     DigitalOutputActivate(board->buzzer);  // F2 = activa buzzer
//         // }

//         // if (DigitalInputWasActivated(board->increment)) {
//         //     DigitalOutputDeactivate(board->buzzer); // F3 = apaga buzzer
//         // }

//         // // if (DigitalInputWasActivated(board->decrement)) {
//         // //     DigitalOutputToggle(board->led_red);   // F4 = vuelve a togglear rojo
//         // // }
    
//         divisor++;
//         if (divisor == 5) {
//             divisor = 0;
//             ScreenToggleDot(board->screen, 0);
//             ScreenToggleDot(board->screen, 1);
//             ScreenToggleDot(board->screen, 2);
//             ScreenToggleDot(board->screen, 3);

         
//         }
//         ScreenRefresh(board->screen);
//             for (int delay = 0; delay < 25000; delay++) {
//                 __asm("NOP");
//             }
//     }
// }

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
