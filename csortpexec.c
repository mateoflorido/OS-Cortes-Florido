#include "csortp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string.h>
#define MAXPROCESS 200000
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

    pid_t pid;
    struct Process *processes;
    char temp_number[6];
    char tmp_filename[10];
    int n = 1, status, num_process = 0, i;
    FILE *nf;
    FILE *tmpf;
    FILE *arrf;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    unsigned short r = 0;

    if (strcmp(argv[n], "-r") == 0)
    {
        r = 1;
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
        num_process++;
    }

    if (pid == 0)
    {

        strcpy(tmp_filename, "tmp");
        snprintf(temp_number, 10, "%d", n);
        strcat(tmp_filename, temp_number);
        if (r == 0)
            execl("/usr/bin/sort", "/usr/bin/sort", "-b", "-k4n", "-k", "5.7,5.8", "-k", "5.1,5.2", "-k", "5.4,5.5", "-k", "6.1,6.2", "-k", "6.4,6.5", "-k", "6.7,6.8", argv[n], "-o", tmp_filename, NULL);
        else
            execl("/usr/bin/sort", "/usr/bin/sort", "-b", "-k4nr", "-k", "5.7,5.8r", "-k", "5.1,5.2r", "-k", "5.4,5.5r", "-k", "6.1,6.2r", "-k", "6.4,6.5r", "-k", "6.7,6.8r", argv[n], "-o", tmp_filename, NULL);

        exit(1);
    }

    while ((pid = wait(&status)) != -1)
        ;

    printf("Numero p %d", num_process);
    execl("/usr/bin/cat", "/usr/bin/cat", "tmp*", ">>", "tmpfinal", NULL);

    if (r == 0)
        execl("/usr/bin/sort", "/usr/bin/sort", "-b", "-k4n", "-k", "5.7,5.8", "-k", "5.1,5.2", "-k", "5.4,5.5", "-k", "6.1,6.2", "-k", "6.4,6.5", "-k", "6.7,6.8", "tmpfinal", "-o", argv[n], NULL);
    else
        execl("/usr/bin/sort", "/usr/bin/sort", "-b", "-k4nr", "-k", "5.7,5.8r", "-k", "5.1,5.2r", "-k", "5.4,5.5r", "-k", "6.1,6.2r", "-k", "6.4,6.5r", "-k", "6.7,6.8r", "tmpfinal", "-o",  argv[n], NULL);

    return (0);
}
