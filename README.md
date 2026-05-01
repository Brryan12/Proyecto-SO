# Proyecto-SO: Simulador de Sistema de Colas con Sincronización de Threads

Simulación de un sistema de muelles de carga donde múltiples camiones (threads) compiten por acceso a recursos limitados usando sincronización con mutexes y variables de condición en C.

[PDF con las instrucciones del proyecto](./Primer%20proyecto.pdf)

## Descripción del Proyecto

Este proyecto implementa un sistema de planificación de procesos tipo **FIFO (First In, First Out)** con sincronización de threads. Simula un puerto o centro de distribución con un número limitado de muelles de carga. Los camiones llegan de forma concurrente y deben:

1. Obtener un ticket (número de turno)
2. Esperar a que haya un muelle disponible
3. Usar el muelle durante su tiempo de procesamiento (burst_time)
4. Liberar el muelle para el siguiente camión

## Estructura del Proyecto

```
Proyecto-SO/
├── main.c              # Punto de entrada, inicializa camiones y threads
├── tipos.h             # Definiciones de tipos y estructuras
├── camion.h/.c         # Lógica del pipeline de cada camión
├── sync.h/.c           # Primitivas de sincronización (mutexes, condiciones)
├── tabla.h/.c          # Generación de tabla de resultados
├── Primer proyecto.pdf # Especificación del proyecto
└── README.md           # Este archivo
```

## Componentes Principales

### `tipos.h`
Define las estructuras base:
- **state_t**: Enumeración de estados del camión (NUEVO, LISTO, EJECUCION, BLOQUEADO, TERMINADO)
- **Camion**: Estructura que representa cada camión con:
  - `id`: Identificador único
  - `burst_time`: Tiempo de procesamiento en segundos
  - `estado`: Estado actual
  - `thread_id`: ID del thread POSIX
  - Tiempos de llegada, inicio y finalización (para análisis)

### `sync.h/.c`
Gestiona la sincronización global:
- **mutex_log**: Protege la salida a consola
- **mutex_cola**: Protege el estado compartido de la cola
- **cond_cola**: Variable de condición para coordinar camiones
- **muelles_libres**: Contador de muelles disponibles (máximo 3)
- **next_ticket** / **serving**: Control del orden FIFO

**Funciones:**
- `sync_init()`: Inicializa mutexes y variables de condición
- `sync_destroy()`: Libera recursos
- `event()`: Registra cambios de estado con sincronización

### `camion.h/.c`
Implementa el pipeline de cada camión:
1. Llega y obtiene ticket
2. Espera si hay muelles ocupados (BLOQUEADO)
3. Entra al muelle (EJECUCION)
4. Completa su trabajo (sleep)
5. Libera el muelle y termina (TERMINADO)

### `tabla.h/.c`
Genera estadísticas de rendimiento:
- Tiempo de espera: diferencia entre inicio y llegada
- Tiempo de retorno: diferencia entre finalización y llegada
- Promedios de ambas métricas

### `main.c`
Orquesta la simulación:
- Crea 6 camiones con diferentes burst times
- Lanza threads para cada camión
- Espera completación
- Imprime tabla de resultados

## Compilación y Ejecución

### Compilación
```bash
gcc -pthread -o terminal main.c camion.c sync.c tabla.c 
```
### Ejecución
```bash
./terminal
```

## Sincronización Implementada

### Mutex para Cola (mutex_cola)
Protege el acceso a:
- `muelles_libres`: Contador de muelles disponibles
- `next_ticket`: Número de turno siguiente
- `serving`: Número de turno siendo atendido
- Acción: Tomar ticket y esperar condiciones

### Mutex para Log (mutex_log)
Protege:
- Salida a `printf()` para evitar intercalamiento de texto
- Crítico para mantener logs legibles en multithreading

### Variable de Condición (cond_cola)
Coordina:
- Camiones esperan hasta que sea su turno AND haya muelle libre
- `pthread_cond_wait()`: Espera en el mutex_cola
- `pthread_cond_broadcast()`: Notifica cuando hay cambios

### Algoritmo de Planificación: FIFO
```
while (mi_turno != serving || muelles_libres == 0)
    pthread_cond_wait(&cond_cola, &mutex_cola);
```

Garantiza que los camiones son atendidos en orden de llegada sin inanición.
