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

/** @file digitales.c
 ** @brief Codigo fuente del modulo para la gestión entradas y salidas digitales
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digitales.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
/* === Macros definitions ========================================================================================== */

/*#ifdef USAR_MEMORIA_ESTATICA
    #ifndef ALUMNOS_MAX_INSTANCIAS
        #define ALUMNOS_MAX_INSTANCIAS 2 //!<Cantidad maxima de alumnos
    #endif
#endif*/

/* === Private data type declarations ============================================================================== */



/* === Private function declarations =============================================================================== */

/**
 * @brief Crea una nueva instancia de la estructura alumno_s.
 *
 * @return Puntero a la nueva instancia de alumno_s, o NULL si no hay espacio disponible.
 */
/*#ifdef USAR_MEMORIA_ESTATICA
static alumno_t CrearInstancia(void);
#endif*/

/**
 * @brief Serializa un campo numérico clave-valor en formato JSON.
 *
 * @param campo Nombre del campo
 * @param valor Valor numérico
 * @param buffer Buffer de salida
 * @param size  Tamaño disponible para escribir la cadena
 * @return int Longitud del texto generado o -1 si el espacio no es suficiente
 */
static int SerializarNumero(char campo[], int valor, char buffer[], uint32_t size);

/* === Private variable definitions ================================================================================ */
//#ifdef USAR_MEMORIA_ESTATICA
//    static struct alumno_s instancias[ALUMNOS_MAX_INSTANCIAS]={0};
//#endif
/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */
//#ifdef USAR_MEMORIA_ESTATICA
//static alumno_t CrearInstancia(){
//    alumno_t self = NULL;
//    for (int i = 0; i < ALUMNOS_MAX_INSTANCIAS; i++)
//    {
//        if (!instancias[i].ocupado)
//        {
//            instancias[i].ocupado = true;
  //          self = &instancias[i];
 //           break;
  //      }
        
   // }
    
   //return self;
    
//}
//#endif


/* === Public function implementation ============================================================================== */

//alumno_t CrearAlumno(char* nombre, char* apellido, uint32_t dni) {
//    #ifdef USAR_MEMORIA_DINAMICA
//        alumno_t self = malloc(sizeof(struct alumno_s));
//    #else
//        alumno_t self = CrearInstancia();
//    #endif
//    if (self != NULL) {
//        self->documento = dni;
//        strncpy(self->nombre, nombre, sizeof(self->nombre) - 1);
//        strncpy(self->apellido, apellido, sizeof(self->apellido) - 1);
//    } else{
//        printf("Error alumno\n");
//    }
//    return self;
//}

/* === End of documentation ======================================================================================== */
