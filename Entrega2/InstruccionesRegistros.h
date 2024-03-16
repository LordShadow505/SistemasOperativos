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
/*
 * LiberarMemoria
 * 
 * Parámetros:
 * - pcb: Puntero a la estructura del PCB
 * 
 * Objetivo:
 * - Liberar la memoria reservada para la estructura del PCB
 * - 
 * Descripción:
 * - Libera la memoria reservada para la estructura del PCB
 

 */
void LiberarMemoria (struct PCB *pcb){
    free(pcb);
}


//================================= ABRIR Y VERIFICAR =================================
/*
 * AbrirYVerificar
 * 
 * Parámetros:
 * - archivo: Nombre del archivo a abrir
 * 
 * Objetivo:
 * - Abrir un archivo y verificar si se abrió correctamente
 * 
 * Descripción:
 * - Abre el archivo en modo lectura
 * - Verifica si el archivo se abrió correctamente
 * - Devuelve un código de error si no se pudo abrir el archivo
 * - Devuelve 0 si el archivo se abrió correctamente
 */
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
/*
 * ConversorStrings
 * 
 * Parámetros:
 * - VentanaMensajes: Ventana de mensajes
 * - valor: Valor a convertir
 * - pcb: Puntero a la estructura del PCB
 * 
 * Valor de retorno:
 * - Si valor es un número, devuelve el valor convertido a entero
 * - Si valor es un registro del PCB, devuelve el valor correspondiente
 * - Si valor no es un número ni un registro del PCB, devuelve un código de error
 * 
 * Objetivo:
 * - Convertir un valor a entero
 * - 
 * Descripción:
 * - Si valor es un número, conviértelo a entero o explota si no es un número
 * - Si valor es un registro del PCB, asigna su valor correspondiente
 */
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
/*
 * EsDigito
 * 
 * Parámetros:
 * - valor: Valor a comprobar
 * 
 * Valor de retorno:
 * - 1 si el valor es un número
 * - 0 si el valor no es un número
 * 
 * Objetivo:
 * - Comprobar si un valor es un número
 * - 
 * Descripción:
 * - Comprueba si el valor es un número, incluyendo números negativos
 */
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
/*
 * Mayusculainador
 * 
 * Parámetros:
 * - LineaArchivo: Línea de archivo a convertir
 * 
 * Valor de retorno:
 * - Ninguno
 * 
 * Objetivo:
 * - Convertir una línea de archivo a mayúsculas
 * - 
 * Descripción:
 * - Convierte una línea de archivo a mayúsculas
 */
int Mayusculainador(char *LineaArchivo){
    int i;
    for(i = 0; LineaArchivo[i]; i++){
        LineaArchivo[i] = toupper(LineaArchivo[i]);
    }
}


//================================= LINEA VACIA =================================
/*
 * EsLineaVacia
 * 
 * Parámetros:
 * - linea: Línea a comprobar
 * 
 * Valor de retorno:
 * - 1 si la línea está vacía
 * - 0 si la línea no está vacía
 * 
 * Objetivo:
 * - Comprobar si una línea está vacía
 * - 
 * Descripción:
 * - Comprueba si una línea está vacía, es decir, si solo contiene espacios en blanco
 */
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
/*
* MOV
* 
* Parámetros:
* - IDventanaMensajes: Ventana de mensajes
* - registro: Registro de destino
* - valor: Valor a mover
* - pcb: Puntero a la estructura del PCB
* 
* Valor de retorno:
* - Ninguno
* 
* Objetivo:
* - Realizar la operación MOV
* - 
* Descripción:
* - Mueve un valor a un registro del PCB 
*/
int MOV(WINDOW *IDventanaMensajes, char *registro, char *valor, struct PCB *pcb){
    int valor_numerico = ConversorStrings(IDventanaMensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX = valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX = valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX = valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX = valor_numerico;
}

/*
* ADD
*
* Parámetros:
* - IDventanaMensajes: Ventana de mensajes
* - registro: Registro de destino
* - valor: Valor a sumar
* - pcb: Puntero a la estructura del PCB
*
* Valor de retorno:
* - Ninguno
*
* Objetivo:
* - Realizar la operación ADD
* -
* Descripción:
* - Suma un valor a un registro del PCB
*/
int ADD(WINDOW *IDventanaMensajes, char *registro, char *valor, struct PCB *pcb){
    int valor_numerico = ConversorStrings(IDventanaMensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX += valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX += valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX += valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX += valor_numerico;
}

/*
* SUB
*
* Parámetros:
* - IDventanaMensajes: Ventana de mensajes
* - registro: Registro de destino
* - valor: Valor a restar
* - pcb: Puntero a la estructura del PCB
*
* Valor de retorno:
* - Ninguno
*
* Objetivo:
* - Realizar la operación SUB
* -
* Descripción:
* - Resta un valor a un registro del PCB
*/
int SUB(WINDOW *IDventanaMensajes, char *registro, char *valor, struct PCB *pcb){
    int valor_numerico = ConversorStrings(IDventanaMensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX -= valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX -= valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX -= valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX -= valor_numerico;
}

/*
* MUL
*
* Parámetros:
* - IDventanaMensajes: Ventana de mensajes
* - registro: Registro de destino
* - valor: Valor a multiplicar
* - pcb: Puntero a la estructura del PCB
*
* Valor de retorno:
* - Ninguno
*
* Objetivo:
* - Realizar la operación MUL
* -
* Descripción:
* - Multiplica un valor a un registro del PCB
*/
int MUL(WINDOW *IDventanaMensajes, char *registro, char *valor, struct PCB *pcb){
    int valor_numerico = ConversorStrings(IDventanaMensajes, valor, pcb);
        if (strcmp(registro, "AX") == 0) pcb->AX *= valor_numerico;
        else if (strcmp(registro, "BX") == 0) pcb->BX *= valor_numerico;
        else if (strcmp(registro, "CX") == 0) pcb->CX *= valor_numerico;
        else if (strcmp(registro, "DX") == 0) pcb->DX *= valor_numerico;
}

/*
* DIV
*
* Parámetros:
* - IDventanaMensajes: Ventana de mensajes
* - registro: Registro de destino
* - valor: Valor a dividir
* - pcb: Puntero a la estructura del PCB
*
* Valor de retorno:
* - Ninguno
*
* Objetivo:
* - Realizar la operación DIV
* -
* Descripción:
* - Divide un valor a un registro del PCB
*/
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

/*
* INC
*
* Parámetros:
* - IDventanaMensajes: Ventana de mensajes
* - registro: Registro de destino
* - pcb: Puntero a la estructura del PCB
*
* Valor de retorno:
* - Ninguno
*
* Objetivo:
* - Realizar la operación INC
* -
* Descripción:
* - Incrementa un valor a un registro del PCB
*/
int INC(WINDOW *IDventanaMensajes, char *registro, struct PCB *pcb){
    if (strcmp(registro, "AX") == 0) pcb->AX++;
        else if (strcmp(registro, "BX") == 0) pcb->BX++;
        else if (strcmp(registro, "CX") == 0) pcb->CX++;
        else if (strcmp(registro, "DX") == 0) pcb->DX++;
}

/* 
* DEC
*
* Parámetros:
* - IDventanaMensajes: Ventana de mensajes
* - registro: Registro de destino
* - pcb: Puntero a la estructura del PCB
*
* Valor de retorno:
* - Ninguno
*
* Objetivo:
* - Realizar la operación DEC
* -
* Descripción:
* - Decrementa un valor a un registro del PCB
*/
int DEC(WINDOW *IDventanaMensajes, char *registro, struct PCB *pcb){
    if (strcmp(registro, "AX") == 0) pcb->AX--;
        else if (strcmp(registro, "BX") == 0) pcb->BX--;
        else if (strcmp(registro, "CX") == 0) pcb->CX--;
        else if (strcmp(registro, "DX") == 0) pcb->DX--;
}
//==========================================================================================



//================================= EJECUTAR INSTRUCCION =================================
/*
* EjecutarInstruccion
*
* Parámetros:
* - IDventanaRegistros: Ventana de registros
* - IDventanaMensajes: Ventana de mensajes
* - pcb: Puntero a la estructura del PCB
* - LineaArchivo: Línea de archivo a ejecutar
*
* Valor de retorno:
* - 0 si no hay errores
* - 1 si se llegó al fin del archivo
* - 401 si la instrucción no es válida
* - 404 si se intenta dividir entre cero
* - 406 si hay más de 3 parámetros
* - 408 si hay solo una instrucción distinta a "END"
*
* Objetivo:
* - Ejecutar una instrucción
* -
* Descripción:
* - Lee la instrucción y el registro de la línea
* - Verifica si la instrucción es válida
* - Ejecuta la instrucción
* - Devuelve un código de error si no se pudo ejecutar la instrucción
*/

int EjecutarInstruccion(WINDOW *IDventanaRegistros, WINDOW *IDventanaMensajes, struct PCB *pcb, char *LineaArchivo) {
    char instruccion[100], registro[100];
    char valor[100] = "";
    int codigoError = 0;
    char parametroInvalido[100];
    //Instruccion Registro Registro/Valor
    
    
    // Leer la instrucción y el registro de la línea
    if (EsLineaVacia(LineaArchivo)) {
        return 0; // Si la línea está vacía, no hacer nada
    }


    // Si hay más de 3 parámetros, devuelve un error
    if (sscanf(LineaArchivo, "%s %s %s %s", instruccion, registro, valor, parametroInvalido) == 4) {
        return 406; // Si hay más de 3 parámetros, devuelve un error
    }

    //Si hay 2 o 3, entonces se leeen
    
    else if(sscanf(LineaArchivo, "%s %s %s", instruccion, registro, valor) == 3){}
    else if(sscanf(LineaArchivo, "%s %s", instruccion, registro) == 2){}
    else if (sscanf(LineaArchivo, "%s", instruccion) == 1) {
        // Ignorar la instrucción "END" y sus variaciones
        if (strcasecmp(instruccion, "END") == 0) {
            strcpy(pcb->IR, LineaArchivo);
            return 1; // Fin de archivo
        }
        return 408; // Si hay solo una instrucción distinta a "END", devuelve un error
    }


    //ahora debe verificar que instruccion sea valida
    if (strcmp(instruccion, "MOV") != 0 &&
        strcmp(instruccion, "ADD") != 0 &&
        strcmp(instruccion, "SUB") != 0 &&     
        strcmp(instruccion, "MUL") != 0 && 
        strcmp(instruccion, "DIV") != 0 && 
        strcmp(instruccion, "INC") != 0 && 
        strcmp(instruccion, "DEC") != 0 && 
        strcmp(instruccion, "END") != 0) {
        return 401; // Si la instrucción no es válida, devuelve un error
    }



    int valor_numerico = 0;

    strncpy(pcb->IR, LineaArchivo, sizeof(pcb->IR) - 1); // Copia la línea de instrucción en el IR
    pcb->IR[sizeof(pcb->IR) - 1] = '\0';               // Asegura que el IR termine con un carácter nulo

    Mayusculainador(LineaArchivo);

    if (strcmp(instruccion, "END") == 0) {
        strcpy(pcb->IR, LineaArchivo);
        // VentanaRegistros(IDventanaRegistros, pcb);
        codigoError = 1;
        // ErroresInstrucciones(IDventanaMensajes, codigoError, pcb);
        return 1; // Fin de archivo
    }


    //================================= INSTRUCCIONES PARA MOV =================================
    else if (strcmp(instruccion, "MOV") == 0 && ( (strcmp(valor, "AX") == 0) ||
                                                  (strcmp(valor, "BX") == 0) ||
                                                  (strcmp(valor, "CX") == 0) ||
                                                  (strcmp(valor, "DX") == 0) ||
                                                  (valor_numerico) == 0)) {
    //MoV Ax -12                                                    
        valor_numerico = EsDigito(valor); // Comprueba si el valor es un registro o un número
        
        // Si el valor es un número, entonces se puede hacer la operacion
        if (valor_numerico == 1) 
         {
            MOV(IDventanaMensajes, registro, valor, pcb);       
        }

        // Si el valor es un registro, entonces se debe comprobar si es un registro válido
        // Si el registro es válido, entonces se puede hacer la operación
        //MoV Ax Riata 
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
