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

    // 1. Inicialización de estructuras de datos
    crear_tabla();
    printf("Inicio del programa\n");
    // Impresión de la tabla
    imprimir_tabla();
    printf("\n");

    // 2. Etapa de compilación (análisis léxico)
    printf("Análisis léxico\n");
    iniciar_analisis(argv[1]);
    printf("\n");
    // Impresión de la tabla
    imprimir_tabla();
    printf("\n");

    // 3. Fase de finalización
    printf("Final del programa\n");
    // Liberación de la memoria de la tabla
    eliminar_tabla();

    return 0;
}
