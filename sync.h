#ifndef SYNC_H
#define SYNC_H

#include <pthread.h>
#include "tipos.h"

#define MAX_MUELLES 3

extern pthread_mutex_t mutex_log;
extern pthread_mutex_t mutex_cola;
extern pthread_cond_t  cond_cola;
extern int muelles_libres;
extern int next_ticket;
extern int serving;

void sync_init();
void sync_destroy();
void event(Camion *c, state_t estado_nuevo, const char *detalle);

#endif