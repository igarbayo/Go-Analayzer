
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
contenedor c = {-1, NULL};
int linea = 0;
int columna = 0;

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
 * Reconoce un identificador y lo inserta en la TS
 */
void _procesarIdentificador();

/**
 * Procesa un operador de 1 o 2 caracteres
 * @param primerCaracter del operador
 */
void _procesarOperador(char primerCaracter);

/**
 * Procesa una String o Rune bien construida
 * No se contemplan caracteres Unicode
 * @param separador las Strings pueden empezar por ` o por "
 * @return 1 si hay error de construcción
 * @return 0 si está bien construida
 */
int _procesarStringRune(char separador);

int _procesarNumero(char primerCaracter);

/**
 * Procesa un número Hexadecimal. Se utiliza en _procesarNumero()
 * @return 1 si hay error de construcción
 * @return 0 si está bien construido
 */
int _procesarHexadecimal();

/**
 * Si no está en la TS, inserta un lexema
 * @param comp_lexico asignado al lexema en la TS
 */
void _buscar_insertar_lexema(int comp_lexico);


// Funciones públicas

void iniciar_analisis_lexico(char* fichero) {
    iniciar_sistema_entrada(fichero);
}

contenedor sig_comp_lexico() {
    char sig = sig_caracter();
    _vaciar_contenedor();

    if (sig != EOF) {
        if (isalpha(sig) || sig == '_') {
            _procesarIdentificador();
        } else if (isdigit(sig) || sig == '.') {
            _procesarNumero(sig);
        } else if (sig == '`' || sig == '"') {
            _procesarStringRune(sig);
        } else if (sig == '/') {
            _procesarComentario();
            // De momento, ignoramos los tres puntos (...)
        } else if (sig == '+' || sig == '-' || sig == '*' || sig == '%' || sig == '&' || sig == '|' ||
                   sig == '^' || sig == '<' || sig == '>' || sig == '=' || sig == '!' || sig == '~' ||
                   sig == ':' || sig == ',' || sig == ';' || sig == '(' || sig == ')' || sig == '[' ||
                   sig == ']' || sig == '{' || sig == '}') {
            _procesarOperador(sig);
            // Si no es ninguno de los anteriores
        } else {
            ignorar_caracter();
            // Recursividad al ignorar caracteres (hacer de otra forma)
            sig_comp_lexico();
        }
        // El final del archivo
    } else {
        c.comp_lexico = EOF;
        c.lexema = NULL;
    }

    return c;
}




// Funciones privadas (implementación)

void _vaciar_contenedor() {
    if (c.lexema != NULL) {
        // Liberamos memoria
        free(c.lexema);
        // Volvemos a los valores iniciales
        c.comp_lexico = -1;
        c.lexema = NULL;
    }

}

void _procesarComentario() {
    char sig;
    int estado = 0;

    // segundoCaracter puede ser / (empieza por //) o * (empieza por / y * y acaba en * seguido de/)
    char segundoCaracter = sig_caracter();

    switch (segundoCaracter) {
        case '/':
            // Tipo "//"
            sig = sig_caracter();
            while (sig != '\n') {
                sig = sig_caracter();
            }
            break;
        case '*':
            // Tipo "/**/"
            sig = sig_caracter();
            while (estado != FIN_COMPONENTE) {
                switch (estado) {
                    case 0:
                        if (sig == '*') {
                            estado = 1;
                        }
                        break;
                    case 1:
                        if (sig == '/') {
                            estado = FIN_COMPONENTE;
                        } else {
                            estado = 0;
                        }
                        break;
                    default:
                        break;
                }
                sig = sig_caracter();
            }
            break;
        default:
            devolver_un_caracter();
            _procesarOperador('/');
            break;

    }

    ignorar_lexema();
}

void _procesarIdentificador() {
    char sig;
    int estado = 0;
    while (estado != FIN_COMPONENTE) {
        sig = sig_caracter();
        if (!isalnum(sig) && sig != '_') {
            estado = FIN_COMPONENTE;
            // Se devuelve el carácter leído de más
            devolver_un_caracter();
            // Aceptamos el lexema
            copiar_lexema(&c);

            if (c.lexema != NULL) {
                _buscar_insertar_lexema(ID);
            }
        }
    }
}

void _procesarOperador(char primerCaracter) {
    char segundoCaracter = sig_caracter();
    char tercerCaracter = sig_caracter();

    // Comprobamos si es alguno de los operadores multicarácter
    if (primerCaracter == '<' && segundoCaracter == '<' && tercerCaracter == '=') {
        (c.comp_lexico) = MENORDOBLEIGUAL;
    } else if (primerCaracter == '>' && segundoCaracter == '>' && tercerCaracter == '=') {
        (c.comp_lexico) = MAYORDOBLEIGUAL;
    } else if (primerCaracter == '&' && segundoCaracter == '^' && tercerCaracter == '=') {
        (c.comp_lexico) = AMPERSANDCIRCUNFLEJOIGUAL;
    } else if (primerCaracter == '.' && segundoCaracter == '.' && tercerCaracter == '.') {
        (c.comp_lexico) = TRESPUNTOS;
    } else if (primerCaracter == '<' && segundoCaracter == '<') {
        (c.comp_lexico) = MENORDOBLE;
        // Devolvemos un caracter leído de más
        devolver_un_caracter();
    } else if (primerCaracter == '>' && segundoCaracter == '>') {
        (c.comp_lexico) = MAYORDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '&' && segundoCaracter == '^') {
        (c.comp_lexico) = AMPERSANDCIRCUNFLEJO;
        devolver_un_caracter();
    } else if (primerCaracter == '+' && segundoCaracter == '=') {
        (c.comp_lexico) = MASIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '-' && segundoCaracter == '=') {
        (c.comp_lexico) = MENOSIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '*' && segundoCaracter == '=') {
        (c.comp_lexico) = PORIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '/' && segundoCaracter == '=') {
        (c.comp_lexico) = BARRAIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '%' && segundoCaracter == '=') {
        (c.comp_lexico) = PORCENTAJEIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '&' && segundoCaracter == '=') {
        (c.comp_lexico) = AMPERSANDIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '|' && segundoCaracter == '=') {
        (c.comp_lexico) = BARRAVERTICALIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '^' && segundoCaracter == '=') {
        (c.comp_lexico) = CIRCUNFLEJOIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '&' && segundoCaracter == '&') {
        (c.comp_lexico) = AMPERSANDDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '|' && segundoCaracter == '|') {
        (c.comp_lexico) = BARRAVERTICALDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '<' && segundoCaracter == '-') {
        (c.comp_lexico) = MENORMENOS;
        devolver_un_caracter();
    } else if (primerCaracter == '+' && segundoCaracter == '+') {
        (c.comp_lexico) = MASDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '-' && segundoCaracter == '-') {
        (c.comp_lexico) = MENOSDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '=' && segundoCaracter == '=') {
        (c.comp_lexico) = IGUALDOBLE;
        devolver_un_caracter();
    } else if (primerCaracter == '!' && segundoCaracter == '=') {
        (c.comp_lexico) = EXCLAMACIONIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '<' && segundoCaracter == '=') {
        (c.comp_lexico) = MENORIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == '>' && segundoCaracter == '=') {
        (c.comp_lexico) = MAYORIGUAL;
        devolver_un_caracter();
    } else if (primerCaracter == ':' && segundoCaracter == '=') {
        (c.comp_lexico) = DOSPUNTOSIGUAL;
        devolver_un_caracter();
    } else {
        // Si tiene 1 solo carácter, asignamos ASCII y devolvemos dos caracteres leídos de más
        (c.comp_lexico) = (int) primerCaracter;
        devolver_dos_caracteres();
    }
    copiar_lexema(&c);

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
                    error_string(linea, columna);
                    return 1;
                }
            }
            break;
        // Interpreted string
        case '"':
            while (estado != FIN_COMPONENTE) {
                sig = sig_caracter();
                switch (estado) {
                    case 0:
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
                            error_string(linea, columna);
                            return 1;
                        }
                        break;
                    case 1:
                        // Caracteres de escape válidos
                        if (sig != 'a' && sig != 'b' && sig !='f' && sig != 'n' && sig != 'r' &&
                            sig != 't' && sig != 'v' && sig != '\\' && sig != '"' & sig != '\'') {
                            error_string(linea, columna);
                            return 1;
                            // Si se llega al final del archivo antes, hay un error
                        } else if (sig == EOF) {
                            error_string(linea, columna);
                            return 1;
                        } else {
                            estado = 0;
                        }
                        break;
                    default:
                        break;
                }
            }
            // Aceptamos el lexema
            copiar_lexema(&c);
            c.comp_lexico = STRING;
            break;
        default:
            return 1;
    }

    return 0;
}

int _procesarNumero(char primerCaracter) {
    char sig = primerCaracter;
    int estado = 0;

    while (estado != FIN_COMPONENTE) {
        switch (estado) {
            case 0:
                if (sig == '0') {
                    estado = 1; // Puede ser decimal 0, binario, octal o hexadecimal
                } else if (isdigit(sig)) {
                    estado = 10; // Decimal
                } else if (sig == '.') {
                    estado = 19; // Float empezando con punto
                } else {
                    error_entero(linea, columna);
                    return 1; // Error
                }
                break;
            case 1:
                if (sig == 'b' || sig == 'B') estado = 2; // Binario
                else if (sig == 'o' || sig == 'O') estado = 3; // Octal
                else if (sig == 'x' || sig == 'X') {
                    _procesarHexadecimal(); // Hexadecimal
                    return 0;
                } else if (isdigit(sig)) estado = 10; // Decimal con ceros iniciales
                else {
                    devolver_un_caracter();
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = INT;
                }
                break;
            case 2: // Binario
                if (sig == '_') break;
                if (sig == '0' || sig == '1') estado = 2;
                else {
                    devolver_un_caracter();
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = BINARY;
                }
                break;
            case 3: // Octal
                if (sig == '_') break;
                if (sig >= '0' && sig <= '7') estado = 3;
                else {
                    devolver_un_caracter();
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = OCTAL;
                }
                break;
            case 10: // Decimal normal
                if (sig == '_') break;
                if (isdigit(sig)) estado = 10;
                else if (sig == '.') {
                    estado = 20;
                } else if (sig == 'e' || sig == 'E') {
                    estado = 30;
                } else if (sig == 'i') {
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else {
                    devolver_un_caracter();
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = INT;
                }
                break;
            case 19: // inicio de .123
                if (isdigit(sig)) estado = 21;
                else if (sig == 'e' || sig == 'E') estado = 30;
                else if (isalpha(sig)) {
                    devolver_un_caracter();
                    _procesarOperador('.');
                    return 1;
                }
                break;
            case 20: // Parte fraccionaria de decimal o inicio de .123
                if (isdigit(sig)) estado = 21;
                else if (sig == 'e' || sig == 'E') estado = 30;
                else {
                    devolver_un_caracter();
                    error_float(linea, columna);
                    return 1;
                }
                break;
            case 21:
                if (sig == '_') break;
                if (isdigit(sig)) estado = 21;
                else if (sig == 'e' || sig == 'E') estado = 30;
                else if (sig == 'i') {
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else {
                    devolver_un_caracter();
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = FLOAT;
                }
                break;
            case 30: // Exponente
                if (sig == '+' || sig == '-') estado = 31;
                else if (isdigit(sig)) estado = 32; // Permitir 1.e0
                else {
                    devolver_un_caracter();
                    error_float(linea, columna);
                    return 1;
                }
                break;
            case 31: // Signo después de exponente
                if (isdigit(sig)) estado = 32;
                else {
                    devolver_un_caracter();
                    error_float(linea, columna);
                    return 1;
                }
                break;
            case 32: // Dígitos en el exponente
                if (sig == '_') break;
                if (isdigit(sig)) estado = 32;
                else if (sig == 'i') { // Para casos como 1.e0+0i
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else if (sig == '+' || sig == '-') { // Para manejar 1.e0+0
                    estado = 40;
                } else {
                    devolver_un_caracter();
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = FLOAT;
                }
                break;
            case 40: // Manejo del signo después del exponente (como en 1.e0+0i)
                if (isdigit(sig)) estado = 41;
                else {
                    devolver_un_caracter();
                    error_float(linea, columna);
                    return 1;
                }
                break;
            case 41: // Dígitos después del signo en el exponente
                if (sig == '_') break;
                if (isdigit(sig)) estado = 41;
                else if (sig == 'i') { // Para casos como 1.e0+0i
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else {
                    devolver_un_caracter();
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = FLOAT;
                }
                break;
        }
        if (estado != FIN_COMPONENTE) {
            sig = sig_caracter();
        }
    }
    copiar_lexema(&c);
    return 0;
}




int _procesarHexadecimal() {
    char sig;
    int estado = 0;

    while (estado != FIN_COMPONENTE) {
        sig = sig_caracter();
        switch (estado) {
            case 0:
                // Comienzo de la lectura
                if (isxdigit(sig)) {
                    estado = 1;
                } else if (sig == '_') {
                    estado = 2;
                } else {
                    // Si no hay digito HEX o _
                    error_hexadecimal(linea, columna);
                    return 1;
                }
                break;
            case 1:
                // Se ha leído un dígito HEX
                if (!isxdigit(sig)) {
                    if (sig == '_') {
                        estado = 2;
                    } else {
                        // Si ya ha acabado el literal HEX
                        estado = FIN_COMPONENTE;
                        (c.comp_lexico) = HEX;
                    }
                }
                break;
            case 2:
                // Se ha leído una _
                if (!isxdigit(sig)) {
                    // No se puede acabar en _
                    error_hexadecimal(linea, columna);
                    return 1;
                } else {
                    estado = 1;
                }
                break;
            default:
                break;
        }
    }

    // Si no hay errores de construcción
    return 0;
}


void _buscar_insertar_lexema(int comp_lexico) {
    //Si no ha habido un error, se continua
    int busqueda = buscar_elemento(c.lexema);
    //Si no está, lo introducimos en la tabla.
    if (busqueda == -1) {
        c.comp_lexico = comp_lexico;
        insertar_elemento(c);
    } else {
        //Si ya estaba, le asignamos el componente que hay en la tabla.
        c.comp_lexico = busqueda;
    }
}