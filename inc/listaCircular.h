#include <stdio.h>
#ifndef LISTACIRCULAR_H
#define LISTACIRCULAR_H

typedef struct ListaC ListaC;
struct ListaC{
    char* dato; //String dinámico para poder imprimir numeros o cadenas a corde a las expresiones regulares
    struct ListaC *siguiente; //para poder apuntar al siguiente nodo
    int posicion; //índice de la tabla
    int tipo; //-1 para tabla de símbolos y también sirve para la clase del token para la lista de tokens
};

ListaC * CreadorDeNodos(int pos, char* dato, int tipo);//posición, dato y tipo, retorna un puntero al nodo creado
//void InsertarInicio(ListaC ** inicio, char* dato);
//void eliminar_nodo(ListaC *nodo_a_eliminar);
//Al usar puntero doble ** se puede modificar el inicio de la lista
void InsertarFinal(ListaC ** inicio, int pos, char* dato, int tipo);
void InsertarToken(int clase, int valor); //clase: tipo de token y valor: posicion en la tabla
//void imprimirTodo(ListaC ** inicio);
//void imprimirToken(ListaC ** inicio, int posicion);
//int BuscarNumero(ListaC **inicio, char* valor);
ListaC* BuscarNodoPorDato(ListaC *inicio, char* valor); //identificar si un string ya existe en la lista y retorna el puntero que apunta al nodo encontrado
void imprimirLista(ListaC *inicio, FILE *output_file); //se manda el primero nodo de la lista y el archivo de salida donde se va a escribir
void imprimirTokens(ListaC *inicio, FILE *output_file); //para la parte de análisis, para verificar cada token (clase, posicion)
#endif 