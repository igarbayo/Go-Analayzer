#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabla_simbolos.h"
#include "definiciones.h"
#include "avl.h"


// Variable global
tabla_simbolos tabla;

// FUNCIONES PRIVADAS //////////////////////

/**
 * Imprime el contenido de la tabla en inorden
 * @param tabla a imprimir
 */
void _inorden(tabla_simbolos tabla) {
    tipoelem e;
    // Recorre subárbol derecho e izquierdo de forma recursiva
    // Si es no vacío
    if (!vacia(tabla)) {
        _inorden(izq(tabla));
        leer(tabla, &e);
        printf("<%d, \"%s\">\n", e.comp_lexico, e.lexema);
        _inorden(der(tabla));
    }
}

// FUNCIONES PÚBLICAS /////////////////////////

void crear_tabla() {
    int i=0;
    contenedor c;

    // Creación del AVL
    crear(&tabla);

    // Array de keywords
    char *keywords[25];
    keywords[0] = "break";
    keywords[1] = "case";
    keywords[2] = "chan";
    keywords[3] = "const";
    keywords[4] = "continue";
    keywords[5] = "default";
    keywords[6] = "defer";
    keywords[7] = "else";
    keywords[8] = "fallthrough";
    keywords[9] = "for";
    keywords[10] = "func";
    keywords[11] = "go";
    keywords[12] = "goto";
    keywords[13] = "if";
    keywords[14] = "import";
    keywords[15] = "interface";
    keywords[16] = "map";
    keywords[17] = "package";
    keywords[18] = "range";
    keywords[19] = "return";
    keywords[20] = "select";
    keywords[21] = "struct";
    keywords[22] = "switch";
    keywords[23] = "type";
    keywords[24] = "var";

    // Para cada keyword, creamos el contenedor e insertamos en el AVL
    for (i=0; i<25; i++) {
        c.lexema = (char *) malloc ((strlen(keywords[i])+1)*sizeof(char));
        strcpy(c. lexema, keywords[i]);
        c.comp_lexico = 301 + i;
        insertar(&tabla, (tipoelem) c);
        free(c.lexema);
    }
}

void insertar_elemento(contenedor elemento) {
    insertar(&tabla, (tipoelem) elemento);
}

int buscar_elemento(char* clave) {
    tipoelem e;
    // Hacemos es_miembro con el AVL
    if (es_miembro_clave(tabla, clave) == 1) {
        // Buscamos en el AVL
        buscar_nodo(tabla, clave, &e);
        return e.comp_lexico;
    } else {
        return -1;
    }
}

void eliminar_tabla() {
    // Función del AVL
    destruir(&tabla);
}

void imprimir_tabla() {
    printf("----------- Tabla de símbolos -----------\n");
    // Se imprime en inorden, con una función para la recursividad
    _inorden(tabla);
    printf("-----------------------------------------\n");

}