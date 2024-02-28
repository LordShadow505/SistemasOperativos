#include <stdio.h>
#include "lista.h"

int main() {
    PCB *lista= NULL;
    printf("Inicio: %x\n", lista);
    ListaExtraeInicio(&lista, ListaCreaNodo());
    printf("PrimerNodo: %x\n", lista);
    ListaInsertarFinal(&lista, ListaCreaNodo());
    printf("Fin: %x\n", lista);
    LiberaLista(&lista);
    printf("Fin: %x\n", lista);
    return 0;
}
