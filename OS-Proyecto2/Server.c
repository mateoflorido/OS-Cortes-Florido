//
// Created by mateo on 9/11/19.
//

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

int main( int argc, char** argv )
{
    int fd, pid, n, bytes, creado = 0;
    int i = 0, j = 0;
    FILE* fileRelations;
    int argvpipe, argvarc;
    mode_t fifo_mode = S_IRUSR | S_IWUSR;

    /*Check input parameters from console*/


    /*Allocate all memory required*/
    relations = malloc( 10 * sizeof( char* ));
    cache = malloc( 10 * sizeof( Response* ));
    connected = malloc( 10 * sizeof( char ));
    for ( i = 0; i < 10; i++ )
    {
        connected[ i ] = '0';
        relations[ i ] = malloc( 10 * sizeof( char ));
    }
    printf( "Created Relations 2 \n" );

    /*Check input from console*/
    argvarc = ( strcmp( argv[ 1 ], "-r" ) == 0 ) ? 2 : 4;
    argvpipe = ( argvarc == 2 ) ? 4 : 2;

    printf( "FILE: %s \n", argv[ argvarc ] );
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
    for ( i = 0; i < 10; i++ )
    {
        for ( j = 0; j < 10; j++ )
        {
            printf( "|%c|", relations[ i ][ j ] );
        }
        printf( "\n" );
    }

    /*Unlinking Pipe*/
    unlink( argv[ argvpipe ] );

    if ( mkfifo( argv[ argvpipe ], fifo_mode ) == -1 )
    {
        perror( "MKFIFO ERROR" );
        exit( 1 );
    }
    printf( "--------  TWITTER SERVER SERVICE  --------\n" );
    printf( "-------- STARTING SERVER SERVICE --------\n\n" );
    /*Creating Pipes*/
    pthread_create( &threadID[ 0 ], NULL, &seekingRequest, argv[ argvpipe ] );
    pthread_join( threadID[ 0 ], NULL);

}

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
        do
        {
            fd = open( namedPipe, O_RDONLY );
            if ( fd == -1 )
                perror( "PIPE NOT FOUND... EXIT\n" );
            else
            {
                printf( "Request incoming...\n" );
                creado = 1;
            }

        }
        while ( creado == 0 );
        creado = 0;
        read( fd, req, sizeof( struct Request ));
        printf( "Request:\n\t clientID: %c \n\t Request: %c \n\t clientState: %c \n\t Params: %s \n", req->clientID,
                req->operationID,
                connected[ req->clientID - '0' ], req->params );
        if ( req->operationID == '0' )
        {
            if ( connected[ req->clientID - '0' ] == '0' )
            {
                connected[ req->clientID - '0' ] = '1';
                j = sizeof( cache[ req->clientID - '0' ] ) / sizeof( Response );
                printf( "Size of Cache from client %c : %d\n", req->clientID, j );
                if ( j > 0 )
                {
                    sleep(( 1 ));
                    printf( "Size of Cache from client %c : %d\n", req->clientID, j );
                    for ( i = 0; i < ( sizeof( cache[ req->clientID - '0' ] ) / sizeof( Response )); i++ )
                    {
                        pipeCOM = malloc( 5 * sizeof( char ));
                        strcpy( pipeCOM, "sc" );
                        printf( "Pipe: %s Client: %c\n", pipeCOM, req->clientID );

                        strncat( pipeCOM, &( req->clientID ), 1 );
                        printf( "Pipe: %s\n", pipeCOM );

                        fd = open( pipeCOM, O_WRONLY | O_NONBLOCK );
                        if ( fd == -1 )
                        {
                            perror( "PIPE NOT FOUND... EXIT" );
                            printf( "Pipe: %s\n", pipeCOM );
                        }
                        printf( "Tweet Cached: %s\n", cache[ req->clientID - '0' ][ i ].message );
                        write( fd, &( cache[ req->clientID - '0' ][ i ] ), sizeof( Response ));
                    }
                    valid = 'n';
                    close( fd );
                }
            }
            else if ( connected[ req->clientID - '0' ] == '1' )
            {
                printf( "User %c already connected! \n", req->clientID );
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
                strcpy( res->message, "Ahora estás siguiendo a usuario " );
                strcat( res->message, &( req->params[ 0 ] ));
                strcat( res->message, "!\n" );
                res->responseType = 0;
                valid = 's';
            }
            else
            {
                res = malloc( sizeof( Response ));
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
                strcpy( res->message, "Dejaste de seguir a usuario " );
                strcat( res->message, &( req->params[ 0 ] ));
                strcat( res->message, "!\n" );
                res->responseType = 0;
                valid = 's';
            }
            else
            {
                res = malloc( sizeof( Response ));
                strcpy( res->message, "No puedes dejar de seguir a alguien que no sigues!\n" );
                res->responseType = 0;
                valid = 's';
            }
        }
        else if ( req->operationID == '3' )
        {
            self = req->clientID - '0';

            res = malloc( sizeof( Response ));
            strcpy( res->message, req->params );
            res->responseType = 0;

            for ( i = 0; i < 10; i++ )
            {
                printf( "Relations between %d and %d : %c\n", self, i, relations[ i ][ self ] );
                if ( relations[ i ][ self ] == '1' )
                {
                    printf( "Client %d followed by %d \n", self, i );
                    if ( connected[ i ] == '1' )
                    {
                        printf( "Client %d followed by %d \n", self, i );
                        pipeCOM = malloc( 5 * sizeof( char ));
                        sprintf( number, "%d", i );
                        strcpy( pipeCOM, "sc" );
                        strncat( pipeCOM, number, 1 );
                        printf( "Pipe: %s Client: %c\n", pipeCOM, req->clientID );
                        fd = open( pipeCOM, O_WRONLY | O_NONBLOCK );
                        if ( fd == -1 )
                        {
                            perror( "PIPE NOT FOUND... EXIT" );
                            printf( "Pipe: %s\n", pipeCOM );
                        }
                        write( fd, res, sizeof( Response ));
                        close( fd );
                        free( pipeCOM );
                        valid = 'n';
                    }
                    else
                    {
                        printf("Old size: %d\n", sizeof( cache[ i ] ));
                        cache[ i ] = (Response*) realloc( cache[ i ], sizeof( cache[ i ] ) + sizeof( Response ));
                        printf("New size: %d\n", sizeof( cache[ i ] ) + sizeof( Response ));
                        total = sizeof( cache[ i ] ) / sizeof( Response );
                        strcpy( cache[ i ][ total - 1 ].message, res->message );
                        printf( "Tweet cached: %s", cache[ i ][ total - 1 ].message );
                    }
                }
            }


        }
        if ( valid != 'n' )
        {
            pipeCOM = malloc( 5 * sizeof( char ));
            strcpy( pipeCOM, "sc" );
            printf( "Pipe: %s Client: %c\n", pipeCOM, req->clientID );

            strncat( pipeCOM, &( req->clientID ), 1 );
            printf( "Pipe: %s\n", pipeCOM );

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