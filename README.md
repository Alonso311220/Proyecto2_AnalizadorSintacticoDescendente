# Proyecto2_AnalizadorSintacticoDescendente
Se trata de un analizador sintáctico descendete predictivo y recursivo, donde se hace uso de las expresiones regulares que se usan por medio de flex en C. Con esto se facilita el análisis de cualquier programa para su correcto análisis, tanto sintáctico como semántico. Se puede abrir la puerta hacia algo más grande como un compilador o un traductor
Se puede correr desde la consola ingresando a la carpeta src e insertando los siguientes comandos:

flex ident.l

gcc lex.yy.c listaCircular.c -o salida

.\salida archivo_entrada.txt

Se hace uso de flex para las expresiones regulares.
Las tablas son dinámicas y se crean por medio de una lista circular
se usa el archivo_entrada.txt para poder verificar si el analizador léxico y sintáctico funcionan correctamente
