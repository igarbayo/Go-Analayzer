#ifndef P1_COMPGO_SISTEMA_ENTRADA_H
#define P1_COMPGO_SISTEMA_ENTRADA_H

#include "definiciones.h"

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
 * Cierra el sistema de entrada y su fichero asociado
 */
void cerrar_sistema_entrada();

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
 * Copia una cadena al lexema del contenedor
 * @param c contenedor donde copiar
 * @param lexema cadena a copiar
 */
void asignar_lexema(contenedor* c, char* lexema);

/**
 * Retrocede el procesamiento de un carácter
 */
void devolver_un_caracter();

/**
 * Ignora el siguiente carácter
 * Similar a sig_caracter() pero sin devolver nada
 */
void ignorar_caracter();

#endif //P1_COMPGO_SISTEMA_ENTRADA_H
