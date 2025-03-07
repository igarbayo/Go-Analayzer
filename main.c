

#include <stdio.h>
#include <stdlib.h>
#include "tabla_simbolos.h"
#include "analisis_sintactico.h"


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("\nError: número de parámetros incorrecto. Usar %s <código_fuente>", argv[0]);
    }

    crear_tabla();
    printf("Inicio del programa\n");
    //imprimir_tabla();
    printf("\n");

    printf("Análisis léxico\n");
    iniciar_analisis(argv[1]);
    printf("\n");

    printf("Final del programa\n");
    //imprimir_tabla();
    eliminar_tabla();

    return 0;
}
