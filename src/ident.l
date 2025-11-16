%{
    /*
    Proyecto 2 - Analizador Léxico + Sintáctico
    Integrantes:
    - Persona 1: Martínez Araujo Jesús Alonso - Adaptación léxica y cadena de átomos
    - Persona 2: Emilia Macarena Arriaga Rodríguez - Analizador Predictivo
    - Persona 3: Marco Gael Ventura Picazo - Analizador Recursivo
    
    Fecha: 10/11/2025
    */
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   #include "../inc/listaCircular.h"
  
   // ==================== TABLAS DEL ANALIZADOR LÉXICO ====================
   ListaC * TS_inicio = NULL;      // Tabla de Símbolos
   ListaC * TLN_inicio = NULL;     // Tabla de Literales Numéricas (Clase 2)
   ListaC * TLR_inicio = NULL;     // Tabla de Literales Reales (Clase 3) - NUEVA
   ListaC * TLC_inicio = NULL;     // Tabla de Literales Cadenas
   ListaC * ListaT_inicio = NULL;  // Lista de TOKENS

   // Contadores de posición
   int pos_ts = 0;
   int pos_tln = 0;
   int pos_tlr = 0;  // NUEVO: Para reales
   int pos_tlc = 0;
   int pos_token = 0;

   // ==================== NUEVA: CADENA DE ÁTOMOS ====================
   typedef struct {
       int clase;
       int valor;
       char atomo[3];  // Átomo correspondiente 
   } MapeoAtomo;

   // Mapeo completo según el proyecto
   MapeoAtomo tabla_atomos[] = {
       // Palabras Reservadas (Clase 0)
    //    {0, 0, "alternative"}, {0, 1, "big"}, {0, 2, "evaluate"},
    //    {0, 3, "instead"}, {0, 4, "large"}, {0, 5, "loop"},
    //    {0, 6, "make"}, {0, 7, "number"}, {0, 8, "other"},
    //    {0, 9, "real"}, {0, 10, "repeat"}, {0, 11, "select"},
    //    {0, 12, "small"}, {0, 13, "step"}, {0, 14, "stop"},
    //    {0, 15, "symbol"}, {0, 16, "throw"},
    {0, 0, "a"}, {0, 1, "b"}, {0, 2, "f"},
    {0, 3, "t"}, {0, 4, "g"}, {0, 5, "w"},
    {0, 6, "m"}, {0, 7, "#"}, {0, 8, "o"},
    {0, 9, "f"}, {0, 10, "j"}, {0, 11, "h"},
    {0, 12, "p"}, {0, 13, "c"}, {0, 14, "q"},
    {0, 15, "y"}, {0, 16, "z"},
       
       // Identificadores (Clase 1) → i
       {1, -1, "i"},
       
       // Literales Numéricas (Clase 2) → n
       {2, -1, "n"},
       
       // Literales Reales (Clase 3) → r
       {3, -1, "r"},
       
       // Literales Cadena (Clase 4) → s
       {4, -1, "s"},
       
       // Delimitadores (Clase 5)
       {5, 0, "["}, {5, 1, "]"}, {5, 2, "("}, {5, 3, ")"},
       {5, 4, "{"}, {5, 5, "}"}, {5, 6, ","}, {5, 7, ":"},
       {5, 8, ";"},
       
       // Operadores Aritméticos (Clase 6)
       {6, 0, "+"}, {6, 1, "-"}, {6, 2, "*"}, {6, 3, "/"},
       {6, 4, "%"}, {6, 5, "\\"}, {6, 6, "^"},
       
       // Operadores Relacionales (Clase 7)
    //    {7, 0, "<"}, {7, 1, ">"}, {7, 2, "<="}, {7, 3, ">="},
    //    {7, 4, "=="}, {7, 5, "!="},
       {7, 0, "<"}, {7, 1, ">"}, {7, 2, "l"}, {7, 3, "u"},
       {7, 4, "e"}, {7, 5, "d"},
       
       // Asignación (Clase 8)
       {8, 0, "="},
       
       {-1, -1, ""}  // Terminador
   };

   char *cadena_atomos = NULL;
   size_t tam_atomos = 0;
   size_t capacidad_atomos = 1024;

   // Variables de archivos
   extern FILE *yyout;
   extern FILE *yyin;
   FILE *apSal;
   
   // ==================== DECLARACIONES DE FUNCIONES ====================
   void mostrar_reporte_completo(int sintacticamente_correcto);
   const char* obtener_atomo(int clase, int valor);
   void agregar_atomo(const char *atomo);
   void inicializar_cadena_atomos();
   void imprimirTokens(ListaC *inicio, FILE *output_file);

   // ==================== FUNCIONES PRINCIPALES PARA EL ANALIZADOR LÉXICO ====================
   
   void imprimir_token(int clase, int pos) {
       // 1. Insertar en la lista de tokens
       InsertarToken(clase, pos);
       
       // 2. Obtener y agregar átomo
       const char *atomo = obtener_atomo(clase, pos);
       agregar_atomo(atomo);
       
       // 3. Imprimir (para debugging)
       printf("Token: (%d, %d) -> Atomo: %s\n", clase, pos, atomo);
       fprintf(apSal, "Token: (%d, %d) -> Atomo: %s\n", clase, pos, atomo);
       fflush(apSal);
   }
   
   void imprimirTokens(ListaC *inicio, FILE *output_file){
    if(inicio == NULL){
        printf("La lista está vacía.\n");
        if (output_file) fprintf(output_file, "La lista está vacía.\n");
        return;
    }

    ListaC *corredor = inicio;
    int contador = 0;
    
    do {
        const char *atomo = obtener_atomo(corredor->tipo, corredor->posicion);
        
        printf("Token %d: Clase=%d, Valor=%d, Atomo=%s\n", 
               ++contador,
               corredor->tipo,
               corredor->posicion,
               atomo);
               
        if (output_file) {
            fprintf(output_file, "Token %d: Clase=%d, Valor=%d, Atomo=%s\n", 
                    contador,
                    corredor->tipo,
                    corredor->posicion,
                    atomo);
        }
        
        corredor = corredor->siguiente;
    } while (corredor != inicio);
}

   void InsertarToken(int clase, int valor){
       int pos_actual = pos_token;
       InsertarFinal(&ListaT_inicio, pos_actual, NULL, clase);
       pos_token++;
   }

   // ==================== FUNCIONES DE BÚSQUEDA/INSERCIÓN ====================
   
   int buscar_o_insertar_identificador(char *ident){
       ListaC *nodo_encontrado = BuscarNodoPorDato(TS_inicio, ident);
       
       if(nodo_encontrado != NULL){
           return nodo_encontrado->posicion;
       }
       
       int posicion_actual = pos_ts;
       InsertarFinal(&TS_inicio, posicion_actual, ident, -1); 
       pos_ts++;
       
       return posicion_actual;
   }

   int buscar_o_insertar_literal_num(char *numero) {
       ListaC *nodo_encontrado = BuscarNodoPorDato(TLN_inicio, numero);
       
       if(nodo_encontrado != NULL){
           return nodo_encontrado->posicion;
       }

       int posicion_actual = pos_tln;
       InsertarFinal(&TLN_inicio, posicion_actual, numero, -1); 
       pos_tln++;
       
       return posicion_actual;
   }

   // NUEVA: Función separada para literales reales
   int buscar_o_insertar_literal_real(char *numero) {
       ListaC *nodo_encontrado = BuscarNodoPorDato(TLR_inicio, numero);
       
       if(nodo_encontrado != NULL){
           return nodo_encontrado->posicion;
       }

       int posicion_actual = pos_tlr;
       InsertarFinal(&TLR_inicio, posicion_actual, numero, -1); 
       pos_tlr++;
       
       return posicion_actual;
   }

   int buscar_o_insertar_literal_cad(char *cadena) {
       ListaC *nodo_encontrado = BuscarNodoPorDato(TLC_inicio, cadena);
       
       if(nodo_encontrado != NULL){
           return nodo_encontrado->posicion;
       }

       int posicion_actual = pos_tlc;
       InsertarFinal(&TLC_inicio, posicion_actual, cadena, -1); 
       pos_tlc++;
       
       return posicion_actual;
   }

%}

%%

[ \t\n]+        { /* Ignorar espacios en blanco */ }

"alternative"   { imprimir_token(0, 0); }
"big"           { imprimir_token(0, 1); }
"evaluate"      { imprimir_token(0, 2); }
"instead"       { imprimir_token(0, 3); }
"large"         { imprimir_token(0, 4); }
"loop"          { imprimir_token(0, 5); }
"make"          { imprimir_token(0, 6); }
"number"        { imprimir_token(0, 7); }
"other"         { imprimir_token(0, 8); }
"real"          { imprimir_token(0, 9); }
"repeat"        { imprimir_token(0, 10); }
"select"        { imprimir_token(0, 11); }
"small"         { imprimir_token(0, 12); }
"step"          { imprimir_token(0, 13); }
"stop"          { imprimir_token(0, 14); }
"symbol"        { imprimir_token(0, 15); }
"throw"         { imprimir_token(0, 16); }

\$[a-zA-Z_]+     {
                    int pos = buscar_o_insertar_identificador(yytext);
                    imprimir_token(1, pos);
                }

0|([1-9][0-9]*) {
                    int pos = buscar_o_insertar_literal_num(yytext);
                    imprimir_token(2, pos);
                }
                
[Oo][0-7]+      {
                    int pos = buscar_o_insertar_literal_num(yytext);
                    imprimir_token(2, pos);
                }

[0-9]*\.[0-9]+  {
                    int pos = buscar_o_insertar_literal_real(yytext);
                    imprimir_token(3, pos);
                }

\"[^"']+\"|\'[^"']\'|\'\"\'|\"\'\" {
                    char* valor_limpio = strdup(yytext + 1);
                    valor_limpio[strlen(valor_limpio) - 1] = '\0';
                    int pos = buscar_o_insertar_literal_cad(valor_limpio);
                    imprimir_token(4, pos);
                    free(valor_limpio);
                }

[\[]    { imprimir_token(5, 0); }
[\]]    { imprimir_token(5, 1); }
[\(]    { imprimir_token(5, 2); }
[\)]    { imprimir_token(5, 3); }
[\{]    { imprimir_token(5, 4); }
[\}]    { imprimir_token(5, 5); }
[,]     { imprimir_token(5, 6); }
[:]     { imprimir_token(5, 7); }
[;]     { imprimir_token(5, 8); }
       

[+]     { imprimir_token(6, 0); }
[-]     { imprimir_token(6, 1); }
[*]     { imprimir_token(6, 2); }
[/]     { imprimir_token(6, 3); }
[%]     { imprimir_token(6, 4); }
[\\]    { imprimir_token(6, 5); }
[\^]    { imprimir_token(6, 6); }

"<"    { imprimir_token(7, 0); }
">"    { imprimir_token(7, 1); }
"<="    { imprimir_token(7, 2); }
">="    { imprimir_token(7, 3); }
"=="     { imprimir_token(7, 4); }
"!="     { imprimir_token(7, 5); }

"="     { imprimir_token(8, 0); }

.       { 
            fprintf(stderr, "Error Lexico: Simbolo no reconocido '%s'\n", yytext);
            fprintf(apSal, "Error Lexico: Simbolo no reconocido '%s'\n", yytext);
        }

%%

int yywrap() {
    return 1;
}


// ==================== FUNCIÓN MAIN ====================

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Uso: %s <archivo_entrada>\n", argv[0]);
        printf("Ejemplo: %s programa.txt\n", argv[0]);
        return 1;
    }
    
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        printf("Error: No se pudo abrir el archivo '%s'\n", argv[1]);
        return 1;
    }
    
    apSal = fopen("resultados.txt", "w");
    if (apSal == NULL) {
        printf("Error: No se pudo crear el archivo 'resultados.txt'\n");
        fclose(yyin);
        return 1;
    }
    
    inicializar_cadena_atomos();
    
    printf("\n-----------------------------------------------------------\n");
    printf("|        COMPILADOR - PROYECTO 2                             |\n");
    printf("|        Analizador Lexico + Sintactico                      |\n");
    printf("-------------------------------------------------------------\n\n");
    
    printf("Analizando el archivo: %s\n", argv[1]);
    printf("Los resultados se guardaran en: resultados.txt\n\n");
    
    fprintf(apSal, "-------------------------------------------------------------\n");
    fprintf(apSal, "|        COMPILADOR - PROYECTO 2                             |\n");
    fprintf(apSal, "|       Analizador Lexico + Sintactico                      |\n");
    fprintf(apSal, "-------------------------------------------------------------\n\n");
    fprintf(apSal, "Archivo de entrada: %s\n\n", argv[1]);
    fprintf(apSal, "=== FASE 1: ANALISIS LEXICO ===\n\n");
    fflush(apSal);
    
    yyout = apSal;
    
    printf("--- FASE 1: ANALISIS LEXICO ---\n");
    yylex();
    printf(" Analisis lexico completado!\n\n");
    
    fprintf(apSal, "\n=== FASE 2: ANALISIS SINTACTICO ===\n\n");
    fflush(apSal);
    
    printf("--- FASE 2: ANALISIS SINTACTICO ---\n");
    
    int resultado_sintactico = 1;
    
    // TODO: Aquí las Personas 2 y 3 llamarán a sus funciones:
    // resultado_sintactico = analisis_predictivo(cadena_atomos);
    // resultado_sintactico = analisis_recursivo();
    
    printf("(Esperando implementación de analizadores sintácticos)\n");
    fprintf(apSal, "(Esperando implementación de analizadores sintácticos)\n");
    
    printf("Analisis sintactico completado!\n\n");
    
    fprintf(apSal, "\n=== FASE 3: REPORTE FINAL ===\n");
    mostrar_reporte_completo(resultado_sintactico);
    
    free(cadena_atomos);
    
    fclose(yyin);
    fclose(apSal);
    
    printf("\nAnalisis completado. Revisa el archivo 'resultados.txt' para ver los resultados :3\n");
    
    liberarLista(&TS_inicio);
    liberarLista(&TLN_inicio);
    liberarLista(&TLR_inicio);
    liberarLista(&ListaT_inicio);
    liberarLista(&TLC_inicio);

    return 0;
}

// ==================== IMPLEMENTACIÓN DE FUNCIONES ====================

// Inicializar la cadena de átomos
void inicializar_cadena_atomos() {
    cadena_atomos = (char*)malloc(capacidad_atomos * sizeof(char));
    if (cadena_atomos == NULL) {
        perror("Error al asignar memoria para cadena de átomos");
        exit(EXIT_FAILURE);
    }
    cadena_atomos[0] = '\0';
    tam_atomos = 0;
}


// Agregar átomo a la cadena
void agregar_atomo(const char *atomo) {
    size_t len_atomo = strlen(atomo);
    size_t espacio_necesario = tam_atomos + len_atomo + 2;
    
    if (espacio_necesario > capacidad_atomos) {
        capacidad_atomos *= 2;
        char *nuevo = (char*)realloc(cadena_atomos, capacidad_atomos);
        if (nuevo == NULL) {
            perror("Error al expandir cadena de átomos");
            free(cadena_atomos);
            exit(EXIT_FAILURE);
        }
        cadena_atomos = nuevo;
    }
    
    if (tam_atomos > 0) {
        strcat(cadena_atomos, " ");
        tam_atomos++;
    }
    
    strcat(cadena_atomos, atomo);
    tam_atomos += len_atomo;
}

// Buscar átomo correspondiente a un token
const char* obtener_atomo(int clase, int valor) {
    // Casos especiales: i, n, r, s (valor no importa)
    if (clase == 1) return "i";
    if (clase == 2) return "n";
    if (clase == 3) return "r";
    if (clase == 4) return "s";
    
    // Buscar en la tabla
    for (int i = 0; tabla_atomos[i].clase != -1; i++) {
        if (tabla_atomos[i].clase == clase && 
            tabla_atomos[i].valor == valor) {
            return tabla_atomos[i].atomo;
        }
    }
    
    return "ERROR";
}


// Módulo de Reporte Final
void mostrar_reporte_completo(int sintacticamente_correcto) {
    printf("\n");
    printf("--------------------------------------------------------------\n");
    printf("|         REPORTE COMPLETO DEL ANALISIS                      \n");
    printf("--------------------------------------------------------------\n\n");
    
    fprintf(apSal, "\n");
    fprintf(apSal, "-------------------------------------------------------------\n");
    fprintf(apSal, "|          REPORTE COMPLETO DEL ANALISIS                     |\n");
    fprintf(apSal, "--------------------------------------------------------------\n\n");
    
    // 1. Cadena de Átomos
    printf("--- CADENA DE ATOMOS ---\n");
    printf("%s\n\n", cadena_atomos);
    fprintf(apSal, "--- CADENA DE ATOMOS ---\n");
    fprintf(apSal, "%s\n\n", cadena_atomos);
    
    // 2. Tokens Generados
    printf("--- TOKENS GENERADOS ---\n");
    fprintf(apSal, "--- TOKENS GENERADOS ---\n");
    imprimirTokens(ListaT_inicio, apSal);
    
    // 3. Tabla de Símbolos
    printf("\n--- TABLA DE SIMBOLOS ---\n");
    fprintf(apSal, "\n--- TABLA DE SIMBOLOS ---\n");
    if (TS_inicio != NULL) {
        imprimirLista(TS_inicio, apSal);
    } else {
        printf("(vacia)\n");
        fprintf(apSal, "(vacia)\n");
    }
    
    // 4. Tabla de Literales Numéricas (Clase 2)
    printf("\n--- TABLA DE LITERALES NUMERICAS (Enteros/Octales) ---\n");
    fprintf(apSal, "\n--- TABLA DE LITERALES NUMERICAS (Enteros/Octales) ---\n");
    if (TLN_inicio != NULL) {
        imprimirLista(TLN_inicio, apSal);
    } else {
        printf("(vacia)\n");
        fprintf(apSal, "(vacia)\n");
    }
    
    // 5. Tabla de Literales Reales (Clase 3) - NUEVA
    printf("\n--- TABLA DE LITERALES REALES (Decimales) ---\n");
    fprintf(apSal, "\n--- TABLA DE LITERALES REALES (Decimales) ---\n");
    if (TLR_inicio != NULL) {
        imprimirLista(TLR_inicio, apSal);
    } else {
        printf("(vacia)\n");
        fprintf(apSal, "(vacia)\n");
    }
    
    // 6. Tabla de Literales Cadena
    printf("\n--- TABLA DE LITERALES CADENAS ---\n");
    fprintf(apSal, "\n--- TABLA DE LITERALES CADENAS ---\n");
    if (TLC_inicio != NULL) {
        imprimirLista(TLC_inicio, apSal);
    } else {
        printf("(vacia)\n");
        fprintf(apSal, "(vacia)\n");
    }
    
    // 7. Resultado del análisis sintáctico
    printf("\n");
    printf("--------------------------------------------------------------\n");
    if (sintacticamente_correcto) {
        printf("|  PROGRAMA SINTACTICAMENTE CORRECTO  |\n");
    } else {
        printf("|  ERRORES SINTACTICOS ENCONTRADOS  |\n");
    }
    printf("----------------------------------------------------------------\n");
    
    fprintf(apSal, "\n");
    fprintf(apSal, "---------------------------------------------------------------\n");
    if (sintacticamente_correcto) {
        fprintf(apSal, "|  PROGRAMA SINTACTICAMENTE CORRECTO  |\n");
    } else {
        fprintf(apSal, "|  ERRORES SINTACTICOS ENCONTRADOS   |\n");
    }
    fprintf(apSal, "----------------------------------------------------------------\n");
}
