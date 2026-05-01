#include <stdio.h>
#include "sync.h"

const char* estados[] = { "NUEVO", "LISTO", "EJECUCION", "BLOQUEADO", "TERMINADO" };

pthread_mutex_t mutex_log;
pthread_mutex_t mutex_cola;
pthread_cond_t  cond_cola;
int muelles_libres;
int next_ticket;
int serving;

void sync_init() {
    pthread_mutex_init(&mutex_log,  NULL);
    pthread_mutex_init(&mutex_cola, NULL);
    pthread_cond_init(&cond_cola,   NULL);
    muelles_libres = MAX_MUELLES;
    next_ticket    = 0;
    serving        = 0;
}

void sync_destroy() {
    pthread_mutex_destroy(&mutex_log);
    pthread_mutex_destroy(&mutex_cola);
    pthread_cond_destroy(&cond_cola);
}

void event(Camion *c, state_t estado_nuevo, const char *detalle) {
    pthread_mutex_lock(&mutex_log);
    c->estado = estado_nuevo;
    printf("Camion %d | Estado: %s | %s\n", c->id, estados[c->estado], detalle);
    pthread_mutex_unlock(&mutex_log);
}