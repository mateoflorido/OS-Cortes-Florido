/*
Archivo: csortp.c
Realizado por: Mateo Florido y Daniela Cortes
Contiene: Implementación de las funciones que permiten leer logs de
procesos, cargar a la estructura Process y ordenarlos mediante
Merge Sort con un comparador implementado.
Fecha de última modificación: 11/09/2019
*/

#include "csortp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXPROCESS 200000
#define ONEPROCESS 20000

int main(int argc, char *argv[])
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

    char json[6];
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
    if (argc == 3 || ( argc == 4 && (strcmp(argv[n], "-r") == 0)))
    {
        processes = malloc(sizeof(*processes) * ONEPROCESS);
        if ((arg_file = fopen(argv[n], "r")) == NULL)
        {
            perror("Error: Reading file.\n");
            return (1);
        }
        while ((read = getline(&line, &len, arg_file)) != -1)
        {
            FillProcess(line, &processes[num_process]);
            num_process++;
        }

        fclose(arg_file);
        n++;
        MergeSort(processes, num_process, r);
        if ((tmpf = fopen(argv[n], "w")) == NULL)
        {
            perror("Error: Writing file.\n");
            return (1);
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
    
        return ( 0 );

    
    }

    for (; n < argc - 1; n++)
    {
        if ((pid = fork()) == -1)
        {
            perror("Error fork process.\n");
            exit(1);
        }
        if (pid == 0)
        {
            break;
        }
    }

    if (pid == 0)
    {
        processes = malloc(sizeof(*processes) * ONEPROCESS);
        if ((arg_file = fopen(argv[n], "r")) == NULL)
        {
            perror("Error: Reading file.\n");
            return (1);
        }
        while ((read = getline(&line, &len, arg_file)) != -1)
        {
            FillProcess(line, &processes[num_process]);
            num_process++;
        }

        fclose(arg_file);
        MergeSort(processes, num_process, r);
        snprintf(json, 10, "%d", n);
        if ((tmpf = fopen(json, "w")) == NULL)
        {
            perror("Error: Writing file.\n");
            return (1);
        }

        for (i = 0; i < num_process; i++)
        {
            fprintf(tmpf, "%-10s", processes[i].user);
            fprintf(tmpf, "%-9s", processes[i].binary);
            fprintf(tmpf, "%3s ", processes[i].priority);
            fprintf(tmpf, "%7d", processes[i].nice);
            fprintf(tmpf, " %02d/%02d/%d", processes[i].day, processes[i].month, processes[i].year);
            fprintf(tmpf, " %02d:%02d:%02d\n", processes[i].hour, processes[i].minute, processes[i].second);
        }
        fclose(tmpf);

        exit(1);
    }

    while ((pid = wait(&status)) != -1)
        ;
    num_process = 0;
    processes = malloc(sizeof(*processes) * MAXPROCESS);
    for (i = r + 1; i < n; i++)
    {
        snprintf(json, 10, "%d", i);
        if ((arg_file = fopen(json, "r")) == NULL)
        {
            perror("Error: Reading file.\n");
            return (1);
        }

        while ((read = getline(&line, &len, arg_file)) != -1)
        {

            FillProcess(line, &processes[num_process]);
            num_process++;
        }

        fclose(arg_file);
    }

    MergeSort(processes, num_process, r);
    if ((tmpf = fopen(argv[n], "w")) == NULL)
    {
        perror("Error: Writting file.\n");
        return (1);
    }
    for (i = 0; i < num_process; i++)
    {

        fprintf(tmpf, "%-10s", processes[i].user);
        fprintf(tmpf, "%-9s", processes[i].binary);
        fprintf(tmpf, "%3s ", processes[i].priority);
        fprintf(tmpf, "%7d", processes[i].nice);
        fprintf(tmpf, " %02d/%02d/%d", processes[i].day, processes[i].month, processes[i].year);
        fprintf(tmpf, " %02d:%02d:%02d\n", processes[i].hour, processes[i].minute, processes[i].second);
    }
    fclose(tmpf);

    exit(1);

   
}

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
void static _mergeSort(struct Process *values, struct Process *aside, int n, unsigned short r)
{
    if (n < 2)
    {
        return;
    }
    _mergeSort(values, aside, n / 2, r);
    _mergeSort(values + n / 2, aside + n / 2, n - (n / 2), r);

    if (r == 0)
        Merge(values, values + n / 2, aside, n);
    else
        MergeReversed(values, values + n / 2, aside, n);
    memcpy(values, aside, n * sizeof(struct Process));
}

void static MergeSort(struct Process *processes, int n, unsigned short r)
{
    struct Process *aside = malloc(sizeof(struct Process) * n);
    _mergeSort(processes, aside, n, r);
    free(aside);
}
