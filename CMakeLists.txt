cmake_minimum_required(VERSION 3.30)
project(P1_CompGo C)

set(CMAKE_C_STANDARD 11)

add_executable(P1_CompGo main.c
        definiciones.h
        analisis_lexico.c
        analisis_sintactico.c
        analisis_lexico.h
        analisis_sintactico.h)
