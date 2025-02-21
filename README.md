# Analizador léxico en Go
Como parte de la asignatura de **Compiladores e Interprétes**, del Grado en Ingeniería Informática, se desarrolla un analizador léxico que recibe como entrada el fichero $\texttt{concurrentSum.go}$. Basando su funcionamiento
en la especificación del lenguaje Go [*The Go Programming Language Specification*](https://go.dev/ref/spec), el programa imprime por pantalla los pares de la forma $texttt{<componente_lexico, "lexema">}$ que aparecen en
el código fuente. Inicialmente, se cargan en una tabla de simbolos las *keywords* que aparecen en el fichero $\texttt{definiciones.h}$.
