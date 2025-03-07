#ifndef P1_COMPGO_AVL_H
#define P1_COMPGO_AVL_H

#include "definiciones.h"

// Typedefs necesarios para el AVL
typedef char *tipoclave;
typedef contenedor tipoelem;
typedef void *avl;

/**
 * Crea el árbol
 * @param A puntero que se pone a NULL
 */
void crear(avl *A);

/**
 * Destruye el árbol, liberando la memoria
 * @param A árbol a destruir
 */
void destruir(avl *A);

/**
 * Verifica si está vacía
 * @param A árbol
 * @return 1 (verdadero) si el árbol está vacío y 0 (falso) si no lo está.
 */
unsigned vacia(avl A);

/**
 * Lee un elemento de un nodo
 * @param A árbol
 * @param E nodo
 */
void leer(avl A, tipoelem *E);

/**
 * Devuelve subárbol izq
 * @param A árbol
 * @return subárbol izq (avl)
 */
avl izq(avl A);

/**
 * Devuelve subárbol der
 * @param A árbol
 * @return subárbol der (avl)
 */
avl der(avl A);

/**
 * Busca un nodo
 * @param A árbol
 * @param cl contenido a buscar
 * @param nodo nodo como argumento de salida. Vale NULL si no se encuentra cl
 */
void buscar_nodo(avl A, tipoclave cl, tipoelem *nodo);

/**
 * Verifica si una clave está en el árbol
 * @param A árbol
 * @param cl clave
 * @return 0 si no es miembro, 1 si lo es
 */
unsigned es_miembro_clave(avl A, tipoclave cl);

/**
 * Inserta un nodo
 * @param A árbol
 * @param E nodo. Se presupone que no existe su misma clave en A
 */
void insertar(avl *A, tipoelem E);


#endif //P1_COMPGO_AVL_H