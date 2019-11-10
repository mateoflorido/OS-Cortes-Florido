//
// Created by mateo on 9/11/19.
//

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
