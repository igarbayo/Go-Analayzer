//
// Created by ignacio on 21/02/25.
//

#ifndef P1_COMPGO_SISTEMA_ENTRADA_H
#define P1_COMPGO_SISTEMA_ENTRADA_H

#include "definiciones.h"

/**
 * Inicia el sistema de entrada
 * @param fichero con el código fuente
 */
void iniciar_sistema_entrada(char* fichero);

/**
 * Obtiene el siguiente carácter
 * @return char con el carácter siguiente
 */
char sig_caracter();

/**
 * Acepta el lexema procesándose actualmente
 * @param c contenedor con el lexema
 */
void copiar_lexema(contenedor *c);

void devolver_un_caracter();

void devolver_dos_caracteres();

void ignorar_caracter();

#endif //P1_COMPGO_SISTEMA_ENTRADA_H
