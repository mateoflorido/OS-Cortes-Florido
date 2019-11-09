/*
Archivo: csortp.c
Realizado por: Mateo Florido y Daniela Cortes
Contiene: Implementación de las funciones que permiten leer logs de
procesos, cargar a la estructura Process y ordenarlos mediante
Merge Sort.
Fecha de última modificación: 11/09/2019
*/

#include "csortp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXPROCESS 200000
#define ONEPROCESS 20000

int main(int argc, char *argv[ ])
{

    if (argc < 3)
    {
        printf("Usage: csortp||csorth||csortpexe [flag] [input] [output]");
        exit(-1);
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "-r") == 0)
        {
            printf("Usage: csortp [flag] [input (max 10)] [output]");
            exit(-1);
        }
    }
    else if (argc == 13)
    {
        if (strcmp(argv[1], "-r") != 0)
        {
            printf("Usage: csortp [flag] [input (max 10)] [output]");
            exit(-1);
        }
    }
    else if (argc > 13)
    {
            printf("Usage: csortp [flag] [input (max 10)] [output]");
        exit(-1);
    }

    char temp_number[6];
    char tmp_filename[10];
    char *line = NULL;
    struct Process *processes;
    int i, n = 1 , num_process = 0, status;
    pid_t pid, wpid;
    FILE *arg_file, *tmpf, *arrf;
    size_t len = 0;
    ssize_t read;
    unsigned short r = 0;
    

    if (strcmp(argv[n], "-r") == 0)
    {
    
        r = 1;
        n++;
    }
    if ( argc == 3 || ( argc == 4 && ( strcmp( argv[ n ], "-r" ) == 0 ) ) )
    {
        processes = malloc( sizeof( *processes ) * ONEPROCESS );
        num_process = ReadLog( processes, argv[ n ], num_process );
        n++;
        MergeSort(processes, num_process, r);
        WriteLog(processes, argv[n], num_process);

        return ( 0 );
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
        processes = malloc( sizeof(*processes) * ONEPROCESS);
        num_process = ReadLog( processes, argv[ n ] , num_process);

        MergeSort(processes, num_process, r);
        strcpy( tmp_filename, "tmp" );
        snprintf(temp_number, 10, "%d", n);
        strcat( tmp_filename, temp_number );
        WriteLog( processes, tmp_filename, num_process );

        exit(1);
    }

    while ((pid = wait(&status)) != -1);

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

    MergeSort(processes, num_process, r);
    WriteLog(processes,argv[ n ], num_process );

    return(0);

   
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
Función: CompareProcessReversed
Parámetros de Entrada: 
    *p1: proceso a comparar con p2.
    *p2: proceso a comparar con p1.
Valor de Salida: entero sin signo corto.
Descripción: Compara los dos procesos para devolver su orden en
el criterio descendente.
*/
unsigned short static CompareProcessReversed(const struct Process *p1, const struct Process *p2)
{
   
    if (p1->nice > p2->nice)
        return (1);
    else if (p1->nice < p2->nice)
        return (0);
    else
    {
        if (p1->year > p2->year)
        {
            return (1);
        }
        else if (p1->year < p2->year)
            return (0);
        else
        {
            if (p1->month > p2->month)
            {
                return (1);
            }
            else if (p1->month < p2->month)
                return (0);
            else
            {
                if (p1->day > p2->day)
                {
                    return (1);
                }
                else if (p1->day < p2->day)
                    return (0);
                else
                {
                    if (p1->hour > p2->hour)
                    {
                        return (1);
                    }
                    else if (p1->hour < p2->hour)
                        return (0);
                    else
                    {
                        if (p1->minute > p2->minute)
                        {
                            return (1);
                        }
                        else if (p1->minute < p2->minute)
                            return (0);
                        else
                        {
                            if (p1->second >= p2->second)
                            {
                                return (1);
                            }
                            else if (p1->second < p2->second)
                                return (0);
                        }
                    }
                }
            }
        }
    }
}
/*
Función: CompareProcess
Parámetros de Entrada: 
    *p1: proceso a comparar con p2.
    *p2: proceso a comparar con p1.
Valor de Salida: entero sin signo corto.
Descripción: Compara los dos procesos para devolver su orden en
el criterio ascendente.
*/
unsigned short static CompareProcess(const struct Process *p1, const struct Process *p2)
{
  
    if (p1->nice < p2->nice)
        return (1);
    else if (p1->nice > p2->nice)
        return (0);
    else
    {
        if (p1->year < p2->year)
        {
            return (1);
        }
        else if (p1->year > p2->year)
            return (0);
        else
        {
            if (p1->month < p2->month)
            {
                return (1);
            }
            else if (p1->month > p2->month)
                return (0);
            else
            {
                if (p1->day < p2->day)
                {
                    return (1);
                }
                else if (p1->day > p2->day)
                    return (0);
                else
                {
                    if (p1->hour < p2->hour)
                    {
                        return (1);
                    }
                    else if (p1->hour > p2->hour)
                        return (0);
                    else
                    {
                        if (p1->minute < p2->minute)
                        {
                            return (1);
                        }
                        else if (p1->minute > p2->minute)
                            return (0);
                        else
                        {
                            if (p1->second <= p2->second)
                            {
                                return (1);
                            }
                            else if (p1->second > p2->second)
                                return (0);
                        }
                    }
                }
            }
        }
    }
}
/*
Función: Merge
Parámetros de Entrada: 
    left: sublista izquierda.
    right: sublista derecha.
    res: lista mezclada final.
    n: tamaño de las sublistas.
Valor de Salida: Ninguna
Descripción: Ordena cada sublista aplicando el ordenamiento
por mezcla de forma ascendente y las junta en una sola.
*/
void static Merge(const struct Process *left, const struct Process *right, struct Process *res, int n)
{
    int i = 0, j = 0;

    while ((i < n / 2) && (j < n - (n / 2)))
    {
        if (CompareProcess(&left[i], &right[j]) == 1)
        {
            res[i + j] = left[i];
            i++;
        }
        else
        {
            res[i + j] = right[j];
            j++;
        }
    
    }
    while (i < n / 2)
    {
        res[i + j] = left[i];
        i++;
    }
    while (j < n - (n / 2))
    {
        res[i + j] = right[j];
        j++;
    }
  
    return;
}
/*
Función: MergeReversed
Parámetros de Entrada: 
    left: sublista izquierda.
    right: sublista derecha.
    res: lista mezclada final.
    n: tamaño de las sublistas.
Valor de Salida: Ninguna
Descripción: Ordena cada sublista aplicando el ordenamiento
por mezcla de forma descendente y las junta en una sola.
*/
void static MergeReversed(const struct Process *left, const struct Process *right, struct Process *res, int n)
{
    int i = 0, j = 0;

    while ((i < n / 2) && (j < n - (n / 2)))
    {
        if (CompareProcessReversed(&left[i], &right[j]) == 1)
        {
            res[i + j] = left[i];
            i++;
        }
        else
        {
            res[i + j] = right[j];
            j++;
        }
    }
    while (i < n / 2)
    {
        res[i + j] = left[i];
        i++;
    }
    while (j < n - (n / 2))
    {
        res[i + j] = right[j];
        j++;
    }
    return;
}
/*
Función: _mergeSort
Parámetros de Entrada: 
    processes: todos los procesos a ordenar.
    sorting: lista auxiliar para ordenar los procesos.
    r: flag para indicar si es ascendente o descendente.
    n: total de procesos en processes.
Valor de Salida: Ninguna
Descripción: Función recursiva que divide la lista en dos sublistas.
*/
void static _mergeSort(struct Process *processes, struct Process *sorting, int n, unsigned short r)
{
    if (n < 2)
    {
        return;
    }
    _mergeSort(processes, sorting, n / 2, r);
    _mergeSort(processes + n / 2, sorting + n / 2, n - (n / 2), r);

    if (r == 0)
        Merge(processes, processes + n / 2, sorting, n);
    else
        MergeReversed(processes, processes + n / 2, sorting, n);
    memcpy(processes, sorting, n * sizeof(struct Process));
}
/*
Función: MergeSort
Parámetros de Entrada: 
    processes: todos los procesos a ordenar.
    r: flag para indicar si es ascendente o descendente.
    n: total de procesos en processes.
Valor de Salida: Ninguna
Descripción: Función landing que crea la memoria necesaria para el
proceso de ordenamiento y luego realiza el ordenamiento.
*/
void static MergeSort(struct Process *processes, int n, unsigned short r)
{
    struct Process *sorting = malloc(sizeof(struct Process) * n);
    _mergeSort(processes, sorting, n, r);
    free(sorting);
}