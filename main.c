#include <stdio.h>
#include "tabla_simbolos.h"
#include "analisis_sintactico.h"
#include "errores.h"


int main(int argc, char **argv) {

    // Verificación de número de parámetros
    if (argc != 2) {
        error_parametros(argv[0]);
        return 1;
    }

    // Creación de la tabla
    crear_tabla();
    printf("Inicio del programa\n");
    imprimir_tabla();
    printf("\n");

    // Etapa de análisis léxico
    printf("Análisis léxico\n");
    iniciar_analisis(argv[1]);
    printf("\n");

    // Liberación de la memoria
    printf("Final del programa\n");
    imprimir_tabla();
    eliminar_tabla();

    return 0;
}
