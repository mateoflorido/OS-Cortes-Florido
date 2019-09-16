/* Módulo para el Ordenamiento de Logs por Procesos */
/*
Archivo: csortp.h
Realizado por: Mateo Florido y Daniela Cortes
Contiene: Estructura de un Proceso para facilitar su lectura y
prototipos de funciones que manipulan los logs.
Fecha de última modificación: 11/09/2019
*/
#ifndef CSORTPEXEC_H_
#define CSORTPEXEC_H_

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
#endif
