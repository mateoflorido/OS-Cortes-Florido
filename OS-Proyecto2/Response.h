/*
Archivo: Response.h
Realizado por: Mateo Florido y Daniela Cortes
Contiene: Estructura de una respuesta para facilitar la 
comunicación entre el cliente y el servidor.
Fecha de última modificación: 10/11/2019
*/

#ifndef RESPONSE_H
#define RESPONSE_H
typedef struct Response{
  unsigned int responseType;
  char message[1000];
} Response;

#endif //RESPONSE_H
