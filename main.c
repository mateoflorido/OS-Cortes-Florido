
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc,char  *argv[]){

	if (argc < 4) 
	{
	    printf ("Modo de uso: csortp||csorth||csortpexe [flag] [input] [output] \n");
	    exit(-1);
	}
	else if (argc == 4) 
	{
		if(strcmp(argv[2],"-r")==0)
	    {
	    	printf ("Modo de uso: csortp||csorth||csortpexe [flag] [input (max 10)] [output] \n");
	    	exit(-1);
	    }
	}
	else if (argc == 14) 
	{
		if(strcmp(argv[2],"-r")!=0)
	    {
	    	printf ("Modo de uso: csortp||csorth||csortpexe [flag] [input (max 10)] [output] \n");
	    	exit(-1);
	    }
	}
	else if (argc > 14) 
	{
	    printf ("Modo de uso: csortp||csorth||csortpexe [flag] [input (max 10)] [output] \n");
	    exit(-1);
	}


	int n = 2;
	printf("\n compile: %s ,",argv[0]);
	printf("\ncomand: %s ,",argv[1]);

	if(strcmp(argv[n],"-r")==0)
	{
		//Inverse
	  	n++;
	}
	
	for( ; n< argc-1 ; n++)
	{
	   	printf(" input: %s ,",argv[n]);
	}
	printf("\n output: %s", argv[n] );






}


