/*

Disciplina: Computacao Concorrente
Prof.: Silvana Rossetto
Módulo 1 - Laboratório: 3
Codigo: Atividade 2

O objetivo deste Laboratório é projetar e implementar uma versão concorrente para o problema de calcular a soma de uma série de valores reais que aproxima o valor de π; e avaliar o desempenho da aplicação em termos de tempo de execução. Usaremos a linguagem C e a biblioteca Pthreads.

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

typedef struct{
  int id;
  int nthreads;
  long long int elementos;
} Argumento;

void *calcula_pi(void *container) {
  Argumento *arg = (Argumento*) container;
  long double *pi_aprox;
  pi_aprox = malloc(sizeof(long double));
  *pi_aprox = 0;
  long long int i, limite_inferior, limite_superior;
  limite_inferior = (arg->id)*(arg->elementos)/(arg->nthreads);
  limite_superior = ((arg->id)+1)*(arg->elementos)/(arg->nthreads);
  i = limite_inferior;

  while(i < limite_superior) {
    *pi_aprox = *pi_aprox + (long double)(1-(2*(i%2)))/(2*i+1);
    i++;
  }
  *pi_aprox = (*pi_aprox)*4;

  pthread_exit((void *) pi_aprox);
}

int main(int argc, char* argv[]) {
  int nthreads, i;
  pthread_t *tid; 
  Argumento *args;
  long double *parcelas_pi_aprox, pi_aprox = 0;
  long long int elementos;
  double inicio, fim, delta, delta_acumulado;

  GET_TIME(inicio);

  if(argc!=3) {
    printf("Digite: %s <número de elementos> <número de threads>\n", argv[0]);
    return 1;
  }
  elementos = atoi(argv[1]);
  
  if(elementos < 3) {
    fprintf(stderr, "Quantidade de elementos informada é muito pequena\n");
    return 2;
  }

  nthreads = atoi(argv[2]);
  if (nthreads < 1) {
    fprintf(stderr, "Quantidade de threads não pode ser abaixo de 1\n");
    return 3;
  }

  tid = (pthread_t *) malloc(sizeof(pthread_t)*nthreads);
  args = (Argumento *) malloc(sizeof(Argumento)*nthreads);
  if (args == NULL || tid == NULL) {
    printf("ERRO -> malloc\n");
    return 4;
  }

  GET_TIME(fim);
  delta = fim - inicio;
  printf("Tempo inicialização:%lf\n", delta);
  delta_acumulado = delta;

  GET_TIME(inicio);

  for(i=0; i<nthreads; i++) {
    (args+i)->id = i;
    (args+i)->nthreads = nthreads;
    (args+i)->elementos = elementos;
    if(pthread_create((tid+i), NULL, calcula_pi, (void*) (args+i))) {
      printf("ERRO -> pthread_create\n");
      return 5;
    }
  } 

  for(i=0; i<nthreads; i++) {
    if(pthread_join(*(tid+i), (void **) &parcelas_pi_aprox)) {
      printf("ERRO -> pthread_create\n");
      return 6;
    }

    pi_aprox += *parcelas_pi_aprox;
    free(parcelas_pi_aprox);
  }


  for(i=0; i<nthreads; i++) {
  }

  GET_TIME(fim)   
  delta = fim - inicio;
  printf("M_PI(%.10lf) - pi_aprox(%.10Lf)  = %.10Lf\n", M_PI, pi_aprox, M_PI-((long double)pi_aprox));
  printf("Tempo calculo de pi com %lld elementos: %lf\n", elementos, delta);
  delta_acumulado += delta;

  GET_TIME(inicio);
  free(args);
  free(tid);
  GET_TIME(fim);   
  delta = fim - inicio;
  printf("Tempo finalizacao:%lf\n", delta);
  delta_acumulado += delta;
  printf("Tempo total:%lf\n", delta_acumulado);

  return 0;
}
