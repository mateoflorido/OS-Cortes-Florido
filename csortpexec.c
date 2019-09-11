
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

struct Process
{
    char *user[36];
    char *binary[100];
    char priority;
    int nice;
    unsigned short day;
    unsigned short month;
    unsigned short year;
    unsigned short hour;
    unsigned short minute;
    unsigned short second;
};

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        printf("Modo de uso: csortpexe [flag] [input] [output]");
        exit(-1);
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "-r") == 0)
        {
            printf("Modo de uso: csortpexe [flag] [input (max 10)] [output]");
            exit(-1);
        }
    }
    else if (argc == 13)
    {
        if (strcmp(argv[1], "-r") != 0)
        {
            printf("Modo de uso: csortpexe [flag] [input (max 10)] [output]");
            exit(-1);
        }
    }
    else if (argc > 13)
    {
        printf("Modo de uso: csortpexe [flag] [input (max 10)] [output]");
        exit(-1);
    }

    printf("\n compile: %s ,", argv[0]);
    printf("\ncomand: %s ,", argv[1]);
    pid_t pid;
    int n = 1;
    struct Process *processes;
    int n = 1, status, num_process = 0;
    FILE *nf;
    FILE *tmpf;
    FILE *arrf;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    unsigned short r = 0; 

    if (strcmp(argv[n], "-r") == 0)
    {
        //Inverse
        n++;
    }

    for (; n < argc - 1; n++)
    {
        if ((pid = fork()) == -1)
        {
            perror("Error fork");
            exit(1);
        }
        if (pid == 0)
        {
            break;
        }
    }

    if (pid == 0)
    {
        processes = malloc(sizeof(*processes) * 20000);
        if ((nf = fopen(argv[n], "r")) == NULL)
        {
            perror("ERROR OPEN: Reading file. ");
            return (1);
        }
        while ((read = getline(&line, &len, nf)) != -1)
        {
            fillProcess(line, &processes[num_process]);
            num_process++;
        }

        fclose(nf);
        
        // Sort del SO.EXEC

        exit(1);
    }

    while ((pid = wait(&status)) != -1)
        ;
    
  
    
     printf(" PADRE->escribir archivo: %s, %d ,", argv[n], getpid());
    printf("\n output: %s", argv[n]);
}
