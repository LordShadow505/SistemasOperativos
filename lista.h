#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura de un nodo PCB
typedef struct PCB {
    int PID;
    int AX;
    int BX;
    int CX;
    int DX;
    int PC;
    char IR[100];
    char fileName[256];
    char LineaLeida[100];
    FILE *programa;
    struct PCB *sig;
} PCB;

// Variable global para la lista
PCB *Lista = NULL;

// Función para crear un nuevo nodo PCB
PCB* listaCreaNodo(char *fileName) {
    PCB *nuevoNodo = (PCB *)malloc(sizeof(PCB));
    if (nuevoNodo == NULL) {
        printf("Error al reservar memoria para el nodo\n");
        return NULL;
    }
    nuevoNodo->PID = 0; // Se inicializa en 0 y se asignará un valor correcto posteriormente
    nuevoNodo->AX = 0;
    nuevoNodo->BX = 0;
    nuevoNodo->CX = 0;
    nuevoNodo->DX = 0;
    nuevoNodo->PC = 0;
    strcpy(nuevoNodo->IR, "");
    strcpy(nuevoNodo->fileName, fileName);
    strcpy(nuevoNodo->LineaLeida, "");
    nuevoNodo->programa = fopen(fileName, "r"); // Abrir el archivo en modo lectura
    if (nuevoNodo->programa == NULL) {
        printf("Error al abrir el archivo: %s\n", fileName);
        free(nuevoNodo);
        return NULL;
    }
    nuevoNodo->sig = NULL; // El nuevo nodo se insertará al final, por lo que su siguiente es NULL
    return nuevoNodo;
}

// Función para insertar un nodo al final de la lista
void listaInsertarFinal(PCB *nodo) {
    if (Lista == NULL) {
        Lista = nodo;
        return;
    }
    PCB *temp = Lista;
    while (temp->sig != NULL) {
        temp = temp->sig;
    }
    temp->sig = nodo;
}

// Función para extraer el primer nodo de la lista
PCB* listaExtraeInicio() {
    if (Lista == NULL) {
        printf("Lista vacía\n");
        return NULL;
    }
    PCB *nodoExtraido = Lista;
    Lista = Lista->sig; // El segundo nodo se convierte en el primero
    nodoExtraido->sig = NULL; // Separar el nodo extraído de la lista
    return nodoExtraido;
}

// Función para liberar un nodo PCB
void liberaNodo(PCB *nodo) {
    if (nodo != NULL) {
        fclose(nodo->programa); // Cerrar el archivo
        free(nodo); // Liberar la memoria del nodo
    }
}

// Función para imprimir la lista en la pantalla
void imprimirLista() {
    printf("Lista = ");
    PCB *temp = Lista;
    while (temp != NULL) {
        printf("|%d", temp->PID);
        temp = temp->sig;
    }
    printf("|¬\n");
}

// Función para liberar toda la lista
void liberaLista() {
    PCB *temp;
    while (Lista != NULL) {
        temp = Lista;
        Lista = Lista->sig;
        liberaNodo(temp); // Liberar cada nodo
    }
}

// Función para asignar un PID al nodo
void asignarPID(PCB *nodo) {
    static int contadorPID = 1; // Inicializar el contador de PID
    nodo->PID = contadorPID++; // Asignar el PID y luego incrementar el contador
}
