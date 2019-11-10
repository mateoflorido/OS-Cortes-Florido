#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "Server.h"
#include "Request.h"
#include "Response.h"
#include <pthread.h>

/*
Función: main
Parámetros de Entrada:
    argc: Son la cantidad de parámetros que recibe la función.
    ** argv: Son los parámetros que llegan a la función.
Descripción: Es el método principal de archivo .c desde donde se invocan
todas las demás funcionalidades.
*/
int main( int argc, char** argv )
{
    int fd, pid, n, bytes, creado = 0;
    int i = 0, j = 0;
    FILE* fileRelations;
    int argvpipe, argvarc;
    mode_t fifo_mode = S_IRUSR | S_IWUSR;

    /*Check input parameters from console*/

    if ( argc != 5 )
    {
        perror( "Usage: gestor -r [RELATIONS] -p [pipeName]\n" );
        exit( 1 );
    }
    if ( strcmp( argv[ 1 ], "-r" ) == 0 )
    {
        if ( strcmp( argv[ 3 ], "-p" ) != 0 )
        {
            perror( "Usage: gestor -r [RELATIONS] -p [pipeName]\n" );
            exit( 1 );
        }
    }
    else if ( strcmp( argv[ 3 ], "-r" ) == 0 )
    {
        if ( strcmp( argv[ 1 ], "-p" ) != 0 )
        {
            perror( "Usage: gestor -r [RELATIONS] -p [pipeName]\n" );
            exit( 1 );
        }
    }

    /*Allocate all memory required*/
    relations = malloc( 10 * sizeof( char* ));
    cache = calloc( 10, sizeof( Response* ));
    connected = malloc( 10 * sizeof( char ));
    for ( i = 0; i < 10; i++ )
    {
        connected[ i ] = '0';
        cached[ i ] = 0;
        relations[ i ] = malloc( 10 * sizeof( char ));
        cache[ i ] = malloc( 50 * sizeof( Response ));
    }

    /*Check input from console*/
    argvarc = ( strcmp( argv[ 1 ], "-r" ) == 0 ) ? 2 : 4;
    argvpipe = ( argvarc == 2 ) ? 4 : 2;

    fileRelations = fopen( argv[ argvarc ], "r" );
    if ( !fileRelations )
        perror( "FOPEN ERROR " );
    printf( "FILE: %s Open \n", argv[ argvarc ] );
    for ( i = 0; i < 10; i++ )
    {
        for ( j = 0; j < 10; j++ )
        {
            if ( !fscanf( fileRelations, "%c ", &relations[ i ][ j ] ))
                break;
        }
    }
    /*for ( i = 0; i < 10; i++ )
    {
        for ( j = 0; j < 10; j++ )
        {
            printf( "|%c|", relations[ i ][ j ] );
        }
        printf( "\n" );
    }*/
    printf( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@              @@@@@@@@@@@@@@(@@\n" );
    printf( "@@@@@ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*                    @@@@@@@    @@@\n" );
    printf( "@@@@   &@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.                                @@@@\n" );
    printf( "@@@       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                @@@@@@\n" );
    printf( "@@@         @@@@@@@@@@@@@@@@@@@@@@@@@@@@                               @@&    @\n" );
    printf( "@@@           @@@@@@@@@@@@@@@@@@@@@@@@@                                     @@@\n" );
    printf( "@@@              &@@@@@@@@@@@@@@@@@@@@@                                   &@@@@\n" );
    printf( "@@@/                 @@@@@@@@@@@@@@@@@#                                 @@@@@@@\n" );
    printf( "@@@@                      @@@@@@@@@@@@@                                @@@@@@@@\n" );
    printf( "@@@@@                            .@@@@@                                @@@@@@@@\n" );
    printf( "@@@@@@@                                                                @@@@@@@@\n" );
    printf( "@@@ @@@@/                                                              @@@@@@@@\n" );
    printf( "@@@                                                                    @@@@@@@@\n" );
    printf( "@@@                                                                   @@@@@@@@@\n" );
    printf( "@@@@                                                                  @@@@@@@@@\n" );
    printf( "@@@@@                                                                @@@@@@@@@@\n" );
    printf( "@@@@@@@                                                              @@@@@@@@@@\n" );
    printf( "@@@@@@@@@                                                           @@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@                                                       @@@@@@@@@@@@\n" );
    printf( "@@@@@@@@&,@@&/                                                    @@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@.                                                       @@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@/                                                     @@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@                                                  (@@@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@@@,                                              @@@@@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@@@@@@@                                         .@@@@@@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@@@@@@@@@@@@                                  ,@@@@@@@@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@@@@@@@@*                                   &@@@@@@@@@@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@@@@                                      @@@@@@@@@@@@@@@@@@@@@@@@@@\n" );
    printf( "/@@@@/*                                           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" );
    printf( "@@@                                            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@                                   ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@@/                      &@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" );
    printf( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n" );

    /*Unlinking Pipe*/
    unlink( argv[ argvpipe ] );

    if ( mkfifo( argv[ argvpipe ], fifo_mode ) == -1 )
    {
        perror( "MKFIFO ERROR" );
        exit( 1 );
    }
    printf( "\t\t------------------------------------------\n" );
    printf( "\t\t------------------------------------------\n" );
    printf( "\t\t--------  TWITTER SERVER SERVICE  --------\n" );
    printf( "\t\t--------  STARTING SERVER SERVICE --------\n\n" );
    printf( "\t\t------------------------------------------\n" );
    printf( "\t\t------------------------------------------\n" );
    /*Creating Pipes*/
    pthread_create( &threadID[ 0 ], NULL, &seekingRequest, argv[ argvpipe ] );
    pthread_join( threadID[ 0 ], NULL);

}


/*
Función: clientHandler
Descripción:Este método es el hilo que esta constantemente preguntando en consola
las nuevas acciones que se desean realizar, además verifica esta información y 
escribe en el pipe que se comunica con los demás clientes.
*/
void* seekingRequest( void* arg )
{
    char* namedPipe = ( char* ) arg;
    int fd = 0, creado = 0, i = 0, j = 0, self = 0, total = 0;
    char valid = 'n';
    char number[2];
    char* pipeCOM = NULL;
    struct Request* req = malloc( sizeof( struct Request ));
    Response* res = NULL;
    while ( 1 )
    {
        /*Open Pipe and Wait for Info Incoming*/
        do
        {
            fd = open( namedPipe, O_RDONLY );
            if ( fd == -1 )
                perror( "PIPE NOT FOUND... EXIT\n" );
            else
            {
                printf( "\n[Solicitud entrante]...\n" );
                creado = 1;
            }

        }
        while ( creado == 0 );
        creado = 0;
        /*Read from Named Pipe*/
        read( fd, req, sizeof( struct Request ));
        /*Print Request Received*/
        printf( "\n[Solicitud]:\n\t [ID Usuario]: %c \n\t [Operación]: %c \n\t [Estado de Cliente]: %c \n\t [Parametros] %s \n",
                req->clientID,
                req->operationID,
                connected[ req->clientID - '0' ], req->params );
        /*Connect client*/
        if ( req->operationID == '0' )
        {
            self = req->clientID - '0';
            /*Check Client Already Connected*/
            if ( connected[ self ] == '0' )
            {
                connected[ self ] = '1';
                Response* ptrCached = cache[ self ];

                if ( cached[ self ] > 0 )
                {
                    sleep(( 1 ));
                    pipeCOM = malloc( 5 * sizeof( char ));
                    strcpy( pipeCOM, "sc" );
                    strncat( pipeCOM, &( req->clientID ), 1 );
                    printf( "[Pipe COM] %s Enviando Tweets en Cache\n", pipeCOM );

                    fd = open( pipeCOM, O_WRONLY | O_NONBLOCK );
                    if ( fd == -1 )
                    {
                        perror( "PIPE NOT FOUND... EXIT" );
                    }
                    for ( i = 0; i < cached[ self ]; i++ )
                    {
                        if ( i == 0 )
                        {
                            ptrCached[ i ].responseType = cached[ self ];
                        }
                        write( fd, &( ptrCached[ i ] ), sizeof( Response ));
                    }
                    valid = 'n';
                    cached[ self ] = 0;
                    close( fd );
                }
            }
            else if ( connected[ self ] == '1' )
            {
                printf( "[ERROR] Usuario %c ya conectado! \n", req->clientID );
                kill( req->pID, SIGUSR1 );
            }
        }
        else if ( req->operationID == '1' )
        {
            i = req->clientID - '0';
            j = req->params[ 0 ] - '0';
            if ( relations[ i ][ j ] == '0' )
            {
                relations[ i ][ j ] = '1';
                res = malloc( sizeof( Response ));
                printf( "[UPDATE] Usuario %c ahora sigue a Usuario %d \n", req->clientID, j );
                strcpy( res->message, "Ahora estás siguiendo a usuario " );
                strcat( res->message, &( req->params[ 0 ] ));
                strcat( res->message, "!\n" );
                res->responseType = 0;
                valid = 's';
            }
            else
            {
                res = malloc( sizeof( Response ));
                printf( "[ERROR] Usuario %c intento seguir a Usuario %d que ya seguia. \n", req->clientID, j );
                strcpy( res->message, "No puedes seguir a alguien que ya sigues!\n" );
                res->responseType = 0;
                valid = 's';
            }
        }
        else if ( req->operationID == '2' )
        {
            i = req->clientID - '0';
            j = req->params[ 0 ] - '0';
            if ( relations[ i ][ j ] == '1' )
            {
                relations[ i ][ j ] = '0';
                res = malloc( sizeof( Response ));
                printf( "[UPDATE] Usuario %c dejo de seguir a Usuario %d \n", req->clientID, j );
                strcpy( res->message, "Dejaste de seguir a usuario " );
                strcat( res->message, &( req->params[ 0 ] ));
                strcat( res->message, "!\n" );
                res->responseType = 0;
                valid = 's';
            }
            else
            {
                res = malloc( sizeof( Response ));
                printf( "[ERROR] Usuario %c intento dejar de seguir a Usuario %d que no seguia. \n", req->clientID, j );
                strcpy( res->message, "No puedes dejar de seguir a alguien que no sigues!\n" );
                res->responseType = 0;
                valid = 's';
            }
        }
        else if ( req->operationID == '3' )
        {
            self = req->clientID - '0';

            res = malloc( sizeof( Response ));
            strcpy( res->message, "-> Usuario " );
            strncat( res->message, &( req->clientID ), 1 );
            strcat( res->message, " @" );
            strncat( res->message, &( req->clientID ), 1 );
            strcat( res->message, ":\n\t" );
            strcat( res->message, req->params );
            strcat( res->message, "\n\n" );

            res->responseType = 0;

            printf( "[TWEET] Usuario %c twitteó: \n %s \n", req->clientID, res->message );

            for ( i = 0; i < 10; i++ )
            {
                if ( relations[ i ][ self ] == '1' )
                {
                    if ( connected[ i ] == '1' )
                    {
                        pipeCOM = malloc( 5 * sizeof( char ));
                        sprintf( number, "%d", i );
                        strcpy( pipeCOM, "sc" );
                        strncat( pipeCOM, number, 1 );
                        fd = open( pipeCOM, O_WRONLY | O_NONBLOCK );
                        if ( fd == -1 )
                        {
                            perror( "PIPE NOT FOUND... EXIT" );
                        }
                        write( fd, res, sizeof( Response ));
                        close( fd );
                        free( pipeCOM );
                        valid = 'n';
                    }
                    else
                    {
                        Response* ptrCached = cache[ i ];
                        cached[ i ] = cached[ i ] + 1;

                        strcpy( ptrCached[ cached[ i ] - 1 ].message, res->message );
                        ptrCached[ cached[ i ] - 1 ].responseType = 0;
                    }
                }
            }


        }
        else if ( req->operationID == '4' )
        {
            printf( "[UPDATE] Usuario %c ha cerrado sesion. \n", req->clientID );

            valid = 'n';
            self = req->clientID - '0';
            connected[ self ] = '0';
        }
        if ( valid != 'n' )
        {
            pipeCOM = malloc( 5 * sizeof( char ));
            strcpy( pipeCOM, "sc" );

            strncat( pipeCOM, &( req->clientID ), 1 );

            fd = open( pipeCOM, O_WRONLY | O_NONBLOCK );
            if ( fd == -1 )
            {
                perror( "PIPE NOT FOUND... EXIT" );
                printf( "Pipe: %s\n", pipeCOM );
            }
            write( fd, res, sizeof( Response ));
            valid = 'n';
            close( fd );
            free( pipeCOM );
            free( res );
        }
        free( req );
        req = malloc( sizeof( struct Request ));

        close( fd );
    }
}