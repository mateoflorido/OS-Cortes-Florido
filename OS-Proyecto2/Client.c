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

    if ( argc != 5 )
    {
        perror( "Usage: cliente -i [ID] -p [pipeName]\n" );
        exit( 1 );
    }
    if ( strcmp( argv[ 1 ], "-i" ) == 0 )
    {
        if ( strcmp( argv[ 3 ], "-p" ) != 0 )
        {
            perror( "Usage: cliente -i [ID] -p [pipeName]\n" );
            exit( 1 );
        }
    }
    else if ( strcmp( argv[ 3 ], "-i" ) == 0 )
    {
        if ( strcmp( argv[ 1 ], "-p" ) != 0 )
        {
            perror( "Usage: cliente -i [ID] -p [pipeName]\n" );
            exit( 1 );
        }
    }

    argvid = ( strcmp( argv[ 1 ], "-i" ) == 0 ) ? 2 : 4;
    argvpipe = ( argvid == 2 ) ? 4 : 2;
    pipeServ = malloc( sizeof( argv[ argvpipe ] ));
    selfID = malloc( 4 * sizeof( char ));
    strcpy( pipeServ, argv[ argvpipe ] );

    signal( SIGUSR1, SIGUSRHandler );
    signal( SIGINT, SIGUSRHandler );

    /*Unlinking Pipe*/
    pipeCOM = malloc( 5 * sizeof( char ));
    strcpy( pipeCOM, "sc" );
    strcat( pipeCOM, argv[ argvid ] );
    unlink( pipeCOM );
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
    strcpy( selfID, &( argv[ argvid ][ 0 ] ));
    init->operationID = '0';
    init->pID = getpid( );
    strcpy( init->params, "ACK" );
    write( fd, init, sizeof( struct Request ));
    close( fd );

    printf("\n[Bienvenido a Twitter de nuevo!]\n");
    pthread_create( &comThread, NULL, &receiveData, argv[ argvpipe ] );
    pthread_create( &selfThread, NULL, &clientHandler, argv[ argvid ] );

    pthread_join( selfThread, NULL);
    pthread_cancel( comThread );
    printf( "[Sesión Cerrada]\n" );


}

void SIGUSRHandler( int sigNum )
{
    if ( sigNum == SIGUSR1 )
    {
        printf( "No puede iniciar sesion con otra sesion abierta, cerrando...\n" );
        exit( 1 );
    }
    else if ( sigNum == SIGINT )
    {
        int fd = 0;

        pthread_cancel( comThread );
        pthread_cancel( selfThread );

        struct Request* req = malloc( sizeof( struct Request ));
        strcpy( &( req->clientID ), selfID );
        req->operationID = '4';
        req->pID = getpid( );
        printf( "\n\n[Cerrando Sesión. Tardará un momento...]\n" );

        fd = open( pipeServ, O_WRONLY | O_NONBLOCK );
        if ( fd == -1 )
        {
            perror( "PIPE NOT FOUND... EXIT" );
            exit( 1 );
        }

        write( fd, req, sizeof( struct Request ));
        close( fd );

        free( req );
        unlink( pipeCOM );
        printf( "[Sesión Cerrada]\n" );
        exit( 1 );
    }
}

void* receiveData( void* arg )
{
    int fd = 0, control = 0, checkRun = 0;
    Response* inResponse = malloc( sizeof( Response ));

    while ( 1 )
    {
        if ( checkRun == 0 )
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
                    control = 1;
                }
            }
            while ( control == 0 );
            control = 0;

        }
        read( fd, inResponse, sizeof( Response ));
        if ( checkRun == 0 )
            checkRun = inResponse->responseType;
        if ( checkRun > 0 )
        {
            checkRun = checkRun - 1;
        }
        printf( "\n %s \n", inResponse->message );
        free( inResponse );
        inResponse = malloc( sizeof( Response ));

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
        printf("~ -> Escribe una opción: ");
        scanf( "%99s", command );
        getchar( );
        token = strtok( command, delim );
        if ( strncmp( token, "follow", 6 ) == 0 )
        {
            strcpy( &( req->clientID ), clientID );
            req->operationID = '1';
            req->pID = getpid( );
            printf( "ID Usuario: " );
            scanf( "%s", op );
            strcpy( req->params, op );
            getchar( );
            valid = 's';

        }
        else if ( strncmp( token, "tweet", 5 ) == 0 )
        {
            strcpy( &( req->clientID ), clientID );
            req->operationID = '3';
            req->pID = getpid( );
            printf( "Escribe tu tweet: " );
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

        }
        else if ( strncmp( token, "unfollow", 8 ) == 0 )
        {
            strcpy( &( req->clientID ), clientID );
            req->operationID = '2';
            req->pID = getpid( );
            printf( "ID Usuario: " );
            scanf( "%s", op );
            strcpy( req->params, op );
            getchar( );
            valid = 's';

        }
        else if ( strncmp( token, "logout", 8 ) == 0 )
        {
            strcpy( &( req->clientID ), clientID );
            req->operationID = '4';
            req->pID = getpid( );
            printf( "\n[Cerrando Sesión. Tardará un momento...]\n" );
            valid = 's';
        }

        if ( valid != 'n' )
        {
            fd = open( pipeServ, O_WRONLY | O_NONBLOCK );
            if ( fd == -1 )
            {
                perror( "PIPE NOT FOUND... EXIT" );
                exit( 1 );
            }
            printf("\n[Enviando Solicitud]\n");
            write( fd, req, sizeof( struct Request ));
            valid = 'n';
            close( fd );
            if ( req->operationID == '4' )
            {
                free( req );
                free( op );
                free( command );
                unlink( pipeCOM );
                pthread_exit(NULL);
            }
        }
        free( req );
        req = malloc( sizeof( struct Request ));

    }
}