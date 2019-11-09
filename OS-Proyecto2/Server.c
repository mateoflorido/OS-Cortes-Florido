#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include "Server.h"
#include "Request.h"

int main(int argc, char **argv)
{
	int fd, pid, n, bytes, creado = 0;
	int i = 0, j = 0;
	FILE *fileRelations;
	int argvpipe, argvarc;
	mode_t fifo_mode = S_IRUSR | S_IWUSR;

	/*Check input parameters from console*/
	assert( argc == 5 );

	/*Allocate all memory required*/
	relations = malloc( 10 * sizeof( char* ) );
	connected = malloc( 10 * sizeof( char ) );
	for( i = 0; i < 10 ; i++ )
	{
		connected[ i ] = 0;
		relations[ i ] = malloc( 10 * sizeof( char ) );
	}
	printf("Created Relations 2 \n");

	/*Check input from console*/
	argvarc = (strcmp( argv[ 1 ], "-r" ) == 0 ) ? 2 : 4;
	argvpipe = ( argvarc == 2 ) ? 4 : 2;
	
	printf( "FILE: %s \n", argv[ argvarc ] );
	fileRelations = fopen( argv[ argvarc ], "r" );
	if(!fileRelations)
		perror("FOPEN ERROR ");
	printf( "FILE: %s Open \n", argv[ argvarc ]);
	for( i = 0; i < 10; i++ )
	{
		for( j = 0; j < 10; j++ )
		{
			if( !fscanf( fileRelations, "%c", &relations[ i ][ j ]))
				break;
		}
	}/*
	for( i = 0; i < 10; i++ )
	{
		for( j = 0; j < 10; j++ )
		{
			printf("%c ", relations[ i ][ j ]);
		}
		printf("\n");
	}*/

	/*Unlinking Pipe*/
	unlink( argv[ argvpipe ] );

	if( mkfifo( argv[ argvpipe ], fifo_mode ) == -1 )
	{
		perror( "MKFIFO ERROR" );
		exit( 1 );
	}

	/*Creating Pipes*/
	do
	{
		printf("Trying\n");
		fd = open( argv[ argvpipe ], O_RDONLY );
		if( fd == -1 )
			perror("Pipe se perdio");
		else
		{
			printf("Me vooy");
			creado = 1;
		}
		
	} while( creado == 0 );

	read( fd, &req, sizeof( struct Request ));
	
}