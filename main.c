//
// Created by ignacio on 21/02/25.
//

#include <stdio.h>
#include <stdlib.h>
#include "tabla_simbolos.h"
#include "analisis_sintactico.h"


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("\nError: número de parámetros incorrecto. Usar %s <código_fuente>", argv[0]);
    }

    crear_tabla();
    imprimir_tabla();

    iniciar_analisis(argv[1]);

    imprimir_tabla();
    eliminar_tabla();
}
