#include "Client.h"
#include "Request.h"

int main( int argc, char * argv[ ] )
{
    int argvid, argvpipe, fd = -1;
   	argvid = (strcmp( argv[ 1 ], "-i" ) == 0 ) ? 2 : 4;
    argvpipe = ( argvid == 2 ) ? 4 : 2;
    
    fd = open( argv[ argvpipe ], O_WRONLY | O_NONBLOCK );
    if(fd == -1 )
    {
        perror("PIPE NOT FOUND... EXIT");
        exit( 1 );
    }
    struct Request *init = malloc( sizeof( struct Request ) );
    init->clientID = argv[ argvid ];
    init->operationID = 0;
    write( fd, &init, sizeof( struct Request ) );

}