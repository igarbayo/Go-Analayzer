
#ifndef P1_COMPGO_ERRORES_H
#define P1_COMPGO_ERRORES_H

// Errores léxicos
void error_entero(int linea, int columna);

void error_float(int linea, int columna);

void error_string(int linea, int columna);

void error_hexadecimal(int linea, int columna);


// Errores de ejecución
void error_fichero();

void error_memoria();



#endif //P1_COMPGO_ERRORES_H
