#include <stdio.h>
#include <time.h>
#include "tabla.h"

void imprimir_tabla(Camion *camiones, int n) {
    double suma_espera = 0, suma_retorno = 0;

    printf("\n========== Tabla Comparativa FIFO ==========\n");
    printf("%-8s %-8s %-12s %-12s\n",
           "Camion", "Burst", "T.Espera", "T.Retorno");
    printf("---------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        double espera  = difftime(camiones[i].tiempo_inicio, camiones[i].tiempo_llegada);
        double retorno = difftime(camiones[i].tiempo_fin,    camiones[i].tiempo_llegada);
        suma_espera  += espera;
        suma_retorno += retorno;

        printf("%-8d %-8d %-12.2f %-12.2f\n",
               camiones[i].id,
               camiones[i].burst_time,
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