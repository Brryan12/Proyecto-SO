#include <time.h>
#include <unistd.h>
#include "camion.h"
#include "sync.h"

void* camion_pipeline(void *camion) {
    Camion* c = (Camion*) camion;
    c->tiempo_llegada = time(NULL);
    event(c, NUEVO, "Camion creado, entrando");

    // Tomar ticket en orden de llegada
    pthread_mutex_lock(&mutex_cola);
    int mi_turno = next_ticket++;
    pthread_mutex_unlock(&mutex_cola);

    event(c, LISTO, "Esperando turno en la cola de planificacion");

    // Si no es su turno o no hay muelle → BLOQUEADO
    pthread_mutex_lock(&mutex_cola);
    if (mi_turno != serving || muelles_libres == 0) {
        pthread_mutex_unlock(&mutex_cola);
        event(c, BLOQUEADO, "Todos los muelles ocupados, esperando en cola FIFO");
        pthread_mutex_lock(&mutex_cola);
        while (mi_turno != serving || muelles_libres == 0)
            pthread_cond_wait(&cond_cola, &mutex_cola);
    }

    // Entrar al muelle
    muelles_libres--;
    serving++;
    pthread_cond_broadcast(&cond_cola); // avisar al siguiente en cola
    pthread_mutex_unlock(&mutex_cola);

    c->tiempo_inicio = time(NULL);
    event(c, EJECUCION, "Entrando al muelle");
    sleep(c->burst_time);

    // Liberar muelle y avisar
    pthread_mutex_lock(&mutex_cola);
    muelles_libres++;
    pthread_cond_broadcast(&cond_cola);
    pthread_mutex_unlock(&mutex_cola);

    event(c, BLOQUEADO, "Esperando acceso al log de operaciones");
    event(c, TERMINADO, "Carga finalizada, saliendo");
    c->tiempo_fin = time(NULL);

    return NULL;
}