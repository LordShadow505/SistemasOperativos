#include <stdio.h>
#include <curses.h>
#include <time.h> // Para usar la función time
#include <string.h> // Para usar la función strcat, strcpy, strlen, memset, 
#include <unistd.h> // Para usar la función usleep
#include <stdlib.h> // Para usar la función exit, malloc y free
#include "kbhit.h"
#include "lista.h"
#include <ctype.h> // Para usar la función isdigit 

PCB *lista_pcb = NULL;

void Prompt(WINDOW *comandos, int NumLinea, char *comando) {
    int height, width;
    getmaxyx(comandos, height, width);
    wbkgd(comandos, COLOR_PAIR(1));
    box(comandos, 0, 0);
    mvwprintw(comandos, NumLinea + 1, 2, "%d>> %s", NumLinea, comando);
    wrefresh(comandos);
}

void Mensajes(WINDOW *mensajes, char *mensaje) {
    int height, width;
    getmaxyx(stdscr, height, width);
    wbkgd(mensajes, COLOR_PAIR(2));
    box(mensajes, 0, 0);
    mvwprintw(mensajes, 0, 2, "REGISTROS");
    mvwprintw(mensajes, 3, 2, "%s", mensaje);
    wrefresh(mensajes);
    box(mensajes, 0, 0);
}

void Registros(WINDOW *registros, PCB *pcb) {
    int height, width;
    getmaxyx(stdscr, height, width);
    wbkgd(registros, COLOR_PAIR(3));
    box(registros, 0, 0);
    mvwprintw(registros, 0, 2, "REGISTROS");
    mvwprintw(registros, 2, 10, "AX: %d        ", pcb->AX);
    mvwprintw(registros, 2, 25, "BX: %d        ", pcb->BX);
    mvwprintw(registros, 4, 10, "CX: %d        ", pcb->CX);
    mvwprintw(registros, 4, 25, "DX: %d        ", pcb->DX);
    mvwprintw(registros, 6, 10, "PC: %d        ", pcb->PC);
    mvwprintw(registros, 6, 25, "IR: %s        ", pcb->IR);
    mvwprintw(registros, 8, 10, "Linea leida: %s       ", pcb->LineaLeida);
    wrefresh(registros);
    box(registros, 0, 0);
}

void ImprimirLista(WINDOW *lista) {
    int height, width;
    getmaxyx(stdscr, height, width);
    wbkgd(lista, COLOR_PAIR(4));
    box(lista, 0, 0);
    mvwprintw(lista, 0, 2, "LISTA");
    mvwprintw(lista, 2, 5, "Lista = ");
    PCB *temp = lista_pcb;
    int SaltoLinea = 4;
    while (temp != NULL) {
        mvwprintw(lista, SaltoLinea, 5, "PID: %d, Nombre del programa: %s", temp->PID, temp->fileName);
        temp = temp->sig;
        SaltoLinea++;
    }

    wrefresh(lista);
    box(lista, 0, 0);
}


int Errores(WINDOW *mensajes, int codigoError, char *comando, int *j, PCB *pcb) {
    if (codigoError == 104) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Saliendo del programa...");
        clear();
        usleep(1000000); // Espera antes de salir
        liberaLista(lista_pcb); // Liberar la memoria reservada para la lista de PCB    
        free(pcb); // Liberar la memoria reservada para la estructura PCB
        Mensajes(mensajes, "Liberando...             ");
        usleep(1000000);
        endwin();
        exit(0); // Salir del programa
    }

    // ----- ERRORES PARA CARGAR -----

    if (codigoError == 101) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Error: No se ingresó un nombre de archivo, uso: cargar <nombre_archivo>");
        //memset(comando, 0, sizeof(comando));
        comando[*j] = '\0';
        *j = 0;
    }

    if (codigoError == 102) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Error: El archivo no existe");
        //memset(comando, 0, sizeof(comando)); // Limpiar el comando
        comando[*j] = '\0'; 
        *j = 0; // Reiniciar el contador de caractere
    }

    if (codigoError == 103) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Archivo leído exitosamente");
        //memset(comando, 0, sizeof(comando)); // Limpiar el comando
        comando[*j] = '\0'; 
        *j = 0; // Reiniciar el contador de caractere
    }

    if (codigoError == 110) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Registros limpiados exitosamente");
        
        //memset(comando, 0, sizeof(comando)); // Limpiar el comando 
        comando[*j] = '\0';
        *j = 0; // Reiniciar el contador de caracteres
    }
    //--------------------------------------------------------------------------------



    // ----- ERRORES PARA COMANDOS -----

    if (codigoError == 106) {
        char Frase[100] = "Error: Comando no reconocido: >> ";
        strcat(Frase, comando);
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, Frase);
        //memset(comando, 0, sizeof(comando)); // Limpiar el comando 
        comando[*j] = '\0';
        *j = 0; // Reiniciar el contador de caracteres
    }

    //--------------------------------------------------------------------------------
    return 0;
}


int ErroresInstrucciones(WINDOW *mensajes, int codigoError, PCB *pcb) {
    Mensajes(mensajes,"                                                                        ");
    if (codigoError == 108) {
        char Frase[100] = "Error: División por cero";
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, Frase);
        codigoError = 0;
    }

    else if (codigoError == 109) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "END: Terminando la ejecución del programa...");        
        codigoError = 0;
    }

    else if (codigoError == 107) {
        char Frase[100] = "Error: Instrucción no reconocida >>";
        strcat(Frase, pcb->IR);
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, Frase);
        codigoError = 0;
    }

    else if (codigoError == 115) {
        char Frase[100] = "Error: División no exacta, (FLOAT) >> ";
        strcat(Frase, pcb->IR);
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, Frase);
        codigoError = 0;
    }

    else if (codigoError != 107, 108, 109) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes,"Leyendo archivo...");
    }
}


//================================= MOUSEKEHERRAMIENTAS MISTERIOSAS =================================

//================================= CONVERSOR DE STRINGS =================================
int ConversorStrings(WINDOW *mensajes, char *valor, PCB *pcb) {
    int valor_numerico = 0;
    int codigoError = 0;
    if (isdigit(valor[0]) || (valor[0] == '-' && isdigit(valor[1]))) {
        // Si valor es un número, conviértelo a entero o explota si no es un número
        valor_numerico = atoi(valor);
    } else {
        // Si valor es un registro del PCB, asigna su valor correspondiente
        if (strcmp(valor, "AX") == 0) valor_numerico = pcb->AX; 
        else if (strcmp(valor, "BX") == 0) valor_numerico = pcb->BX;
        else if (strcmp(valor, "CX") == 0) valor_numerico = pcb->CX;
        else if (strcmp(valor, "DX") == 0) valor_numerico = pcb->DX;
        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 107;
            ErroresInstrucciones(mensajes, codigoError, pcb);
            return codigoError;
        }

    }
    return valor_numerico; 
}


//================================= COMPROBADOR DE DIGITOS =================================
int EsDigito(char *valor){
    int i = 0;
    if(valor[0] == '-'){
        i = 1;
    }
    for(; valor[i]; i++){
        if(!isdigit(valor[i])){
            return 0;
        }
    }
    return 1;
}

//================================= MAYUSCULAINADOR INADOR =================================
int Mayusculainador(char *LineaArchivo){
    int i;
    for(i = 0; LineaArchivo[i]; i++){
        LineaArchivo[i] = toupper(LineaArchivo[i]);
    }
}

//================================= LINEA VACIA =================================
int EsLineaVacia(const char *linea) {
    while (*linea != '\0') {
        if (!isspace(*linea)) {
            return 0; // No es una línea vacía
        }
        linea++;
    }
    return 1; // Es una línea vacía
}
//==========================================================================================






//================================= OPERACIONES =================================
int MOV(WINDOW *mensajes, char *registro, char *valor, PCB *pcb){
    int valor_numerico = ConversorStrings(mensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX = valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX = valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX = valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX = valor_numerico;
}

int ADD(WINDOW *mensajes, char *registro, char *valor, PCB *pcb){
    int valor_numerico = ConversorStrings(mensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX += valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX += valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX += valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX += valor_numerico;
}

int SUB(WINDOW *mensajes, char *registro, char *valor, PCB *pcb){
    int valor_numerico = ConversorStrings(mensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX -= valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX -= valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX -= valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX -= valor_numerico;
}

int MUL(WINDOW *mensajes, char *registro, char *valor, PCB *pcb){
    int valor_numerico = ConversorStrings(mensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX *= valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX *= valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX *= valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX *= valor_numerico;
}

int DIV(WINDOW *mensajes, char *registro, char *valor, PCB *pcb){
    int valor_numerico = ConversorStrings(mensajes, valor, pcb);

    if (valor_numerico == 0) {
        ErroresInstrucciones(mensajes, 108, pcb);
        return 108;
    } else if (strcmp(registro, "AX") == 0) {
        if (pcb->AX % valor_numerico != 0) {
            ErroresInstrucciones(mensajes, 115, pcb);
            return 115;
        }
        pcb->AX /= valor_numerico;
    } else if (strcmp(registro, "BX") == 0) {
        if (pcb->BX % valor_numerico != 0) {
            ErroresInstrucciones(mensajes, 115, pcb);
            return 115;
        }
        pcb->BX /= valor_numerico;
    } else if (strcmp(registro, "CX") == 0) {
        if (pcb->CX % valor_numerico != 0) {
            ErroresInstrucciones(mensajes, 115, pcb);
            return 115;
        }
        pcb->CX /= valor_numerico;
    } else if (strcmp(registro, "DX") == 0) {
        if (pcb->DX % valor_numerico != 0) {
            ErroresInstrucciones(mensajes, 115, pcb);
            return 115;
        }
        pcb->DX /= valor_numerico;
    }
}

int INC(WINDOW *mensajes, char *registro, PCB *pcb){
    if (strcmp(registro, "AX") == 0) pcb->AX++;
        else if (strcmp(registro, "BX") == 0) pcb->BX++;
        else if (strcmp(registro, "CX") == 0) pcb->CX++;
        else if (strcmp(registro, "DX") == 0) pcb->DX++;
}

int DEC(WINDOW *mensajes, char *registro, PCB *pcb){
    if (strcmp(registro, "AX") == 0) pcb->AX--;
        else if (strcmp(registro, "BX") == 0) pcb->BX--;
        else if (strcmp(registro, "CX") == 0) pcb->CX--;
        else if (strcmp(registro, "DX") == 0) pcb->DX--;
}
//==========================================================================================


//================================= EJECUTAR INSTRUCCION =================================
int EjecutarInstruccion(WINDOW *registros, WINDOW *mensajes, PCB *pcb, char *LineaArchivo) {
    char instruccion[100], registro[100];
    char valor[100] = "";
    int codigoError = 0;

    // Leer la instrucción y el registro de la línea
    if (EsLineaVacia(LineaArchivo)) {
        return 0; //Si la linea esta vacia, no hacer nada
    }

    sscanf(LineaArchivo, "%s %s %s", instruccion, registro, valor);

    int valor_numerico = 0;


    //strcpy(pcb->IR, LineaArchivo); // Almacenar la línea en el registro IR
    // Incrementar el PC
    strncpy(pcb->IR, LineaArchivo, sizeof(pcb->IR) - 1);
        pcb->IR[sizeof(pcb->IR) - 1] = '\0'; 
    /*
    Si hay algun segmentacion fault, probar a hacer algo asi:

    strncpy(pcb->IR, LineaArchivo, sizeof(pcb->IR) - 1);
    pcb->IR[sizeof(pcb->IR) - 1] = '\0'; 

    */
    

    Mayusculainador(LineaArchivo);

    if (strcmp(instruccion, "END") == 0) {
        strcpy(pcb->IR, LineaArchivo);
        Registros(registros, pcb);
        codigoError = 109;
        ErroresInstrucciones(mensajes, codigoError, pcb);
        return 109; // Fin de archivo
    }

    //================================= INSTRUCCIONES PARA MOV =================================
    else if (strcmp(instruccion, "MOV") == 0 && ( (strcmp(valor, "AX") == 0) ||
                                                  (strcmp(valor, "BX") == 0) ||
                                                  (strcmp(valor, "CX") == 0) ||
                                                  (strcmp(valor, "DX") == 0) ||
                                                  (valor_numerico) == 0)) {

        valor_numerico = EsDigito(valor); // Comprueba si el valor es un registro o un número
        
        // Si el valor es un número, entonces se puede hacer la operacion
        if (valor_numerico == 1) 
         {
            MOV(mensajes, registro, valor, pcb);       
        }

        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {

            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; MOV(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; MOV(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; MOV(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; MOV(mensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                Registros(registros, pcb);
                codigoError = 107;
                ErroresInstrucciones(mensajes, codigoError, pcb);
                return codigoError;
            }
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 107;
            ErroresInstrucciones(mensajes, codigoError, pcb);
            return codigoError;
        }                   
        
    } 
    //================================================================================================



    //================================= INSTRUCCIONES PARA ADD =================================
    else if (strcmp(instruccion, "ADD") == 0 && ( (strcmp(valor, "AX") == 0) ||
                                                  (strcmp(valor, "BX") == 0) ||
                                                  (strcmp(valor, "CX") == 0) ||
                                                  (strcmp(valor, "DX") == 0) ||
                                                  (valor_numerico) == 0)) {
        valor_numerico = EsDigito(valor); // Comprueba si el valor es un registro o un número
        
        // Si el valor es un número, entonces se puede hacer la operacion
        if (valor_numerico == 1) 
         {
            ADD(mensajes, registro, valor, pcb);       
        }

        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {

            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; ADD(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; ADD(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; ADD(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; ADD(mensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                Registros(registros, pcb);
                codigoError = 107;
                ErroresInstrucciones(mensajes, codigoError, pcb);
                return codigoError;
            }
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 107;
            ErroresInstrucciones(mensajes, codigoError, pcb);
            return codigoError;
        }                   
        
    } 
    //================================================================================================


    
    //================================= INSTRUCCIONES PARA SUB =================================
    else if (strcmp(instruccion, "SUB") == 0 && ( (strcmp(valor, "AX") == 0) ||
                                                  (strcmp(valor, "BX") == 0) ||
                                                  (strcmp(valor, "CX") == 0) ||
                                                  (strcmp(valor, "DX") == 0) ||
                                                  (valor_numerico) == 0)) {
        valor_numerico = EsDigito(valor); // Comprueba si el valor es un registro o un número
        
        // Si el valor es un número, entonces se puede hacer la operacion
        if (valor_numerico == 1) 
         {
            SUB(mensajes, registro, valor, pcb);       
        }

        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {

            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; SUB(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; SUB(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; SUB(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; SUB(mensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                Registros(registros, pcb);
                codigoError = 107;
                ErroresInstrucciones(mensajes, codigoError, pcb);
                return codigoError;
            }
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 107;
            ErroresInstrucciones(mensajes, codigoError, pcb);
            return codigoError;
        }                   
        
    } 
    //================================================================================================
    
    //================================= INSTRUCCIONES PARA MUL =================================
    else if (strcmp(instruccion, "MUL") == 0 && ( (strcmp(valor, "AX") == 0) ||
                                                  (strcmp(valor, "BX") == 0) ||
                                                  (strcmp(valor, "CX") == 0) ||
                                                  (strcmp(valor, "DX") == 0) ||
                                                  (valor_numerico) == 0)) {
        valor_numerico = EsDigito(valor); // Comprueba si el valor es un registro o un número
        
        // Si el valor es un número, entonces se puede hacer la operacion
        if (valor_numerico == 1) 
         {
            MUL(mensajes, registro, valor, pcb);       
        }

        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {

            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; MUL(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; MUL(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; MUL(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; MUL(mensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                Registros(registros, pcb);
                codigoError = 107;
                ErroresInstrucciones(mensajes, codigoError, pcb);
                return codigoError;
            }
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 107;
            ErroresInstrucciones(mensajes, codigoError, pcb);
            return codigoError;
        }                   
        
    } 
    //================================================================================================ 



    //================================= INSTRUCCIONES PARA DIV =================================
    else if (strcmp(instruccion, "DIV") == 0 && ( (strcmp(valor, "AX") == 0) ||
                                                  (strcmp(valor, "BX") == 0) ||
                                                  (strcmp(valor, "CX") == 0) ||
                                                  (strcmp(valor, "DX") == 0) ||
                                                  (valor_numerico) == 0)) {
        valor_numerico = EsDigito(valor); // Comprueba si el valor es un registro o un número
        // Si el valor es un número, entonces se puede hacer la operacion
        if (valor_numerico == 1) 
         {
            DIV(mensajes, registro, valor, pcb);       
        }
        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {

            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; DIV(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; DIV(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; DIV(mensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; DIV(mensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                Registros(registros, pcb);
                codigoError = 107;
                ErroresInstrucciones(mensajes, codigoError, pcb);
                return codigoError;
            }
            
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 107;
            ErroresInstrucciones(mensajes, codigoError, pcb);
            return codigoError;
        }                   
        
    } 
    //================================================================================================



    //================================= INSTRUCCIONES PARA INC =================================
    else if (strcmp(instruccion, "INC") == 0 && ( (strcmp(registro, "AX") == 0) ||
                                                  (strcmp(registro, "BX") == 0) ||
                                                  (strcmp(registro, "CX") == 0) ||
                                                  (strcmp(registro, "DX") == 0)
                                                )) {


        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación


        if     (strcmp(registro, "AX") == 0) { INC(mensajes, registro, pcb);}
        else if (strcmp(registro, "BX") == 0) { INC(mensajes, registro, pcb);}
        else if (strcmp(registro, "CX") == 0) { INC(mensajes, registro, pcb);}
        else if (strcmp(registro, "DX") == 0) { INC(mensajes, registro, pcb);}

        else {
            // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                Registros(registros, pcb);
                codigoError = 107;
                ErroresInstrucciones(mensajes, codigoError, pcb);
                return codigoError;
        }

    }
                 
        
    


    //================================================================================================ 
    


    //================================= INSTRUCCIONES PARA DEC =================================
    else if (strcmp(instruccion, "DEC") == 0 && ( (strcmp(registro, "AX") == 0) ||
                                                  (strcmp(registro, "BX") == 0) ||
                                                  (strcmp(registro, "CX") == 0) ||
                                                  (strcmp(registro, "DX") == 0)
                                                )) {


        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        
        if     (strcmp(registro, "AX") == 0) { DEC(mensajes, registro, pcb);}
        else if (strcmp(registro, "BX") == 0) { DEC(mensajes, registro, pcb);}
        else if (strcmp(registro, "CX") == 0) { DEC(mensajes, registro, pcb);}
        else if (strcmp(registro, "DX") == 0) { DEC(mensajes, registro, pcb);}

        else {
            // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                Registros(registros, pcb);
                codigoError = 107;
                ErroresInstrucciones(mensajes, codigoError, pcb);
                return codigoError;
        }

    }
    //================================================================================================ 
    

    // ERROR
    else {
        // Si la instrucción no es válida, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                Registros(registros, pcb);
                codigoError = 107;
                ErroresInstrucciones(mensajes, codigoError, pcb);
                return codigoError;
    }

    // No hay error
    pcb->PC++;
    codigoError = 0;
    return 0;
}


int LeerArchivo(WINDOW *registros,WINDOW *mensajes, PCB *pcb, FILE *archivo, char *LineaArchivo) {
    LineaArchivo[strcspn(LineaArchivo, "\n")] = '\0'; // Eliminar el salto de línea (Para que se vea bonito en el prompt)
        // Ejecutar la instrucción de la línea
        Mayusculainador(LineaArchivo);
        
        int codigoError = EjecutarInstruccion(registros,mensajes, pcb, LineaArchivo);

        if (codigoError == 109) {
            fclose(archivo);
            return 109; // Fin de archivo
        }

        if (codigoError == 103) {
            fclose(archivo);
            return 103; // Fin de archivo
        }
        
        if (codigoError != 0) {
            fclose(archivo);
            return codigoError;
        }


        strcpy(pcb->IR, LineaArchivo);
        strcpy(pcb->LineaLeida, LineaArchivo);
        Registros(registros, pcb);
        usleep(100000);
        return 1;
}

int Cargar(WINDOW *registros, WINDOW *mensajes, PCB *pcb, char *nombre_archivo, FILE **archivo, char *LineaArchivo) {
    if (nombre_archivo[0] == '\0') { // Esto significa que no se ingresó un nombre de archivo
        return 101;
    }

    *archivo = fopen(nombre_archivo, "r"); // Abrir el archivo en modo lectura
    if (*archivo == NULL) { // Si el archivo no existe
        return 102;
    } else {
        // Crear un nuevo nodo PCB
        PCB *nuevoNodo = listaCreaNodo(nombre_archivo);
        if (nuevoNodo == NULL) {
            fclose(*archivo);
            return 102; // Error al crear el nodo
        }
        // Asignar un PID al nuevo nodo
        asignarPID(nuevoNodo);
        // Insertar el nodo en la lista
        listaInsertarFinal(nuevoNodo);
        // Actualizar los registros y mensajes
        strcpy(pcb->IR, LineaArchivo);
        Registros(registros, pcb);
    }

    pcb->PC = 0; // Inicializar PC en 0
    return 0; // Regresar 0 significa que no hubo errores
}


int Enter(WINDOW *mensajes, WINDOW *registros, char *comando, PCB *pcb, FILE **archivo, char *LineaArchivo) {
    char cmd[100];
    
    char param1[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    char param2[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    // -------------- SECCION DE COMANDOS ---------------

    // ---- EXIT ----
    sscanf(comando, "%s", cmd); // Leer el comando (la primera palabra del comando)
    Mayusculainador(cmd);
    if ((strcmp(cmd, "SALIR") == 0) || (strcmp(cmd, "EXIT") == 0) || (strcmp(cmd, "MORIR") == 0)){
        return 104; // Esto sale del programa porque se regresa 0 a la función main y el ciclo se rompe
    }

    // ---- CARGAR ----
    sscanf(comando, "%s", cmd); // Leer el comando
    Mayusculainador(cmd);
    if ((strcmp(cmd, "CARGAR") == 0) || (strcmp(cmd, "LOAD") == 0)){
        param1[0] = '\0';
        sscanf(comando, "%*s %s", param1); // Leer el primer parámetro
        
        int resultado = Cargar(registros, mensajes, pcb, param1, archivo, LineaArchivo);
        //memset(comando, 0, sizeof(comando)); // Limpiar el comando
        return resultado;
    }



    sscanf(comando, "%s", cmd); // Leer el comando
    Mayusculainador(cmd);
    if ((strcmp(cmd, "INSERTA") == 0) || (strcmp(cmd, "INSERT") == 0)){
        param1[0] = '\0';
        sscanf(comando, "%*s %s", param1); // Leer el primer parámetro
        PCB *nuevoNodo = listaCreaNodo(param1);
        if (nuevoNodo == NULL) {
            return 102; // Error al crear el nodo
        }
        //Establecer el PID correspondiente (PID del nodo creado previamente +1)
        asignarPID(nuevoNodo);
        //Establecer registros #X en 0.
        nuevoNodo->AX = 0;
        nuevoNodo->BX = 0;
        nuevoNodo->CX = 0;
        nuevoNodo->DX = 0;
        //Establecer IR como cadena vacía.
        strcpy(nuevoNodo->IR, "");
        //Establecer fileName como <fileName>
        strcpy(nuevoNodo->fileName, param1);
        //Establecer programa, como el puntero hacia el archivo <fileName> abierto en modo lectura.
        nuevoNodo->programa = fopen(param1, "r");
        if (nuevoNodo->programa == NULL) {
            return 102; // Error al abrir el archivo
        }
        //Establecer sig como NULL (0).
        nuevoNodo->sig = NULL;
        // retornar el puntero al nodo PCB creado
        listaInsertarFinal(nuevoNodo);
        //enviar el nodo recién creado a una función listaInsertarFinal
        //manda un mensaje a la ventana de mensajes indicando el nodo
        // que ha sido insertado en la lista, en imprimir lista
        // debera imprimir el nodo->PID y el nodo->fileName
        Mensajes(mensajes, "Nodo insertado en la lista");
        
        return 0;
    }


    // ---- ClEAR PCB ----
    sscanf(comando, "%s", cmd);
    Mayusculainador(cmd);
    if ((strcmp(cmd, "LIMPIAR") == 0) || (strcmp(cmd, "CLEAR") == 0)){
        pcb->AX = 0;
        pcb->BX = 0;
        pcb->CX = 0;
        pcb->DX = 0;
        pcb->PC = 0;
        strcpy(pcb->IR, "                      ");
        strcpy(pcb->LineaLeida, "                ");
        Registros(registros, pcb);
        return 110;
    }

    return 106; // Comando no reconocido
}


int LineaComandos(WINDOW *comandos, WINDOW *mensajes, WINDOW *registros, char *comando, int *j, int *NumLinea, PCB *pcb, FILE **archivo, char *LineaArchivo) {
    int caracter = 0;

    if (kbhit()) { // kbhit() devuelve 1 si se ha presionado una tecla y 0 si no
        caracter = getch(); // Leer la tecla presionada (caracter)
        if (caracter != ERR) { // Verificar si se presionó una tecla
            if (caracter == '\n') { // Verificar si la tecla fue Enter
                comando[*j] = '\0'; // Finalizar el comando
                int codigoError = Enter(mensajes, registros, comando, pcb, archivo, LineaArchivo);
                *j = 0; // Reiniciar el contador de caracteres
                (*NumLinea)++; // Incrementar el contador de líneas
                //reiniciar pc a 0  
                pcb -> PC = 0;
                //agregar finalizador de linea
                Errores(mensajes, codigoError, comando, j, pcb);
                comando[*j] = '\0';
            } 
            
            else if (caracter == 127) {  // Tecla 127 Backspace, verificar si es
                if (*j > 0) { // Si hay caracteres para borrar
                    (*j)--;
                    comando[*j] = '\0';

                    int y, x;

                    getyx(comandos, y, x); // Ventana de comandos
                    mvwaddch(comandos, y, x - 1, ' ');
                    wmove(comandos, y, x - 1);
                    mvwprintw(comandos, y, x - 2, "                    ");
                    wrefresh(comandos);
                }
            } 
            
            else { // Agregar el carácter al búfer
                comando[*j] = caracter; // Guardar el carácter en el comando
                comando[(*j) + 1] = '\0';
                (*j)++; // Incrementar el contador de caracteres
            }
        }
        // Prompt(comandos, *linea, comando);
    }

    return 0;
}


int MaxLinea(WINDOW *comandos, int *y, int *x){
    getmaxyx(comandos, *y, *x);
    return *y;
}

int MaxAnchor(WINDOW *comandos, int *y, int *x){
    getmaxyx(comandos, *y, *x);
    return *x;
}


int main(void) {

    initscr();  // Inicializar la pantalla
    start_color();
    init_color(1, 0, 0, 0);
    init_color(2, 250, 0, 150);
    init_pair(1, COLOR_WHITE, 1);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, 2);
    init_color(5, 0, 154, 128);
    init_pair(4, COLOR_WHITE, 5);
    attron(COLOR_PAIR(1));

    cbreak(); // Deshabilitar el buffer de entrada
    noecho(); // No mostrar los caracteres ingresados
    
    int height, width;
    getmaxyx(stdscr, height, width);

    // Crear 3 ventanas para las áreas
    WINDOW *comandos = newwin(height / 2.3 , width, 0, 0);
    wbkgd(comandos, COLOR_PAIR(1));
    box(comandos, 0, 0);
    mvwprintw(comandos, 0, 2, "COMANDOS");


    WINDOW *mensajes = newwin(height / 7, width, height / 2.3, 0);
    wbkgd(mensajes, COLOR_PAIR(2));
    box(mensajes, 0, 0);
    mvwprintw(mensajes, 0, 2, "MENSAJES");
    attroff(COLOR_PAIR(1));

    WINDOW *registros = newwin(height / 2.4, width/2, 2 * height / 3.5, 0);
    wbkgd(registros, COLOR_PAIR(3));
    box(registros, 0, 0);
    mvwprintw(registros, 0, 2, "REGISTROS");

    WINDOW *lista = newwin(height / 2.4, width/2, 2 * height / 3.5, width/2);
    wbkgd(lista, COLOR_PAIR(4));
    box(lista, 0, 0);
    mvwprintw(lista, 0, 2, "LISTA DE INSTRUCCIONES");

    WINDOW *deco = newwin(height / 10.5, width, 3.4 * height / 3.5, 0);
    wbkgd(deco, COLOR_PAIR(1));
    mvwprintw(deco, 0, 10, "Windos I . Version 1.3 . 2024 Copyrigth (c) Chado & Michel Corporeishon.");

    // Actualizar la pantalla para mostrar los cambios
    refresh();
    wrefresh(comandos);
    wrefresh(mensajes);
    wrefresh(registros);
    wrefresh(deco);
    wrefresh(lista);
    
    char comando[1000] = {0}; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    int j = 0; // Contador de caracteres
    int NumLineas = 0; // Contador de líneas

    PCB *pcb = (PCB*)malloc(sizeof(PCB)); // Reservar memoria para la estructura PCB
    //memset(pcb, 0, sizeof(PCB)); // Inicializar la estructura a 0, para evitar basura
    // Insertar el nodo PCB en la lista


    Prompt(comandos, NumLineas, comando);

    Registros(registros, pcb);
    FILE *archivo = NULL;
    int y, x;

    int i = 0;                
    while (1) {
        int codigoError = LineaComandos(comandos, mensajes, registros, comando, &j, &NumLineas, pcb, &archivo, pcb->LineaLeida);
        Prompt(comandos, NumLineas, comando);
        if (NumLineas > (MaxLinea(comandos, &y, &x) - 3)){
            NumLineas = 0;
            wclear(comandos);
            box(comandos, 0, 0);
            mvwprintw(comandos, 0, 2, "COMANDOS");
            wrefresh(comandos);
        }

        ImprimirLista(lista);

        //Verificar si el puntero a archivo es diferente de null
        if (archivo != NULL) {
            
            char LineaArchivo[100]; // Variable para almacenar la línea leída del archivo
            if (fgets(LineaArchivo, 100, archivo) != NULL) { // Leer una línea del archivo
                LeerArchivo(registros, mensajes, pcb, archivo, LineaArchivo);
            } else { // Cerrar el archivo si ya no hay más líneas que leer
                fclose(archivo);
                archivo = NULL; // Igualar el puntero a null
            }

        }


    }

    LiberaLista(&lista_pcb);
    free(pcb); // Liberar la memoria reservada para la estructura PCB
    endwin();
    return 0;
}
