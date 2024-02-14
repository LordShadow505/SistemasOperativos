#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "kbhit.h"

int main(void) {
    char comando[100] = "";
    int contador = 0;
    int j = 0;

    while (1) {
    
        printf("%d> %s\n", contador++, comando); //Contador y prompt
        
        if (contador > 10) {
            contador = 0;
        }

        if (kbhit()) { // kbhit() devuelve 1 si se ha presionado una tecla y 0 si no
            comando[j++] = getchar(); // Leer la tecla presionada (caracter)
            comando[j] = '\0'; // Agregar el caracter nulo al final del comando
            
            if (comando[j-1] == 10) { // 13 es el código ASCII de Enter
                // Si tecla = Enter, mostrar el comando ingresado y reiniciar
                printf("\nComando: %s\n", comando);
                comando[0] = '\0'; // Reiniciar
                j = 0; // Reiniciar
                usleep(1500000); // Esperar 1.5s

            } 

        }
    }

}

//Notas
/*
kbhit() devuelve 1 si se ha presionado una tecla y 0 si no
getch() devuelve el código ASCII de la tecla presionada
13 es el código ASCII de Enter, en Linux es 10

*/
