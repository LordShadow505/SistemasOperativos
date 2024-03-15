#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "kbhit.h"
#include "InstruccionesRegistros.h"
#include "lista.h"

char ArchivoCargado[200]; // variable global de ArchivoCargado
char PIDKill[200]; //
int MAXQUANTUM = 5;
int Delay = 300000;
int codigoError = 0;
int ContadorHistorial  = 0;

/**
 * VentanaMensajes
 * 
 * Parámetros:
 * - IDventanaMensajes: Puntero a la ventana de mensajes.
 * - mensaje: Mensaje a mostrar en la ventana.
 * 
 * Objetivo:
 * - Mostrar un mensaje en la ventana de mensajes.
 * 
 * Descripción:
 * - Esta función configura el color de fondo de la ventana de mensajes y dibuja un borde alrededor de ella.
 * - Imprime el título "MENSAJES" en la parte superior de la ventana.
 * - Muestra el mensaje proporcionado en la posición (3, 2) de la ventana.
 * - Actualiza la ventana de mensajes y vuelve a dibujar el borde.
 */
void VentanaMensajes(WINDOW *IDventanaMensajes, char *mensaje)
{
    int height, width;
    getmaxyx(stdscr, height, width);
    wbkgd(IDventanaMensajes, COLOR_PAIR(2));
    box(IDventanaMensajes, 0, 0);
    mvwprintw(IDventanaMensajes, 0, 2, "MENSAJES");
    mvwprintw(IDventanaMensajes, 3, 2, "%s", mensaje);
    wrefresh(IDventanaMensajes);
    box(IDventanaMensajes, 0, 0);
}

/**
 * VentanaPrompt
 * 
 * Parámetros:
 * - IDventanaPromt: Puntero a la ventana de prompt.
 * - NumLinea: Número de línea en la ventana.
 * - ComandoIngresado: Comando ingresado por el usuario.
 * 
 * Objetivo:
 * - Mostrar el prompt en la ventana de prompt.
 * 
 * Descripción:
 * - Esta función configura el color de fondo de la ventana de prompt y dibuja un borde alrededor de ella.
 * - Imprime el título "COMANDOS" en la parte superior de la ventana.
 * - Muestra el número de línea y el comando ingresado en la posición correspondiente de la ventana.
 * - Actualiza la ventana de prompt y vuelve a dibujar el borde.
 */
void VentanaPrompt(WINDOW *IDventanaPromt, int NumLinea, char *ComandoIngresado)
{
    int height, width;
    getmaxyx(IDventanaPromt, height, width);
    wbkgd(IDventanaPromt, COLOR_PAIR(1));
    box(IDventanaPromt, 0, 0);
    mvwprintw(IDventanaPromt, 0, 2, "COMANDOS"); // Imprimir texto en la ventana
    mvwprintw(IDventanaPromt, NumLinea + 1, 2, "%d>> %s", NumLinea, ComandoIngresado);
    wrefresh(IDventanaPromt);
}


/**
 * VentanaRegistros
 *  
 * Parámetros:
 * - IDventanaRegistros: Puntero a la ventana de registros.
 * - pcb: Puntero a la estructura PCB.
 *  
 * Objetivo:
 * - Mostrar los registros del PCB en la ventana de registros.
 *  
 * Descripción:
 * - Esta función configura el color de fondo de la ventana de registros y dibuja un borde alrededor de ella.
 * - Imprime el título "REGISTROS" en la parte superior de la ventana.
 * - Muestra los valores de los registros AX, BX, CX, DX, PC, IR, PID y nombre del archivo en la ventana.
 * - Actualiza la ventana de registros y vuelve a dibujar el borde.
*/
void VentanaRegistros(WINDOW *IDventanaRegistros, struct PCB *pcb)
{
    // Obtener el valor de PID
    int height, width;
    getmaxyx(stdscr, height, width);
    wbkgd(IDventanaRegistros, COLOR_PAIR(3));
    box(IDventanaRegistros, 0, 0);
    mvwprintw(IDventanaRegistros, 0, 2, "REGISTROS");
    mvwprintw(IDventanaRegistros, 2, 5, "AX: %d        ", pcb->AX);
    mvwprintw(IDventanaRegistros, 2, 20, "BX: %d        ", pcb->BX);
    mvwprintw(IDventanaRegistros, 4, 5, "CX: %d        ", pcb->CX);
    mvwprintw(IDventanaRegistros, 4, 20, "DX: %d        ", pcb->DX);
    mvwprintw(IDventanaRegistros, 6, 5, "PC: %d        ", pcb->PC);
    mvwprintw(IDventanaRegistros, 6, 20, "IR: %s  ", pcb->IR);
    mvwprintw(IDventanaRegistros, 8, 5, "PID: %d        ", pcb->PID);
    mvwprintw(IDventanaRegistros, 10, 5, "NAME: %s        ", pcb->fileName);
    mvwprintw(IDventanaRegistros, 15, 5, "DELAY: %d        ", Delay);
    mvwprintw(IDventanaRegistros, 17, 5, "Codigo de Error: %d        ", codigoError);
    wrefresh(IDventanaRegistros);
    box(IDventanaRegistros, 0, 0);
}



/**
 * ImprimirEjecutandose
 * 
 * Parámetros:
 * - IDventanaProcesos: Puntero a la ventana de procesos.
 * - PrimerNodo: Puntero al primer nodo de la lista de PCB.
 * 
 * Objetivo:
 * - Imprimir la información del proceso en ejecución en la ventana de procesos.
 * 
 * Descripción:
 * - Esta función muestra la información del proceso en ejecución en la ventana de procesos.
 * - La información impresa incluye el PID, nombre del archivo y los valores de los registros AX, BX, CX, DX, PC e IR.
 * - Si no hay ningún proceso en ejecución, la ventana de procesos se muestra vacía.
 * - La ventana de procesos se actualiza y se vuelve a dibujar el borde después de imprimir la información.
 */
void ImprimirEjecutandose(WINDOW *IDventanaProcesos, struct PCB *PrimerNodo)
{
    struct PCB *primerNodo = PrimerNodo; // Guarda referencia al primer nodo

    // Limpiar la sección de Proceso en Ejecución
    mvwprintw(IDventanaProcesos, 3, 2, "                                                                                               ");
    mvwprintw(IDventanaProcesos, 4, 2, "                                                                                               ");
    if (primerNodo != NULL)
    {
        wbkgd(IDventanaProcesos, COLOR_PAIR(4));
        box(IDventanaProcesos, 0, 0);
        mvwprintw(IDventanaProcesos, 0, 2, "LISTA DE PROCESOS");
        mvwprintw(IDventanaProcesos, 2, 2, "Proceso en Ejecucion: ");
        mvwprintw(IDventanaProcesos, 3, 2, "PID: %d, FileName: %s, AX: %d, BX: %d, CX: %d, DX: %d, PC: %d, IR: %s", primerNodo->PID, primerNodo->fileName, primerNodo->AX, primerNodo->BX, primerNodo->CX, primerNodo->DX, primerNodo->PC, primerNodo->IR);
        wrefresh(IDventanaProcesos);
        box(IDventanaProcesos, 0, 0);
    }
}

/**
 * ImprimirListos
 * 
 * Parámetros:
 * - IDventanaProcesos: Puntero a la ventana de procesos.
 * - PrimerNodo: Puntero al primer nodo de la lista de PCB.
 * 
 * Objetivo:
 * - Imprimir la información de los procesos en espera en la ventana de procesos.
 * 
 * Descripción:
 * - Esta función recorre la lista de PCB y muestra la información de los procesos en espera en la ventana de procesos.
 * - La información impresa incluye el PID, nombre del archivo, valores de los registros AX, BX, CX, DX, PC e IR.
 * - La ventana de procesos se actualiza y se vuelve a dibujar el borde después de imprimir la información.
 * - Si no hay procesos en espera, se muestra un mensaje vacío en la ventana de procesos.
 */
void ImprimirListos(WINDOW *IDventanaProcesos, struct PCB *PrimerNodo)
{
    struct PCB *NodoActual = PrimerNodo;
    int i = 0;
    int contador = 1; // Contador de procesos en espera

    if (NodoActual != NULL)
    {
        wbkgd(IDventanaProcesos, COLOR_PAIR(4));
        box(IDventanaProcesos, 0, 0);
        mvwprintw(IDventanaProcesos, 0, 2, "LISTA DE PROCESOS");
        mvwprintw(IDventanaProcesos, 10, 2, "Procesos en Espera");
        while (NodoActual != NULL)
        {
            mvwprintw(IDventanaProcesos, 10, 2, "Procesos en Espera");

            wmove(IDventanaProcesos, 11 + i, 2);
            wclrtoeol(IDventanaProcesos);
            mvwprintw(IDventanaProcesos, 11 + i, 2, "PID: %d, FileName: %s, AX: %d, BX: %d, CX: %d, DX: %d, PC: %d, IR: %s",
                      NodoActual->PID, NodoActual->fileName, NodoActual->AX, NodoActual->BX, NodoActual->CX, NodoActual->DX, NodoActual->PC, NodoActual->IR);
            NodoActual = NodoActual->sig;
            i++;
            contador++; // Incrementar el contador de procesos en espera
            mvwprintw(IDventanaProcesos, 10 + contador, 2, "                                                                                                     ");
                                                                
        }

        wrefresh(IDventanaProcesos);
        box(IDventanaProcesos, 0, 0);
    }
    else if (NodoActual == NULL)
    {
         mvwprintw(IDventanaProcesos, 11, 2, "                                                                                                                ");
    }
}

/**
 * ImprimirTerminados
 * 
 * Parámetros:
 * - IDventanaProcesos: Puntero a la ventana de procesos.
 * - PrimerNodo: Puntero al primer nodo de la lista de PCB.
 * 
 * Objetivo:
 * - Imprimir la información de los procesos terminados en la ventana de procesos.
 * 
 * Descripción:
 * - Esta función recorre la lista de PCB y muestra la información de los procesos terminados en la ventana de procesos.
 * - La información impresa incluye el PID, nombre del archivo, valores de los registros AX, BX, CX, DX, PC e IR.
 * - La ventana de procesos se actualiza y se vuelve a dibujar el borde después de imprimir la información.
 */
void ImprimirTerminados(WINDOW *IDventanaProcesos, struct PCB *PrimerNodo)
{
    int height, width;
    getmaxyx(stdscr, height, width); // Obtener las dimensiones de la pantalla
    struct PCB *NodoActual = PrimerNodo;
    int i = 0;

    if (NodoActual != NULL)
    {
        wbkgd(IDventanaProcesos, COLOR_PAIR(4));                              // Establecer el color de fondo de la ventana
        box(IDventanaProcesos, 0, 0);                                         // Dibujar el borde de la ventana
        mvwprintw(IDventanaProcesos, 0, 2, "LISTA DE PROCESOS");              // Título
        mvwprintw(IDventanaProcesos, height / 2, 2, "Procesos Terminados: "); // Título de la sección de procesos terminados

        // Recorrer la lista de procesos terminados e imprimir la información de cada uno
        while (NodoActual != NULL)
        {
            mvwprintw(IDventanaProcesos, height / 2 + 1 + i, 2, "PID: %d, FileName: %s, AX: %d, BX: %d, CX: %d, DX: %d, PC: %d, IR: %s",
                      NodoActual->PID, NodoActual->fileName, NodoActual->AX, NodoActual->BX, NodoActual->CX, NodoActual->DX, NodoActual->PC, NodoActual->IR);
            NodoActual = NodoActual->sig;
            i++;
        }

        wrefresh(IDventanaProcesos);  // Actualizar la ventana
        box(IDventanaProcesos, 0, 0); // Volver a dibujar el borde de la ventana
    }
}


/*
---------- BIBLIA PARA LOS CODIGOS DE ERROR COMANDOS -----------
        - 10X: Estados de Comandos
        - 14X: Errores de Comandos
        - 20X: Estados de Archivos
        - 24X: Errores de Archivos
        - 30X: Estados de Kill
        - 34X: Errores de Kill
*/
/**
 * ManejadorErroresComandos
 * 
 * Parámetros:
 * - IDventanaPrompt: Puntero a la ventana del prompt.
 * - IDventanaMensajes: Puntero a la ventana de mensajes.
 * - Opcion: Opción que indica el tipo de error a manejar.
 * - ComandoIngresado: Cadena de caracteres que contiene el comando ingresado por el usuario.
 * - j: Puntero al contador de caracteres del comando ingresado.
 * - NumLinea: Puntero al número de línea actual.
 * 
 * Objetivo:
 * - Manejar los errores relacionados con comandos y mostrar mensajes correspondientes en la ventana de mensajes.
 * 
 * Descripción:
 * - La función ManejadorErroresComandos recibe una opción que indica el tipo de error a manejar y muestra un mensaje correspondiente en la ventana de mensajes.
 * - Opción 201: Mensaje de estado para ejecutar el comando "load".
 * - Opción 242: Mensaje de error para archivo inexistente.
 * - Opción 243: Mensaje de error para exceso de parámetros en el comando "cargar".
 * - Opción 244: Mensaje de error para falta de nombre de archivo en el comando "cargar".
 * - Opción 301: Mensaje de estado para ejecutar el comando "kill".
 * - Opción 341: Mensaje de error para parámetro no entero en el comando "kill".
 * - Opción 141: Mensaje de error para comando no reconocido.
 * 
 * 
 * Retorno:
 * - 0: Indica que no se requiere realizar ninguna acción adicional.
 * - 201: Indica que se debe ejecutar el comando "load".
 * - 301: Indica que se debe ejecutar el comando "kill".
 */
int ManejadorErroresComandos(WINDOW *IDventanaPrompt, WINDOW *IDventanaMensajes, int Opcion, char *ComandoIngresado, int *j, int *NumLinea)
{
    int EstadoArchivo = 0;
    mvwprintw(IDventanaMensajes, 0, 2, "MENSAJES");
    switch (Opcion)
    {
    // Estados de Archivos (20X)
    case 201:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "Estado: Ejecutando Load");
        memset(ComandoIngresado, 0, (*j));
        (*j) = 0;
        EstadoArchivo = AbrirYVerificar(ArchivoCargado);
        if (EstadoArchivo == 241)
        {
            VentanaMensajes(IDventanaMensajes, "                                                                        ");
            VentanaMensajes(IDventanaMensajes, "Error: No se pudo abrir el archivo");
        }
        else
        {
            VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
            return 201;
        }
        VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
        break;

    // Errores de Archivos (24X)
    case 242:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "Error: El archivo no existe");
        memset(ComandoIngresado, 0, (*j));
        (*j) = 0;
        VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
        break;

    case 243:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "Error: Hay demasiados parametros, uso: cargar <nombre_archivo>");
        memset(ComandoIngresado, 0, (*j));
        (*j) = 0;
        VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
        break;

    case 244:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "Error: No se ingresó un nombre de ArchivoCargado, uso: cargar <nombre_archivo>");
        memset(ComandoIngresado, 0, (*j));
        (*j) = 0;
        VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
        break;
        
    // Estados de Kill (30X)
    case 301: 
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "Estado: Ejecutando Kill");
        memset(ComandoIngresado, 0, (*j));
        (*j) = 0;
        VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
        return 301;
        break;

    // Errores de Kill (34X)
    case 341:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "Error: Se necesita un numero entero como parametro");
        memset(ComandoIngresado, 0, (*j));
        (*j) = 0;
        VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
        break;
    
    case 342:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "Error: Hay demasiados parametros, uso: kill <numero de PID>");
        memset(ComandoIngresado, 0, (*j));
        (*j) = 0;
        VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
        break;

    case 343: 
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "Error: PID no encontrado");
        memset(ComandoIngresado, 0, (*j));
        (*j) = 0;
        VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
        break;

    // Errores de Comandos (14X)
    case 141:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        char Frase[100] = "Error: Comando no reconocido: >> ";
        strcat(Frase, ComandoIngresado);
        VentanaMensajes(IDventanaMensajes, Frase);
        memset(ComandoIngresado, 0, (*j));
        (*j) = 0;
        VentanaPrompt(IDventanaPrompt, *NumLinea, ComandoIngresado);
        break;
    }

    wrefresh(IDventanaMensajes);
    wrefresh(IDventanaPrompt);
    return 0;
}

/*
---------- BIBLIA PARA LOS CODIGOS DE ERROR INSTRUCCIONES -----------
        - 40X: Error de Instrucciones
*/
/**
 * ManejadorErroresArchivo
 * 
 * Parámetros:
 * - IDventanaMensajes: Puntero a la ventana de mensajes.
 * - linea: Cadena de caracteres que contiene la línea de código.
 * - Opcion: Opción que indica el tipo de error a manejar.
 * 
 * Objetivo:
 * - Manejar los errores relacionados con archivos y mostrar mensajes correspondientes en la ventana de mensajes.
 * 
 * Descripción:
 * - La función ManejadorErroresArchivo recibe una opción que indica el tipo de error a manejar y muestra un mensaje correspondiente en la ventana de mensajes.
 * - Opción 1: Mensaje de terminación de la ejecución del programa.
 * - Opción 401: Mensaje de error para operación no reconocida.
 * - Opción 402: Mensaje de error para registro no reconocido.
 * - Opción 403: Mensaje de error para valor o registro inválido.
 * - Opción 404: Mensaje de error para división entre 0.
 * - Opción 405: Mensaje de estado para terminar la ejecución del programa.
 * - Opción 406: Mensaje de error para instrucción con demasiados argumentos.
 * - Opción 407: Mensaje de error para instrucción con más registros de los esperados.
 * - Opción 408: Mensaje de error para instrucción con falta de registro o valor.
 * - Opción 409: Mensaje de error para parámetros insuficientes.
 */
void ManejadorErroresArchivo(WINDOW *IDventanaMensajes, char linea[100], int Opcion)
{
    char Frase[100];
    char operacion[60];
    char p1[60];
    char p2[60];
    char error[60];

    mvwprintw(IDventanaMensajes, 0, 2, "MENSAJES");
    VentanaMensajes(IDventanaMensajes, "                                                                        ");
    sscanf(linea, "%s %s %s %s", operacion, p1, p2, error);
    switch (Opcion)
    {
    case 1:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "END: Terminando la ejecución del programa...");

        break;
    case 401:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        strncpy(Frase, "Error: Operacion no reconocida >>", 100);
        strcat(Frase, operacion);
        strcat(Frase, " ");
        strcat(Frase, p1);
        strcat(Frase, " ");
        strcat(Frase, p2);
        VentanaMensajes(IDventanaMensajes, Frase);
        break;
    case 402:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        strncpy(Frase, "Error: Registro no reconocido >>", 100);
        strcat(Frase, operacion);
        strcat(Frase, " ");
        strcat(Frase, p1);
        strcat(Frase, " ");
        strcat(Frase, p2);
        VentanaMensajes(IDventanaMensajes, Frase);
        break;
    case 403:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        strncpy(Frase, "Error: No hay un valor o registro valido >>", 100);
        strcat(Frase, operacion);
        strcat(Frase, " ");
        strcat(Frase, p1);
        strcat(Frase, " ");
        strcat(Frase, p2);
        VentanaMensajes(IDventanaMensajes, Frase);
        break;
    case 404:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        strncpy(Frase, "Error: Division entre 0 >>", 100);
        strcat(Frase, operacion);
        strcat(Frase, " ");
        strcat(Frase, p1);
        strcat(Frase, " ");
        strcat(Frase, p2);
        VentanaMensajes(IDventanaMensajes, Frase);
        break;
    case 405:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        VentanaMensajes(IDventanaMensajes, "Estado: Terminando la ejecución del programa...");
        break;
    case 406:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        strncpy(Frase, "Error: Hay muchos argumentos en la instruccion >>", 100);
        strcat(Frase, operacion);
        strcat(Frase, " ");
        strcat(Frase, p1);
        strcat(Frase, " ");
        strcat(Frase, p2);
        VentanaMensajes(IDventanaMensajes, Frase);
        break;
    case 407:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        strncpy(Frase, "Error: Hay mas registros de los esperados en la instruccion >>", 100);
        strcat(Frase, operacion);
        strcat(Frase, " ");
        strcat(Frase, p1);
        strcat(Frase, " ");
        strcat(Frase, p2);
        strcat(Frase, " ");
        strcat(Frase, "+ ?");
        VentanaMensajes(IDventanaMensajes, Frase);
        break;
    case 408:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        strncpy(Frase, "Error: Falta un registro o valor en la instruccion >>", 100);
        strcat(Frase, operacion);
        
        VentanaMensajes(IDventanaMensajes, Frase);
        break;
    case 409:
        VentanaMensajes(IDventanaMensajes, "                                                                        ");
        strncpy(Frase, "Error: Parametros insuficientes >>", 100);
        strcat(Frase, operacion);
        strcat(Frase, " ");
        strcat(Frase, p1);
        strcat(Frase, " ");
        strcat(Frase, p2);
        VentanaMensajes(IDventanaMensajes, Frase);
        break;
    }
}

/**
 * ManejadorErroresEjecucion
 * 
 * Parámetros:
 * - IDventanaMensajes: Puntero a la ventana de mensajes.
 * - Opcion: Opción que indica el tipo de error a manejar.
 * 
 * Objetivo:
 * - Manejar los errores de ejecución y mostrar mensajes correspondientes en la ventana de mensajes.
 * 
 * Descripción:
 * - La función ManejadorErroresEjecucion recibe una opción que indica el tipo de error a manejar y muestra un mensaje correspondiente en la ventana de mensajes.
 * - Opción 1: Mensaje de error para PID inexistente.
 * - Opción 2: Mensaje de estado para matar un proceso.
 */
void ManejadorErroresEjecucion(WINDOW *IDventanaMensajes, int Opcion)
{
  // Seleccionar el mensaje a mostrar según la opción.
  switch (Opcion)
  {
    // Opción 1: Mensaje de error para PID inexistente.
    case 1:
      // Borrar la ventana de mensajes.
      VentanaMensajes(IDventanaMensajes, " ");
      // Mostrar el mensaje de error.
      VentanaMensajes(IDventanaMensajes, "                                                                        ");
      VentanaMensajes(IDventanaMensajes, "Error: No existe un proceso con ese PID");
      break;

    // Opción 2: Mensaje de estado para matar un proceso.
    case 2:
      // Borrar la ventana de mensajes.
      VentanaMensajes(IDventanaMensajes, " ");
      // Mostrar el mensaje de estado.
      VentanaMensajes(IDventanaMensajes, "                                                                        ");
      VentanaMensajes(IDventanaMensajes, "Estado: Matando proceso...");
      break;
  }

  // Refrescar la ventana de mensajes.
    wrefresh(IDventanaMensajes);
}


/**
 * Enter
 * 
 * Parámetros:
 * - ComandoIngresado: Puntero al comando ingresado por el usuario.
 * 
 * Valor de retorno:
 * - 0 si la función se ejecutó correctamente.
 * - 666 si el comando es "SALIR", "EXIT" o "MORIR".
 * - 243 si se proporcionaron demasiados parámetros en el comando "CARGAR" o "LOAD".
 * - 244 si falta el nombre del archivo en el comando "CARGAR" o "LOAD".
 * - 201 si el archivo se cargó exitosamente en el comando "CARGAR" o "LOAD".
 * - 242 si el archivo especificado no existe en el comando "CARGAR" o "LOAD".
 * - 341 si falta el PID del proceso en el comando "KILL" o "MATAR".
 * - 301 si el proceso se eliminó exitosamente en el comando "KILL" o "MATAR".
 * - 141 si el comando no es reconocido.
 * 
 * Objetivo:
 * - Manejar los comandos ingresados por el usuario.
 * 
 * Descripción:
 * - La función Enter recibe el comando ingresado por el usuario y realiza diferentes acciones según el comando. 
 * - Si el comando es "SALIR", "EXIT" o "MORIR", la función retorna 666 para indicar que el programa debe finalizar.
 * - Si el comando es "CARGAR" o "LOAD", la función intenta leer el nombre del archivo a cargar y verifica si existe. 
 *   - Si se proporcionan demasiados parámetros, retorna 243.
 *   - Si falta el nombre del archivo, retorna 244.
 *   - Si el archivo existe, guarda el nombre del archivo cargado y retorna 201.
 *   - Si el archivo no existe, retorna 242.
 * - Si el comando es "KILL" o "MATAR", la función intenta leer el PID del proceso a eliminar y verifica si es válido.
 *   - Si falta el PID del proceso, retorna 341.
 *   - Si el PID es válido, guarda el PID del proceso a eliminar y retorna 301.
 *   - Si el PID no es válido, retorna 341.
 * - Si el comando no es reconocido, retorna 141.
 * - La función retorna 0 si se ejecuta correctamente.
 */
int Enter(char *ComandoIngresado)
{
  // Definir variables
  char cmd[100], param1[100] = "", param2[100] = "";

  // Extraer el primer comando usando sscanf
  sscanf(ComandoIngresado, "%s", cmd);
  Mayusculainador(cmd); //Mayusculainador convierte el comando a mayúsculas

  //------------------------SALIR (EXIT, MORIR)------------------------
  // Comprobar si el comando es "SALIR", "EXIT" o "MORIR" (ignorar mayúsculas/minúsculas)
  if ((strcmp(cmd, "SALIR") == 0) || (strcmp(cmd, "EXIT") == 0) || (strcmp(cmd, "MORIR") == 0)) {
    return 666; // Fuga pa' Sahuayo
  }

  //------------------------CARGAR (LOAD)------------------------
  // Comprobar si el comando es "CARGAR" o "LOAD" (ignorar mayúsculas/minúsculas)
  if ((strcmp(cmd, "CARGAR") == 0) || (strcmp(cmd, "LOAD") == 0)) {
    param1[0] = '\0'; // Limpiar el primer parámetro

    // Intentar leer el nombre del archivo a cargar
    sscanf(ComandoIngresado, "%*s %s", param1);

    // Comprobar si se proporciona un segundo parámetro (no permitido)
    if (sscanf(ComandoIngresado, "%*s %s %s", param1, param2) == 2) {
      return 243; // Error: se proporcionaron demasiados parámetros
    }

    // Comprobar si no se proporcionó ningún nombre de archivo
    if (param1[0] == '\0') {
      return 244; // Error: falta el nombre del archivo
    }

    // Verificar si el archivo existe
    else if (access(param1, F_OK) != -1) {
      strcpy(ArchivoCargado, param1); // Guardar el nombre del archivo cargado
      return 201; // Archivo cargado exitosamente
    }

    else {
      return 242; // Error: el archivo no existe
    }
  }

  //------------------------KILL (MATAR)------------------------
  // Comprobar si el comando es "KILL" o "MATAR" (ignorar mayúsculas/minúsculas)
  if ((strcmp(cmd, "KILL") == 0) || (strcmp(cmd, "MATAR") == 0)) {
    param1[0] = '\0'; // Limpiar el primer parámetro

    // Intentar leer el PID del proceso a eliminar
    sscanf(ComandoIngresado, "%*s %s", param1);

    // Comprobar si no se proporcionó ningún PID
    if (param1[0] == '\0') {
      return 341; // Error: falta el PID del proceso
    }

    else if(sscanf(ComandoIngresado, "%*s %s %s", param1, param2) == 2){
      return 342; // Error: se proporcionaron demasiados parámetros
    }

    // Convertir el PID a entero (asumiendo que param1 contiene un número válido)
    else if (atoi(param1)){
      strcpy(PIDKill, param1); // Guardar el PID del proceso a eliminar
      return 301; // Proceso eliminado exitosamente
    }

    else {
      return 343; // Error: PID del proceso no válido
    }
  }

  // Comando desconocido
  else {
    return 141; // Error: comando no reconocido
  }

  return 0; // Alcanzado solo si hay errores de compilación (generalmente nunca se retorna)
}


/**
 * LeerArchivo
 * 
 * Parámetros:
 * - IDventanaMensajes: Puntero a la ventana de mensajes.
 * - IDventanaRegistros: Puntero a la ventana de registros.
 * - Ejecucion: Puntero al puntero del PCB en ejecución.
 * - SePuedeLeer: Puntero a la variable que indica si se puede leer el archivo.
 * - n_archivo: Puntero al archivo que se está leyendo.
 * - Quantum: Puntero al valor del quantum actual.
 * - ProcesoCargado: Puntero a la variable que indica si hay un proceso cargado.
 * - Listos: Puntero al puntero de la lista de procesos listos.
 * 
 * Valor de retorno:
 * - 0 si la función se ejecutó correctamente.
 * 
 * Objetivo:
 * - Leer una línea del archivo y ejecutarla como instrucción.
 * 
 * Descripción:
 * - La función LeerArchivo lee una línea del archivo especificado por n_archivo y la guarda en el IR (Instruction Register) del PCB actual. Si la lectura es exitosa, se convierte la línea a mayúsculas y se ejecuta la instrucción utilizando la función EjecutarInstruccion. Luego, se incrementa el contador de programa (PC) y el quantum actual. La función también actualiza la ventana de registros con la información del PCB actual. Si se produce un error al ejecutar la instrucción, se cierra el archivo, se indica que no se puede seguir leyendo, se reinicia el quantum y se muestra un mensaje de error al usuario. Si se llega al final del archivo, se cierra el archivo, se indica que no se puede seguir leyendo, se reinicia el quantum y se muestra un mensaje de fin de archivo al usuario. Si el quantum actual alcanza el máximo permitido, se indica que no se puede seguir leyendo, se indica que no hay un proceso cargado, se inserta el PCB actual en la lista de procesos listos, se elimina el proceso actual y se reinicia el quantum. Finalmente, la función se duerme durante un tiempo determinado para evitar un uso excesivo de la CPU.
 */
int LeerArchivo(WINDOW *IDventanaMensajes, WINDOW *IDventanaRegistros, struct PCB **Ejecucion, int *SePuedeLeer, FILE *n_archivo, int *Quantum, int *ProcesoCargado, struct PCB **Listos)
{
    int codigoError = 0;

    // Lee una línea del archivo y la guarda en el IR del PCB actual.
    // IR = Instruction Register, o sea el registro donde se guardan las instrucciones.
    // Si la lectura es exitosa (no se llega al final del archivo), se continúa.
    if (fgets((*Ejecucion)->IR, sizeof((*Ejecucion)->IR), n_archivo) != NULL)
    {
        // Convierte la línea a mayúsculas para facilitar la comparación con las instrucciones.
        Mayusculainador((*Ejecucion)->IR);

        // Ejecuta la instrucción que se encuentra en el IR del PCB actual.
        // Se pasan como parámetros las ventanas de mensajes y registros, el PCB actual,
        // el código de la instrucción y el quantum actual.
        codigoError = EjecutarInstruccion(IDventanaRegistros, IDventanaMensajes, *Ejecucion, (*Ejecucion)->IR);

        // Se incrementa el contador de programa del PCB actual.
        (*Ejecucion)->PC++;

        // Se incrementa el quantum actual.
        (*Quantum)++;

        // Se actualiza la ventana de registros con la información del PCB actual.
        VentanaRegistros(IDventanaRegistros, *Ejecucion);

        // Si se produjo un error al ejecutar la instrucción, se cierra el archivo,
        // se indica que no se puede seguir leyendo, se reinicia el quantum y se muestra
        // un mensaje de error al usuario.
        if (codigoError != 0)
        {
            fclose(n_archivo);
            (*SePuedeLeer) = 0;
            (*Quantum) = 0;
            ManejadorErroresArchivo(IDventanaMensajes, (*Ejecucion)->IR, codigoError);
        }
    }
    else
    {
        // Si se llegó al final del archivo, se cierra el archivo, se indica que no se puede
        // seguir leyendo, se reinicia el quantum y se muestra un mensaje de fin de archivo
        // al usuario.
        fclose(n_archivo);
        (*SePuedeLeer) = 0;
        (*Quantum) = 0;
        ManejadorErroresArchivo(IDventanaMensajes, (*Ejecucion)->IR, 1);
    }

    // Si el quantum actual alcanzó el máximo permitido, se indica que no se puede seguir
    // leyendo, se indica que no hay un proceso cargado, se inserta el PCB actual en la
    // lista de listos, se elimina el proceso actual y se reinicia el quantum.
    if ((*Quantum) >= MAXQUANTUM)
    {
        (*SePuedeLeer) = 0;
        (*ProcesoCargado) = 0;
        InsertarNuevo(Listos, *Ejecucion);
        Kill(Ejecucion, (*Ejecucion)->PID);
        (*Quantum) = 0;
    }

    // Se duerme el proceso durante un tiempo determinado para evitar un uso excesivo
    // de la CPU.
    usleep(Delay);

    // Se retorna 0 para indicar que la función se ejecutó correctamente.
    return 0;
}

/**
 * ManejadorProcesos
 * 
 * Parámetros:
 * - IDventanaMensajes: Puntero a la ventana de mensajes.
 * - Listos: Puntero al puntero de la lista de procesos listos.
 * - Terminados: Puntero al puntero de la lista de procesos terminados.
 * - Ejecucion: Puntero al puntero del proceso en ejecución.
 * - SePuedeLeer: Puntero a la variable que indica si se puede leer el archivo.
 * - ProcesoCargado: Puntero a la variable que indica si hay un proceso cargado.
 * - EjecucionComandos: Entero que indica el comando de ejecución.
 * - ArchivoCargadoValido: Cadena de caracteres que almacena el nombre del archivo cargado.
 * - EstadoProcesos: Entero que indica el estado de los procesos.
 * 
 * Valor de retorno:
 * - Ninguno.
 * 
 * Objetivo:
 * - Manejar los procesos, incluyendo la carga, eliminación y finalización de procesos.
 * 
 * Descripción:
 * - La función ManejadorProcesos se encarga de manejar los procesos en el sistema operativo. Recibe como parámetros un puntero a la ventana de mensajes (IDventanaMensajes), un puntero al puntero de la lista de procesos listos (Listos), un puntero al puntero de la lista de procesos terminados (Terminados), un puntero al puntero del proceso en ejecución (Ejecucion), un puntero a la variable que indica si se puede leer el archivo (SePuedeLeer), un puntero a la variable que indica si hay un proceso cargado (ProcesoCargado), un entero que indica el comando de ejecución (EjecucionComandos), una cadena de caracteres que almacena el nombre del archivo cargado (ArchivoCargadoValido) y un entero que indica el estado de los procesos (EstadoProcesos).
 * - La función realiza diferentes acciones dependiendo del comando de ejecución recibido:
 *   - Si el comando de ejecución es 201, se carga un nuevo proceso en la lista de procesos listos. El nombre del archivo cargado se almacena en la variable ArchivoCargadoValido.
 *   - Si el comando de ejecución es 301, se elimina un proceso de la lista de procesos listos o de ejecución, y se coloca en la lista de procesos terminados. Si el proceso no se encuentra en ninguna lista, se muestra un mensaje de error.
 *   - Si no hay un proceso cargado, se extrae el primer proceso de la lista de procesos listos y se marca como cargado y habilita la lectura.
 *   - Si hay un proceso cargado y no se puede leer el archivo, se coloca el proceso en la lista de procesos terminados y se marca como no cargado.
 */
void ManejadorProcesos(WINDOW *IDventanaMensajes, struct PCB **Listos, struct PCB **Terminados, struct PCB **Ejecucion, int *SePuedeLeer, int *ProcesoCargado, int EjecucionComandos, char ArchivoCargadoValido[200], int EstadoProcesos)
{
  // Verifica si se está cargando un nuevo proceso.
  if (EjecucionComandos == 201)
  {
    // Se carga un nuevo proceso en la lista de procesos listos.
    strcpy(ArchivoCargadoValido, ArchivoCargado);
    Insertar(Listos, ArchivoCargadoValido);
  }
  // Verifica si se está eliminando un proceso.
  else if (EjecucionComandos == 301)
  {
    // Se elimina un proceso de la lista de procesos listos o de ejecución, y se coloca en la lista de procesos terminados.
    EstadoProcesos = KillFinal(Listos, atoi(PIDKill), Terminados);
    if (EstadoProcesos != 0)
    {
      EstadoProcesos = KillFinal(Ejecucion, atoi(PIDKill), Terminados);
      if (EstadoProcesos != 0)
      {
        ManejadorErroresEjecucion(IDventanaMensajes, 1);

      }
      else
      {
        *SePuedeLeer = 0;
        *ProcesoCargado = 0;
        ManejadorErroresEjecucion(IDventanaMensajes, 2);
      }
    }
  }
  
  // Verifica si no hay un proceso cargado.
  if (*ProcesoCargado == 0)
  {
    // Si no hay un proceso cargado, se extrae el primer proceso de la lista de procesos listos y se marca como cargado y habilita la lectura.
    *Ejecucion = listaExtraeInicio(Listos);
    if (*Ejecucion != NULL)
    {
      *ProcesoCargado = 1;
      *SePuedeLeer = 1;
    }
    else{
      *ProcesoCargado = 0;
      *SePuedeLeer = 0;
    }
  }


  // Verifica si hay un proceso cargado y no se puede leer el archivo.
  if (*ProcesoCargado == 1)
  {
    if (*SePuedeLeer == 0)
    {
      // Si hay un proceso cargado y no se puede leer el archivo, se coloca el proceso en la lista de procesos terminados y se marca como no cargado.
      ListaInsertarFinal(Terminados, *Ejecucion);
      *ProcesoCargado = 0;
    }
  }
}


/**
 * LineaComandos
 * 
 * Parámetros:
 * - IDventanaPromt: Puntero a la ventana de comandos.
 * - ComandoIngresado: Cadena de caracteres que almacena el comando ingresado por el usuario.
 * - j: Puntero a la posición actual en la cadena de caracteres del comando.
 * - NumLinea: Puntero al número de líneas en la ventana de comandos.
 * 
 * Valor de retorno:
 * - Entero que indica el estado de la función.
 * 
 * Objetivo:
 * - Manejar la entrada del usuario en la línea de comandos, procesando las teclas presionadas y actualizando la ventana de comandos.
 * 
 * Descripción:
 * - La función LineaComandos se encarga de manejar la entrada del usuario en la línea de comandos de la ventana de comandos. Recibe como parámetros un puntero a la ventana de comandos (IDventanaPromt), una cadena de caracteres que almacena el comando ingresado por el usuario (ComandoIngresado), un puntero a la posición actual en la cadena de caracteres del comando (j) y un puntero al número de líneas en la ventana de comandos (NumLinea).
 * - La función verifica si hay una tecla disponible utilizando la función kbhit(). Si hay una tecla disponible, obtiene la tecla presionada utilizando la función getch(). Luego, realiza diferentes acciones dependiendo de la tecla presionada:
 *   - Si se presiona Enter, agrega un caracter nulo al final del comando, incrementa el número de líneas y devuelve el estado "Enter".
 *   - Si se presiona Backspace y el cursor no está al inicio, decrementa el índice del caracter actual, elimina el caracter de la pantalla, mueve el cursor un caracter a la izquierda, borra el caracter anterior y 5 espacios más, y actualiza la pantalla.
 *   - Si se presiona la secuencia para las flechas, obtiene la siguiente tecla y realiza diferentes acciones dependiendo de la tecla presionada:
 *     - Si se presiona la flecha derecha y el retardo es menor a 800000 microsegundos, aumenta el retardo en 50000 microsegundos.
 *     - Si se presiona la flecha izquierda y el retardo es mayor a 0, disminuye el retardo en 50000 microsegundos.
 *   - Si se presiona cualquier otra tecla, almacena la tecla en el comando, agrega un caracter nulo al final, y incrementa el índice del caracter actual.
 * - Si el número de líneas supera el límite de 20, limpia todas las líneas de la ventana de comandos, restablece el número de líneas a 0 y actualiza la pantalla.
 * - Finalmente, devuelve el estado "Normal".
 */
int LineaComandos(WINDOW *IDventanaPromt, char *ComandoIngresado, int *j, int *NumLinea, char Historial[10][100])
{

  // Si hay una tecla disponible
  if (kbhit())
  {
    // Obtener la tecla presionada
    int Tecla = getch();

    // Si se presiona Enter
    if (Tecla == '\n')
    {
      // Agregar un caracter nulo al final del comando
      ComandoIngresado[*j] = '\0';

      // Incrementar el número de líneas
      (*NumLinea)++;

      // Devolver el estado "Enter"
      ContadorHistorial = 0;
      return Enter(ComandoIngresado);
    }

    // Si se presiona Backspace y el cursor no está al inicio
    else if (Tecla == 127 && *j > 0)
    {
      // Lógica para la tecla retroceso:
      // - Decrementar el índice del caracter actual
      (*j)--;

      // Eliminar el caracter de la pantalla
      ComandoIngresado[*j] = '\0';

      // Obtener las coordenadas del cursor
      int y, x;
      getyx(IDventanaPromt, y, x);

      // Mover el cursor un caracter a la izquierda
      wmove(IDventanaPromt, y, x - 1);

      // Borrar el caracter anterior y 5 espacios más, ¿Porque? Porque si
      mvwprintw(IDventanaPromt, y, x - 2, "                  ");

      // Actualizar la pantalla
      wrefresh(IDventanaPromt);
    }

    // Si se presiona la secuencia para las flechas
    else if (Tecla == 27)
    {
      // Obtener la siguiente tecla
      Tecla = getch();

      // Si se presiona la tecla '['
      if (Tecla == 91)
      {
        // Obtener la última tecla de la secuencia
        Tecla = getch();

        // Si se presiona la flecha derecha
        if (Tecla == 67)
        {
          // Si el retardo es menor a 800000 microsegundos
          if (Delay < 800000)
          {
            // Aumentar el retardo en 50000 microsegundos
            Delay = Delay + 50000;
          }
        }

        // Si se presiona la flecha izquierda
        else if (Tecla == 68)
        {
          // Si el retardo es mayor a 0
          if (Delay > 0)
          {
            // Disminuir el retardo en 50000 microsegundos
            Delay = Delay - 50000;
          }
        }

        else if (Tecla == 65)
        {
          ContadorHistorial++;
          if (*NumLinea > 0 && ContadorHistorial <= *NumLinea)
          {
            strcpy(ComandoIngresado, Historial[*NumLinea - ContadorHistorial]);
            *j = strlen(ComandoIngresado);
            mvwprintw(IDventanaPromt, *NumLinea +1, 5, "                                   ");
          }


        }
        else if (Tecla == 66)
        {
          ContadorHistorial--;
          if (ContadorHistorial > 0)
          {
            strcpy(ComandoIngresado, Historial[*NumLinea - ContadorHistorial]);
            *j = strlen(ComandoIngresado);
            mvwprintw(IDventanaPromt, *NumLinea +1, 5, "                                   ");
          }
        }
      }
    }

    // Si se presiona cualquier otra tecla
    else
    {
      // Almacenar la tecla en el comando
      ComandoIngresado[*j] = Tecla;

      // Agregar un caracter nulo al final
      ComandoIngresado[*j + 1] = '\0';

      // Incrementar el índice del caracter actual
      (*j)++;
    }

    // Si el número de líneas supera el límite
    if (*NumLinea >= 20)
    {
      // Limpiar todas las líneas
      for (int i = 0; i <= 20; i++)
      {
            wclear(IDventanaPromt);
            box(IDventanaPromt, 0, 0); // Dibujar un borde alrededor de la ventana
            mvwprintw(IDventanaPromt, 0, 2, "COMANDOS"); // Imprimir texto en la ventana
      }

      // Restablecer el número de líneas a 0
      *NumLinea = 0;
    }

    // Actualizar la pantalla
    wrefresh(IDventanaPromt);
  }

  // Devolver el estado "Normal"
  return 0;
}





/*
 * Main
 * 
 * Parámetros:
 * - Ninguno
 * 
 * Objetivo:
 * - Inicializar la pantalla de ncurses, definir colores, crear ventanas, manejar la entrada del usuario, ejecutar comandos y administrar los procesos.
 * - 
 * Descripción:
 * - La función inicia la pantalla de ncurses, 
 * - define colores personalizados, crea ventanas para las áreas de comandos,
 * - mensajes, registros y procesos, maneja la entrada del usuario, ejecuta
 * - comandos y administra los procesos.

 */
int main(void)
{
    initscr(); // Inicializar la pantalla de ncurses
    start_color(); // Inicializar el modo de color

    // Definir colores personalizados
    init_color(1, 0, 0, 0);
    init_color(2, 250, 0, 150);
    init_color(5, 0, 154, 128);

    // Asociar pares de colores con combinaciones de colores predefinidas
    init_pair(1, COLOR_WHITE, 1);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, 2);
    init_pair(4, COLOR_WHITE, 5);

    attron(COLOR_PAIR(1)); // Activar el par de colores

    cbreak(); // Deshabilitar el búfer de entrada
    noecho(); // No mostrar los caracteres ingresados

    int height, width;
    getmaxyx(stdscr, height, width); // Obtener las dimensiones de la pantalla

    // Crear 3 ventanas para las áreas
    WINDOW *IDventanaPromt = newwin(height / 2.3, width / 2, 0, 0);
    wbkgd(IDventanaPromt, COLOR_PAIR(1)); // Establecer el color de fondo de la ventana
    box(IDventanaPromt, 0, 0); // Dibujar un borde alrededor de la ventana
    mvwprintw(IDventanaPromt, 0, 2, "COMANDOS"); // Imprimir texto en la ventana

    WINDOW *IDventanaMensajes = newwin(height / 7, width / 2, height / 2.3, 0);
    wbkgd(IDventanaMensajes, COLOR_PAIR(2));
    box(IDventanaMensajes, 0, 0);
    mvwprintw(IDventanaMensajes, 0, 2, "MENSAJES");

    WINDOW *IDventanaRegistros = newwin(height / 2.4, width / 2, 2 * height / 3.5, 0);
    wbkgd(IDventanaRegistros, COLOR_PAIR(3));
    box(IDventanaRegistros, 0, 0);
    mvwprintw(IDventanaRegistros, 0, 2, "REGISTROS");

    // Crear ventana para la lista de procesos
    WINDOW *IDventanaProcesos = newwin(height - 2, width / 2, 0, width / 2);
    wbkgd(IDventanaProcesos, COLOR_PAIR(4));
    box(IDventanaProcesos, 0, 0);
    mvwprintw(IDventanaProcesos, 0, 2, "LISTA DE PROCESOS");
    mvwprintw(IDventanaProcesos, 2, 2, "Proceso en Ejecucion: ");
    mvwprintw(IDventanaProcesos, 3, 2, "...");
    mvwprintw(IDventanaProcesos, 10, 2, "Procesos en Espera: ");
    mvwprintw(IDventanaProcesos, 11, 2, "...");
    mvwprintw(IDventanaProcesos, height / 2, 2, "Procesos Terminados: ");
    mvwprintw(IDventanaProcesos, height / 2 + 1, 2, "...");

    // Crear ventana decorativa
    WINDOW *IDventanaDeco = newwin(height / 10.5, width, 3.4 * height / 3.5, 0);
    wbkgd(IDventanaDeco, COLOR_PAIR(1));
    mvwprintw(IDventanaDeco, 0, 10, "Windos I . Version 1.3 . 2024 Copyrigth (c) Chado & Michel Corporeishon.");

    // Actualizar la pantalla para mostrar los cambios
    refresh();
    wrefresh(IDventanaPromt);
    wrefresh(IDventanaMensajes);
    wrefresh(IDventanaRegistros);
    wrefresh(IDventanaProcesos);
    wrefresh(IDventanaDeco);

    // Declarar variables y estructuras necesarias para el manejo de procesos y comandos
    struct PCB *Listos = NULL; // Inicializar la lista de procesos listos
    struct PCB *Terminados = NULL; // Inicializar la lista de procesos terminados
    struct PCB *Ejecucion = NULL; // Inicializar la lista de procesos en ejecución
    int Quantum = 0; // Inicializar el contador de Quantum

    char ComandoIngresado[100] = {0}; // Inicializar la cadena de comandos
    int CharPos = 0; // Posición del cursor en la línea de comandos
    int EstadoLineaComandos = 0; // Estado de la línea de comandos
    int NumLineas = 0; // Número de líneas de comandos ingresadas
    int SePuedeLeer = 0; // Bandera para saber si se puede leer el archivo
    int EjecucionComandos = 0; // Estado de la ejecución de comandos
    int ProcesoCargado = 0; // Bandera para saber si hay un proceso cargado
    int EstadoProcesos = 0; // Estado de los procesos
    char ArchivoCargadoValido[200]; // Nombre del archivo cargado
    char HistorialComandos[10][100] = {0};     //Cadena para almacenar el historial de comandos
    int ContadorLectura = 0; //Contador para la lectura de comandos
    int LeerArchivoFlag = 0; // Bandera para indicar si se debe leer el archivo
    int MaxIteraciones = 10; // número máximo de iteraciones

    // Ciclo principal del programa
    while (EstadoLineaComandos != 666)
    {
      // Actualizar la ventana de línea de comandos
      VentanaPrompt(IDventanaPromt, NumLineas, ComandoIngresado);

      // Obtener el estado de la línea de comandos
      EstadoLineaComandos = LineaComandos(IDventanaPromt, ComandoIngresado, &CharPos, &NumLineas, HistorialComandos);
      // Ejecutar comandos y manejar errores
      EjecucionComandos = ManejadorErroresComandos(IDventanaPromt, IDventanaMensajes, EstadoLineaComandos, ComandoIngresado, &CharPos, &NumLineas);
      // Manejar procesos y ejecución de comandos
      ManejadorProcesos(IDventanaMensajes, &Listos, &Terminados, &Ejecucion, &SePuedeLeer, &ProcesoCargado, EjecucionComandos, ArchivoCargadoValido, EstadoProcesos);

      // Imprimir información de los procesos en la ventana de procesos
      ImprimirEjecutandose(IDventanaProcesos, Ejecucion);
      ImprimirListos(IDventanaProcesos, Listos);
      ImprimirTerminados(IDventanaProcesos, Terminados);
      mvwprintw(IDventanaProcesos, 7, 2, "                                                                        ");



    //Solo leer cada 5 iteraciones para poder escribir en la ventana de comandos
    //sin tanto delay
      if (SePuedeLeer == 1 && ContadorLectura < 10)
      {
        ContadorLectura++;
      }
      else if (ContadorLectura >= 10)
      {
        LeerArchivoFlag = 1;
        ContadorLectura = 0;
      }

      if (LeerArchivoFlag == 1)
      {
        LeerArchivo(IDventanaMensajes, IDventanaRegistros, &Ejecucion, &SePuedeLeer, Ejecucion->programa, &Quantum, &ProcesoCargado, &Listos);
        LeerArchivoFlag = 0;
      }
    }

    // Finalizar el modo de pantalla de ncurses
    endwin();
    // Liberar memoria utilizada
    printf("Liberando memoria...\n");
    LiberarMemoria(Ejecucion);
    LiberaListos(&Listos);
    LiberaTerminados(&Terminados);
    printf("Memoria liberada.\n");
    return 0;
}
