/* Módulo para el Ordenamiento de Logs por Procesos */
/*
Archivo: csortp.h
Realizado por: Mateo Florido y Daniela Cortes
Contiene: Estructura de un Proceso para facilitar su lectura y
prototipos de funciones que manipulan los logs.
Fecha de última modificación: 11/09/2019
*/
#ifndef CSORTP_H_
#define CSORTP_H_

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
void static FillProcess(char *line, struct Process *p);
int static ReadLog(struct Process *processes, char *filename, int start);
void static WriteLog(struct Process *processes, char *filename, int num_process);
void static Merge(const struct Process *left, const struct Process *right, struct Process *res, int n);
void static MergeReversed(const struct Process *left, const struct Process *right, struct Process *res, int n);
void static MergeSort(struct Process *processes, int n, unsigned short r);
void static _mergeSort(struct Process *values, struct Process *aside, int n, unsigned short r);
unsigned short static CompareProcess(const struct Process *p1, const struct Process *p2);
unsigned short static CompareProcessReversed(const struct Process *p1, const struct Process *p2);

#endif