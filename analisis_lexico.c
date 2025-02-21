//
// Created by ignacio on 19/02/25.
//

#include <stdio.h>
#include <stdlib.h>
#include "definiciones.h"
#include "analisis_lexico.h"
#include "sistema_entrada.h"

void iniciar_analisis_lexico(char* fichero) {
    iniciar_sistema_entrada(fichero);
}

contenedor sig_comp_lexico() {
    contenedor c;
    return c;
}