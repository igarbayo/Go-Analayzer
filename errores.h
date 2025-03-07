#ifndef P1_COMPGO_ERRORES_H
#define P1_COMPGO_ERRORES_H

// ERRORES LÉXICOS //////////////////////////////////////////

/**
 * Error de construcción de integer literal
 * @param linea del error
 * @param columna del error
 * @param lexema erróneo
 */
void error_entero(int linea, int columna, char* lexema);

/**
 * Error de construcción de floating-point literal
 * @param linea del error
 * @param columna del error
 * @param lexema erróneo
 */
void error_float(int linea, int columna, char* lexema);

/**
 * Error de construcción de string literal
 * @param linea del error
 * @param columna del error
 * @param lexema erróneo
 */
void error_string(int linea, int columna, char* lexema);

/**
 * Error de construcción de hexadecimal integer literal o hexadecimal floating-point literal
 * @param linea del error
 * @param columna del error
 * @param lexema erróneo
 */
void error_hexadecimal(int linea, int columna, char* lexema);


// ERRORES DE EJECUCIÓN ///////////////////////////////////////

/**
 * Error de lectura en fichero de entrada
 */
void error_fichero();

/**
 * Error de reserva de memoria
 */
void error_memoria();

/**
 * Error de número de parámetros de entrada del programa
 * @param programa nombre del ejecutable para imprimir
 */
void error_parametros(char* programa);

#endif //P1_COMPGO_ERRORES_H
