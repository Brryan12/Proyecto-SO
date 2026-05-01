#include "tipos.h"
#include "sync.h"
#include "camion.h"
#include "tabla.h"

#define NUM_CAMIONES 6 

int main() {
  sync_init();

  // aqui se crean los camiones y se inicializan como hilos y se les asigna un burst_time
  Camion camiones[NUM_CAMIONES]={
    {.id=1, .burst_time=2},
    {.id=2, .burst_time=3},
    {.id=3, .burst_time=1},
    {.id=4, .burst_time=4}, 
    {.id=5, .burst_time=2},
    {.id=6, .burst_time=2}
  };

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
  sync_destroy();
  
  

  return 0;
}





