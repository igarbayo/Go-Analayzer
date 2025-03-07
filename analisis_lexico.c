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
char* semicolon;

// Variable global para el lexema de EOF
char* end_of_file;

// Variable global para el resultado de la suma de strings
char* resultado;


// FUNCIONES PRIVADAS (CABECERAS) ////////////////////////////////////

/**
 * Vacía el contenedor global, utilizado anteriormente
 */
void _vaciar_contenedor();

/**
 * Salta un comentario
 * @return 1 si se procesa / como un operador
 * @return 0 si es un comentario
 */
int _procesarComentario();

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
int _procesarString(char separador);

/**
 * Procesa una raw string para no tener en cuenta el carry return (\r)
 * @return 1 si hay error de construcción
 * @return 0 en otro caso
 */
int _procesarSinCarryReturn();
/**
 * Concatena a mano dos strings
 * @param str1 primera string
 * @param str2 segunda string
 * @return string concatenada
 */
char* _sumar_strings(const char* str1, const char* str2);

/**
 * Reconoce un integer o floating-point literal
 * @param primerCaracter
 * @return 1 si hay error de ocnstrucción
 * @return 0 en otro caso
 */
int _procesarNumero(char primerCaracter);

/**
 * Procesa un número Hexadecimal. Se utiliza en _procesarNumero()
 * @return 1 si hay error de construcción
 * @return 0 si está bien construido
 */
int _procesarHexadecimal();

/**
 * Guarda en el contenedor un semicolon (;)
 */
void _devolverSemicolon();

/**
 * Si no está en la TS, inserta un lexema
 * @param comp_lexico asignado al lexema en la TS
 */
void _buscar_insertar_lexema(int comp_lexico);

/**
 * Suma una unidad al conteo de columnas del programa.
 * Si hay salto de línea, lo procesa
 * @param caracter procesado
 */
void _sumar_columna(char caracter);

/**
 * Resta una unidad al conteo de columnas del programa.
 * Si hay salto de línea, lo procesa.
 * @param caracter procesado
 */
void _restar_columna(char caracter);

/**
 * Pasa los contadores a la línea siguiente a la actual.
 * Se utiliza en _sumar_columna()
 */
void _sumar_linea();

/**
 * Pasa los contadores a la línea anterior a la actual.
 * Se utiliza en _restar_columna()
 */
void _restar_linea();


// FUNCIONES PÚBLICAS /////////////////////////////////////////

void iniciar_analisis_lexico(char* fichero) {
    iniciar_sistema_entrada(fichero);
}

void terminar_analisis_lexico() {
    if (end_of_file != NULL) {
        free(end_of_file);
    }
    if (resultado != NULL) {
        free(resultado);
    }
    cerrar_sistema_entrada();
}

contenedor sig_comp_lexico() {

    // Consume un carácter del sistema de entrada
    char sig = sig_caracter(); _sumar_columna(sig);

    // Vacía el contenedor global
    _vaciar_contenedor();

    // Mientras no se alcanza el final del fichero
    if (sig != EOF) {
        if (isalpha(sig) || sig == '_') {
            _procesarIdentificador();
        } else if (isdigit(sig) || sig == '.') {
            if (_procesarNumero(sig) == 1) {
                // En caso de error, continúa el análisis
                return sig_comp_lexico();
            }
        } else if (sig == '`' || sig == '"') {
            if (_procesarString(sig) == 1) {
                // En caso de error, continúa el análisis
                return sig_comp_lexico();
            }
        } else if (sig == '/') {
            if (_procesarComentario() == 0) {
                // En caso de que / se procese como un operador
                if (insertarSemicolon == 1) {
                    // En caso de que haya que insertar ;
                    _devolverSemicolon();
                    devolver_un_caracter();
                } else {
                    // Si es un comentario, continúa el análisis
                    insertarSemicolon = 0;
                    return sig_comp_lexico();
                }
            }
        } else if (sig == '+' || sig == '-' || sig == '*' || sig == '%' || sig == '&' || sig == '|' ||
                   sig == '^' || sig == '<' || sig == '>' || sig == '=' || sig == '!' || sig == '~' ||
                   sig == ':' || sig == ',' || sig == ';' || sig == '(' || sig == ')' || sig == '[' ||
                   sig == ']' || sig == '{' || sig == '}') {
            _procesarOperador(sig);
        } else if (sig == '\n') {
            // Para los saltos de línea
            if (insertarSemicolon == 1) {
                // Si hay que insertar semicolon
                _devolverSemicolon();
                ignorar_caracter();
            } else {
                ignorar_caracter();
                return sig_comp_lexico();
            }
        } else {
            // Resto de caracteres
            ignorar_caracter();
            // Recursividad al ignorar caracteres
            return sig_comp_lexico();
        }
    } else {
        // El final del archivo
        if (insertarSemicolon == 1) {
            // Si hay que insertar semicolon
            _devolverSemicolon();
            devolver_un_caracter();
        } else {
            // En otro caso, devolvemos el último componente léxico
            c.comp_lexico = FINFICHERO;
            end_of_file = (char*) malloc (4*sizeof(char));
            end_of_file[0] = 'E';
            end_of_file[1] = '0';
            end_of_file[2] = 'F';
            end_of_file[3] = '\0';
            asignar_lexema(&c, end_of_file);
        }
    }

    // Devolvemos el contenedor con el componente léxico y el lexema
    return c;
}


// FUNCIONES PRIVADAS (IMPLEMENTACIÓN) ///////////////////////////////////////////7

void _vaciar_contenedor() {
    if (c.lexema != NULL) {
        // Liberamos memoria
        free(c.lexema);
        // Volvemos a los valores iniciales
        c.comp_lexico = -1;
        c.lexema = NULL;
    }
}

int _procesarComentario() {
    char sig;
    int estado = 0;

    // segundoCaracter puede ser / (empieza por //) o * (empieza por / y * y acaba en * seguido de/)
    char segundoCaracter = sig_caracter(); _sumar_columna(segundoCaracter);

    switch (segundoCaracter) {
        case '/':
            // Tipo "//"
            sig = sig_caracter(); _sumar_columna(sig);
            while (sig != '\n') {
                // Mientras no haya saltos de línea
                sig = sig_caracter(); _sumar_columna(sig);
            }
            break;
        case '*':
            // Tipo "/**/"
            sig = sig_caracter(); _sumar_columna(sig);
            while (estado != FIN_COMPONENTE) {
                switch (estado) {
                    case 0:
                        // Si leemos un *
                        if (sig == '*') {
                            estado = 1;
                        }
                        break;
                    case 1:
                        if (sig == '/') {
                            // Si leemos */
                            estado = FIN_COMPONENTE;
                            insertarSemicolon = 0;
                        } else {
                            // Si solo era *
                            estado = 0;
                        }
                        break;
                    default:
                        break;
                }
                // Consumimos un carácter
                sig = sig_caracter(); _sumar_columna(sig);
            }
            break;
        default:
            // Se procesa / como un operador
            devolver_un_caracter(); _restar_columna(segundoCaracter);
            _procesarOperador('/');
            insertarSemicolon = 0;
            return 1;
            break;

    }

    // Si es un comentario, se ignora
    ignorar_lexema();

    return 0;
}

void _procesarIdentificador() {
    char sig;
    int estado = 0;
    while (estado != FIN_COMPONENTE) {
        // Se consume un carácter
        sig = sig_caracter(); _sumar_columna(sig);
        if (!isalnum(sig) && sig != '_') {
            // Si no es un alphanum o un underscore (_)
            estado = FIN_COMPONENTE;
            // Se devuelve el carácter leído de más
            devolver_un_caracter(); _restar_columna(sig);
            // Aceptamos el lexema
            copiar_lexema(&c);
            insertarSemicolon = 1;

            if (c.lexema != NULL) {
                // Si hay lexema, vemos si hace falta insertarlo en la TS
                _buscar_insertar_lexema(ID);
            }
        }
    }
}

void _procesarOperador(char primerCaracter) {
    // Leemos otros dos caracteres para el procesamiento
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

        // En estos casos, si termina línea, hará falta insertar semicolon (;)
        insertarSemicolon = 1;
    } else {
        // Si tiene 1 solo carácter, asignamos ASCII y devolvemos dos caracteres leídos de más
        (c.comp_lexico) = (int) primerCaracter;
        devolver_un_caracter(); _restar_columna(tercerCaracter);
        devolver_un_caracter(); _restar_columna(segundoCaracter);
        insertarSemicolon = 0;
    }

    // Copiamos el lexema y finalizamos el procesamiento
    copiar_lexema(&c);
}

int _procesarString(char separador) {
    char sig;
    int estado = 0;

    switch (separador) {
        // Raw string
        case '`':
            while (estado != FIN_COMPONENTE) {
                // Consumimos un carácter
                sig = sig_caracter(); _sumar_columna(sig);
                // Terminan con `
                if (sig == '`') {
                    estado = FIN_COMPONENTE;
                }
                // Si se llega al final del archivo antes, hay un error
                if (sig == EOF) {
                    devolver_un_caracter();
                    _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = ERRORLEXICO;
                    copiar_lexema(&c);
                    error_string(linea, columna, c.lexema);
                    return 1;
                }
                if (sig == '\r') {
                    // Se ignoran los \r en el caso de raw strings
                    return _procesarSinCarryReturn();
                }
            }
            // Aceptamos el lexema
            copiar_lexema(&c);
            c.comp_lexico = STRING;

            // Si termina línea, hay que insertar semicolon
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
                            devolver_un_caracter();
                            _restar_columna(sig);
                            estado = FIN_COMPONENTE;
                            c.comp_lexico = ERRORLEXICO;
                            copiar_lexema(&c);
                            error_string(linea, columna, c.lexema);
                            return 1;
                        }
                        break;
                    case 1:
                        // Caracteres de escape válidos
                        if (sig != 'a' && sig != 'b' && sig !='f' && sig != 'n' && sig != 'r' &&
                            sig != 't' && sig != 'v' && sig != '\\' && sig != '"' & sig != '\'') {
                            devolver_un_caracter();
                            _restar_columna(sig);
                            estado = FIN_COMPONENTE;
                            c.comp_lexico = ERRORLEXICO;
                            copiar_lexema(&c);
                            error_string(linea, columna, c.lexema);
                            return 1;
                            // Si se llega al final del archivo antes, hay un error
                        } else if (sig == EOF) {
                            devolver_un_caracter();
                            _restar_columna(sig);
                            estado = FIN_COMPONENTE;
                            c.comp_lexico = ERRORLEXICO;
                            copiar_lexema(&c);
                            error_string(linea, columna, c.lexema);
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

            // Si termina línea, hay que insertar semicolon
            insertarSemicolon = 1;
            break;
        default:
            // Si no es ninguno de estos inicios, hay un error
            return 1;
    }

    return 0;
}

int _procesarSinCarryReturn() {
    char sig;
    int estado = 0;

    // Contenedor auxiliar con la primera parte del lexema
    contenedor aux = {-1, NULL};
    devolver_un_caracter();
    copiar_lexema(&aux);

    // Consumimos un carácter
    sig = sig_caracter();
    _sumar_columna(sig);

    // Si leemos carry-return
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
                devolver_un_caracter();
                _restar_columna(sig);
                estado = FIN_COMPONENTE;
                c.comp_lexico = ERRORLEXICO;
                copiar_lexema(&c);
            }
            if (sig == '\r') {
                // Es una función de comportamiento recursivo
                _procesarSinCarryReturn();
            }
        }
        // Aceptamos el lexema
        copiar_lexema(&c);
        if (c.comp_lexico == ERRORLEXICO) {
            error_string(linea, columna, c.lexema);
            return 1;
        } else {
            c.comp_lexico = STRING;
        }

        // Se asigna la suma de los dos lexemas
        asignar_lexema(&c, _sumar_strings(aux.lexema, c.lexema));

        // Si termina línea, hay que insertar semicolon
        insertarSemicolon = 1;
    }

    return 0;
}

// Función para concatenar dos strings manualmente
char* _sumar_strings(const char* str1, const char* str2) {
    // Calcular longitudes de los strings
    size_t len1 = 0, len2 = 0;

    while (str1[len1] != '\0') len1++;  // Calcular longitud de str1
    while (str2[len2] != '\0') len2++;  // Calcular longitud de str2

    // Reservar memoria para el nuevo string (+1 para el '\0')
    if (resultado != NULL) {
        free(resultado);
    }

    resultado = (char*)malloc(len1 + len2 + 1);
    if (resultado == NULL) {
        error_memoria();
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

    // Tipo de number literal reconocido (1 = int, 2 = floating-point)
    int tipo = 0;

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
                    tipo = 1;
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
            case 1:
                if (sig == 'b' || sig == 'B') estado = 2; // Binario
                else if (sig == 'o' || sig == 'O') estado = 3; // Octal
                else if (sig == 'x' || sig == 'X') {
                    return _procesarHexadecimal(); // Hexadecimal
                } else if (isdigit(sig)) estado = 10; // Decimal con ceros iniciales
                else if (sig == '_') estado = 11;   // Manejo de _
                else if (sig == 'i') { // Imaginario
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                }
                else { // es un entero 0
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = INT;
                }
                break;
            case 2: // Binario
                if (sig == '_') estado = 12; // Binario con _
                else if (sig == '0' || sig == '1') estado = 2; // Binario con dígito
                else {
                    // Número binario
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = BINARY_INT;
                }
                break;
            case 3: // Octal
                if (sig == '_') estado = 13;  // Octal con _
                else if (sig >= '0' && sig <= '7') estado = 3; // Octal con dígitos
                else {
                    // Número octal
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = OCTAL_INT;
                }
                break;
            case 10: // Decimal normal
                if (sig == '_') estado = 11;  // Decimal con _
                else if (isdigit(sig)) estado = 10;     // Decimal con dígitos
                else if (sig == '.') {  // Decimal con .
                    estado = 20;
                } else if (sig == 'e' || sig == 'E') {  // Decimal con exponente
                    estado = 30;
                } else if (sig == 'i') {  // Decimal con parte imaginaria
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else if (sig == 'p' || sig == 'P') {  // Error de exponente hexadecimal
                    devolver_un_caracter(); _restar_columna(sig);
                    tipo = 1;
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = ERRORLEXICO;
                } else {
                    // Número decimal
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = INT;
                }
                break;
            case 11: // Decimal con _
                if (isdigit(sig)) estado = 10; // tiene que separar dígitos
                else {
                    // Si no, hay error
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    tipo = 1;
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
            case 12: // Binario con _
                if (sig == 0 || sig == 1) estado = 2; // tiene que separar dígitos
                else {
                    // Si no, hay error
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    tipo = 1;
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
            case 13: // Octal con _
                if (sig >= '0' && sig <= '7') estado = 3; // tiene que separar dígitos
                else {
                    // Si no, hay error
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    tipo = 1;
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
            case 19: // inicio de .123
                if (isdigit(sig)) estado = 21;
                else if (sig == 'e' || sig == 'E') estado = 30;
                else if (sig == '.') {
                    // pueden ser 3 puntos
                    devolver_un_caracter();
                    _restar_columna(sig);
                    _procesarOperador('.');
                    tipo = 3;
                } else {
                    // Resto de casos
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    _procesarOperador('.');
                    tipo = 3;
                }
                break;
            case 20: // Parte fraccionaria de decimal o inicio de .123
                if (isdigit(sig)) estado = 21;
                // Puede haber exponente
                else if (sig == 'e' || sig == 'E') estado = 30;
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    tipo = 2;
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
            case 21: // Parte fraccionaria de decimal con al menos un dígito
                if (sig == '_') estado = 22;
                else if (isdigit(sig)) estado = 21;
                // Puede haber exponente
                else if (sig == 'e' || sig == 'E') estado = 30;
                // En este estado, puede haber parte imaginaria
                else if (sig == 'i') {
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = FLOAT;
                }
                break;
            case 22: // Parte fraccionaria de decimal con _
                if (isdigit(sig)) estado = 21;
                // Tiene que separar dígitos necesariamente
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    tipo = 2;
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
            case 30: // Exponente
                if (sig == '+' || sig == '-') estado = 31;
                else if (isdigit(sig)) estado = 32; // Permitir 1.e0
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    tipo = 2;
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
            case 31: // Signo después de exponente
                if (isdigit(sig)) estado = 32;  // Tiene que haber un dígito
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    tipo = 2;
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
            case 32: // Dígitos en el exponente
                if (sig == '_') estado = 33; // si hay _
                else if (isdigit(sig)) estado = 32; // mantenemos el estado
                else if (sig == 'i') { // Para casos como 1.e0+0i
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = IMAGINARY;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = FLOAT;
                }
                break;
            case 33: // Dígitos en el exponente con _
                if (isdigit(sig)) estado = 32;
                // Debe separar dígitos necesariamente
                else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    tipo = 2;
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
        }
        if (estado != FIN_COMPONENTE) {
            sig = sig_caracter(); _sumar_columna(sig);
        }
    }

    // Si no se ha terminado procesando un operador (se procesa un número)
    if (tipo != 3) {
        copiar_lexema(&c);

        // Si termina línea, hay que insertar semicolon
        insertarSemicolon = 1;
    }

    // Manejo de tipo de error
    if (c.comp_lexico == ERRORLEXICO) {
        switch (tipo) {
            case 1:
                // enteros
                error_entero(linea, columna, c.lexema);
                break;
            case 2:
                // floats
                error_float(linea, columna, c.lexema);
                break;
            default:
                break;
        }
        // En caso de error
        return 1;
    }

    // En caso de éxito
    return 0;
}

int _procesarHexadecimal() {
    char sig;
    int estado = 0;

    while (estado != FIN_COMPONENTE) {
        // Consume un carácter
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
                    c.comp_lexico = ERRORLEXICO;
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
                    c.comp_lexico = ERRORLEXICO;
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
                    c.comp_lexico = ERRORLEXICO;
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
            case 9: // e o E del exponente
                if (sig == '+' || sig == '-') {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = ERRORLEXICO;
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
                    c.comp_lexico = ERRORLEXICO;
                }
                break;

            case 6: // exponente signo
                if (isdigit(sig)) {
                    estado = 7;
                } else {
                    devolver_un_caracter(); _restar_columna(sig);
                    estado = FIN_COMPONENTE;
                    c.comp_lexico = ERRORLEXICO;
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
                    c.comp_lexico = ERRORLEXICO;
                }
                break;
        }
    }

    copiar_lexema(&c);

    // Si termina línea, hay que insertar semicolon
    insertarSemicolon = 1;

    // Manejo de error
    if (c.comp_lexico == ERRORLEXICO) {
        error_hexadecimal(linea, columna, c.lexema);
        return 1;
    }

    // Si no hay errores de construcción
    return 0;
}

void _devolverSemicolon() {
    // Guarda el componente léxico
    c.comp_lexico = (int) ';';

    semicolon = (char*) malloc (2*sizeof(char));
    if (semicolon == NULL) {
        error_memoria();
    } else {
        semicolon[0] = ';';
        semicolon[1] = '\0';
        c.lexema = semicolon;
    }
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
        // si se cambia de línea
        _sumar_linea();
    } else {
        columna++;
    }
}

void _restar_columna(char caracter) {
    if (caracter == '\n') {
        // si se cambia de línea
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