#include "csortpexec.h"
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

        num_process = 0;
        processes = malloc(sizeof(*processes) * MAXPROCESS);
        for (i = r + 1; i < n; i++)
        {
            strcpy( tmp_filename, "tmp" );
            snprintf(temp_number, 10, "%d", i);
            strcat( tmp_filename, temp_number );
            num_process = ReadLog( processes, tmp_filename, num_process );
            remove( tmp_filename );
        }
        WriteLog(processes,"tmpfinal", num_process );


    if (r == 0)
        execl("/usr/bin/sort", "/usr/bin/sort", "-b", "-k4n", "-k", "5.7,5.8", "-k", "5.1,5.2", "-k", "5.4,5.5", "-k", "6.1,6.2", "-k", "6.4,6.5", "-k", "6.7,6.8", "tmpfinal", "-o", argv[n], NULL);
    else
        execl("/usr/bin/sort", "/usr/bin/sort", "-b", "-k4nr", "-k", "5.7,5.8r", "-k", "5.1,5.2r", "-k", "5.4,5.5r", "-k", "6.1,6.2r", "-k", "6.4,6.5r", "-k", "6.7,6.8r", "tmpfinal", "-o",  argv[n], NULL);

return (0);
}
/*
Función: ReadLog
Parámetros de Entrada:
    *processes: apuntador al array que contendrá los procesos leídos.
    filename: nombre del archivo a leer.
    start: índice del array donde va la lectura de los procesos.
Valor de Salida: total de procesos contenidos en *processes
Descripción: Lee todos los procesos contenidos en el archivo descrito
y los carga en memoria.
*/
int static ReadLog(struct Process *processes, char *filename, int start)
{
    int num_process = start;
    FILE *arg_file;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    if ((arg_file = fopen(filename, "r")) == NULL)
    {
        perror("Error Reading file");
        exit(1);
    }
    while ((read = getline(&line, &len, arg_file)) != -1)
    {
        FillProcess(line, &processes[num_process]);
        num_process++;
    }
    fclose(arg_file);
    return( num_process );
}
/*
Función: FillProcess
Parámetros de Entrada:
    line: cadena de caracteres donde se encuentra la información del proceso.
    *p: estructura donde se guardarán los datos del proceso.
Valor de Salida: Ninguno
Descripción: Descompone la linea de texto recibida y guarda en la estructura la
información encontrada.
*/
void static FillProcess(char *line, struct Process *p)
{

    const char tokens[2] = " ";
    const char tokend[2] = "/";
    const char tokenh[2] = ":";
    char *s, *sd, *sh;
    char aux[20];
    char aux2[20];
    int c = 0;
    s = strtok(line, tokens);
    while (s != NULL)
    {
        if (c == 0)
        {
            strcpy((p->user), s);
        }
        else if (c == 1)
        {
            strcpy((p->binary), s);
        }
        else if (c == 2)
        {
            strcpy((p->priority), s);
        }
        else if (c == 3)
        {
            p->nice = atoi(s);
        }
        else if (c == 4)
        {
            strcpy(aux, s);
        }
        if (c == 5)
        {
            strcpy(aux2, s);
        }

        s = strtok(NULL, tokens);
        c++;
    }

    c = 0;
    sd = strtok(aux, tokend);
    while (sd != NULL || c < 3)
    {
        if (c == 0)
        {
            p->day = atoi(sd);
        }
        else if (c == 1)
        {
            p->month = atoi(sd);
        }
        else if (c == 2)
        {
            p->year = atoi(sd);
        }
        sd = strtok(NULL, tokend);

        c++;
    }
    c = 0;
    sh = strtok(aux2, tokenh);
    while (sh != NULL || c < 3)
    {
        if (c == 0)
        {
            p->hour = atoi(sh);
        }
        else if (c == 1)
        {
            p->minute = atoi(sh);
        }
        else if (c == 2)
        {
            p->second = atoi(sh);
        }
        sh = strtok(NULL, tokenh);
        c++;
    }
}
/*
Función: WriteLog
Parámetros de Entrada:
    *processes: apuntador a todos los procesos a  escribir.
    filename: nombre del archivo.
    num_process: numero de procesos en el array de processes.
Valor de Salida: Ninguno
Descripción: Escribe todos los procesos contenidos en el array
processes en un archivo.
*/
void static WriteLog(struct Process *processes, char *filename, int num_process)
{
    int i;
    FILE *arg_file, *tmpf, *arrf;

    if ((tmpf = fopen(filename, "w")) == NULL)
        {
            perror("Error: Writing file.\n");
            exit(1);
        }

        for (i = 0; i < num_process; i++)
        {
            fprintf(tmpf, "%-10s", processes[i].user);
            fprintf(tmpf, "%-9s", processes[i].binary);
            fprintf(tmpf, "%3s", processes[i].priority);
            fprintf(tmpf, "%7d", processes[i].nice);
            fprintf(tmpf, " %02d/%02d/%d", processes[i].day, processes[i].month, processes[i].year);
            fprintf(tmpf, " %02d:%02d:%02d\n", processes[i].hour, processes[i].minute, processes[i].second);
        }
        fclose(tmpf);
}
