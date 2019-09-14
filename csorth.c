#include "csorth.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define MAX_THREADS 10
#define MAX_PROCESS 21000
#define LIMIT_PROCESS 210000
pthread_t thread_id[ MAX_THREADS ];
struct ThreadData *threads;
unsigned short  r;

int main(int argc, char *argv[])
{
    int n, num_threads, idthread;
    int i, j, k;
    
    struct Process *processes;

    if (argc < 3)
    {
        printf("Modo de uso: csorth [flag] [input] [output]");
        exit(-1);
    }
    else if (argc == 3)
    {
        if (strcmp(argv[1], "-r") == 0)
        {

            printf("Modo de uso: csorth [flag] [input] [output]");
            exit(-1);
        }
    }
    else if (argc == 13)
    {
        if (strcmp(argv[1], "-r") != 0)
        {
            printf("Modo de uso: csorth [flag] [input] [output]");
            exit(-1);
        }
    }
    else if (argc > 13)
    {
        printf("Modo de uso: csorth [flag] [input] [output]");
        exit(-1);
    }

    n = 1;
    r = 0;

    if (strcmp(argv[n], "-r") == 0)
    {
        r = 1;
        n++;
    }

    num_threads = argc - n - 1;
    threads = malloc( sizeof(struct ThreadData) * num_threads);
    i = 0;

    while( n < argc -1 )
    {
        threads[i].num_processes = 0;
        strcpy(threads[i].filename, argv[n]);
        threads[i].thread_index = i;

        //threads[i].processes = malloc( sizeof(struct Process) * 20000 );
        printf("Thread Stacked: \nFilename: %s \n NumProc: %d\n ThreadIndex: %d\n Allocated: %ld\n", threads[i].filename, threads[i].num_processes, threads[i].thread_index, sizeof(threads[i].processes));

        i++;
        n++;
    }

    num_threads = i;
    
    for(i = 0; i < num_threads; i++)
    {
        idthread = pthread_create(&thread_id[i], NULL,(void *) ReadAndSort, (void *) &threads[i]);
        if (idthread != 0)
        {
            perror("Thread creation failed");
            exit(-1);
        }
    }

    for (i = 0; i < num_threads; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
    
    processes = malloc(sizeof(*processes) * LIMIT_PROCESS);

    for(i = 0, k = 0; i < num_threads; i++)
    {
        for(j = 0; j < threads[i].num_processes; j++)
        {
            processes[k] = threads[i].processes[j];
            k++;
        }
    }
    WriteLog(processes, argv[argc - 1], k);
    printf("Gathered All %d Processes Logs\n", k);
    printf("Sorting and Writing\n");
    
    pthread_exit(NULL);
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
    char *s, *sd, *sh, *end;
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
    while (sd != NULL)
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
    while (sh != NULL)
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

void * ReadAndSort(void *thread_data)
{
    char tmp_filename[6];
    char temp_number[4];
    struct ThreadData *data = (struct ThreadData *) thread_data;
    data->processes = malloc( sizeof(data->processes) * 20000);


    data->num_processes = ReadLog( data->processes, data->filename, data->num_processes );
    MergeSort(data->processes, data->num_processes);
    
    strcpy( tmp_filename, "tmp" );
    snprintf(temp_number, 10, "%d", data->thread_index);
    strcat( tmp_filename, temp_number );
    WriteLog( data->processes, tmp_filename, data->num_processes );
    

    pthread_exit((void *) 0);
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
void static _mergeSort(struct Process *processes, struct Process *sorting, int n)
{
    if (n < 2)
    {
        return;
    }
    _mergeSort(processes, sorting, n / 2);
    _mergeSort(processes + n / 2, sorting + n / 2, n - (n / 2));

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
void static MergeSort(struct Process *processes, int n)
{
    struct Process *sorting = malloc(sizeof(struct Process) * n);
    _mergeSort(processes, sorting, n);
    free(sorting);
}