//
// Created by ignacio on 19/02/25.
//

#ifndef P1_COMPGO_DEFINICIONES_H
#define P1_COMPGO_DEFINICIONES_H

typedef struct {
    int comp_lexico;
    char *lexema;
} contenedor;

// Identificador
#define ID 300

// Keywords
#define BREAK 301
#define CASE 302
#define CHAN 303
#define CONST 304
#define CONTINUE 305
#define DEFAULT 306
#define DEFER 307
#define ELSE 308
#define FALLTHROUGH 309
#define FOR 310
#define FUNC 311
#define GO 312
#define GOTO 313
#define IF 314
#define IMPORT 315
#define INTERFACE 316
#define MAP 317
#define PACKAGE 318
#define RANGE 319
#define RETURN 320
#define SELECT 321
#define STRUCT 322
#define SWITCH 323
#define TYPE 324
#define VAR 325

// Operators and punctuaction (dobles y triples)
#define MENORDOBLE 334                      // <<
#define MAYORDOBLE 335                      // >>
#define AMPERSANDCIRCUNFLEJO 336            // &^
#define MASIGUAL 337                        // +=
#define MENOSIGUAL 338                      // -=
#define PORIGUAL 339                        // *=
#define BARRAIGUAL 340                      // /=
#define PORCENTAJEIGUAL 341                 // %=
#define AMPERSANDIGUAL 342                  // &=
#define BARRAVERTICALIGUAL 343              // |=
#define CIRCUNFLEJOIGUAL 344                // ^=
#define MENORDOBLEIGUAL 345                 // <<=
#define MAYORDOBLEIGUAL 346                 // >>=
#define AMPERSANDCIRCUNFLEJOIGUAL 347       // &^=
#define AMPERSANDDOBLE 348                  // &&
#define BARRAVERTICALDOBLE 349              // ||
#define MENORMENOS 350                      // <-
#define MASDOBLE 351                        // ++
#define MENOSDOBLE 352                      // --
#define IGUALDOBLE 353                      // ==
#define EXCLAMACIONIGUAL 359                // !=
#define MENORIGUAL 360                      // <=
#define MAYORIGUAL 361                      // >=
#define DOSPUNTOSIGUAL 362                  // :=
#define TRESPUNTOS 363                      // ...

// Number literals
#define INT 374
#define FLOAT 375
#define HEX 376
#define IMAGINARY 377

// More literals
#define RUNE 378
#define STRING 379

// End-of-file
#define FINFICHERO 500



#endif //P1_COMPGO_DEFINICIONES_H
