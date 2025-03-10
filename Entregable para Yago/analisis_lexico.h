#ifndef P1_COMPGO_ANALISIS_LEXICO_H
#define P1_COMPGO_ANALISIS_LEXICO_H

#include "definiciones.h"

/**
 * Inicia el análisis léxico mediante el sistema de entrada
 * @param fichero con el código fuente
 */
void iniciar_analisis_lexico(char* fichero);

/**
 * Termina el análisis léxico cerrando el sistema de entrada
 */
void terminar_analisis_lexico();

/**
 * Consume un componente léxico (contenedor)
 * @return contenedor con componente léxico y lexema asociado
 */
contenedor sig_comp_lexico();

#endif //P1_COMPGO_ANALISIS_LEXICO_H
