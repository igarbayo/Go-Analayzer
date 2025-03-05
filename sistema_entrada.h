
#ifndef P1_COMPGO_SISTEMA_ENTRADA_H
#define P1_COMPGO_SISTEMA_ENTRADA_H

#include "definiciones.h"


// inicialmente, será muy grande
// luego, se va variando para pruebas
#define TAMBLOQUE 64
#define FINBLOQUE '\0'

// Creación del tipo de dato
typedef struct {
    char array_fisico[2*TAMBLOQUE];
    char *inicio;
    char *delantero;
} centinela;

/**
 * Inicia el sistema de entrada
 * @param fichero con el código fuente
 * @return 1 si hay error al cargar el fichero
 * @return 0 en otro caso
 */
int iniciar_sistema_entrada(char* nombre_fichero);

/**
 * Obtiene el siguiente carácter
 * @return char con el carácter siguiente
 */
char sig_caracter();

/**
 * Copia el lexema procesándose actualmente
 * @param c contenedor donde copiar el lexema
 */
void copiar_lexema(contenedor *c);

/**
 * Ignora el contenido de un lexema.
 * Avanza inicio a delantero
 */
void ignorar_lexema();

/**
 * Retrocede el procesamiento de un carácter
 */
void devolver_un_caracter();

/**
 * Retrocede el procesamiento de dos caracteres
 */
void devolver_dos_caracteres();

/**
 * Ignora el siguiente carácter
 * Similar a sig_caracter() pero sin devolver nada
 */
void ignorar_caracter();

#endif //P1_COMPGO_SISTEMA_ENTRADA_H
