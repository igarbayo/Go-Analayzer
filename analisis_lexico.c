
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

// Contadores globales para impresión de errores
int linea = 1;
int columna = 0;
int columna_antigua = 1;

// Variables globales para manejar semicolon (;)
short insertarSemicolon = 0;

// Funciones privadas (cabeceras)

/**
 * Vacía el contenedor global, utilizado anteriormente
 */
void _vaciar_contenedor();

/**
 * Salta un comentario
 * @param 
 */
void _procesarComentario();

/**
 * Reconoce un identificador y lo inserta en la TS
 * @param 
 */
void _procesarIdentificador();

/**
 * Procesa un operador de 1 o 2 caracteres
 * @param primerCaracter del operador
 * @param 
 */
void _procesarOperador(char primerCaracter);

/**
 * Procesa una String o Rune bien construida
 * No se contemplan caracteres Unicode
 * @param separador las Strings pueden empezar por ` o por "
 * @param 
 * @return 1 si hay error de construcción
 * @return 0 si está bien construida
 */
int _procesarString(char separador);

/**
 *
 */
void _procesarSinCarryReturn();
/**
 *
 * @param str1
 * @param str2
 * @return
 */
char* _sumar_strings(const char* str1, const char* str2);

/**
 *
 * @param primerCaracter
 * @param 
 * @return
 */
int _procesarNumero(char primerCaracter);

/**
 * Procesa un número Hexadecimal. Se utiliza en _procesarNumero()
 * @param 
 * @return 1 si hay error de construcción
 * @return 0 si está bien construido
 */
int _procesarHexadecimal();

/**
 *
 */
void _devolverSemicolon();

/**
 * Si no está en la TS, inserta un lexema
 * @param comp_lexico asignado al lexema en la TS
 */
void _buscar_insertar_lexema(int comp_lexico);

/**
 *
 * @param caracter
 */
void _sumar_columna(char caracter);

/**
 *
 * @param caracter
 */
void _restar_columna(char caracter);

/**
 *
 */
void _sumar_linea();

/**
 *
 */
void _restar_linea();



// Funciones públicas

void iniciar_analisis_lexico(char* fichero) {
    iniciar_sistema_entrada(fichero);
}

void terminar_analisis_lexico() {
    cerrar_sistema_entrada();
}

contenedor sig_comp_lexico() {
    char sig = sig_caracter(); _sumar_columna(sig);
    _vaciar_contenedor();

    if (sig != EOF) {
        if (isalpha(sig) || sig == '_') {
            _procesarIdentificador();
        } else if (isdigit(sig) || sig == '.') {
            _procesarNumero(sig);
        } else if (sig == '`' || sig == '"') {
            _procesarString(sig);
        } else if (sig == '/') {
            _procesarComentario();
            // De momento, ignoramos los tres puntos (...)
        } else if (sig == '+' || sig == '-' || sig == '*' || sig == '%' || sig == '&' || sig == '|' ||
                   sig == '^' || sig == '<' || sig == '>' || sig == '=' || sig == '!' || sig == '~' ||
                   sig == ':' || sig == ',' || sig == ';' || sig == '(' || sig == ')' || sig == '[' ||
                   sig == ']' || sig == '{' || sig == '}') {
            _procesarOperador(sig);
            // Si no es ninguno de los anteriores
        } else if (sig == '\n') {
            if (insertarSemicolon == 1) {
                _devolverSemicolon();
            }
            ignorar_caracter();
        } else {
            ignorar_caracter();
            // Recursividad al ignorar caracteres (hacer de otra forma)
            sig_comp_lexico();
        }
        // El final del archivo
    } else {
        c.comp_lexico = FINFICHERO;
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
    char segundoCaracter = sig_caracter(); _sumar_columna(segundoCaracter);

    switch (segundoCaracter) {
        case '/':
            // Tipo "//"
            sig = sig_caracter(); _sumar_columna(sig);
            while (sig != '\n') {
                sig = sig_caracter(); _sumar_columna(sig);
            }
            break;
        case '*':
            // Tipo "/**/"
            sig = sig_caracter(); _sumar_columna(sig);
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
                sig = sig_caracter(); _sumar_columna(sig);
            }
            break;
        default:
            devolver_un_caracter(); _restar_columna(segundoCaracter);
            _procesarOperador('/');
            break;

    }
    insertarSemicolon = 0;
    ignorar_lexema();
}

void _procesarIdentificador() {
    char sig;
    int estado = 0;
    while (estado != FIN_COMPONENTE) {
        sig = sig_caracter(); _sumar_columna(sig);
        if (!isalnum(sig) && sig != '_') {
            estado = FIN_COMPONENTE;
            // Se devuelve el carácter leído de más
            devolver_un_caracter(); _restar_columna(sig);
            // Aceptamos el lexema
            copiar_lexema(&c);
            insertarSemicolon = 1;

            if (c.lexema != NULL) {
                _buscar_insertar_lexema(ID);
            }
        }
    }
}

void _procesarOperador(char primerCaracter) {
    char segundoCaracter = sig_caracter(); _sumar_columna(segundoCaracter);
    char tercerCaracter = sig_caracter(); _sumar_columna(tercerCaracter);

    // Comprobamos si es alguno de los operadores multicarácter
    if (primerCaracter == '<' && segundoCaracter == '<' && tercerCaracter == '=') {
        (c.comp_lexico) = MENORDOBLEIGUAL;
        insertarSemicolon = 0;
    } else if (primerCaracter == '>' && segundoCaracter == '>' && tercerCaracter == '=') {
        (c.comp_lexico) = MAYORDOBLEIGUAL;
        insertarSemicolon = 0;
    } else if (primerCaracter == '&' && segundoCaracter == '^' && tercerCaracter == '=') {
        (c.comp_lexico) = AMPERSANDCIRCUNFLEJOIGUAL;
        insertarSemicolon = 0;
    } else if (primerCaracter == '.' && segundoCaracter == '.' && tercerCaracter == '.') {
        (c.comp_lexico) = TRESPUNTOS;
        insertarSemicolon = 0;
    } else if (primerCaracter == '<' && segundoCaracter == '<') {
        (c.comp_lexico) = MENORDOBLE;
        // Devolvemos un caracter leído de más
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '>' && segundoCaracter == '>') {
        (c.comp_lexico) = MAYORDOBLE;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '&' && segundoCaracter == '^') {
        (c.comp_lexico) = AMPERSANDCIRCUNFLEJO;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '+' && segundoCaracter == '=') {
        (c.comp_lexico) = MASIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '-' && segundoCaracter == '=') {
        (c.comp_lexico) = MENOSIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '*' && segundoCaracter == '=') {
        (c.comp_lexico) = PORIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '/' && segundoCaracter == '=') {
        (c.comp_lexico) = BARRAIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '%' && segundoCaracter == '=') {
        (c.comp_lexico) = PORCENTAJEIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '&' && segundoCaracter == '=') {
        (c.comp_lexico) = AMPERSANDIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '|' && segundoCaracter == '=') {
        (c.comp_lexico) = BARRAVERTICALIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '^' && segundoCaracter == '=') {
        (c.comp_lexico) = CIRCUNFLEJOIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '&' && segundoCaracter == '&') {
        (c.comp_lexico) = AMPERSANDDOBLE;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '|' && segundoCaracter == '|') {
        (c.comp_lexico) = BARRAVERTICALDOBLE;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '<' && segundoCaracter == '-') {
        (c.comp_lexico) = MENORMENOS;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '+' && segundoCaracter == '+') {
        (c.comp_lexico) = MASDOBLE;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 1;
    } else if (primerCaracter == '-' && segundoCaracter == '-') {
        (c.comp_lexico) = MENOSDOBLE;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 1;
    } else if (primerCaracter == '=' && segundoCaracter == '=') {
        (c.comp_lexico) = IGUALDOBLE;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '!' && segundoCaracter == '=') {
        (c.comp_lexico) = EXCLAMACIONIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '<' && segundoCaracter == '=') {
        (c.comp_lexico) = MENORIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == '>' && segundoCaracter == '=') {
        (c.comp_lexico) = MAYORIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == ':' && segundoCaracter == '=') {
        (c.comp_lexico) = DOSPUNTOSIGUAL;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        insertarSemicolon = 0;
    } else if (primerCaracter == ')' || primerCaracter == ']' || primerCaracter == '}') {
        // Si tiene 1 solo carácter, asignamos ASCII y devolvemos dos caracteres leídos de más
        (c.comp_lexico) = (int) primerCaracter;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        devolver_un_caracter(); _restar_columna(segundoCaracter);
        insertarSemicolon = 1;
    } else {
        // Si tiene 1 solo carácter, asignamos ASCII y devolvemos dos caracteres leídos de más
        (c.comp_lexico) = (int) primerCaracter;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        devolver_un_caracter(); _restar_columna(segundoCaracter);
        insertarSemicolon = 0;
    }
    copiar_lexema(&c);

}

int _procesarString(char separador) {
    char sig;
    int estado = 0;

    switch (separador) {
        // Raw string
        case '`':
            while (estado != FIN_COMPONENTE) {
                sig = sig_caracter(); _sumar_columna(sig);
                // Terminan con `
                if (sig == '`') {
                    estado = FIN_COMPONENTE;
                }
                // Si se llega al final del archivo antes, hay un error
                if (sig == EOF) {
                    error_string(linea, columna);
                    ignorar_lexema();
                    return 1;
                }
                if (sig == '\r') {
                    _procesarSinCarryReturn();
                }
            }
            // Aceptamos el lexema
            copiar_lexema(&c);
            c.comp_lexico = STRING;
            insertarSemicolon = 1;
            break;
        // Interpreted string
        case '"':
            while (estado != FIN_COMPONENTE) {
                sig = sig_caracter(); _sumar_columna(sig);
                switch (estado) {
                    case 0:
                        // Para comprobar el escape de caracteres
                        if (sig == '\\' || sig == '\n') {
                            estado = 1;
                        }
                        // Termina con "
                        if (sig == '"') {
                            estado = FIN_COMPONENTE;
                        }
                        // Si se llega al final del archivo antes, hay un error
                        if (sig == EOF) {
                            error_string(linea, columna);
                            ignorar_lexema();
                            return 1;
                        }
                        break;
                    case 1:
                        // Caracteres de escape válidos
                        if (sig != 'a' && sig != 'b' && sig !='f' && sig != 'n' && sig != 'r' &&
                            sig != 't' && sig != 'v' && sig != '\\' && sig != '"' & sig != '\'') {
                            error_string(linea, columna);
                            ignorar_lexema();
                            return 1;
                            // Si se llega al final del archivo antes, hay un error
                        } else if (sig == EOF) {
                            error_string(linea, columna);
                            ignorar_lexema();
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
            insertarSemicolon = 1;
            break;
        default:
            return 1;
    }

    return 0;
}

void _procesarSinCarryReturn() {
    char sig;
    int estado = 0;
    contenedor aux = {-1, NULL};
    devolver_un_caracter();
    copiar_lexema(&aux);

    sig = sig_caracter();
    _sumar_columna(sig);
    if (sig == '\r') {
        ignorar_caracter();

        while (estado != FIN_COMPONENTE) {
            sig = sig_caracter(); _sumar_columna(sig);
            // Terminan con `
            if (sig == '`') {
                estado = FIN_COMPONENTE;
            }
            // Si se llega al final del archivo antes, hay un error
            if (sig == EOF) {
                error_string(linea, columna);
                ignorar_lexema();
            }
            if (sig == '\r') {
                _procesarSinCarryReturn();
            }
        }
        // Aceptamos el lexema
        copiar_lexema(&c);
        c.comp_lexico = STRING;

        asignar_lexema(&c, _sumar_strings(aux.lexema, c.lexema));
        insertarSemicolon = 1;
    }
}

// Función para concatenar dos strings manualmente
char* _sumar_strings(const char* str1, const char* str2) {
    // Calcular longitudes de los strings
    size_t len1 = 0, len2 = 0;

    while (str1[len1] != '\0') len1++;  // Calcular longitud de str1
    while (str2[len2] != '\0') len2++;  // Calcular longitud de str2

    // Reservar memoria para el nuevo string (+1 para el '\0')
    char* resultado = (char*)malloc(len1 + len2 + 1);
    if (resultado == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }

    // Copiar str1 a resultado
    size_t i;
    for (i = 0; i < len1; i++) {
        resultado[i] = str1[i];
    }

    // Copiar str2 a resultado después de str1
    for (size_t j = 0; j < len2; j++, i++) {
        resultado[i] = str2[j];
    }

    // Agregar el carácter nulo al final
    resultado[i] = '\0';

    return resultado; // Retorna el nuevo string concatenado
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
                }
                break;
            case 1:
                if (sig == 'b' || sig == 'B') estado = 2; // Binario
                else if (sig == 'o' || sig == 'O') estado = 3; // Octal
                else if (sig == 'x' || sig == 'X') {
                    _procesarHexadecimal(); // Hexadecimal
                    return 0;
                } else if (isdigit(sig)) estado = 10; // Decimal con ceros iniciales
                else if (sig == '_') estado = 11;   // Manejo de _
                else if (sig == 'i') {
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                }
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = INT;
                }
                break;
            case 2: // Binario
                if (sig == '_') estado = 12;
                else if (sig == '0' || sig == '1') estado = 2;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = BINARY_INT;
                }
                break;
            case 3: // Octal
                if (sig == '_') estado = 13;
                else if (sig >= '0' && sig <= '7') estado = 3;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = OCTAL_INT;
                }
                break;
            case 10: // Decimal normal
                if (sig == '_') estado = 11;
                else if (isdigit(sig)) estado = 10;
                else if (sig == '.') {
                    estado = 20;
                } else if (sig == 'e' || sig == 'E') {
                    estado = 30;
                } else if (sig == 'i') {
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else if (sig == 'p' || sig == 'P') {
                    devolver_un_caracter(); _restar_columna(sig);
                    error_entero(linea, columna);
                    estado = FIN_COMPONENTE;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = INT;
                }
                break;
            case 11:
                if (isdigit(sig)) estado = 10;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_entero(linea, columna);
                }
                break;
            case 12:
                if (sig == 0 || sig == 1) estado = 2;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_entero(linea, columna);
                }
                break;
            case 13:
                if (sig >= '0' && sig <= '7') estado = 3;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_entero(linea, columna);
                }
                break;
            case 19: // inicio de .123
                if (isdigit(sig)) estado = 21;
                else if (sig == 'e' || sig == 'E') estado = 30;
                else if (isalpha(sig)) {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    _procesarOperador('.');
                }
                break;
            case 20: // Parte fraccionaria de decimal o inicio de .123
                if (isdigit(sig)) estado = 21;
                else if (sig == 'e' || sig == 'E') estado = 30;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_float(linea, columna);
                }
                break;
            case 21:
                if (sig == '_') estado = 22;
                else if (isdigit(sig)) estado = 21;
                else if (sig == 'e' || sig == 'E') estado = 30;
                else if (sig == 'i') {
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = FLOAT;
                }
                break;
            case 22:
                if (isdigit(sig)) estado = 21;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_float(linea, columna);
                }
                break;
            case 30: // Exponente
                if (sig == '+' || sig == '-') estado = 31;
                else if (isdigit(sig)) estado = 32; // Permitir 1.e0
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_float(linea, columna);
                }
                break;
            case 31: // Signo después de exponente
                if (isdigit(sig)) estado = 32;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_float(linea, columna);
                }
                break;
            case 32: // Dígitos en el exponente
                if (sig == '_') estado = 33;
                else if (isdigit(sig)) estado = 32;
                else if (sig == 'i') { // Para casos como 1.e0+0i
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = FLOAT;
                }
                break;
            case 33:
                if (isdigit(sig)) estado = 32;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_float(linea, columna);
                }
                break;
            case 40: // Manejo del signo después del exponente (como en 1.e0+0i)
                if (isdigit(sig)) estado = 41;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_float(linea, columna);
                }
                break;
            case 41: // Dígitos después del signo en el exponente
                if (sig == '_') break;
                else if (isdigit(sig)) estado = 41;
                else if (sig == 'i') { // Para casos como 1.e0+0i
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = FLOAT;
                }
                break;
        }
        if (estado != FIN_COMPONENTE) {
            sig = sig_caracter(); _sumar_columna(sig);
        }
    }
    copiar_lexema(&c);
    insertarSemicolon = 1;
    return 0;
}

int _procesarHexadecimal() {
    char sig;
    int estado = 0;

    while (estado != FIN_COMPONENTE) {
        sig = sig_caracter(); _sumar_columna(sig);
        switch (estado) {
            case 0:
                // Comienzo de la lectura
                if (isxdigit(sig)) {
                    estado = 1;
                } else if (sig == '_') {
                    estado = 2;
                } else if (sig == '.') {
                    estado = 3;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    // Si no hay digito HEX o _
                    error_hexadecimal(linea, columna);
                }
                break;
            case 1:
                // Se ha leído un dígito HEX
                if (!isxdigit(sig)) {
                    if (sig == '_') {
                        estado = 2;
                    } else if (sig == '.') {
                        estado = 3;
                    } else if (sig == 'p' || sig == 'P') {
                        estado = 5;
                    } else {
                        // Si ya ha acabado el literal HEX
                        devolver_un_caracter(); _restar_columna(sig);
                        estado = FIN_COMPONENTE;
                        (c.comp_lexico) = HEX_INT;
                    }
                } else if (sig == '.') estado = 3;
                break;
            case 2:
                // Se ha leído una _
                if (!isxdigit(sig)) {
                    // No se puede acabar en _
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_hexadecimal(linea, columna);
                } else {
                    estado = 1;
                }
                break;
            case 3: // punto
                if (isxdigit(sig)) {
                    estado = 4;
                } else if (sig == 'p' || sig == 'P') {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_hexadecimal(linea, columna);
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    (c.comp_lexico) = HEX_FLOAT;
                }
                break;

            case 4: // mantisa fracción
                if (isxdigit(sig) && sig != 'E' && sig != 'e') {
                    estado = 4;
                } else if (sig == 'p' || sig == 'P') {
                    estado = 5;
                } else if (sig == 'e' || sig == 'E') {
                    estado = 9;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    (c.comp_lexico) = HEX_FLOAT;
                }
                break;
            case 9:
                if (sig == '+' || sig == '-') {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_hexadecimal(linea, columna);
                } else if (isxdigit(sig)) {
                    estado = 4;
                } else if (sig == 'p' || sig == 'P') {
                    estado = 5;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    (c.comp_lexico) = HEX_FLOAT;
                }
                break;

            case 5: // exponente prefijo
                if (sig == '+' || sig == '-') {
                    estado = 6;
                } else if (isdigit(sig)) {
                    estado = 7;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_hexadecimal(linea, columna);
                }
                break;

            case 6: // exponente signo
                if (isdigit(sig)) {
                    estado = 7;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    error_hexadecimal(linea, columna);
                }
                break;

            case 7: // exponente dígitos
                if (isdigit(sig)) {
                    estado = 7;
                } else if (sig == '_') {
                    estado = 8;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    (c.comp_lexico) = HEX_FLOAT;
                }
                break;
            case 8: // exponente dígitos con _
                if (isdigit(sig)) {
                    estado = 7;
                } else {
                    estado = FIN_COMPONENTE;
                    error_hexadecimal(linea, columna);
                }
                break;
            default:
                break;
        }
    }
    copiar_lexema(&c);
    insertarSemicolon = 1;
    // Si no hay errores de construcción
    return 0;
}

void _devolverSemicolon() {
    c.comp_lexico = (int) ';';
    char* semicolon = (char*) malloc (2*sizeof(char));
    semicolon[0] = ';';
    semicolon[1] = '\0';
    c.lexema = semicolon;

    // Ponemos a 0
    insertarSemicolon = 0;
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


void _sumar_columna(char caracter) {
    if (caracter == '\n') {
        _sumar_linea();
    } else {
        columna++;
    }
}

void _restar_columna(char caracter) {
    if (caracter == '\n') {
        _restar_linea();
    } else {
        columna--;
    }
}

void _sumar_linea() {
    linea++;
    columna_antigua = columna;
    columna = 1;
}

void _restar_linea() {
    linea--;
    columna = columna_antigua;
}