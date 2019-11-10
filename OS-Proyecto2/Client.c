//
// Created by mateo on 9/11/19.
//

#include "Client.h"
#include "Request.h"
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "Response.h"

int main( int argc, char** argv )
{
    int argvid, argvpipe, fd = -1;
    mode_t fifo_mode = S_IRUSR | S_IWUSR;

    if ( argc != 4 )
    {
        perror( "Usage: cliente -i [ID] -p [pipeName]\n" );
    }
    if ( strcmp( argv[ 1 ], "-i" ) == 0 || strcmp( argv[ 3 ], "-i" ) == 0 )
    {
        if ( strcmp( argv[ 1 ], "-p" ) != 0 || strcmp( argv[ 1 ], "-p" ) != 0 )
        {
            perror( "Usage: cliente -i [ID] -p [pipeName]\n" );
        }
    }
    else
    {
        perror( "Usage: cliente -i [ID] -p [pipeName]\n" );
    }


    argvid = ( strcmp( argv[ 1 ], "-i" ) == 0 ) ? 2 : 4;
    argvpipe = ( argvid == 2 ) ? 4 : 2;
    pipeServ = malloc( sizeof( argv[ argvpipe ] ));
    strcpy( pipeServ, argv[ argvpipe ] );

    signal( SIGUSR1, SIGUSRHandler );
    signal( SIGINT, SIGUSRHandler );

    /*Unlinking Pipe*/
    pipeCOM = malloc( 5 * sizeof( char ));
    strcpy( pipeCOM, "sc" );
    strcat( pipeCOM, argv[ argvid ] );
    unlink( pipeCOM );
    printf( "Created Pipe %s\n", pipeCOM );
    if ( mkfifo( pipeCOM, fifo_mode ) == -1 )
    {
        perror( "MKFIFO ERROR" );
        exit( 1 );
    }


    fd = open( argv[ argvpipe ], O_WRONLY | O_NONBLOCK );
    if ( fd == -1 )
    {
        perror( "PIPE NOT FOUND... EXIT" );
        exit( 1 );
    }
    struct Request* init = malloc( sizeof( struct Request ));
    init->clientID = argv[ argvid ][ 0 ];
    init->operationID = '0';
    init->pID = getpid( );
    strcpy( init->params, "Something wrong?" );
    printf( "Request:\n\t clientID: %c \n\t Request: %c \n\t", init->clientID, init->operationID );
    write( fd, init, sizeof( struct Request ));
    close( fd );


    pthread_create( &comThread, NULL, &receiveData, argv[ argvpipe ] );
    pthread_create( &selfThread, NULL, &clientHandler, argv[ argvid ] );

    pthread_join( selfThread, NULL);
    pthread_cancel( comThread );


}

void SIGUSRHandler( int sigNum )
{
    if ( sigNum == SIGUSR1 )
    {
        printf( "Not allowed to login, closing...\n" );
        exit( 1 );
    }
    else if ( sigNum == SIGINT )
    {
        exit( 1 );
    }
}

void* receiveData( void* arg )
{
    int fd = 0, control = 0;
    Response* inResponse = malloc( sizeof( Response ));

    while ( 1 )
    {
        do
        {
            fd = open( pipeCOM, O_RDONLY );
            if ( fd == -1 )
            {
                perror( "PIPE NOT FOUND... EXIT" );
                exit( 1 );
            }
            else
            {
                printf( "Tweet incoming ...\n" );
                control = 1;
            }
        }
        while ( control == 0 );
        control = 0;
        read( fd, inResponse, sizeof( Response ));
        printf( "===============================================\n" );
        printf( "%s \n", inResponse->message );
        printf( "===============================================\n" );

    }
    close( fd );

}

void* clientHandler( void* arg )
{
    char* clientID = ( char* ) arg;
    int fd = 0;
    char* op = malloc( 210 * sizeof( char ));
    char valid = 'n';
    struct Request* req = malloc( sizeof( struct Request ));
    char* command = malloc( 100 * sizeof( char ));
    char* token = NULL;
    char delim[2] = " ";

    while ( 1 )
    {
        scanf( "%99s", command );
        getchar( );
        token = strtok( command, delim );
        printf( "Operation %s \n", token );
        if ( strncmp( token, "follow", 6 ) == 0 )
        {
            strcpy( &( req->clientID ), clientID );
            req->operationID = '1';
            req->pID = getpid( );
            printf( "ID Usuario: " );
            scanf( "%s", op );
            strcpy( req->params, op );
            printf( "Params: %s \n", req->params );
            getchar( );
            valid = 's';

        }
        else if ( strncmp( token, "tweet", 5 ) == 0 )
        {
            strcpy( &( req->clientID ), clientID );
            req->operationID = '3';
            req->pID = getpid( );
            printf( "Write your Tweet: " );
            fgets( op, 209, stdin );
            if (( strlen( op ) > 0 ) && ( op[ strlen( op ) - 1 ] == '\n' ))
                op[ strlen( op ) - 1 ] = '\0';
            if (( strlen( op ) <= 200 ) && ( strlen( op ) > 0 ))
            {
                strncpy( req->params, op, 200 );
                valid = 's';
            }
            else
            {
                printf( "Error. Tweet excede 200 caracteres\n" );
                fflush( stdin );
            }

            printf( "Params: %s \n", req->params );

        }
        else if ( strncmp( token, "unfollow", 8 ) == 0 )
        {
            strcpy( &( req->clientID ), clientID );
            req->operationID = '2';
            req->pID = getpid( );
            printf( "ID Usuario: " );
            scanf( "%s", op );
            strcpy( req->params, op );
            printf( "Params: %s \n", req->params );
            getchar( );
            valid = 's';

        }
        else if ( strncmp( token, "logout", 8 ) == 0 )
        {
            strcpy( &( req->clientID ), clientID );
            req->operationID = '4';
            req->pID = getpid( );
            printf( "ID Usuario: " );
            scanf( "%s", op );
            strcpy( req->params, op );
            printf( "Params: %s \n", req->params );
            valid = 's';
            getchar( );
        }
        printf( "Request:\n\t clientID: %c \n\t Request: %c \n\t Params: %s \n", req->clientID, req->operationID,
                req->params );

        if ( valid != 'n' )
        {
            fd = open( pipeServ, O_WRONLY | O_NONBLOCK );
            if ( fd == -1 )
            {
                perror( "PIPE NOT FOUND... EXIT" );
                exit( 1 );
            }
            write( fd, req, sizeof( struct Request ));
            valid = 'n';
            close( fd );
        }
        free( req );
        req = malloc( sizeof( struct Request ));

    }
}