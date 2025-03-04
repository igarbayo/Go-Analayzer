

#include <stdio.h>
#include <stdlib.h>

void _imprimir_error_lexico(const char* mensaje, int linea, int columna) {
    printf("Error léxico: %s\n", mensaje);
    printf("Línea %d, columna %d:\n", linea, columna);

    // Imprimir un indicador de la posición del error
    for (int i = 0; i < columna + 4; i++) {
        printf(" ");
    }
    printf("^\n");

    // Imprimir una sugerencia (opcional)
    printf("Sugerencia: Revise el carácter o símbolo en la posición indicada.\n");
}

void error_string(int linea, int columna) {
    _imprimir_error_lexico("String", linea, columna);
}

void error_hexadecimal(int linea, int columna) {
    _imprimir_error_lexico("Hexadecimal", linea, columna);
}

void error_fichero() {
    printf("Error. No se puede leer el fichero\n");
}

void error_memoria() {
    printf("Error. No se puede reservar memoria para el lexema\n");
}
