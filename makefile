# Nombre del ejecutable
TARGET = P1_CompGo

# Compilador
CC = gcc

# Flags de compilación
CFLAGS = -Wall -Wextra -g

# Archivos fuente y headers
SRCS = main.c avl.c analisis_lexico.c analisis_sintactico.c tabla_simbolos.c sistema_entrada.c errores.c
HEADERS = definiciones.h avl.h analisis_lexico.h analisis_sintactico.h tabla_simbolos.h sistema_entrada.h errores.h

# Objetos generados
OBJS = $(SRCS:.c=.o)

# Regla principal
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regla para compilar archivos .c a .o
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos compilados
clean:
	rm -f $(OBJS) $(TARGET)

# Limpiar todo incluyendo archivos de depuración
cleanall: clean
	rm -f *~

