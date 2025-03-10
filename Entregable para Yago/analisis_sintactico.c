#include <stdio.h>
#include "analisis_sintactico.h"
#include "analisis_lexico.h"

void iniciar_analisis(char *nombre_fichero) {
    contenedor c;
    int i=0;

    // Inicio
    iniciar_analisis_lexico(nombre_fichero);

    // Consumo de componentes léxicos
    do {
       c = sig_comp_lexico();
       printf("%d\t\t<%d, \"%s\">\n", i, c.comp_lexico, c.lexema);
       i++;
    } while (c.comp_lexico != FINFICHERO);

    // Fin
    terminar_analisis_lexico();

}