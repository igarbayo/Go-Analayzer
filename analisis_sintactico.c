

#include <stdio.h>
#include <stdlib.h>
#include "analisis_sintactico.h"
#include "analisis_lexico.h"
#include "sistema_entrada.h"

void iniciar_analisis(char *nombre_fichero) {
    contenedor c;
    int i=0;

    iniciar_analisis_lexico(nombre_fichero);

    do {
       c = sig_comp_lexico();
       if (c.comp_lexico != FINFICHERO && c.comp_lexico != -1) {
            printf("%d\t\t<%d, \"%s\">\n", i, c.comp_lexico, c.lexema);
       }
       i++;
    } while (c.comp_lexico != FINFICHERO);


}