//
// Created by mateo on 9/11/19.
//

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
