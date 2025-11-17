/*
 * ANALIZADORES SINTÁCTICOS - PROYECTO 2
 * VERSIÓN CORREGIDA
 * 
 * Integrantes:
 * - Persona 1: Martínez Araujo Jesús Alonso
 * - Persona 2: Emilia Macarena Arriaga Rodríguez
 * - Persona 3: Marco Gael Ventura Picazo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==================== ESTRUCTURAS COMUNES ====================

// Pila para el analizador predictivo
#define MAX_PILA 100
typedef struct {
    char elementos[MAX_PILA][1000];
    int tope;
} Pila;

void inicializar_pila(Pila *p) {
    p->tope = -1;
}

void push(Pila *p, const char *simbolo) {
    if (p->tope < MAX_PILA - 1) {
        strcpy(p->elementos[++p->tope], simbolo);
    }
}

char* pop(Pila *p) {
    if (p->tope >= 0) {
        return p->elementos[p->tope--];
    }
    return NULL;
}

char* tope_pila(Pila *p) {
    if (p->tope >= 0) {
        return p->elementos[p->tope];
    }
    return NULL;
}

int pila_vacia(Pila *p) {
    return p->tope == -1;
}

// Variables globales para análisis
char **tokens_array;    // Array de tokens (átomos)
int indice_token = 0;   // Índice actual en el array
int total_tokens = 0;   // Total de tokens
int errores_sintacticos = 0;

// ==================== TABLA PARSER ====================

typedef struct {
    char no_terminal[102];
    char terminal[200];
    int produccion;
} EntradaTabla;

EntradaTabla tabla_parser[] = {
    // Program y otraFunc
    {"<Program>", "b", 1}, {"<Program>", "g", 1}, {"<Program>", "#", 1}, {"<Program>", "y", 1}, {"<Program>", "x", 1},
    {"<otraFunc>", "b", 2}, {"<otraFunc>", "g", 2}, {"<otraFunc>", "#", 2}, {"<otraFunc>", "y", 2}, {"<otraFunc>", "x", 2},
    {"<otraFunc>", "$", 3}, 
    
    // Func y Param
    {"<Func>", "b", 4}, {"<Func>", "g", 4}, {"<Func>", "#", 4}, {"<Func>", "y", 4}, {"<Func>", "x", 4},
    {"<Param>", "b", 5}, {"<Param>", "g", 5}, {"<Param>", "#", 5}, {"<Param>", "y", 5}, {"<Param>", "x", 5},
    {"<Param>", ")", 6},
    {"<otroParam>", ",", 7}, {"<otroParam>", ")", 8},
    
    // Cuerpo y Decl
    {"<Cuerpo>", "b", 9}, {"<Cuerpo>", "g", 9}, {"<Cuerpo>", "#", 9}, {"<Cuerpo>", "y", 9}, {"<Cuerpo>", "x", 9},
    {"<Cuerpo>", "i", 9}, {"<Cuerpo>", "f", 9}, {"<Cuerpo>", "h", 9}, {"<Cuerpo>", "w", 9}, {"<Cuerpo>", "j", 9},
    {"<Cuerpo>", "[", 9}, {"<Cuerpo>", "z", 9}, {"<Cuerpo>", "c", 9}, {"<Cuerpo>", "}", 9},
    {"<Decl>", "i", 10}, {"<Decl>", "f", 10}, {"<Decl>", "h", 10}, {"<Decl>", "w", 10}, {"<Decl>", "j", 10},
    {"<Decl>", "[", 10}, {"<Decl>", "z", 10}, {"<Decl>", "c", 10}, {"<Decl>", "}", 10},
    {"<Decl>", "b", 11}, {"<Decl>", "g", 11}, {"<Decl>", "#", 11}, {"<Decl>", "y", 11}, {"<Decl>", "x", 11},
    
    // D, Tipo, K, Q, N, C
    {"D", "b", 12}, {"D", "g", 12}, {"D", "#", 12}, {"D", "y", 12}, {"D", "x", 12},
    {"<Tipo>", "b", 13}, {"<Tipo>", "g", 14}, {"<Tipo>", "#", 15}, {"<Tipo>", "y", 16}, {"<Tipo>", "x", 17},
    {"K", "i", 18},
    {"Q", ";", 19}, {"Q", "=", 20}, {"Q", ",", 21},
    {"N", "n", 22}, {"N", "r", 23}, {"N", "s", 24},
    {"C", ";", 25}, {"C", ",", 26},
    
    // A y expresiones
    {"A", "i", 27},
    {"A'", "s", 28}, {"A'", "(", 29}, {"A'", "i", 29}, {"A'", "n", 29}, {"A'", "r", 29}, {"A'", "[", 29},
    {"E", "(", 30}, {"E", "i", 30}, {"E", "n", 30}, {"E", "r", 30}, {"E", "[", 30},
    {"E'", "+", 31}, {"E'", "-", 32}, {"E'", ")", 33}, {"E'", ";", 33}, {"E'", ",", 33},
    {"T", "(", 34}, {"T", "i", 34}, {"T", "n", 34}, {"T", "r", 34}, {"T", "[", 34},
    {"T'", "*", 35}, {"T'", "/", 36}, {"T'", "\\", 37}, {"T'", "%", 38}, {"T'", "^", 39},
    {"T'", "+", 40}, {"T'", "-", 40}, {"T'", ")", 40}, {"T'", ";", 40}, {"T'", ",", 40},
    {"F", "(", 41}, {"F", "i", 42}, {"F", "n", 43}, {"F", "r", 44}, {"F", "[", 45},
    
    // Relacionales
    {"R", "i", 46}, {"R", "n", 47}, {"R", "r", 48}, {"R", "s", 49},
    {"R'", ">", 50}, {"R'", "<", 51}, {"R'", "l", 52}, {"R'", "e", 53}, {"R'", "d", 54}, {"R'", "u", 55},
    {"V", "i", 56}, {"V", "n", 57}, {"V", "r", 58}, {"V", "s", 59},
    {"V'", "n", 60}, {"V'", "i", 61},
    {"V''", "r", 62}, {"V''", "i", 63},
    {"V'''", "s", 64}, {"V'''", "i", 65},
    
    // Proposiciones
    {"P", "i", 66}, {"P", "f", 67}, {"P", "h", 68}, {"P", "w", 69}, {"P", "j", 70},
    {"P", "[", 71}, {"P", "z", 72}, {"P", "c", 73},
    {"<listaP>", "}", 74},
    {"<listaP>", "i", 75}, {"<listaP>", "f", 75}, {"<listaP>", "h", 75}, {"<listaP>", "w", 75},
    {"<listaP>", "j", 75}, {"<listaP>", "[", 75}, {"<listaP>", "z", 75}, {"<listaP>", "c", 75},
    
    // Estructuras de control
    {"W", "w", 76},
    {"I", "f", 77},
    {"I'", "t", 78}, {"I'", ":", 79},
    {"J", "j", 80},
    {"Y", "i", 81}, {"Y", ";", 82},
    {"X", "i", 83}, {"X", "n", 83}, {"X", "r", 83}, {"X", "s", 83}, {"X", ";", 84},
    {"Z", "i", 85}, {"Z", ")", 86},
    {"H", "h", 87},
    {"C'", "a", 88}, {"C'", "o", 89}, {"C'", "}", 89}, {"C'", "$", 89},
    {"O'", "o", 90}, {"O'", "}", 91},
    {"U", "q", 92}, {"U", "a", 93}, {"U", "$", 93}, {"U", "o", 93}, {"U", "}", 93},
    
    // Devuelve, Llama, arg
    {"<Devuelve>", "z", 94},
    {"<valor>", "i", 95}, {"<valor>", "n", 95}, {"<valor>", "r", 95}, {"<valor>", "s", 95}, {"<valor>", ")", 96},
    {"<Llama>", "[", 97},
    {"<arg>", ")", 98}, {"<arg>", "i", 99}, {"<arg>", "n", 99}, {"<arg>", "r", 99}, {"<arg>", "s", 99},
    {"<otroArg>", ",", 100}, {"<otroArg>", ")", 101}, {"<otroArg>", "$", 101},
    
    {"", "", -1}  // Terminador
};

int consultar_tabla(const char *no_terminal, const char *terminal) {
    for (int i = 0; tabla_parser[i].produccion != -1; i++) {
        if (strcmp(tabla_parser[i].no_terminal, no_terminal) == 0 &&
            strcmp(tabla_parser[i].terminal, terminal) == 0) {
            return tabla_parser[i].produccion;
        }
    }
    return 0;
}

const char* obtener_produccion(int num_prod) {
    switch(num_prod) {
        case 1: return "<Program> -> <Func> <otraFunc>";
        case 2: return "<otraFunc> -> <Func> <otraFunc>";
        case 3: return "<otraFunc> -> 3";
        case 4: return "<Func> -> <Tipo> i ( <Param> ) { <Cuerpo> }";
        case 5: return "<Param> -> <Tipo> i <otroParam>";
        case 6: return "<Param> -> 3";
        case 7: return "<otroParam> -> , <Tipo> i <otroParam>";
        case 8: return "<otroParam> -> 3";
        case 9: return "<Cuerpo> -> <Decl> <listaP>";
        case 10: return "<Decl> -> 3";
        case 11: return "<Decl> -> D <Decl>";
        case 12: return "D -> <Tipo> K ;";
        case 13: return "<Tipo> -> b";
        case 14: return "<Tipo> -> g";
        case 15: return "<Tipo> -> #";
        case 16: return "<Tipo> -> y";
        case 17: return "<Tipo> -> x";
        case 18: return "K -> i Q";
        case 19: return "Q -> 3";
        case 20: return "Q -> = N C";
        case 21: return "Q -> , K";
        case 22: return "N -> n";
        case 23: return "N -> r";
        case 24: return "N -> s";
        case 25: return "C -> 3";
        case 26: return "C -> , K";
        case 27: return "A -> i = A' ;";
        case 28: return "A' -> s";
        case 29: return "A' -> E";
        case 30: return "E -> T E'";
        case 31: return "E' -> + T E'";
        case 32: return "E' -> - T E'";
        case 33: return "E' -> 3";
        case 34: return "T -> F T'";
        case 35: return "T' -> * F T'";
        case 36: return "T' -> / F T'";
        case 37: return "T' -> \\ F T'";
        case 38: return "T' -> % F T'";
        case 39: return "T' -> ^ F T'";
        case 40: return "T' -> 3";
        case 41: return "F -> ( E )";
        case 42: return "F -> i";
        case 43: return "F -> n";
        case 44: return "F -> r";
        case 45: return "F -> <Llama>";
        case 46: return "R -> i R' V";
        case 47: return "R -> n R' V'";
        case 48: return "R -> r R' V''";
        case 49: return "R -> s R' V'''";
        case 50: return "R' -> >";
        case 51: return "R' -> <";
        case 52: return "R' -> l";
        case 53: return "R' -> e";
        case 54: return "R' -> d";
        case 55: return "R' -> u";
        case 56: return "V -> i";
        case 57: return "V -> n";
        case 58: return "V -> r";
        case 59: return "V -> s";
        case 60: return "V' -> n";
        case 61: return "V' -> i";
        case 62: return "V'' -> r";
        case 63: return "V'' -> i";
        case 64: return "V''' -> s";
        case 65: return "V''' -> i";
        case 66: return "P -> A";
        case 67: return "P -> I";
        case 68: return "P -> H";
        case 69: return "P -> W";
        case 70: return "P -> J";
        case 71: return "P -> <Llama>";
        case 72: return "P -> <Devuelve>";
        case 73: return "P -> c ;";
        case 74: return "<listaP> -> 3";
        case 75: return "<listaP> -> P <listaP>";
        case 76: return "W -> w ( R ) m { <listaP> }";
        case 77: return "I -> f ( R ) <listaP> I' :";
        case 78: return "I' -> t <listaP>";
        case 79: return "I' -> 3";
        case 80: return "J -> j ( Y X Z { <listaP> }";
        case 81: return "Y -> i = E ;";
        case 82: return "Y -> ;";
        case 83: return "X -> R ;";
        case 84: return "X -> ;";
        case 85: return "Z -> i = E )";
        case 86: return "Z -> )";
        case 87: return "H -> h ( i ) { C' O' }";
        case 88: return "C' -> a n : <listaP> U C'";
        case 89: return "C' -> 3";
        case 90: return "O' -> o : <listaP>";
        case 91: return "O' -> 3";
        case 92: return "U -> q";
        case 93: return "U -> 3";
        case 94: return "<Devuelve> -> z ( <valor> ) ;";
        case 95: return "<valor> -> V";
        case 96: return "<valor> -> 3";
        case 97: return "<Llama> -> [ i ( <arg> ) ]";
        case 98: return "<arg> -> 3";
        case 99: return "<arg> -> V <otroArg>";
        case 100: return "<otroArg> -> , V <otroArg>";
        case 101: return "<otroArg> -> 3";
        default: return "ERROR";
    }
}

// ==================== ANALIZADOR PREDICTIVO ====================

void aplicar_produccion(Pila *pila, int num_prod) {
    printf("  Aplicando: %s\n", obtener_produccion(num_prod));
    
    switch(num_prod) {
        case 1: push(pila, "<otraFunc>"); push(pila, "<Func>"); break;
        case 2: push(pila, "<otraFunc>"); push(pila, "<Func>"); break;
        case 3: break;
        case 4: push(pila, "}"); push(pila, "<Cuerpo>"); push(pila, "{"); push(pila, ")"); 
                push(pila, "<Param>"); push(pila, "("); push(pila, "i"); push(pila, "<Tipo>"); break;
        case 5: push(pila, "<otroParam>"); push(pila, "i"); push(pila, "<Tipo>"); break;
        case 6: break;
        case 7: push(pila, "<otroParam>"); push(pila, "i"); push(pila, "<Tipo>"); push(pila, ","); break;
        case 8: break;
        case 9: push(pila, "<listaP>"); push(pila, "<Decl>"); break;
        case 10: break;
        case 11: push(pila, "<Decl>"); push(pila, "D"); break;
        case 12: push(pila, ";"); push(pila, "K"); push(pila, "<Tipo>"); break;
        case 13: push(pila, "b"); break;
        case 14: push(pila, "g"); break;
        case 15: push(pila, "#"); break;
        case 16: push(pila, "y"); break;
        case 17: push(pila, "x"); break;
        case 18: push(pila, "Q"); push(pila, "i"); break;
        case 19: break;
        case 20: push(pila, "C"); push(pila, "N"); push(pila, "="); break;
        case 21: push(pila, "K"); push(pila, ","); break;
        case 22: push(pila, "n"); break;
        case 23: push(pila, "r"); break;
        case 24: push(pila, "s"); break;
        case 25: break;
        case 26: push(pila, "K"); push(pila, ","); break;
        case 27: push(pila, ";"); push(pila, "A'"); push(pila, "="); push(pila, "i"); break;
        case 28: push(pila, "s"); break;
        case 29: push(pila, "E"); break;
        case 30: push(pila, "E'"); push(pila, "T"); break;
        case 31: push(pila, "E'"); push(pila, "T"); push(pila, "+"); break;
        case 32: push(pila, "E'"); push(pila, "T"); push(pila, "-"); break;
        case 33: break;
        case 34: push(pila, "T'"); push(pila, "F"); break;
        case 35: push(pila, "T'"); push(pila, "F"); push(pila, "*"); break;
        case 36: push(pila, "T'"); push(pila, "F"); push(pila, "/"); break;
        case 37: push(pila, "T'"); push(pila, "F"); push(pila, "\\"); break;
        case 38: push(pila, "T'"); push(pila, "F"); push(pila, "\%"); break;
        case 39: push(pila, "T'"); push(pila, "F"); push(pila, "^"); break;
        case 40: break;
        case 41: push(pila, ")"); push(pila, "E"); push(pila, "("); break;
        case 42: push(pila, "i"); break;
        case 43: push(pila, "n"); break;
        case 44: push(pila, "r"); break;
        case 45: push(pila, "<Llama>"); break;
        case 46: push(pila, "V"); push(pila, "R'"); push(pila, "i"); break;
        case 47: push(pila, "V'"); push(pila, "R'"); push(pila, "n"); break;
        case 48: push(pila, "V''"); push(pila, "R'"); push(pila, "r"); break;
        case 49: push(pila, "V'''"); push(pila, "R'"); push(pila, "s"); break;
        case 50: push(pila, ">"); break;
        case 51: push(pila, "<"); break;
        case 52: push(pila, "l"); break;
        case 53: push(pila, "e"); break;
        case 54: push(pila, "d"); break;
        case 55: push(pila, "u"); break;
        case 56: push(pila, "i"); break;
        case 57: push(pila, "n"); break;
        case 58: push(pila, "r"); break;
        case 59: push(pila, "s"); break;
        case 60: push(pila, "n"); break;
        case 61: push(pila, "i"); break;
        case 62: push(pila, "r"); break;
        case 63: push(pila, "i"); break;
        case 64: push(pila, "s"); break;
        case 65: push(pila, "i"); break;
        case 66: push(pila, "A"); break;
        case 67: push(pila, "I"); break;
        case 68: push(pila, "H"); break;
        case 69: push(pila, "W"); break;
        case 70: push(pila, "J"); break;
        case 71: push(pila, "<Llama>"); break;
        case 72: push(pila, "<Devuelve>"); break;
        case 73: push(pila, ";"); push(pila, "c"); break;
        case 74: break;
        case 75: push(pila, "<listaP>"); push(pila, "P"); break;
        case 76: push(pila, "}"); push(pila, "<listaP>"); push(pila, "{"); push(pila, "m"); 
                 push(pila, ")"); push(pila, "R"); push(pila, "("); push(pila, "w"); break;
        case 77: push(pila, ":"); push(pila, "I'"); push(pila, "<listaP>"); push(pila, ")"); 
                 push(pila, "R"); push(pila, "("); push(pila, "f"); break;
        case 78: push(pila, "<listaP>"); push(pila, "t"); break;
        case 79: break;
        case 80: push(pila, "}"); push(pila, "<listaP>"); push(pila, "{"); push(pila, "Z"); 
                 push(pila, "X"); push(pila, "Y"); push(pila, "("); push(pila, "j"); break;
        case 81: push(pila, ";"); push(pila, "E"); push(pila, "="); push(pila, "i"); break;
        case 82: push(pila, ";"); break;
        case 83: push(pila, ";"); push(pila, "R"); break;
        case 84: push(pila, ";"); break;
        case 85: push(pila, ")"); push(pila, "E"); push(pila, "="); push(pila, "i"); break;
        case 86: push(pila, ")"); break;
        case 87: push(pila, "}"); push(pila, "O'"); push(pila, "C'"); push(pila, "{"); 
                 push(pila, ")"); push(pila, "i"); push(pila, "("); push(pila, "h"); break;
        case 88: push(pila, "C'"); push(pila, "U"); push(pila, "<listaP>"); push(pila, ":"); 
                 push(pila, "n"); push(pila, "a"); break;
        case 89: break;
        case 90: push(pila, "<listaP>"); push(pila, ":"); push(pila, "o"); break;
        case 91: break;
        case 92: push(pila, "q"); break;
        case 93: break;
        case 94: push(pila, ";"); push(pila, ")"); push(pila, "<valor>"); push(pila, "("); push(pila, "z"); break;
        case 95: push(pila, "V"); break;
        case 96: break;
        case 97: push(pila, "]"); push(pila, ")"); push(pila, "<arg>"); push(pila, "("); 
                 push(pila, "i"); push(pila, "["); break;
        case 98: break;
        case 99: push(pila, "<otroArg>"); push(pila, "V"); break;
        case 100: push(pila, "<otroArg>"); push(pila, "V"); push(pila, ","); break;
        case 101: break;
        default: break;
    }
}

int es_no_terminal(const char *simbolo) {
    return (strcmp(simbolo, "<Program>") == 0 || strcmp(simbolo, "<otraFunc>") == 0 ||
            strcmp(simbolo, "<Func>") == 0 || strcmp(simbolo, "<Param>") == 0 ||
            strcmp(simbolo, "<otroParam>") == 0 || strcmp(simbolo, "<Cuerpo>") == 0 ||
            strcmp(simbolo, "<Decl>") == 0 || strcmp(simbolo, "D") == 0 ||
            strcmp(simbolo, "<Tipo>") == 0 || strcmp(simbolo, "K") == 0 ||
            strcmp(simbolo, "Q") == 0 || strcmp(simbolo, "N") == 0 ||
            strcmp(simbolo, "C") == 0 || strcmp(simbolo, "A") == 0 ||
            strcmp(simbolo, "A'") == 0 || strcmp(simbolo, "E") == 0 ||
            strcmp(simbolo, "E'") == 0 || strcmp(simbolo, "T") == 0 ||
            strcmp(simbolo, "T'") == 0 || strcmp(simbolo, "F") == 0 ||
            strcmp(simbolo, "R") == 0 || strcmp(simbolo, "R'") == 0 ||
            strcmp(simbolo, "V") == 0 || strcmp(simbolo, "V'") == 0 ||
            strcmp(simbolo, "V''") == 0 || strcmp(simbolo, "V'''") == 0 ||
            strcmp(simbolo, "P") == 0 || strcmp(simbolo, "<listaP>") == 0 ||
            strcmp(simbolo, "W") == 0 || strcmp(simbolo, "I") == 0 ||
            strcmp(simbolo, "I'") == 0 || strcmp(simbolo, "J") == 0 ||
            strcmp(simbolo, "Y") == 0 || strcmp(simbolo, "X") == 0 ||
            strcmp(simbolo, "Z") == 0 || strcmp(simbolo, "H") == 0 ||
            strcmp(simbolo, "C'") == 0 || strcmp(simbolo, "O'") == 0 ||
            strcmp(simbolo, "U") == 0 || strcmp(simbolo, "<Devuelve>") == 0 ||
            strcmp(simbolo, "<valor>") == 0 || strcmp(simbolo, "<Llama>") == 0 ||
            strcmp(simbolo, "<arg>") == 0 || strcmp(simbolo, "<otroArg>") == 0);
}

int analisis_predictivo(const char *cadena_atomos) {
    printf("\n=== ANALIZADOR PREDICTIVO ===\n");
    printf("Entrada: %s\n\n", cadena_atomos);
    
    char *copia = strdup(cadena_atomos);
    char *token = strtok(copia, " ");
    int capacidad = 50;
    tokens_array = malloc(capacidad * sizeof(char*));
    total_tokens = 0;
    
    while (token != NULL) {
        tokens_array[total_tokens] = strdup(token);
        total_tokens++;
        token = strtok(NULL, " ");
    }
    tokens_array[total_tokens] = strdup("$");
    total_tokens++;
    free(copia);
    
    Pila pila;
    inicializar_pila(&pila);
    push(&pila, "$");
    push(&pila, "<Program>");
    
    indice_token = 0;
    errores_sintacticos = 0;
    int paso = 1;
    
    while (!pila_vacia(&pila)) {
        char *X = tope_pila(&pila);
        char *a = tokens_array[indice_token];
        
        printf("Paso %d: Pila: %s | Entrada: %s\n", paso++, X, a);
        
        if (strcmp(X, "$") == 0 && strcmp(a, "$") == 0) {
            printf("  ACEPTADO\n\n");
            pop(&pila);
            break;
        }
        
        if (!es_no_terminal(X)) {
            if (strcmp(X, a) == 0) {
                printf("  Match: %s\n", X);
                pop(&pila);
                indice_token++;
            } else {
                printf("  ERROR: Se esperaba '%s' pero se encontró '%s'\n", X, a);
                errores_sintacticos++;
                return 0;
            }
        } else {
            int produccion = consultar_tabla(X, a);
            if (produccion > 0) {
                pop(&pila);
                aplicar_produccion(&pila, produccion);
            } else {
                printf("  ERROR SINTÁCTICO: No hay producción para [%s, %s]\n", X, a);
                errores_sintacticos++;
                return 0;
            }
        }
        printf("\n");
    }
    
    for (int i = 0; i < total_tokens; i++) {
        free(tokens_array[i]);
    }
    free(tokens_array);
    
    return (errores_sintacticos == 0);
}

// ==================== ANALIZADOR RECURSIVO DESCENDENTE ====================

char *token_actual;

void avanzar() {
    if (indice_token < total_tokens - 1) {
        indice_token++;
        token_actual = tokens_array[indice_token];
    }
}

int match(const char *esperado) {
    printf("  Match esperado: '%s' | actual: '%s'\n", esperado, token_actual);
    if (strcmp(token_actual, esperado) == 0) {
        avanzar();
        return 1;
    }
    printf("  ERROR: Se esperaba '%s' pero se encontró '%s'\n", esperado, token_actual);
    errores_sintacticos++;
    return 0;
}

// Declaraciones adelantadas
void Program(); void otraFunc(); void Func(); void Param(); void otroParam();
void Cuerpo(); void Decl(); void D(); void Tipo(); void K(); void Q();
void N(); void C(); void A(); void AP(); void E(); void EP(); void T();
void TP(); void F(); void R(); void RP(); void V(); void VP(); void VPP();
void VPPP(); void P(); void listaP(); void W(); void I(); void IP();
void J(); void Y(); void X(); void Z(); void H(); void CP(); void OP();
void U(); void Devuelve(); void valor(); void Llama(); void arg(); void otroArg();

// ==================== IMPLEMENTACIÓN DE FUNCIONES ====================

void Program() {
    printf("Entrando a <Program>\n");
    Func();
    otraFunc();
}

void otraFunc() {
    printf("Entrando a <otraFunc>\n");
    // FIRST(otraFunc) = {'b', 'g', '#', 'y', 'x'}
    // FOLLOW(otraFunc) = {'}
    if (strcmp(token_actual, "b") == 0 || strcmp(token_actual, "g") == 0 ||
        strcmp(token_actual, "#") == 0 || strcmp(token_actual, "y") == 0 ||
        strcmp(token_actual, "x") == 0) {
        Func();
        otraFunc();
    }
    // Si es ', producción 3 (no hacer nada)
}

void Func() {
    printf("Entrando a <Func>\n");
    Tipo();
    if (strcmp(token_actual, "i") == 0) {
        match("i");
        match("(");
        Param();
        match(")");
        match("{");
        Cuerpo();
        match("}");
    } else {
        printf(" ERROR: Se esperaba 'i'\n");
        errores_sintacticos++;
    }
}

void Param() {
    printf("Entrando a <Param>\n");
    // FIRST(Param) = {'b', 'g', '#', 'y', 'x'}
    // FOLLOW(Param) = {')'}
    if (strcmp(token_actual, "b") == 0 || strcmp(token_actual, "g") == 0 ||
        strcmp(token_actual, "#") == 0 || strcmp(token_actual, "y") == 0 ||
        strcmp(token_actual, "x") == 0) {
        Tipo();
        if (strcmp(token_actual, "i") == 0) {
            match("i");
            otroParam();
        } else {
            printf(" ERROR: Se esperaba 'i'\n");
            errores_sintacticos++;
        }
    }
    // Si es ')', producción 3
}

void otroParam() {
    printf("Entrando a <otroParam>\n");
    if (strcmp(token_actual, ",") == 0) {
        match(",");
        Tipo();
        if (strcmp(token_actual, "i") == 0) {
            match("i");
            otroParam();
        } else {
            printf("  ERROR: Se esperaba 'i'\n");
            errores_sintacticos++;
        }
    }
    // Si es ')', producción 3
}

void Cuerpo() {
    printf("Entrando a <Cuerpo>\n");
    Decl();
    listaP();
}

void Decl() {
    printf("Entrando a <Decl>\n");
    // FIRST(D) = {'b', 'g', '#', 'y', 'x'}
    if (strcmp(token_actual, "b") == 0 || strcmp(token_actual, "g") == 0 ||
        strcmp(token_actual, "#") == 0 || strcmp(token_actual, "y") == 0 ||
        strcmp(token_actual, "x") == 0) {
        D();
        Decl();
    }
    // Si está en FOLLOW(Decl), producción 3
}

void D() {
    printf("Entrando a D\n");
    Tipo();
    K();
    if (strcmp(token_actual, ";") == 0) {
        match(";");
    } else {
        printf(" ERROR: Se esperaba ';'\n");
        errores_sintacticos++;
    }
}

void Tipo() {
    printf("Entrando a <Tipo>\n");
    if (strcmp(token_actual, "b") == 0) {
        match("b");
    } else if (strcmp(token_actual, "g") == 0) {
        match("g");
    } else if (strcmp(token_actual, "#") == 0) {
        match("#");
    } else if (strcmp(token_actual, "y") == 0) {
        match("y");
    } else if (strcmp(token_actual, "x") == 0) {
        match("x");
    } else {
        printf("  ✗ ERROR: Se esperaba 'b', 'g', '#', 'y' o 'x'\n");
        errores_sintacticos++;
    }
}

void K() {
    printf("Entrando a K\n");
    if (strcmp(token_actual, "i") == 0) {
        match("i");
        Q();
    } else {
        printf("  ERROR: Se esperaba 'i'\n");
        errores_sintacticos++;
    }
}

void Q() {
    printf("Entrando a Q\n");
    if (strcmp(token_actual, "=") == 0) {
        match("=");
        N();
        C();
    } else if (strcmp(token_actual, ",") == 0) {
        match(",");
        K();
    }
    // Si es ';', producción 3
}

void N() {
    printf("Entrando a N\n");
    if (strcmp(token_actual, "n") == 0) {
        match("n");
    } else if (strcmp(token_actual, "r") == 0) {
        match("r");
    } else if (strcmp(token_actual, "s") == 0) {
        match("s");
    } else {
        printf(" ERROR: Se esperaba 'n', 'r' o 's'\n");
        errores_sintacticos++;
    }
}

void C() {
    printf("Entrando a C\n");
    if (strcmp(token_actual, ",") == 0) {
        match(",");
        K();
    }
    // Si es ';', producción 3
}

void A() {
    printf("Entrando a A\n");
    if (strcmp(token_actual, "i") == 0) {
        match("i");
        match("=");
        AP();
        match(";");
    } else {
        printf(" ERROR: Se esperaba 'i'\n");
        errores_sintacticos++;
    }
}

void AP() {
    printf("Entrando a A'\n");
    if (strcmp(token_actual, "s") == 0) {
        match("s");
    } else if (strcmp(token_actual, "(") == 0 || strcmp(token_actual, "i") == 0 ||
               strcmp(token_actual, "n") == 0 || strcmp(token_actual, "r") == 0 ||
               strcmp(token_actual, "[") == 0) {
        E();
    } else {
        printf(" ERROR: Se esperaba 's' o una expresión\n");
        errores_sintacticos++;
    }
}

void E() {
    printf("Entrando a E\n");
    T();
    EP();
}

void EP() {
    printf("Entrando a E'\n");
    if (strcmp(token_actual, "+") == 0) {
        match("+");
        T();
        EP();
    } else if (strcmp(token_actual, "-") == 0) {
        match("-");
        T();
        EP();
    }
    // Si está en FOLLOW(E'), producción 3
}

void T() {
    printf("Entrando a T\n");
    F();
    TP();
}

void TP() {
    printf("Entrando a T'\n");
    if (strcmp(token_actual, "*") == 0) {
        match("*");
        F();
        TP();
    } else if (strcmp(token_actual, "/") == 0) {
        match("/");
        F();
        TP();
    } else if (strcmp(token_actual, "\\") == 0) {
        match("\\");
        F();
        TP();
    } else if (strcmp(token_actual, "%") == 0) {
        match("%");
        F();
        TP();
    } else if (strcmp(token_actual, "^") == 0) {
        match("^");
        F();
        TP();
    }
    // Si está en FOLLOW(T'), producción 3
}

void F() {
    printf("Entrando a F\n");
    if (strcmp(token_actual, "(") == 0) {
        match("(");
        E();
        match(")");
    } else if (strcmp(token_actual, "i") == 0) {
        match("i");
    } else if (strcmp(token_actual, "n") == 0) {
        match("n");
    } else if (strcmp(token_actual, "r") == 0) {
        match("r");
    } else if (strcmp(token_actual, "[") == 0) {
        Llama();
    } else {
        printf("  ✗ ERROR: Se esperaba '(', 'i', 'n', 'r' o '['\n");
        errores_sintacticos++;
    }
}

void R() {
    printf("Entrando a R\n");
    if (strcmp(token_actual, "i") == 0) {
        match("i");
        RP();
        V();
    } else if (strcmp(token_actual, "n") == 0) {
        match("n");
        RP();
        VP();
    } else if (strcmp(token_actual, "r") == 0) {
        match("r");
        RP();
        VPP();
    } else if (strcmp(token_actual, "s") == 0) {
        match("s");
        RP();
        VPPP();
    } else {
        printf("  ✗ ERROR: Se esperaba 'i', 'n', 'r' o 's'\n");
        errores_sintacticos++;
    }
}

void RP() {
    printf("Entrando a R'\n");
    if (strcmp(token_actual, ">") == 0) {
        match(">");
    } else if (strcmp(token_actual, "<") == 0) {
        match("<");
    } else if (strcmp(token_actual, "l") == 0) {
        match("l");
    } else if (strcmp(token_actual, "e") == 0) {
        match("e");
    } else if (strcmp(token_actual, "d") == 0) {
        match("d");
    } else if (strcmp(token_actual, "u") == 0) {
        match("u");
    } else {
        printf("  ✗ ERROR: Se esperaba operador relacional\n");
        errores_sintacticos++;
    }
}

void V() {
    printf("Entrando a V\n");
    if (strcmp(token_actual, "i") == 0) {
        match("i");
    } else if (strcmp(token_actual, "n") == 0) {
        match("n");
    } else if (strcmp(token_actual, "r") == 0) {
        match("r");
    } else if (strcmp(token_actual, "s") == 0) {
        match("s");
    } else {
        printf("  ✗ ERROR: Se esperaba 'i', 'n', 'r' o 's'\n");
        errores_sintacticos++;
    }
}

void VP() {
    printf("Entrando a V'\n");
    if (strcmp(token_actual, "n") == 0) {
        match("n");
    } else if (strcmp(token_actual, "i") == 0) {
        match("i");
    } else {
        printf("  ✗ ERROR: Se esperaba 'n' o 'i'\n");
        errores_sintacticos++;
    }
}

void VPP() {
    printf("Entrando a V''\n");
    if (strcmp(token_actual, "r") == 0) {
        match("r");
    } else if (strcmp(token_actual, "i") == 0) {
        match("i");
    } else {
        printf("  ✗ ERROR: Se esperaba 'r' o 'i'\n");
        errores_sintacticos++;
    }
}

void VPPP() {
    printf("Entrando a V'''\n");
    if (strcmp(token_actual, "s") == 0) {
        match("s");
    } else if (strcmp(token_actual, "i") == 0) {
        match("i");
    } else {
        printf("  ✗ ERROR: Se esperaba 's' o 'i'\n");
        errores_sintacticos++;
    }
}

void P() {
    printf("Entrando a P\n");
    if (strcmp(token_actual, "i") == 0) {
        A();
    } else if (strcmp(token_actual, "f") == 0) {
        I();
    } else if (strcmp(token_actual, "h") == 0) {
        H();
    } else if (strcmp(token_actual, "w") == 0) {
        W();
    } else if (strcmp(token_actual, "j") == 0) {
        J();
    } else if (strcmp(token_actual, "[") == 0) {
        Llama();
    } else if (strcmp(token_actual, "z") == 0) {
        Devuelve();
    } else if (strcmp(token_actual, "c") == 0) {
        match("c");
        match(";");
    } else {
        printf("  ✗ ERROR: Se esperaba una proposición válida\n");
        errores_sintacticos++;
    }
}

void listaP() {
    printf("Entrando a <listaP>\n");
    // FIRST(P) = {'i', 'f', 'h', 'w', 'j', '[', 'z', 'c'}
    if (strcmp(token_actual, "i") == 0 || strcmp(token_actual, "f") == 0 ||
        strcmp(token_actual, "h") == 0 || strcmp(token_actual, "w") == 0 ||
        strcmp(token_actual, "j") == 0 || strcmp(token_actual, "[") == 0 ||
        strcmp(token_actual, "z") == 0 || strcmp(token_actual, "c") == 0) {
        P();
        listaP();
    }
    // Si es '}', producción 3
}

void W() {
    printf("Entrando a W\n");
    if (strcmp(token_actual, "w") == 0) {
        match("w");
        match("(");
        R();
        match(")");
        match("m");
        match("{");
        listaP();
        match("}");
    } else {
        printf("  ✗ ERROR: Se esperaba 'w'\n");
        errores_sintacticos++;
    }
}

void I() {
    printf("Entrando a I\n");
    if (strcmp(token_actual, "f") == 0) {
        match("f");
        match("(");
        R();
        match(")");
        match("{");
        listaP();
        match("}");
        IP();
        match(":");
    } else {
        printf("  ✗ ERROR: Se esperaba 'f'\n");
        errores_sintacticos++;
    }
}

void IP() {
    printf("Entrando a I'\n");
    if (strcmp(token_actual, "t") == 0) {
        match("t");
        match("{");
        listaP();
        match("}");
    }
    // Si es ':', producción 3
}

void J() {
    printf("Entrando a J\n");
    if (strcmp(token_actual, "j") == 0) {
        match("j");
        match("(");
        Y();
        X();
        Z();
        match("{");
        listaP();
        match("}");
    } else {
        printf("  ✗ ERROR: Se esperaba 'j'\n");
        errores_sintacticos++;
    }
}

void Y() {
    printf("Entrando a Y\n");
    if (strcmp(token_actual, "i") == 0) {
        match("i");
        match("=");
        E();
        match(";");
    } else if (strcmp(token_actual, ";") == 0) {
        match(";");
    } else {
        printf("  ✗ ERROR: Se esperaba 'i' o ';'\n");
        errores_sintacticos++;
    }
}

void X() {
    printf("Entrando a X\n");
    // FIRST(R) = {'i', 'n', 'r', 's'}
    if (strcmp(token_actual, "i") == 0 || strcmp(token_actual, "n") == 0 ||
        strcmp(token_actual, "r") == 0 || strcmp(token_actual, "s") == 0) {
        // P83: X → R ;
        R();
        if (strcmp(token_actual, ";") == 0) {
            match(";");
        } else {
            printf("  ✗ ERROR: Se esperaba ';' después de expresión\n");
            errores_sintacticos++;
        }
    } else if (strcmp(token_actual, ";") == 0) {
        // P84: X → ;
        match(";");
    } else {
        printf("  ✗ ERROR: Se esperaba 'i', 'n', 'r', 's' o ';'\n");
        errores_sintacticos++;
    }
}

void Z() {
    printf("Entrando a Z\n");
    if (strcmp(token_actual, "i") == 0) {
        match("i");
        match("=");
        E();
        match(")");
    } else if (strcmp(token_actual, ")") == 0) {
        match(")");
    } else {
        printf("  ✗ ERROR: Se esperaba 'i' o ')'\n");
        errores_sintacticos++;
    }
}

void H() {
    printf("Entrando a H\n");
    if (strcmp(token_actual, "h") == 0) {
        match("h");
        match("(");
        match("i");
        match(")");
        match("{");
        CP();
        OP();
        match("}");
    } else {
        printf("  ERROR: Se esperaba 'h'\n");
        errores_sintacticos++;
    }
}

void CP() {
    printf("Entrando a C'\n");
    if (strcmp(token_actual, "a") == 0) {
        match("a");
        match("n");
        match(":");
        listaP();
        U();
        CP();
    }
    // Si es 'o' o '}', producción 3
}

void OP() {
    printf("Entrando a O'\n");
    if (strcmp(token_actual, "o") == 0) {
        match("o");
        match(":");
        listaP();
    }
    // Si es '}', producción 3
}

void U() {
    printf("Entrando a U\n");
    if (strcmp(token_actual, "q") == 0) {
        match("q");
    }
    // Si está en FOLLOW(U), producción 3
}

void Devuelve() {
    printf("Entrando a <Devuelve>\n");
    if (strcmp(token_actual, "z") == 0) {
        match("z");
        match("(");
        valor();
        match(")");
        match(";");
    } else {
        printf("  ✗ ERROR: Se esperaba 'z'\n");
        errores_sintacticos++;
    }
}

void valor() {
    printf("Entrando a <valor>\n");
    if (strcmp(token_actual, "i") == 0 || strcmp(token_actual, "n") == 0 ||
        strcmp(token_actual, "r") == 0 || strcmp(token_actual, "s") == 0) {
        V();
    }
    // Si es ')', producción 3
}

void Llama() {
    printf("Entrando a <Llama>\n");
    if (strcmp(token_actual, "[") == 0) {
        match("[");
        match("i");
        match("(");
        arg();
        match(")");
        match("]");
    } else {
        printf("  ✗ ERROR: Se esperaba '['\n");
        errores_sintacticos++;
    }
}

void arg() {
    printf("Entrando a <arg>\n");
    if (strcmp(token_actual, "i") == 0 || strcmp(token_actual, "n") == 0 ||
        strcmp(token_actual, "r") == 0 || strcmp(token_actual, "s") == 0) {
        V();
        otroArg();
    }
    // Si es ')', producción 3
}

void otroArg() {
    printf("Entrando a <otroArg>\n");
    if (strcmp(token_actual, ",") == 0) {
        match(",");
        V();
        otroArg();
    }
    // Si es ')', producción 3
}

int analisis_recursivo(const char *cadena_atomos) {
    printf("\n=== ANALIZADOR RECURSIVO DESCENDENTE ===\n");
    printf("Entrada: %s\n\n", cadena_atomos);
    
    char *copia = strdup(cadena_atomos);
    char *token = strtok(copia, " ");
    int capacidad = 103;
    tokens_array = malloc(capacidad * sizeof(char*));
    total_tokens = 0;
    
    while (token != NULL) {
        tokens_array[total_tokens] = strdup(token);
        total_tokens++;
        token = strtok(NULL, " ");
    }
    tokens_array[total_tokens] = strdup("$");
    total_tokens++;
    free(copia);
    
    indice_token = 0;
    token_actual = tokens_array[0];
    errores_sintacticos = 0;
    
    Program();
    
    if (strcmp(token_actual, "$") == 0 && errores_sintacticos == 0) {
        printf("\n✓ ANÁLISIS EXITOSO\n");
    } else {
        printf("\n✗ ANÁLISIS CON ERRORES\n");
    }
    
    for (int i = 0; i < total_tokens; i++) {
        free(tokens_array[i]);
    }
    free(tokens_array);
    
    return (errores_sintacticos == 0);
}

// ==================== MAIN DE PRUEBA ====================

int main() {
    const char *ejemplos[] = {
        "b i ( ) { }",  // Función vacía
        "n + n * n",    // Expresión
        "/ / b i = n , i ; f i ; y i = s , i ; / / i = i + n * ( n - n ) ; i = s ; i = r ; / / r * n ( r + i ) / [ i ( n ) ] / / n l i i e s r > i / / w w ( i > n ) m { < > } / / f f ( < > ) < > t < > f ( < > ) < > / / j j ( i = n ; i < n ; i = i + n ) { < > } / / h h ( i ) { n : < > q n : < > o : < > } / / z z ( i ) ; z ( s ) ; z ( ) ; / / [ i ( i , n ) ] / / # i ( f i , # i ) { < > }",
        NULL
    };
    
    for (int i = 0; ejemplos[i] != NULL; i++) {
        printf("\n");
        printf("-------------------------------------------------------\n");
        printf("EJEMPLO %d: %s\n", i+1, ejemplos[i]);
        printf("-------------------------------------------------------\n");
        
        int resultado1 = analisis_predictivo(ejemplos[i]);
        printf("\n");
        int resultado2 = analisis_recursivo(ejemplos[i]);
        
        printf("\nResultados: Predictivo=%s | Recursivo=%s\n", 
               resultado1 ? "Bien" : "Mal",
               resultado2 ? "Bien" : "Mal");
    }
    
    return 0;
}