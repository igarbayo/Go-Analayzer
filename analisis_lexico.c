
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "definiciones.h"
#include "analisis_lexico.h"
#include "sistema_entrada.h"
#include "errores.h"
#include "tabla_simbolos.h"


// Definición para las implementaciones

#define FIN_COMPONENTE 100

// Variable global (se le asignan valores iniciales)
contenedor c = {0, NULL};

// Funciones privadas (cabeceras)

/**
 * Vacía el contenedor global, utilizado anteriormente
 */
void _vaciar_contenedor();

/**
 * Salta un comentario
 */
void _procesarComentario();

/**
 * Reconoce un identificador
 * @param c contenedor con el componente léxico a asignar
 */
void _procesarIdentificador(contenedor *c);

/**
 * Procesa un operador de 1 o 2 caracteres
 * @param primerCaracter del operador
 * @param c contenedor con el componente léxico a asignar
 */
void _procesarOperador(char primerCaracter, contenedor *c);

/**
 * Procesa una String o Rune bien construida
 * No se contemplan caracteres Unicode
 * @param separador las Strings pueden empezar por ` o por "
 * @return 1 si hay error de construcción
 * @return 0 si está bien construida
 */
int _procesarStringRune(char separador);

int _procesarNumero();

/**
 * Procesa un número Hexadecimal. Se utiliza en _procesarNumero()
 * @param c contenedor con el componente léxico a asignar
 * @return 1 si hay error de construcción
 * @return 0 si está bien construido
 */
int _procesarHexadecimal(contenedor *c);


// Funciones públicas

void iniciar_analisis_lexico(char* fichero) {
    iniciar_sistema_entrada(fichero);
}

contenedor sig_comp_lexico() {
    char sig = sig_caracter();
    _vaciar_contenedor();

    while (sig != EOF) {
        sig = sig_caracter();
        if (isalpha(sig) || sig == '_') {
            _procesarIdentificador(&c);
        } else if (isdigit(sig) || sig == '.') {
            _procesarNumero();
        } else if (sig == '`' || sig == '"') {
            _procesarStringRune(sig);
        } else if (sig == '/') {
            _procesarComentario();
            // De momento, ignoramos los tres puntos (...)
        } else if (sig == '+' || sig == '-' || sig == '*' || sig == '%' || sig == '&' || sig == '|' ||
                   sig == '^' || sig == '<' || sig == '>' || sig == '=' || sig == '!' || sig == '~' ||
                   sig == ':' || sig == ',' || sig == ';' || sig == '(' || sig == ')' || sig == '[' ||
                   sig == ']' || sig == '{' || sig == '}') {
            _procesarOperador(sig, &c);
            // El final del archivo
        } else if (sig == EOF) {
            c.comp_lexico = EOF;
            c.lexema = NULL;
            // Si no es ninguno de los anteriores
        } else {
            ignorar_caracter();
        }
    }

    return c;
}




// Funciones privadas (implementación)

void _vaciar_contenedor() {
    if (c.lexema != NULL) {
        // Liberamos memoria
        free(c.lexema);
        // Volvemos a los valores iniciales
        c.comp_lexico = 0;
        c.lexema = NULL;
    }

}

void _procesarComentario() {
    char sig;
    int estado = 0;

    // segundoCaracter puede ser / (empieza por //) o * (empieza por / y * y acaba en * seguido de/)
    char segundoCaracter = sig_caracter();

    // Tipo "//"
    if (segundoCaracter == '/') {
        sig = sig_caracter();
        while (sig != '\n') {
            sig = sig_caracter();
        }
    }

    // Tipo "/**/"
    if (segundoCaracter == '*') {
        sig = sig_caracter();
        while (estado != FIN_COMPONENTE) {
            if (estado == 0) {
                if (sig == '*') {
                    estado = 1;
                }
            }
            if (estado == 1) {
                if (sig == '/') {
                    estado = FIN_COMPONENTE;
                } else {
                    estado = 0;
                }
            }
            sig = sig_caracter();
        }
    }
}

void _procesarIdentificador(contenedor *c) {
    char sig;
    int estado = 0;
    while (estado != FIN_COMPONENTE) {
        sig = sig_caracter();
        if (!isalnum(sig) && sig != '_') {
            estado = FIN_COMPONENTE;
            // Se devuelve el carácter leído de más
            devolver_un_caracter();
            // Aceptamos el lexema
            copiar_lexema(c);

            if (c->lexema != NULL) {
                buscar_insertar_elemento(*c);
            }
        }
    }
}

void _procesarOperador(char primerCaracter, contenedor *c) {
    char segundoCaracter = sig_caracter();
    char tercerCaracter = sig_caracter();

    // Comprobamos si es alguno de los operadores multicarácter
    if (primerCaracter == '<' && segundoCaracter == '<' && tercerCaracter == '=') {
        (c->comp_lexico) = MENORDOBLEIGUAL;
    } else if (primerCaracter == '>' && segundoCaracter == '>' && tercerCaracter == '=') {
        (c->comp_lexico) = MAYORDOBLEIGUAL;
    } else if (primerCaracter == '&' && segundoCaracter == '^' && tercerCaracter == '=') {
        (c->comp_lexico) = AMPERSANDCIRCUNFLEJOIGUAL;
    } else if (primerCaracter == '.' && segundoCaracter == '.' && tercerCaracter == '.') {
        (c->comp_lexico) = TRESPUNTOS;
    } else if (primerCaracter == '<' && segundoCaracter == '<') {
        (c->comp_lexico) = MENORDOBLE;
        // Devolvemos un caracter leído de más
        devolver_un_caracter();
    } else if (primerCaracter == '>' && segundoCaracter == '>') {
        (c->comp_lexico) = MAYORDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '&' && segundoCaracter == '^') {
        (c->comp_lexico) = AMPERSANDCIRCUNFLEJO;
        devolver_un_caracter();
    } else if (primerCaracter == '+' && segundoCaracter == '=') {
        (c->comp_lexico) = MASIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '-' && segundoCaracter == '=') {
        (c->comp_lexico) = MENOSIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '*' && segundoCaracter == '=') {
        (c->comp_lexico) = PORIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '/' && segundoCaracter == '=') {
        (c->comp_lexico) = BARRAIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '%' && segundoCaracter == '=') {
        (c->comp_lexico) = PORCENTAJEIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '&' && segundoCaracter == '=') {
        (c->comp_lexico) = AMPERSANDIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '|' && segundoCaracter == '=') {
        (c->comp_lexico) = BARRAVERTICALIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '^' && segundoCaracter == '=') {
        (c->comp_lexico) = CIRCUNFLEJOIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '&' && segundoCaracter == '&') {
        (c->comp_lexico) = AMPERSANDDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '|' && segundoCaracter == '|') {
        (c->comp_lexico) = BARRAVERTICALDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '<' && segundoCaracter == '-') {
        (c->comp_lexico) = MENORMENOS;
        devolver_un_caracter();
    } else if (primerCaracter == '+' && segundoCaracter == '+') {
        (c->comp_lexico) = MASDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '-' && segundoCaracter == '-') {
        (c->comp_lexico) = MENOSDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '=' && segundoCaracter == '=') {
        (c->comp_lexico) = IGUALDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '!' && segundoCaracter == '=') {
        (c->comp_lexico) = EXCLAMACIONIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '<' && segundoCaracter == '=') {
        (c->comp_lexico) = MENORIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '>' && segundoCaracter == '=') {
        (c->comp_lexico) = MAYORIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == ':' && segundoCaracter == '=') {
        (c->comp_lexico) = DOSPUNTOSIGUAL;
        devolver_un_caracter();
    } else {
        // Si tiene 1 solo carácter, asignamos ASCII y devolvemos dos caracteres leídos de más
        (c->comp_lexico) = (int) primerCaracter;
        devolver_dos_caracteres();
    }

}

int _procesarStringRune(char separador) {
    char sig;
    int estado = 0;

    switch (separador) {
        // Raw string
        case '`':
            while (estado != FIN_COMPONENTE) {
                sig = sig_caracter();
                // Terminan con `
                if (sig == '`') {
                    estado = FIN_COMPONENTE;
                }
                // Si se llega al final del archivo antes, hay un error
                if (sig == EOF) {
                    error_string();
                    return 1;
                }
            }
            break;
        // Interpreted string
        case '"':
            while (estado != FIN_COMPONENTE) {
                sig = sig_caracter();
                if (estado == 0) {
                    // Para comprobar el escape de caracteres
                    if (sig == '\\') {
                        estado = 1;
                    }
                    // Termina con "
                    if (sig == '"') {
                        estado = FIN_COMPONENTE;
                    }
                    // Si se llega al final del archivo antes, hay un error
                    if (sig == EOF) {
                        error_string();
                        return 1;
                    }
                } else if (estado == 1) {
                    // Caracteres de escape válidos
                    if (sig != 'a' && sig != 'b' && sig !='f' && sig != 'n' && sig != 'r' &&
                        sig != 't' && sig != 'v' && sig != '\\' && sig != '"' & sig != '\'') {
                        error_string();
                        return 1;
                    // Si se llega al final del archivo antes, hay un error
                    } else if (sig == EOF) {
                        error_string();
                        return 1;
                    } else {
                        estado = 0;
                    }
                }
            }
            break;
        default:
            return 1;
    }

    return 0;
}

int _procesarNumero() {

}

int _procesarHexadecimal(contenedor *c) {
    char sig;
    int estado = 0;

    while (estado != FIN_COMPONENTE) {
        sig = sig_caracter();
        // Comienzo de la lectura
        if (estado == 0) {
            if (isxdigit(sig)) {
                estado = 1;
            } else if (sig == '_') {
                estado = 2;
            } else {
                // Si no hay digito HEX o _
                error_hexadecimal();
                return 1;
            }
        // Se ha leído un dígito HEX
        } else if (estado == 1) {
            if (!isxdigit(sig)) {
                if (sig == '_') {
                    estado = 2;
                } else {
                    // Si ya ha acabado el literal HEX
                    estado = FIN_COMPONENTE;
                    (c->comp_lexico) = HEX;
                }
            }
        // Se ha leído una _
        } else if (estado == 2) {
            if (!isxdigit(sig)) {
                // No se puede acabar en _
                error_hexadecimal();
                return 1;
            } else {
                estado = 1;
            }
        }
    }

    // Si no hay errores de construcción
    return 0;
}
