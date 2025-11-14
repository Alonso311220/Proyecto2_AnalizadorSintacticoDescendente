#include "../inc/listaCircular.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ListaC* CreadorDeNodos(int pos, char* dato, int tipo){
    // 1. Asignación de memoria para el nodo principal
    ListaC* nodo = malloc(sizeof(ListaC));
    if (nodo == NULL) {
        perror("Fallo al asignar memoria para el nodo");
        exit(EXIT_FAILURE);
    }
    // 2. Asignar los campos numéricos
    nodo->posicion = pos;
    nodo->tipo = tipo;
    
    // 3. Asignar memoria para el STRING (solo si 'dato' no es NULL)
    if (dato != NULL) {
        // Asignando memoria para la cadena +1 para el fin de cadena
        nodo->dato = malloc(strlen(dato) + 1); 
        
        if (nodo->dato == NULL) { 
            perror("Fallo al asignar memoria para el string");
            free(nodo);
            exit(EXIT_FAILURE); 
        }
        
        // Aquí ya se copia el string de la entrada a la memoria que se le asigno
        strcpy(nodo->dato, dato);
    } else {
        nodo->dato = NULL; // Deja el puntero NULL si no hay string (ej. para tokens simples)
    }

    nodo->siguiente = NULL; // Por defecto, es un nodo nuevo y no está en la lista
    return nodo;
}

// void InsertarInicio(ListaC ** inicio, char* dato){
//     tamanio++;
//     ListaC* nodo = CreadorDeNodos(dato);
//     if(*inicio == NULL){
//         *inicio = nodo;
//         (*inicio)->siguiente = (*inicio);
//     } else{
//         ListaC *aux = *inicio;
//         while(aux->siguiente != (*inicio)){
//             aux = aux->siguiente;
//         }
//         aux->siguiente = nodo; //El último nodo apunta al nuevo nodo
//         nodo->siguiente = (*inicio); //El nuevo nodo apunta al inicio 
//         *inicio = nodo; //El nuevo nodo se convierte en el inicio (ahora es la cabeza)
//         nodo->posicion = tamanio;
//     }
// }

// listaCircular.c

// ELIMINAR O COMENTAR: void InsertarInicio(ListaC ** inicio, char* dato){...}

void InsertarFinal(ListaC ** inicio, int pos, char* dato, int tipo){
    // Nota: Eliminé la variable global 'tamanio' ya que no es necesaria para la inserción
    
    // Usar el nuevo CreadorDeNodos con todos los parámetros
    ListaC *nodo = CreadorDeNodos(pos, dato, tipo);
    ListaC *aux;
    
    if(*inicio == NULL){
        // Caso 1: Lista vacía. El nodo se apunta a sí mismo.
        *inicio = nodo;
        (*inicio)->siguiente = (*inicio);
    }
    else{
        // Caso 2: Buscar el último nodo (el que apunta a *inicio)
        aux = (*inicio);
        while(aux->siguiente != *inicio){
            aux = aux->siguiente;
        }
        // aux es ahora el último nodo
        
        aux->siguiente = nodo; // El último nodo (aux) apunta al nuevo nodo
        nodo->siguiente = (*inicio); // El nuevo nodo apunta al inicio (cierra el círculo)        
    }
}

// void InsertarFinal(ListaC **inicio, char* dato){
//     tamanio++;
//     ListaC *nodo = CreadorDeNodos(dato);
//     ListaC *aux;
//     if(*inicio == NULL){
//         *inicio = nodo;
//         (*inicio)->siguiente = (*inicio);
//     }
//     else{
//         aux = (*inicio);
//         while(aux->siguiente != *inicio){
//             aux = aux->siguiente;
//         }
//         aux->siguiente = nodo; //El último nodo (aux) apunta al nuevo nodo        
//         nodo->siguiente = (*inicio);
//         nodo->posicion = tamanio;
//     }
// }
// listaCircular.c

ListaC* BuscarNodoPorDato(ListaC *inicio, char* valor){
    if(inicio == NULL || valor == NULL){
        return NULL; // Lista vacía o valor de búsqueda inválido
    }
    
    ListaC *aux = inicio;
    
    // Recorre la lista una vez
    do {
        // Verifica si el campo dato es NULL (ej. si es un token simple) o si coincide
        //cuando son iguales las cadenas strcmp es igual a 0
        if (aux->dato != NULL && strcmp(aux->dato, valor) == 0) {
            return aux; // Nodo encontrado
        }
        aux = aux->siguiente;
    } while (aux != inicio); // El bucle se detiene después de visitar el inicio una vez

    return NULL; // No se encontró coincidencia
}

void imprimirLista(ListaC *inicio, FILE *output_file){ 
    if(inicio == NULL){
        printf("La lista está vacía.\n");
        if (output_file) fprintf(output_file, "La lista está vacía.\n");
        return;
    }

    ListaC *corredor = inicio;
    // Recorre la lista completa
    do {
        // Formato para TS/TLN/TLC
        printf("Pos: %d, Dato: %s, Tipo: %d\n", 
               corredor->posicion, 
               (corredor->dato != NULL ? corredor->dato : "(Token sin dato)"),
               corredor->tipo);
               
        if (output_file) {
            fprintf(output_file, "Pos: %d, Dato: %s, Tipo: %d\n", 
                    corredor->posicion, 
                    (corredor->dato != NULL ? corredor->dato : "(Token sin dato)"),
                    corredor->tipo);
        }
        
        corredor = corredor->siguiente;
    } while (corredor != inicio);
}

void imprimirTokens(ListaC *inicio, FILE *output_file){
    if(inicio == NULL){
        printf("La lista de tokens está vacía .\n");
        if(output_file) fprintf(output_file, "La lista de tokens está vacía .\n");
        return;
    }

    ListaC *corredor = inicio;
    do{
        printf("Token: (%d, %d)\n", corredor->tipo, corredor->posicion);

        if(output_file){
            fprintf(output_file, "Token: (%d, %d)\n", corredor->tipo, corredor->posicion);
        }
        corredor = corredor->siguiente;
    }while(corredor->siguiente != inicio);
}

void liberarLista(ListaC **inicio) {
    if (*inicio == NULL) return;
    
    ListaC *actual = *inicio;
    ListaC *temp;
    
    do {
        temp = actual;
        actual = actual->siguiente;
        free(temp->dato);  // Liberar string
        free(temp);        // Liberar nodo
    } while (actual != *inicio);
    
    *inicio = NULL;
}
// COMENTAR/ELIMINAR: int BuscarNumero(ListaC ** inicio, char* nombre){...} ya que se reemplaza por BuscarNodoPorDato


// void imprimirToken(ListaC ** inicio, int posicion){
//     //verificación de lista vacía
//     if(*inicio == NULL){
//         printf("No hay tokens por imprimir. \n");
//         return;
//     }
    
//     //verificación de límites (asumiendo que 'tamanio' es global y correcto)
//     if (posicion <= 0 || posicion > tamanio) {
//         printf("Error: Posición %d fuera de rango (1 a %zu). \n", posicion, tamanio);
//         return;
//     }

//     // Empezamos en *inicio (posición 1)
//     ListaC *corredor = *inicio;
    
//     // El bucle NO se ejecuta si posicion es 1
//     for(int i = 1; i < posicion; i++){
//         corredor = corredor->siguiente;
//     }
//     // Imprime el dato
//     printf("%s \n", corredor->dato);
// }




//al eliminar un nodo primero se elimina el string
// void eliminar_nodo(ListaC *nodo_a_eliminar) {
//     if (nodo_a_eliminar->dato != NULL) {
//         free(nodo_a_eliminar->dato); //se libera la memoria del string
//     }
//     free(nodo_a_eliminar); //se libera la memoria del nodo
// }
