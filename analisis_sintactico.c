//
// Created by ignacio on 19/02/25.
//

#include <stdio.h>
#include <stdlib.h>
#include "analisis_sintactico.h"
#include "analisis_lexico.h"

void iniciar_analisis(char *nombre_fichero) {
    contenedor c;
    do {
       c = sig_comp_lexico();
       if (c.comp_lexico != FINFICHERO) {
            printf("<%d, %s>\n", c.comp_lexico, c.lexema);
       }
       free(c.lexema);
    } while (c.comp_lexico != FINFICHERO);
}