/*

Disciplina: Computacao Concorrente
Prof.: Silvana Rossetto
Módulo 1 - Trabalho 1
Codigo: Avaliação do método de Integração numérica 

*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

long double calcula_pi(long long int elementos) {
  long long int i = 0;
  long double pi_aprox = 0;
  while(i<elementos) {
    pi_aprox += (long double)(1-(2*(i%2)))/(2*i+1);
    i++;
  }
  pi_aprox = pi_aprox*4;
  return pi_aprox;
}

int main(int argc, char *argv[]) {
  double tick_1, tick_2, tick_acumulado;
  long double pi_aprox;
  long long int elementos;

  //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
  GET_TIME(tick_1);
  if(argc != 2) {
    fprintf(stderr, "Digite: %s <número de elementos>\n", argv[0]);
    return 1; 
  }
  elementos = atoll(argv[1]);

  if(elementos < 3) {
    fprintf(stderr, "Quantidade de elementos informada é muito pequena\n");
    return 2;
  }
  GET_TIME(tick_2);
  printf("Tempo inicialização:  %lf\n", (tick_2-tick_1));
  tick_acumulado = (tick_2-tick_1);

  GET_TIME(tick_1);
  pi_aprox = calcula_pi(elementos);
  GET_TIME(tick_2);
  printf("Tempo calculo de pi com %lld elementos: %lf\n", elementos, (tick_2-tick_1));
  tick_acumulado += (tick_2-tick_1);


  GET_TIME(tick_1);
  printf("pi calculado: %.16Lf\n", pi_aprox);
  printf("pi da math.h: %.16lf\n", M_PI);
  printf("diferença   : %.16Lf\n", M_PI-((long double)pi_aprox));
  GET_TIME(tick_2);
  printf("Tempo finalização:  %lf\n", (tick_2-tick_1));
  tick_acumulado += (tick_2-tick_1);

  printf("Tempo total:  %lf\n", tick_acumulado);

  return 0;
}
