#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h> // Para la función toupper
#include <stdlib.h>
struct PCB
{
    char IR[100];
    int AX;
    int BX;
    int CX;
    int DX;
    int PC;
    int PID;
    char fileName[256];
    FILE *programa; //puntero a un archivo
    int estado; // Campo para representar el estado del proceso
    struct PCB *sig; //siguiente estructura o nodo
};



//================================= MOUSEKEHERRAMIENTAS MISTERIOSAS =================================

//================================= LIBERAR ===============================================
void LiberarMemoria (struct PCB *pcb){
    free(pcb);
}


//================================= ABRIR Y VERIFICAR =================================
int AbrirYVerificar(char archivo[100]){
    FILE *NombreArchivo;
    char Linea[100];
    NombreArchivo = fopen(archivo, "r");
    // Verifica si el archivo se abrió correctamente
    if (NombreArchivo == NULL) {
        return 241; //no se pudo abrir correctamente
    }
    fclose(NombreArchivo);

    return 0;
}
//================================= CONVERSOR DE STRINGS =================================
int ConversorStrings(WINDOW *IDventanaMensajes, char *valor,struct PCB *pcb) {
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
            codigoError = 401;
            ////ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
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
int MOV(WINDOW *IDventanaMensajes, char *registro, char *valor, struct PCB *pcb){
    int valor_numerico = ConversorStrings(IDventanaMensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX = valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX = valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX = valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX = valor_numerico;
}

int ADD(WINDOW *IDventanaMensajes, char *registro, char *valor, struct PCB *pcb){
    int valor_numerico = ConversorStrings(IDventanaMensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX += valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX += valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX += valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX += valor_numerico;
}

int SUB(WINDOW *IDventanaMensajes, char *registro, char *valor, struct PCB *pcb){
    int valor_numerico = ConversorStrings(IDventanaMensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX -= valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX -= valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX -= valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX -= valor_numerico;
}

int MUL(WINDOW *IDventanaMensajes, char *registro, char *valor, struct PCB *pcb){
    int valor_numerico = ConversorStrings(IDventanaMensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX *= valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX *= valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX *= valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX *= valor_numerico;
}

int DIV(WINDOW *IDventanaMensajes, char *registro, char *valor, struct PCB *pcb){
    int valor_numerico = ConversorStrings(IDventanaMensajes, valor, pcb);

    if (valor_numerico == 0) {
        return 404; 
    }   
    else if (strcmp(registro, "AX") == 0) pcb->AX /= valor_numerico;   
    else if (strcmp(registro, "BX") == 0) pcb->BX /= valor_numerico;
    else if (strcmp(registro, "CX") == 0) pcb->CX /= valor_numerico;
    else if (strcmp(registro, "DX") == 0) pcb->DX /= valor_numerico;
}


int INC(WINDOW *IDventanaMensajes, char *registro, struct PCB *pcb){
    if (strcmp(registro, "AX") == 0) pcb->AX++;
        else if (strcmp(registro, "BX") == 0) pcb->BX++;
        else if (strcmp(registro, "CX") == 0) pcb->CX++;
        else if (strcmp(registro, "DX") == 0) pcb->DX++;
}

int DEC(WINDOW *IDventanaMensajes, char *registro, struct PCB *pcb){
    if (strcmp(registro, "AX") == 0) pcb->AX--;
        else if (strcmp(registro, "BX") == 0) pcb->BX--;
        else if (strcmp(registro, "CX") == 0) pcb->CX--;
        else if (strcmp(registro, "DX") == 0) pcb->DX--;
}
//==========================================================================================


//================================= EJECUTAR INSTRUCCION =================================
int EjecutarInstruccion(WINDOW *IDventanaRegistros, WINDOW *IDventanaMensajes, struct PCB *pcb, char *LineaArchivo) {
    char instruccion[100], registro[100];
    char valor[100] = "";
    int codigoError = 0;

    // Leer la instrucción y el registro de la línea
    if (EsLineaVacia(LineaArchivo)) {
        return 0; //Si la linea esta vacia, no hacer nada
    }

    sscanf(LineaArchivo, "%s %s %s", instruccion, registro, valor);

    int valor_numerico = 0;

    strncpy(pcb->IR, LineaArchivo, sizeof(pcb->IR) - 1);
        pcb->IR[sizeof(pcb->IR) - 1] = '\0'; 

    Mayusculainador(LineaArchivo);

    if (strcmp(instruccion, "END") == 0) {
        strcpy(pcb->IR, LineaArchivo);
        //VentanaRegistros(IDventanaRegistros, pcb);
        codigoError = 1;
        //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
        return 1; // Fin de archivo
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
            MOV(IDventanaMensajes, registro, valor, pcb);       
        }

        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {

            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; MOV(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; MOV(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; MOV(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; MOV(IDventanaMensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                //VentanaRegistros(IDventanaRegistros, pcb);
                codigoError = 401;
                //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
                return codigoError;
            }
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 401;
            //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
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
            ADD(IDventanaMensajes, registro, valor, pcb);       
        }

        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {

            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; ADD(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; ADD(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; ADD(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; ADD(IDventanaMensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                //VentanaRegistros(IDventanaRegistros, pcb);
                codigoError = 401;
                //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
                return codigoError;
            }
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 401;
            //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
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
            SUB(IDventanaMensajes, registro, valor, pcb);       
        }

        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {

            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; SUB(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; SUB(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; SUB(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; SUB(IDventanaMensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                //VentanaRegistros(IDventanaRegistros, pcb);
                codigoError = 401;
                //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
                return codigoError;
            }
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 401;
            //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
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
            MUL(IDventanaMensajes, registro, valor, pcb);       
        }

        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {

            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; MUL(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; MUL(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; MUL(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; MUL(IDventanaMensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                //VentanaRegistros(IDventanaRegistros, pcb);
                codigoError = 401;
                //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
                return codigoError;
            }
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 401;
            //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
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
            if (atoi(valor) == 0) {
                codigoError = 404;
                return codigoError;
            }
            DIV(IDventanaMensajes, registro, valor, pcb);  
        }
        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        else if (valor_numerico == 0) {
            if      (strcmp(valor, "AX") == 0) { valor_numerico = pcb->AX; DIV(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "BX") == 0) {valor_numerico = pcb->BX; DIV(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "CX") == 0) {valor_numerico = pcb->CX; DIV(IDventanaMensajes, registro, valor, pcb);}
            else if (strcmp(valor, "DX") == 0) {valor_numerico = pcb->DX; DIV(IDventanaMensajes, registro, valor, pcb);}

            else {
                // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                //VentanaRegistros(IDventanaRegistros, pcb);
                codigoError = 401;
                //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
                return codigoError;
            }
            
        }

        else {
            // Si el registro no es válido, devuelve un error
            codigoError = 401;
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


        if     (strcmp(registro, "AX") == 0) { INC(IDventanaMensajes, registro, pcb);}
        else if (strcmp(registro, "BX") == 0) { INC(IDventanaMensajes, registro, pcb);}
        else if (strcmp(registro, "CX") == 0) { INC(IDventanaMensajes, registro, pcb);}
        else if (strcmp(registro, "DX") == 0) { INC(IDventanaMensajes, registro, pcb);}

        else {
            // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                //VentanaRegistros(IDventanaRegistros, pcb);
                codigoError = 401;
                //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
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
        
        if     (strcmp(registro, "AX") == 0) { DEC(IDventanaMensajes, registro, pcb);}
        else if (strcmp(registro, "BX") == 0) { DEC(IDventanaMensajes, registro, pcb);}
        else if (strcmp(registro, "CX") == 0) { DEC(IDventanaMensajes, registro, pcb);}
        else if (strcmp(registro, "DX") == 0) { DEC(IDventanaMensajes, registro, pcb);}

        else {
            // Si el registro no es válido, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                //VentanaRegistros(IDventanaRegistros, pcb);
                codigoError = 401;
                //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
                return codigoError;
        }

    }
    //================================================================================================ 
    

    // ERROR
    else {
        // Si la instrucción no es válida, devuelve un error
                strcpy(pcb->IR, LineaArchivo); 
                //VentanaRegistros(IDventanaRegistros, pcb);
                codigoError = 401;
                //ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
                return codigoError;
    }

    // No hay error
    codigoError = 0;
    return 0;
}
