/*
Archivo: Tweet.h
Realizado por: Mateo Florido y Daniela Cortes
Contiene: Estructura de un Tweet que facilita el manejo del
mismo dentro del sistema.
Fecha de última modificación: 10/11/2019
*/

#ifndef TWEET_H
#define TWEET_H
typedef struct Tweet {
  char clientID;
  char params[202];
} Tweet;

#endif //TWEET_H
