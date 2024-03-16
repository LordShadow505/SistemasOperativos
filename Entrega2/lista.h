#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>



//-----------------------------------------------------------
//INCLUYE DOCUMENTACION PARA NUESTRAS LISTAS, PARA NO HACERNOS BOLAS 
//-----------------------------------------------------------


/*
 * Extrae el primer nodo de la lista
 * 
 * Parámetros:
 * - PrimerNodo: Puntero al puntero del primer nodo de la lista
 * 
 * Valor de retorno:
 * - Puntero al primer nodo extraído
 * 
 * Objetivo:
 * - Extraer el primer nodo de la lista
 * 
 * Descripción:
 * - Verifica si la lista está vacía.
 * - Si la lista está vacía, retorna NULL.
 * - Si la lista no está vacía, extrae el primer nodo de la lista y actualiza el puntero al primer nodo.
 * - Retorna el puntero al primer nodo extraído.
 */
struct PCB* listaExtraeInicio(struct PCB **PrimerNodo) {
    if (*PrimerNodo == NULL) { //Si la lista está vacía, retorna NULL
        return NULL;
    }

    struct PCB* primerNodo = *PrimerNodo; //Si la lista no está vacía, extrae el primer nodo de la lista
    *PrimerNodo = primerNodo->sig; //Actualiza el puntero al primer nodo
    return primerNodo;
}

/*
 * Libera la memoria de la lista de procesos listos
 * 
 * Parámetros:
 * - PrimerNodo: Puntero al puntero del primer nodo de la lista
 * 
 * Objetivo:
 * - Liberar la memoria de la lista de procesos listos
 * 
 * Descripción:
 * - Recorre la lista de procesos listos y libera la memoria de cada nodo.
 * - Cierra el archivo asociado a cada nodo.
 * - Actualiza el puntero al primer nodo a NULL.
 */
void LiberaListos(struct PCB **PrimerNodo) {
    struct PCB *Actual = *PrimerNodo;
    struct PCB *Aux;

    while (Actual != NULL) {
        Aux = Actual;
        Actual = Actual->sig;
        fclose(Aux->programa);
        free(Aux);
    }

    *PrimerNodo = NULL;
}

/*
 * Libera la memoria de la lista de procesos terminados
 * 
 * Parámetros:
 * - PrimerNodo: Puntero al puntero del primer nodo de la lista
 * 
 * Objetivo:
 * - Liberar la memoria de la lista de procesos terminados
 * 
 * Descripción:
 * - Recorre la lista de procesos terminados y libera la memoria de cada nodo.
 * - Actualiza el puntero al primer nodo a NULL.
 */
void LiberaTerminados(struct PCB **PrimerNodo) {
    struct PCB *Actual = *PrimerNodo;
    struct PCB *Aux;

    while (Actual != NULL) {
        Aux = Actual;
        Actual = Actual->sig;
        free(Aux);
    }

    *PrimerNodo = NULL;
}

/*
 * Inserta un nuevo nodo al final de la lista
 * 
 * Parámetros:
 * - PrimerNodo: Puntero al puntero del primer nodo de la lista
 * - ejecucion: Puntero al nodo que se va a insertar
 * 
 * Objetivo:
 * - Insertar un nuevo nodo al final de la lista
 * 
 * Descripción:
 * - Crea un nuevo nodo y copia los datos del nodo "ejecucion" en él.
 * - Inserta el nuevo nodo al final de la lista.
 * - Si la lista está vacía, el nuevo nodo se convierte en el primer nodo.
 */
void InsertarNuevo(struct PCB **PrimerNodo, struct PCB *ejecucion) {
    struct PCB *nuevoNodo = (struct PCB*)malloc(sizeof(struct PCB));
    if(nuevoNodo == NULL){
        return;
    }
    memcpy(nuevoNodo, ejecucion, sizeof(struct PCB));
    nuevoNodo->sig = NULL;

    if (*PrimerNodo == NULL) {
        *PrimerNodo = nuevoNodo;
        return;
    }

    struct PCB *Ultimo = *PrimerNodo;
    while (Ultimo->sig != NULL) {
        Ultimo = Ultimo->sig;
    }

    Ultimo->sig = nuevoNodo;
}

/*
 * Inserta un nuevo proceso en la lista
 * 
 * Parámetros:
 * - PrimerNodo: Puntero al puntero del primer nodo de la lista
 * - nombrePrograma: Nombre del archivo del programa a insertar
 * 
 * Objetivo:
 * - Insertar un nuevo proceso en la lista
 * 
 * Descripción:
 * - Crea un nuevo nodo y asigna un ID único al proceso.
 * - Abre el archivo del programa y lo asocia al nodo.
 * - Copia el nombre del archivo del programa en el nodo.
 * - Inserta el nuevo nodo al final de la lista.
 * - Si la lista está vacía, el nuevo nodo se convierte en el primer nodo.
 */
int contadorPID = 0;
void Insertar(struct PCB **PrimerNodo, char *nombrePrograma) {
    struct PCB *nuevoNodo = (struct PCB*)malloc(sizeof(struct PCB)); //Reservar memoria para el nuevo nodo
    if(nuevoNodo == NULL){ //Si no se pudo reservar memoria, no se inserta el nodo
        return;
    }
    nuevoNodo->PID = contadorPID++;
    nuevoNodo->programa = fopen(nombrePrograma, "r");
    if (nuevoNodo->programa == NULL) { //Si no se pudo abrir el archivo, no se inserta el nodo
        free(nuevoNodo);
        return;
    }
    strcpy(nuevoNodo->fileName, nombrePrograma); //Copia el nombre del archivo en el nodo

    nuevoNodo->AX = 0;
    nuevoNodo->BX = 0;
    nuevoNodo->CX = 0;
    nuevoNodo->DX = 0;
    nuevoNodo->PC = 0;
    strcpy(nuevoNodo->IR, "");
    
    nuevoNodo->sig = NULL; //El nuevo nodo se convierte en el último nodo de la lista
    

    if (*PrimerNodo == NULL) { //Si la lista está vacía, el nuevo nodo se convierte en el primer nodo
        *PrimerNodo = nuevoNodo;
        return;
    }

    struct PCB *Ultimo = *PrimerNodo; //Si la lista no está vacía, se recorre hasta el último nodo
    while (Ultimo->sig != NULL) {
        Ultimo = Ultimo->sig;
    }

    Ultimo->sig = nuevoNodo;
}

/*
 * Inserta un nuevo nodo al final de la lista de procesos terminados
 * 
 * Parámetros:
 * - PrimerNodo: Puntero al puntero del primer nodo de la lista
 * - ejecucion: Puntero al nodo que se va a insertar
 * 
 * Objetivo:
 * - Insertar un nuevo nodo al final de la lista de procesos terminados
 * 
 * Descripción:
 * - Utiliza la función "InsertarNuevo" para insertar el nodo al final de la lista.
 */
void ListaInsertarFinal(struct PCB **PrimerNodo, struct PCB *ejecucion) {
    InsertarNuevo(PrimerNodo, ejecucion);
}

/*
 * Finaliza un proceso y lo mueve a la lista de procesos terminados
 * 
 * Parámetros:
 * - PrimerNodo: Puntero al puntero del primer nodo de la lista de procesos listos
 * - pid: Identificador del proceso a finalizar
 * - terminados: Puntero al puntero del primer nodo de la lista de procesos terminados
 * 
 * Valor de retorno:
 * - 0 si el proceso se finalizó correctamente, -1 si no se encontró el proceso
 * 
 * Objetivo:
 * - Finalizar un proceso y moverlo a la lista de procesos terminados
 * 
 * Descripción:
 * - Busca el proceso con el ID especificado en la lista de procesos listos.
 * - Si encuentra el proceso, lo extrae de la lista de procesos listos y lo inserta en la lista de procesos terminados.
 * - Cierra el archivo asociado al proceso.
 * - Retorna 0 si el proceso se finalizó correctamente, -1 si no se encontró el proceso.
 */
int KillFinal(struct PCB **PrimerNodo, int pid, struct PCB **terminados){
    if (*PrimerNodo == NULL){
        return -1;
    }

    struct PCB *Actual = *PrimerNodo;
    struct PCB *anterior = NULL;

    while (Actual != NULL && Actual->PID != pid) {
        anterior = Actual;
        Actual = Actual->sig;
    }

    if (Actual != NULL) {
        if (anterior == NULL) {
            *PrimerNodo = Actual->sig;
        } else {
            anterior->sig = Actual->sig;
        }
        Actual->sig = NULL;
        InsertarNuevo(terminados, Actual);
        fclose(Actual->programa);
    } else {
        return -1;
    }
    return 0;
}

/*
 * Finaliza un proceso
 * 
 * Parámetros:
 * - PrimerNodo: Puntero al puntero del primer nodo de la lista de procesos listos
 * - pid: Identificador del proceso a finalizar
 * 
 * Valor de retorno:
 * - 0 si el proceso se finalizó correctamente, -1 si no se encontró el proceso
 * 
 * Objetivo:
 * - Finalizar un proceso
 * 
 * Descripción:
 * - Busca el proceso con el ID especificado en la lista de procesos listos.
 * - Si encuentra el proceso, lo extrae de la lista de procesos listos y libera la memoria del nodo.
 * - Retorna 0 si el proceso se finalizó correctamente, -1 si no se encontró el proceso.
 */
int Kill(struct PCB **PrimerNodo, int pid) {
    if (*PrimerNodo == NULL) {
        return -1;
    }

    struct PCB *Actual = *PrimerNodo;
    struct PCB *anterior = NULL;

    while (Actual != NULL && Actual->PID != pid) {
        anterior = Actual;
        Actual = Actual->sig;
    }

    if (Actual != NULL) {
        if (anterior == NULL) {
            *PrimerNodo = Actual->sig;
        } else {
            anterior->sig = Actual->sig;
        }
        free(Actual);
    } else {
        return -1;
    }

    return 0;
}
