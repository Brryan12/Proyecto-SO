#ifndef TIPOS_H
#define TIPOS_H

#include <pthread.h>

typedef enum { NUEVO, LISTO, EJECUCION, BLOQUEADO, TERMINADO } state_t;
extern const char* estados[];

typedef struct {
    int id;
    int burst_time;
    state_t estado;
    pthread_t thread_id;
    double tiempo_llegada;
    double tiempo_inicio;
    double tiempo_fin;
} Camion;

#endif