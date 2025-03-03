

#include <stdio.h>
#include <stdlib.h>

void _imprimir_error_lexico(char* cadena) {
    printf("Error léxico. %s mal construido\n", cadena);
}

void error_string() {
    _imprimir_error_lexico("String");
}

void error_hexadecimal() {
    _imprimir_error_lexico("Hexadecimal");
}

void error_fichero() {
    printf("Error. No se puede leer el fichero\n");
}

void error_memoria() {
    printf("Error. No se puede reservar memoria para el lexema\n");
}
