//
// Created by mateo on 9/11/19.
//

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
