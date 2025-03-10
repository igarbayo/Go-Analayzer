#include <stdio.h>

// Colores para la impresión
#define RED     "\x1b[31m"
#define YELLOW  "\x1b[33m"
#define RESET   "\x1b[0m"

// ERRORES LÉXICOS //////////////////////////////////

/**
 * Función privada para la impresión parametrizada de los errores
 * @param mensaje tipo de componente léxico que presenta el error
 * @param linea del error
 * @param columna del error
 * @param lexema erróneo
 */
void _imprimir_error_lexico(const char* mensaje, int linea, int columna, char* lexema) {
    printf(RED "\tError léxico: %s\n" RESET, mensaje);
    printf(RED "\tLínea %d, columna %d: " RESET, linea, columna);
    printf(YELLOW "%s\n" RESET, lexema);
    printf(RED "\tSugerencia: Revise el carácter o símbolo en la posición indicada.\n" RESET);
}

void error_string(int linea, int columna, char* lexema) {
    _imprimir_error_lexico("String", linea, columna, lexema);
}

void error_hexadecimal(int linea, int columna, char* lexema) {
    _imprimir_error_lexico("Hexadecimal", linea, columna, lexema);
}

void error_entero(int linea, int columna, char* lexema) {
    _imprimir_error_lexico("Integer", linea, columna, lexema);
}

void error_float(int linea, int columna, char* lexema) {
    _imprimir_error_lexico("Float", linea, columna, lexema);
}

// ERRORES DE EJECUCIÓN ///////////////////////////////////////////

void error_fichero() {
    printf(RED "\tError de ejecución: no se puede leer el fichero.\n" RESET);
}

void error_memoria() {
    printf(RED "\tError de ejecución: no se puede reservar memoria.\n" RESET);
}

void error_parametros(char* programa) {
    printf(RED "\tError de ejecución: número de parámetros incorrecto. Usar %s <código_fuente>.\n" RESET, programa);
}
