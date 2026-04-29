#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

// Definicion de estados de los hilos (camiones)
typedef enum { NUEVO, LISTO, EJECUCION, BLOQUEADO, TERMINADO } state_t;
const char* estados[] = { "NUEVO", "LISTO", "EJECUCION", "BLOQUEADO", "TERMINADO" };


typedef struct {
  int id;
  int burst_time; 
  int prioridad; // Para Round Robin (Quantum) 
  state_t estado;  
  pthread_t thread_id;
  // para la tabla comparativa
  double tiempo_llegada;
  double tiempo_inicio;
  double tiempo_fin;
} Camion;

// globales y sync
#define MAX_MUELLES 3
#define NUM_CAMIONES 6 
sem_t sem_muelles;
pthread_mutex_t mutex_log;



// registrar eventos y actualizacion del estado del camion
void event(Camion *c, state_t estado_nuevo, const char *detalle) {
  // bloqueamos para evitar race condition
  pthread_mutex_lock(&mutex_log);
  c->estado = estado_nuevo;
  printf("Camion %d | Estado: %s | %s\n", c->id, estados[c->estado], detalle);
  
  // liberamos al terminar la accion del hilo
  pthread_mutex_unlock(&mutex_log);
}

void* camion_pipeline(void *camion) {
  Camion* c = (Camion*) camion;
  c->tiempo_llegada = time(NULL);
  event(c, NUEVO, "Camion creado, entrando");
  event(c, LISTO, "Esperando turno en la cola de planificacion");
  
  // hilo espera a que alguno de los 3 muelles se desbloqueen para seguir
  sem_wait(&sem_muelles);


  c->tiempo_inicio = time(NULL);
  event(c, EJECUCION, "Entrando al muelle");
  // se simula el tiempo de CPU en segundos
  sleep(c->burst_time);
  
  // libera y termina
  sem_post(&sem_muelles);


  event(c, BLOQUEADO, "Esperando acceso al log de operaciones");
  event(c, TERMINADO, "Carga finalizada, saliendo");

  c->tiempo_fin = time(NULL);
  
  return NULL;
}

//comparador de qsort es segun prioridad
int cmp_prioridad(const void *a, const void *b) {
  Camion *c1 = (Camion*) a;
  Camion *c2 = (Camion*) b;
  return c2->prioridad - c1->prioridad; // orden descendente
}

// una tabla comparativa para saber si el resultado funka

void imprimir_tabla(Camion *camiones, int n) {
    double suma_espera = 0, suma_retorno = 0;

    printf("\n========== Tabla Comparativa FIFO ==========\n");
    printf("%-8s %-8s %-10s %-12s %-12s\n",
           "Camion", "Burst", "Prioridad", "T.Espera", "T.Retorno");
    printf("---------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        double espera  = difftime(camiones[i].tiempo_inicio, camiones[i].tiempo_llegada);
        double retorno = difftime(camiones[i].tiempo_fin,    camiones[i].tiempo_llegada);
        suma_espera  += espera;
        suma_retorno += retorno;

        printf("%-8d %-8d %-10d %-12.2f %-12.2f\n",
               camiones[i].id,
               camiones[i].burst_time,
               camiones[i].prioridad,
               espera,
               retorno);
    }

    printf("---------------------------------------------\n");
    printf("%-18s %-10s %-12.2f %-12.2f\n",
           "Promedio", "",
           suma_espera  / n,
           suma_retorno / n);
    printf("=============================================\n\n");
}


// HECHO: manejo de race conditions, pipeline del camion y cambios de estados mediante 
// funciones con logs en consola

// PARA SILVA: 
// - Implementar Round Robin o FIFO (igual deje listo el quantum en el camion por si quiere usar RR)
// - En la cola de hilos sacar el camion con mayor prioridad para ejecutar su pipeline
// - Inicializar los camiones como hilos en el main
// - Limpiar recursos despues de pthread_join al finalizar
// Depende de como quiera organizarselo, algunas de estas tareas se las puede delegar a 
// alguien mas o completarlas ud, como guste


int main() {
  sem_init(&sem_muelles, 0, MAX_MUELLES);
  pthread_mutex_init(&mutex_log, NULL);

  // aqui se crean los camiones y se inicializan como hilos, se les asigna un burst_time y prioridad
  Camion camiones[NUM_CAMIONES]={
    {.id=1, .burst_time=2, .prioridad=1},
    {.id=2, .burst_time=3, .prioridad=3},
    {.id=3, .burst_time=1, .prioridad=2},
    {.id=4, .burst_time=4, .prioridad=5}, 
    {.id=5, .burst_time=2, .prioridad=4},
    {.id=6, .burst_time=2, .prioridad=5}
  };

  // se ordenan por prioridad antes de crear los hilos
  qsort(camiones, NUM_CAMIONES, sizeof(Camion), cmp_prioridad);

  // Se lanzan los hilos para cada camion
  for (int i = 0; i < NUM_CAMIONES; i++) {
    pthread_create(&camiones[i].thread_id, NULL, camion_pipeline, &camiones[i]);
  }
  // Se espera a que todos los hilos terminen
  for (int i = 0; i < NUM_CAMIONES; i++) {
    pthread_join(camiones[i].thread_id, NULL);
  }
  //mostramos la tabla de resultados
  imprimir_tabla(camiones, NUM_CAMIONES);

  //limpiamos los recursos de los hilos
  sem_destroy(&sem_muelles);
  pthread_mutex_destroy(&mutex_log);
  
  

  return 0;
}





