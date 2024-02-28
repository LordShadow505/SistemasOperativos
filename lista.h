/*

    Desarrollar una librería (lista.h), con las funciones necesarias para la implementación y manejo de una lista ligada de nodos de la estructura PCB, con los siguientes elementos:
        PID de tipo entero.
        AX de tipo entero.
        BX de tipo entero.
        CX de tipo entero.
        DX de tipo entero.
        PC de tipo entero.
        IR de tipo arreglo de caracteres de 100.
        fileName de tipo arreglo de caracteres de 256.
        programa de tipo puntero a archivo.
        sig de tipo puntero a PCB.
    Donde:
        PID, será el número del nodo acorde a su creación, puede usar una variable global e incrementarla cada que se cree un nodo.
        fileName, será el nombre de un archivo.
        programa, será un puntero al archivo indicado por fileName, abierto en modo lectura.
        sig, será un puntero al siguiente nodo de la lista (NULL si no hay mas nodos).
    La librería lista.h deberá agregarse a su Entrega 1 para cumplir con las siguientes funcionalidades y especificaciones:
        Crear una variable global llamada Lista del tipo puntero a PCB, e inicializada en 0.
            Deberá implementar la siguientes acciones para cuando el usuario teclee el comando INSERTA|INSERT <fileName>
                Se deberá llamar una función: listaCreaNodo. En ella:
                    Reservar la memoria necesaria para almacenar datos en un nuevo nodo PCB.
                    Establecer el PID correspondiente (PID del nodo creado previamente +1)
                    Establecer registros #X en 0.
                    Establecer IR como cadena vacía.
                    Establecer fileName como <fileName>
                    Establecer programa, como el puntero hacia el archivo <fileName> abierto en modo lectura.
                    Establecer sig como NULL (0).
                    Regresar el puntero al nodo PCB creado, si no se presentaron errores.
                Se deberá enviar el nodo recién creado a una función listaInsertarFinal la cual deberá:
                    Buscar el último nodo de Lista.
                    ligar el nodo recién creado, al final de la Lista (después del último nodo encontrado), de forma que el nuevo nodo sea el último.
            Implementar acciones para cuando el usuario teclee el comando EXTRAE|PULL
                Se deberá llamar la función: listaExtraeInicio. En ella:
                    Buscará el primer nodo de Lista, y lo desligará de la misma:
                        La lista deberá entonces iniciar con el antiguo segundo nodo, en caso de que exista.
                    El nodo desligado deberá ser mostrado en el apartado PCB, de ncurses.
                        Deberá incluir espacio para desplegar los nuevos elementos de PCB.
            Deberá implementar acciones para cuando el usuario teclee el comando ENCOLA|PUSH
                Verificar que PCB tenga algún nodo de la lista y si es el caso:
                    Insertar dicho nodo al final de la lista con el método listaInsertarFinal.
                    Restablecer PCB a 0s.
            Implementar el comando ELIMINA|KILL <PID>
                Deberá buscar el PID especificado en PCB o bien en Lista y extraerlo.
                    Una vez extraído el nodo de PCB o Lista, deberá enviarlo a una función liberaNodo, la cual:
                        Deberá cerrar el archivo apuntado por programa.
                        Liberar toda memoria reservada para el nodo.
            Considerar que si se solicita el comando SALIR|EXIT
                Deberá ejecutar ciclicamente los siguientes pasos hasta que PCB y Lista queden vacías.
                    listaExtraeInicio
                    liberaNodo
        Mientras PCB esté mostrando algún nodo de la lista, el comando LOAD no tendrá ningún efecto.
        Deberá imprimir Lista en algún espacio libre de la pantalla en todo momento (considere alguna representación con los PIDs de cada nodo), actualizándola cada que se realice un cambio sobre ella:
            Ejemplos:
                Lista vacía: Lista = ¬
                Lista con dos nodos con PIDs 1 y 2: Lista = |1|2|¬
                Lista con 5 nodos: Lista = |4|5|6|2|3|¬
        En caso de encontrar algún error o excepción, su código deberá indicar los mensajes de retroalimentación correspondientes, y realizar las acciones mas convenientes para que el usuario pueda sobrellevar la situación.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PCB {
    int PID;
    int AX;
    int BX;
    int CX;
    int DX;
    int PC;
    char IR[100];
    char fileName[256];
    FILE *programa;
    struct PCB *sig;
} PCB;


int HolaMundo(){
    printf("Hola Mundo\n");
    return 0;
}

PCB* ListaCreaNodo(){
    PCB *nodo = (PCB *)malloc(sizeof(PCB));
    printf("Nodo: %x\n", nodo);
    if(nodo == NULL){
        printf("Error al reservar memoria para el nodo\n");
        return NULL;
    }
    nodo->PID = 0;
    nodo->AX = 0;
    nodo->BX = 0;
    nodo->CX = 0;
    nodo->DX = 0;
    nodo->PC = 0;
    strcpy(nodo->IR, "");
    strcpy(nodo->fileName, "");
    nodo->programa = NULL;
    nodo->sig = NULL;
    return nodo;
}


int ListaInsertarFinal(PCB **lista, PCB *nodo){
    printf("Nodo: %x\n", nodo);
    if(*lista == NULL){
        *lista = nodo;
        return 1;
    }
    PCB *aux = *lista;
    while(aux->sig != NULL){
        aux = aux->sig;
    }
    aux->sig = nodo;
    return 0;
}



int ListaExtraeInicio(PCB **lista, PCB **nodo){
    if(*lista == NULL){
        printf("Lista vacía\n");
        return 1;
    }
    *nodo = *lista;
    *lista = (*lista)->sig;
    return 0;
}

int LiberaLista(PCB **lista){
    PCB *aux = *lista;
    while(aux != NULL){
        PCB *temp = aux;
        aux = aux->sig;
        free(temp);
    }
    *lista = NULL;
    return 0;
}


