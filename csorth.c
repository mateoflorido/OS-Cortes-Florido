
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

struct Process
{
    char user[36];
    char binary[100];
    char priority[3];
    int nice;
    unsigned short day;
    unsigned short month;
    unsigned short year;
    unsigned short hour;
    unsigned short minute;
    unsigned short second;
};

void fillProcess(char *line, struct Process *p);
void merge(const struct Process *left, const struct Process *right, struct Process *res, int n);
void merger(const struct Process *left, const struct Process *right, struct Process *res, int n);
void mergeSort(struct Process *processes, int n);
void _mergeSort(struct Process *values, struct Process *aside, int n);
unsigned short comparePro(const struct Process *p1, const struct Process *p2);
unsigned short comparePror(const struct Process *p1, const struct Process *p2);
void printProcess(struct Process *processes, int n);
void readFile(void *arg);

struct Process *processesg;
int n_p;
unsigned short  r;
int main(int argc, char *argv[])
{

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

    printf("\n compile: %s ,", argv[0]);

    struct Process **gathered[10];
    int n = 1, status, num_process = 0;
    pthread_t thread[10];
    int i=0, num_File = 0;
    FILE *nf;
    FILE *tmpf;
    void *res;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    r = 0;
    char json[6];

    if (strcmp(argv[n], "-r") == 0)
    {
        r = 1;
        n++;
    }
    num_process = 0;
    processesg = malloc(sizeof(*processesg) * 200000);
    n_p = 0;
    for (; n < argc - 1; n++)
    {
        int idtheard = pthread_create(&thread[i], NULL, (void *)readFile, (void *)argv[n]);
        if (idtheard != 0)
        {
            perror("Thread creation failed");
            exit(-1);
        }
        printf(" input: %s \n", argv[n]);
        num_File++;i++;
    }
    printf(" NUM FILES %d \n", num_File);
    for (i = 0; i < num_File; i++)
    {
        pthread_join(thread[i], NULL);
    }

    mergeSort(processesg, n_p);
    printf("******OUT: %s, %d\n", argv[n], n_p);

    
    if ((tmpf = fopen(argv[n], "w")) == NULL)
    {
        perror("ERROR OPEN: Writting file. ");
        return (1);
    }
    for (i = 0; i < n_p; i++)
    {

        fprintf(tmpf, "%s  ", processesg[i].user);
        fprintf(tmpf, "%2s", processesg[i].binary);
        fprintf(tmpf, "% 7s ", processesg[i].priority);
        fprintf(tmpf, "%  7d", processesg[i].nice);
        fprintf(tmpf, " %02d/%02d/%d", processesg[i].day, processesg[i].month, processesg[i].year);
        fprintf(tmpf, " %02d:%02d:%02d\n", processesg[i].hour, processesg[i].minute, processesg[i].second);
    }
    fclose(tmpf);
    pthread_exit(0);
}

void fillProcess(char *line, struct Process *p)
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
unsigned short comparePror(const struct Process *p1, const struct Process *p2)
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

unsigned short comparePro(const struct Process *p1, const struct Process *p2)
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
void merge(const struct Process *left, const struct Process *right, struct Process *res, int n)
{
    int i = 0, j = 0;

    while ((i < n / 2) && (j < n - (n / 2)))
    {
        if (comparePro(&left[i], &right[j]) == 1)
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

    return res;
}

void merger(const struct Process *left, const struct Process *right, struct Process *res, int n)
{
    int i = 0, j = 0;

    while ((i < n / 2) && (j < n - (n / 2)))
    {
        if (comparePror(&left[i], &right[j]) == 1)
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
    return res;
}
void _mergeSort(struct Process *values, struct Process *aside, int n)
{
    if (n < 2)
    {
        return;
    }
    _mergeSort(values, aside, n / 2);
    _mergeSort(values + n / 2, aside + n / 2, n - (n / 2));

     if (r == 0)
        merge(values, values + n / 2, aside, n);
    else
        merger(values, values + n / 2, aside, n);
    memcpy(values, aside, n * sizeof(struct Process));
}

void mergeSort(struct Process *processes, int n)
{
    struct Process *aside = malloc(sizeof(struct Process) * n);
    _mergeSort(processes, aside, n);
    free(aside);
}
void readFile(void *arg)
{
    FILE *nf;
    struct Process *processes = malloc(sizeof(*processes) * 20000);
    int num_process = 0, i, lim = n_p, j = 0;
    FILE *tmpf;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *nomFile = (char *)arg;
    printf("Name of File: %s\n", nomFile);
    if ((nf = fopen(nomFile, "r")) == NULL)
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

    mergeSort(processes, num_process);
    
    printf("arch:%s, pro:%d\n", nomFile, num_process);
    for (i = n_p; i < lim + num_process; i++)
    {
        processesg[i] = processes[j];
        n_p++;
        j++;
    }
    printf("THEN: %d\n", n_p);
    pthread_exit(NULL);
}
