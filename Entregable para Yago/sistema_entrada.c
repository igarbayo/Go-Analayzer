#include <stdio.h>
#include <stdlib.h>
#include "definiciones.h"
#include "sistema_entrada.h"
#include "errores.h"

// Variables globales
centinela cent;
FILE *fichero;

// Variable para decidir si cargar o no un bloque
short noCargar=0;

// FUNCIONES PRIVADAS ///////////////////////

// Avanza delantero n posiciones
void _avanzar_delantero(int n) {
    for (int i = 0; i < n; i++) {
        cent.delantero++;
    }
}

// Carga el siguiente bloque en el centinela
void _cargar_bloque() {
    if (cent.delantero == &cent.array_fisico[TAMBLOQUE - 1]) {
        // Cargar en el bloque B
        size_t leidos = fread(&cent.array_fisico[TAMBLOQUE], sizeof(char), TAMBLOQUE-1, fichero);

        // Parar al llegar al fin de fichero
        if (leidos < TAMBLOQUE) {
            // Si no leímos todos los caracteres, marcar EOF en el último carácter
            cent.array_fisico[TAMBLOQUE + leidos] = EOF;
        }

        // Asignar fin de bloque
        cent.array_fisico[2 * TAMBLOQUE - 1] = FINBLOQUE;
        // Avanzar delantero al inicio del bloque B
        cent.delantero = &cent.array_fisico[TAMBLOQUE];
    } else if (cent.delantero == &cent.array_fisico[2 * TAMBLOQUE - 1] ||
               cent.delantero == &cent.array_fisico[0]) {
        // Cargar en el bloque A
        size_t leidos = fread(cent.array_fisico, sizeof(char), TAMBLOQUE - 1, fichero);

        // Parar al llegar al fin de fichero
        if (leidos < TAMBLOQUE - 1) {
            // Si no leímos todos los caracteres, marcar EOF en el último carácter
            cent.array_fisico[leidos] = EOF;
        }

        // Asignar fin de bloque
        cent.array_fisico[TAMBLOQUE - 1] = FINBLOQUE;
        // Avanzar delantero al inicio del bloque A
        cent.delantero = &cent.array_fisico[0];
    }
}

// Solo desplaza delantero de bloque, sin cargar
void _mover_bloque() {
    if (cent.delantero == &cent.array_fisico[TAMBLOQUE - 1]) {
        // Avanzar delantero al inicio del bloque B
        cent.delantero = &cent.array_fisico[TAMBLOQUE];
    } else if (cent.delantero == &cent.array_fisico[2 * TAMBLOQUE - 1] ||
               cent.delantero == &cent.array_fisico[0]) {
        // Avanzar delantero al inicio del bloque A
        cent.delantero = &cent.array_fisico[0];
    }
}


// FUNCIONES PÚBLICAS ////////////////

int iniciar_sistema_entrada(char* nombre_fichero) {
    // Inicializar punteros
    cent.inicio = cent.array_fisico;
    cent.delantero = cent.array_fisico;

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

    // En caso de éxito
    return 0;
}

void cerrar_sistema_entrada() {
    if (fichero != NULL) {
        fclose(fichero);
        fichero = NULL;  // Asignar NULL para evitar usos incorrectos
    }
}

char sig_caracter() {
    // Por defecto, devuelve el carácter de fin
    char caracter = EOF;

    // Si se termina un bloque
    if (*cent.delantero == FINBLOQUE) {
        if (noCargar==0) {
            // Si hay que cargar un bloque
            _cargar_bloque();
        } else {
            // Si solo hay que moverse al bloque
            _mover_bloque();
            noCargar = 0;
        }
    }

    // Por defecto
    caracter = *cent.delantero;
    _avanzar_delantero(1);
    return caracter;
}

void copiar_lexema(contenedor *c) {
    // Calcular la longitud del lexema (excluyendo el último carácter)
    int longitud = 0;

    // Varios casos
        // Los dos en A
    if (cent.inicio < (cent.array_fisico + TAMBLOQUE) &&
        cent.delantero < (cent.array_fisico + TAMBLOQUE)) {
        longitud = cent.delantero - cent.inicio;
        // Inicio en A, Delantero en B
    } else if (cent.inicio < (cent.array_fisico + TAMBLOQUE)) {
        longitud = cent.delantero - cent.inicio - 1;
        // Inicio en B, Delantero en A
    } else if (cent.delantero < (cent.array_fisico + TAMBLOQUE)) {
        // Calcular la parte del bloque B + la parte del bloque A
        int longitud_B = &cent.array_fisico[2*TAMBLOQUE] - cent.inicio - 1;
        int longitud_A = cent.delantero - &cent.array_fisico[0];
        longitud = longitud_B + longitud_A;
        // Los dos en B
    } else {
        longitud = cent.delantero - cent.inicio;
    }

    // Verificar si el lexema excede el tamaño máximo permitido
    if (longitud >= TAMBLOQUE - 1) {
        // Truncar el lexema a los últimos TAMBLOQUE - 1 caracteres
        longitud = TAMBLOQUE - 1;
        cent.inicio = cent.delantero - longitud;  // Ajustar inicio para truncar
    }

    // Reservar memoria para el lexema (longitud + 1 para el carácter nulo)
    char* lexema = (char*) malloc((longitud + 1) * sizeof(char));
    if (lexema == NULL) {
        error_memoria();
        c->lexema = NULL;
        // Finalizamos si no se puede reservar memoria
        return;
    }

    // Varios casos
    // Copiar los caracteres desde inicio hasta delantero (excluyendo el último)
    // Hay que manejar los casos donde empieza en A y acaba en B y viceversa
        // Los dos en A
    if (cent.inicio < (cent.array_fisico + TAMBLOQUE) &&
        cent.delantero < (cent.array_fisico + TAMBLOQUE)) {
        // Caso 1: Inicio en A, Delantero en A
        for (int i = 0; i < longitud; i++) {
            lexema[i] = *(cent.inicio + i);
        }
    } else if (cent.inicio < (cent.array_fisico + TAMBLOQUE)) {
        // Caso 2: Inicio en A, Delantero en B
        int longitud_A = (cent.array_fisico + TAMBLOQUE-1) - cent.inicio;
        int longitud_B = cent.delantero - (cent.array_fisico + TAMBLOQUE);

        // Copiar la parte del bloque A
        for (int i = 0; i < longitud_A; i++) {
            lexema[i] = *(cent.inicio + i);
        }

        // Copiar la parte del bloque B
        for (int i = 0; i < longitud_B; i++) {
            lexema[longitud_A + i] = *(cent.array_fisico + TAMBLOQUE + i);
        }
    } else if (cent.delantero < (cent.array_fisico + TAMBLOQUE)) {
        // Caso 3: Inicio en B, Delantero en A
        int longitud_B = (cent.array_fisico + 2 * TAMBLOQUE) - cent.inicio - 1;
        int longitud_A = cent.delantero - cent.array_fisico;

        // Copiar la parte del bloque B
        for (int i = 0; i < longitud_B; i++) {
            lexema[i] = *(cent.inicio + i);
        }

        // Copiar la parte del bloque A
        for (int i = 0; i < longitud_A; i++) {
            lexema[longitud_B + i] = *(cent.array_fisico + i);
        }
    } else {
        // Caso 4: Inicio en B, Delantero en B
        for (int i = 0; i < longitud; i++) {
            lexema[i] = *(cent.inicio + i);
        }
    }

    // Añadir el carácter nulo al final
    lexema[longitud] = '\0';
    cent.inicio = cent.delantero;
    c->lexema = lexema;
}

void ignorar_lexema() {
    // Mueve inicio hasta delantero
    cent.inicio = cent.delantero;
}

void asignar_lexema(contenedor* c, char* lexema) {
    if (lexema != NULL) {
        // Copia la cadena en el campo del lexema
        c->lexema = lexema;
    }
}

void devolver_un_caracter() {
    // Si delantero ya está al inicio del array físico, vamos al final del bloque B
    if (cent.delantero == cent.array_fisico) {
        cent.delantero = &(cent.array_fisico[2*TAMBLOQUE-2]);
        noCargar = 1;
    } else if (cent.delantero == &(cent.array_fisico[TAMBLOQUE])) {
        // Si delantero está al inicio del bloque B, vamos al final del bloque A
        cent.delantero = &(cent.array_fisico[TAMBLOQUE-2]);
        noCargar = 1;
    } else {
        // Retroceder delantero una posición
        cent.delantero--;
    }
}

void ignorar_caracter() {
    // Si delantero apunta a FINBLOQUE, cargar el siguiente bloque
    if (*cent.delantero == FINBLOQUE) {
        if (noCargar==0) {
            _cargar_bloque();
        } else {
            _mover_bloque();
            noCargar = 0;
        }
    }

    // Si delantero apunta a EOF, no hacemos nada
    if (*cent.delantero == EOF) {
        return;
    }

    // Avanzar delantero una posición
    cent.inicio = cent.delantero;
}