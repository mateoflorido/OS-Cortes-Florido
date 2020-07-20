/*
Archivo: Client.h
Realizado por: Mateo Florido y Daniela Cortes
Contiene: Estructura de un Cliente para facilitar el manejo
del mismo en el sistema y así mismo los prototipos de funciones 
que escriben y leen el pipe que se comunica con el servidor.
Fecha de última modificación: 10/11/2019
*/

#ifndef CLIENT_H
#define CLIENT_H
#include <sys/types.h>
#include "Request.h"

pthread_t comThread;
pthread_t selfThread;
char* pipeCOM;
char *selfID;
char* pipeServ;


void SIGUSRHandler(int sigNum);
void* receiveData( void* arg );
void* clientHandler( void* arg );


#endif //CLIENT_H
