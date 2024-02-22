#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "kbhit.h"
#include <ctype.h>

// Estructura para el CPU
typedef struct {
    int AX;
    int BX;
    int CX;
    int DX;
    int PC;
    char IR[100];
    char LineaLeida[100];
} PCB;

void Prompt(WINDOW *comandos, int linea, char *comando) {
    mvwprintw(comandos, linea + 1, 2, "%d>> %s", linea, comando);
    wrefresh(comandos);
}

void Mensajes(WINDOW *mensajes, char *mensaje) {
    mvwprintw(mensajes, 3, 2, "%s", mensaje);
    wrefresh(mensajes);
}

void Registros(WINDOW *registros, PCB *pcb) {
    
    mvwprintw(registros, 2, 10, "AX: %d        ", pcb->AX);
    mvwprintw(registros, 2, 25, "BX: %d        ", pcb->BX);
    mvwprintw(registros, 4, 10, "CX: %d        ", pcb->CX);
    mvwprintw(registros, 4, 25, "DX: %d        ", pcb->DX);
    mvwprintw(registros, 6, 10, "PC: %d        ", pcb->PC);
    mvwprintw(registros, 6, 25, "IR: %s        ", pcb->IR);
    mvwprintw(registros, 8, 10, "Linea leida: %s       ", pcb->LineaLeida);
    wrefresh(registros);
}

int Errores(WINDOW *mensajes, int codigoError, char *comando, int *j, PCB *pcb) {
    if (codigoError == 104) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Saliendo del programa...");
        clear();
        usleep(1000000); // Espera antes de salir
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
        memset(comando, 0, sizeof(comando));
        *j = 0;
    }

    if (codigoError == 102) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Error: El archivo no existe");
        memset(comando, 0, sizeof(comando)); // Limpiar el comando 
        *j = 0; // Reiniciar el contador de caractere
    }

    if (codigoError == 103) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Archivo leído exitosamente");
        memset(comando, 0, sizeof(comando)); // Limpiar el comando 
        *j = 0; // Reiniciar el contador de caractere
    }

    if (codigoError == 110) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "Registros limpiados exitosamente");
        memset(comando, 0, sizeof(comando)); // Limpiar el comando 
        *j = 0; // Reiniciar el contador de caracteres
    }
    //--------------------------------------------------------------------------------



    // ----- ERRORES PARA COMANDOS -----

    if (codigoError == 106) {
        char Frase[100] = "Error: Comando no reconocido: >> ";
        strcat(Frase, comando);
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, Frase);
        memset(comando, 0, sizeof(comando)); // Limpiar el comando 

        *j = 0; // Reiniciar el contador de caracteres
        return 5;
    }

    //--------------------------------------------------------------------------------
    return 0;
}


int ErroresInstrucciones(WINDOW *mensajes, int codigoError, PCB *pcb) {
    if (codigoError == 107) {
        char Frase[100] = "Error: Instrucción no reconocida ";
        strcat(Frase, pcb->IR);
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, Frase);
    }

    if (codigoError == 108) {
        char Frase[100] = "Error: División por cero o registro no válido ";
        strcat(Frase, pcb->IR);
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, Frase);
    }

    if (codigoError == 109) {
        Mensajes(mensajes,"                                                                        ");
        Mensajes(mensajes, "END: Terminando la ejecución del programa...");        
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
        return valor_numerico;
    }
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
int Mayusculainador(char *linea){
    for(int i = 0; linea[i]; i++){
        linea[i] = toupper(linea[i]);
    }
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
            if (strcmp(registro, "AX") == 0) pcb->AX /= valor_numerico;
            else if (strcmp(registro, "BX") == 0) pcb->BX /= valor_numerico;
            else if (strcmp(registro, "CX") == 0) pcb->CX /= valor_numerico;
            else if (strcmp(registro, "DX") == 0) pcb->DX /= valor_numerico;
            
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
int EjecutarInstruccion(WINDOW *registros, WINDOW *mensajes, PCB *pcb, char *linea) {
    char instruccion[100], registro[100];
    char valor[100] = "";
    int codigoError = 0;

    // Leer la instrucción y el registro de la línea
    sscanf(linea, "%s %s %s", instruccion, registro, valor);

    int valor_numerico = 0;
    

    //strcpy(pcb->IR, linea); // Almacenar la línea en el registro IR
    // Incrementar el PC
    

    Mayusculainador(linea);
    
    if (strcmp(instruccion, "END") == 0) {
        strcpy(pcb->IR, linea); 
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
                strcpy(pcb->IR, linea); 
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
                strcpy(pcb->IR, linea); 
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
                strcpy(pcb->IR, linea); 
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
                strcpy(pcb->IR, linea); 
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
                strcpy(pcb->IR, linea); 
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
                strcpy(pcb->IR, linea); 
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
                strcpy(pcb->IR, linea); 
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
                strcpy(pcb->IR, linea); 
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


int LeerArchivo(WINDOW *registros,WINDOW *mensajes, PCB *pcb, FILE *archivo, char *linea) {
    linea[strcspn(linea, "\n")] = '\0'; // Eliminar el salto de línea (Para que se vea bonito en el prompt)
        // Ejecutar la instrucción de la línea
        Mayusculainador(linea);
        
        int codigoError = EjecutarInstruccion(registros,mensajes, pcb, linea);

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

        strcpy(pcb->IR, linea);
        strcpy(pcb->LineaLeida, linea);
        Registros(registros, pcb);
        usleep(100000);
        return 1;
}

int Cargar(WINDOW *registros, WINDOW *mensajes, PCB *pcb, char *nombre_archivo, FILE **archivo) {
    if (nombre_archivo[0] == '\0') { // Esto significa que no se ingresó un nombre de archivo
        return 101;
    }

    *archivo = fopen(nombre_archivo, "r"); // Abrir el archivo en modo lectura
    if (*archivo == NULL) { // Si el archivo no existe
        return 102;
    }

    pcb->PC = 0; // Inicializar PC en 0
    return 0; // Regresar 0 significa que no hubo errores
}

int Enter(WINDOW *mensajes, WINDOW *registros, char *comando, PCB *pcb, FILE **archivo) {
    char cmd[100];
    int height, width;
    getmaxyx(stdscr, height, width);
    wbkgd(registros, COLOR_PAIR(3));
    box(registros, 0, 0);
    mvwprintw(registros, 0, 2, "REGISTROS");
    char param1[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    char param2[100] = ""; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    // -------------- SECCION DE COMANDOS ---------------
    Mayusculainador(comando);

    // ---- EXIT ----
    sscanf(comando, "%s", cmd); // Leer el comando (la primera palabra del comando)
    if ((strcmp(cmd, "SALIR") == 0) || (strcmp(cmd, "EXIT") == 0)){
        return 104; // Esto sale del programa porque se regresa 0 a la función main y el ciclo se rompe
    }

    // ---- CARGAR ----
    sscanf(comando, "%s", cmd); // Leer el comando
    if ((strcmp(cmd, "CARGAR") == 0) || (strcmp(cmd, "LOAD") == 0)){
        sscanf(comando, "%*s %s", param1); // Leer el primer parámetro
        int resultado = Cargar(registros, mensajes, pcb, param1, archivo);
        memset(comando, 0, sizeof(comando)); // Limpiar el comando
        return resultado;
    }

    // ---- ClEAR PCB ----
    sscanf(comando, "%s", cmd);
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


int LineaComandos(WINDOW *comandos, WINDOW *mensajes, WINDOW *registros, char *comando, int *j, int *linea, PCB *pcb, FILE **archivo) {
    int caracter = 0;

    if (kbhit()) { // kbhit() devuelve 1 si se ha presionado una tecla y 0 si no
        caracter = getch(); // Leer la tecla presionada (caracter)
        if (caracter != ERR) { // Verificar si se presionó una tecla
            if (caracter == '\n') { // Verificar si la tecla fue Enter
                comando[*j] = '\0'; // Finalizar el comando
                int codigoError = Enter(mensajes, registros, comando, pcb, archivo);
                *j = 0; // Reiniciar el contador de caracteres
                (*linea)++; // Incrementar el contador de líneas
                
                Errores(mensajes, codigoError, comando, j, pcb);

            } else if (caracter == 127) {  // Tecla 127 Backspace, verificar si es
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
            } else { // Agregar el carácter al búfer
                comando[*j] = caracter; // Guardar el carácter en el comando
                comando[(*j) + 1] = '\0';
                (*j)++; // Incrementar el contador de caracteres
            }
        }
        // Prompt(comandos, *linea, comando);
    }

    return 0;
}


int main(void) {
    initscr();  // Inicializar la pantalla
    start_color();
    init_color(1, 0, 0, 0);
    init_color(2, 250, 0, 150);
    init_pair(1, COLOR_WHITE, 1);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, 2);
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

    WINDOW *registros = newwin(height / 2.4, width, 2 * height / 3.5, 0);
    wbkgd(registros, COLOR_PAIR(3));
    box(registros, 0, 0);
    mvwprintw(registros, 0, 2, "REGISTROS");

    WINDOW *deco = newwin(height / 10.5, width, 3.4 * height / 3.5, 0);
    wbkgd(deco, COLOR_PAIR(1));
    mvwprintw(deco, 0, 20, "Windos I . Version 1.3 . 2024");

    // Actualizar la pantalla para mostrar los cambios
    refresh();
    wrefresh(comandos);
    wrefresh(mensajes);
    wrefresh(registros);
    wrefresh(deco);
    
    char comando[1000] = {0}; // Se inicializa con el caracter nulo para evitar que salgan cosas raras (basura)
    int j = 0; // Contador de caracteres
    int linea = 0; // Contador de líneas

    PCB *pcb = (PCB*)malloc(sizeof(PCB)); // Reservar memoria para la estructura PCB
    memset(pcb, 0, sizeof(PCB)); // Inicializar la estructura a 0
   
    Prompt(comandos, linea, comando);

    Registros(registros, pcb);
    FILE *archivo = 0;
    int y, x;
    //calcular la maxima cantidad de lineas que se pueden escribir en la ventana de comandos
    //sin que se desborde

    int MaxLinea(WINDOW *comandos, int *y, int *x){
        getmaxyx(comandos, *y, *x);
        return *y;
    }


    int i = 0;                
    while (1) {
        int codigoError = LineaComandos(comandos, mensajes, registros, comando, &j, &linea, pcb, &archivo);
        Prompt(comandos, linea, comando);
        if (linea > (MaxLinea(comandos, &y, &x) - 3)){
            linea = 0;
            wclear(comandos);
            box(comandos, 0, 0);
            mvwprintw(comandos, 0, 2, "COMANDOS");
            wrefresh(comandos);
        }

        //Verificar si el puntero a archivo es diferente de null
        if (archivo != NULL) {
            char linea[100]; // Variable para almacenar la línea leída del archivo
            if (fgets(linea, 100, archivo) != NULL) { // Leer una línea del archivo
                Prompt(comandos, *linea, " ");
                LeerArchivo(registros, mensajes, pcb, archivo, linea);
            } else { // Cerrar el archivo si ya no hay más líneas que leer
                fclose(archivo);
                archivo = NULL; // Igualar el puntero a null
            }
        }

        
    }

    free(pcb); // Liberar la memoria reservada para la estructura PCB
    endwin();
    return 0;
}
