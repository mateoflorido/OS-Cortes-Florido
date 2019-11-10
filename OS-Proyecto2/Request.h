/*
Archivo: Request.h
Realizado por: Mateo Florido y Daniela Cortes
Contiene: Estructura de una solititud para facilitar la 
comunicación entre el cliente y el servidor.
Fecha de última modificación: 10/11/2019
*/

#ifndef REQUEST_H
#define REQUEST_H

#include <sys/types.h>

struct Request
{
    char clientID;
    char operationID;
    char params[ 202 ];
    pid_t pID;
};
#endif //REQUEST_H
