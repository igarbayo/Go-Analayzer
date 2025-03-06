

#include <stdio.h>
#include <stdlib.h>


// ERRORES LÉXICOS
void _imprimir_error_lexico(const char* mensaje, int linea, int columna) {
    printf("\tError léxico: %s\n", mensaje);
    printf("\tLínea %d, columna %d:\n", linea, columna);
    // Imprimir una sugerencia (opcional)
    printf("\tSugerencia: Revise el carácter o símbolo en la posición indicada.\n");
}

void error_string(int linea, int columna) {
    _imprimir_error_lexico("String", linea, columna);
}

void error_hexadecimal(int linea, int columna) {
    _imprimir_error_lexico("Hexadecimal", linea, columna);
}

void error_entero(int linea, int columna) {
    _imprimir_error_lexico("Integer", linea, columna);
}

void error_float(int linea, int columna) {
    _imprimir_error_lexico("Float", linea, columna);
}

void error_rune(int linea, int columna) {
    _imprimir_error_lexico("Rune", linea, columna);
}


// ERRORES GENERALES
void error_fichero() {
    printf("Error. No se puede leer el fichero\n");
}

void error_memoria() {
    printf("Error. No se puede reservar memoria para el lexema\n");
}
