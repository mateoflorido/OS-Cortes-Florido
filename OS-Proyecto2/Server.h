/*
Archivo: Server.h
Realizado por: Mateo Florido y Daniela Cortes
Contiene: Estructura de un Servidor para facilitar el manejo
del mismo en el sistema y así mismo los prototipos de funciones 
que escriben y leen el pipe que se comunica con los clientes.
Fecha de última modificación: 10/11/2019
*/

#ifndef SERVER_H
#define SERVER_H

#include "Response.h"

char **relations;
char *connected;
int cached[10];
Response **cache;

pthread_t threadID[10];
/**
 * Seeking Requests
 * @param arg
 * @return
 */
void *seekingRequest ( void *arg );

#endif //SERVER_H
