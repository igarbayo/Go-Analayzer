

#include <stdio.h>
#include <stdlib.h>
#include "definiciones.h"
#include "sistema_entrada.h"
#include "errores.h"

// Variable global
centinela cent;
FILE *fichero;

// Funciones privadas
// Avanza inicio n posiciones
void _avanzar_inicio(int n) {
    for (int i = 0; i < n; i++) {
        cent.inicio++;
    }
}

// Retrocede inicio n posiciones
void _retroceder_inicio(int n) {
    for (int i = 0; i < n; i++) {
        cent.inicio--;
    }
}

// Avanza delantero n posiciones
void _avanzar_delantero(int n) {
    for (int i = 0; i < n; i++) {
        cent.delantero++;
    }
}

// Retrocede delantero n posiciones
void _retroceder_delantero(int n) {
    for (int i = 0; i < n; i++) {
        cent.delantero--;
    }
}

// Carga un carácter desde el fichero
char _cargar_caracter() {
    if (fichero != NULL) {
        return (char) fgetc(fichero);
    }
    return EOF;
}

// Carga el siguiente bloque en el centinela
void _cargar_bloque() {
    if (cent.delantero == &cent.array_fisico[TAMBLOQUE - 1]) {
        // Cargar en el bloque B
        for (int i = TAMBLOQUE; i < 2 * TAMBLOQUE - 1; i++) {
            cent.array_fisico[i] = _cargar_caracter();
            printf("%d\t%c\n", i, cent.array_fisico[i]);
            // Parar al llegar al fin de fichero
            if (cent.array_fisico[i] == EOF) {
                break;
            }
        }
        // Asignar fin de bloque
        cent.array_fisico[2 * TAMBLOQUE - 1] = FINBLOQUE;
        // Avanzar delantero al inicio del bloque B
        cent.delantero = &cent.array_fisico[TAMBLOQUE];
    } else if (cent.delantero == &cent.array_fisico[2 * TAMBLOQUE - 1] ||
               cent.delantero == &cent.array_fisico[0]) {
        // Cargar en el bloque A
        for (int i = 0; i < TAMBLOQUE - 1; i++) {
            cent.array_fisico[i] = _cargar_caracter();
            printf("%d\t%c\n", i, cent.array_fisico[i]);
            // Parar al llegar al fin de fichero
            if (cent.array_fisico[i] == EOF) {
                break;
            }
        }
        // Asignar fin de bloque
        cent.array_fisico[TAMBLOQUE - 1] = FINBLOQUE;
        // Avanzar delantero al inicio del bloque A
        cent.delantero = &cent.array_fisico[0];
    }
}

void _imprimir_bloques() {
    printf("Contenido del bloque A:\n");
    for (int i = 0; i < TAMBLOQUE; i++) {
        if (cent.array_fisico[i] == FINBLOQUE) {
            printf("[FINBLOQUE] ");  // Indicador de fin de bloque
        } else if (cent.array_fisico[i] == EOF) {
            printf("[EOF] ");  // Indicador de fin de archivo
        } else {
            printf("%c ", cent.array_fisico[i]);  // Carácter normal
        }
    }
    printf("\n");

    printf("Contenido del bloque B:\n");
    for (int i = TAMBLOQUE; i < 2 * TAMBLOQUE; i++) {
        if (cent.array_fisico[i] == FINBLOQUE) {
            printf("[FINBLOQUE] ");  // Indicador de fin de bloque
        } else if (cent.array_fisico[i] == EOF) {
            printf("[EOF] ");  // Indicador de fin de archivo
        } else {
            printf("%c ", cent.array_fisico[i]);  // Carácter normal
        }
    }
    printf("\n");
}


// Funciones públicas
int iniciar_sistema_entrada(char* nombre_fichero) {
    // Inicializar punteros
    cent.inicio = cent.array_fisico;
    cent.delantero = cent.array_fisico;  // Corregido: inicializar delantero correctamente

    // Marcar fin de bloques
    cent.array_fisico[TAMBLOQUE - 1] = FINBLOQUE;
    cent.array_fisico[2 * TAMBLOQUE - 1] = FINBLOQUE;

    // Abrir el fichero
    fichero = fopen(nombre_fichero, "r");
    if (fichero == NULL) {
        error_fichero();  // Manejar el error
        return 1;
    }

    // Cargar el primer bloque
    _cargar_bloque();
    _imprimir_bloques();
    return 0;
}

// Función para cerrar el sistema de entrada
void cerrar_sistema_entrada() {
    if (fichero != NULL) {
        fclose(fichero);
        fichero = NULL;  // Opcional: asignar NULL para evitar usos incorrectos
    }
}

char sig_caracter() {
    char c;
    return c;
}

void copiar_lexema(contenedor *c) {

}

void devolver_un_caracter() {

}

void devolver_dos_caracteres() {

}

void ignorar_caracter() {

}