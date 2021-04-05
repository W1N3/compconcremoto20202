/*

Disciplina: Computacao Concorrente
Prof.: Silvana Rossetto
Módulo 1 - Laboratório: 2
Codigo: Atividade 5 

Objetivo: Projetar e implementar uma versão concorrente para o problema de multiplicação de matrizes; e avaliar o desempenho da aplicação em termos de tempo de execuo. Usaremos a linguagem C e a biblioteca Pthreads.

*/
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

typedef struct{
   int id;
   int dim;
   int nthreads;
   float* matriz_1;
   float* matriz_2;
   float* matriz_saida;
} Argumento;

void * multiplica_matrizes(void *container) {
  int linha, coluna, interno, slice_start, slice_end;
   Argumento *arg = (Argumento*) container;
   // a estratégia a ser utilizada é de dividir em n
   slice_start = (arg->id)*(arg->dim)/(arg->nthreads);
   slice_end = (arg->id+1)*(arg->dim)/(arg->nthreads);
   for(linha = slice_start; linha < slice_end; linha++) {
      for(coluna = 0; coluna < arg->dim; coluna++) {
        arg->matriz_saida[linha*(arg->dim)+coluna] = 0;
        for(interno = 0; interno < arg->dim; interno++) {
         // a primeira matriz se varre a linha, ou seja, mantenho a linha e itero a coluna
         // a segunda matriz se varre a coluna, ou seja, mantenho a coluna e itero a linha
         arg->matriz_saida[linha*(arg->dim)+coluna] += arg->matriz_1[linha*(arg->dim)+interno] * arg->matriz_2[interno*(arg->dim)+coluna];
        }
      }
   }
   pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  int dim, nthreads, i, j;
  pthread_t *tid; 
  Argumento *args;
  double inicio, fim, delta;
  float *matriz_1, *matriz_2, *matriz_saida;

  GET_TIME(inicio);

  if(argc!=3) {
    printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
    return 1;
  }
  dim = atoi(argv[1]);
  if(dim < 1) {
    dim = 4;
  }
  nthreads = atoi(argv[2]);
  if (nthreads > dim) {
    nthreads = dim;
  }
  matriz_1 = (float *) malloc(sizeof(float)*dim*dim);
  matriz_2 = (float *) malloc(sizeof(float)*dim*dim);
  matriz_saida = (float *) malloc(sizeof(float)*dim*dim);
  tid = (pthread_t *) malloc(sizeof(pthread_t)*nthreads);
  args = (Argumento *) malloc(sizeof(Argumento)*nthreads);
  if (matriz_1 == NULL || matriz_2 == NULL || matriz_saida == NULL || args == NULL || args == NULL) {
    printf("ERRO--malloc\n");
    return 2;
  }

  for(i=0; i<dim; i++) {
    for(j=0; j<dim; j++) {
      matriz_1[i*dim+j] = 1; 
      matriz_2[i*dim+j] = 2;
      matriz_saida[i*dim+j] = 23;
    }
  }
  GET_TIME(fim);
  delta = fim - inicio;
  printf("Tempo inicializacao:%lf\n", delta);

  GET_TIME(inicio);

  for(int i=0; i<nthreads; i++) {
    (args+i)->id = i;
    (args+i)->dim = dim;
    (args+i)->nthreads = nthreads;
    (args+i)->matriz_1 = matriz_1;
    (args+i)->matriz_2 = matriz_2;
    (args+i)->matriz_saida = matriz_saida;
    if(pthread_create((tid+i), NULL, multiplica_matrizes, (void*) (args+i))) {
       puts("ERRO--pthread_create");
       return 3;
    }
  } 

  for(int i=0; i<nthreads; i++) {
    pthread_join(*(tid+i), NULL);
  }
  GET_TIME(fim)   
  delta = fim - inicio;
  printf("Tempo multiplicacao:%lf\n", delta);

  GET_TIME(inicio);
  free(matriz_1);
  free(matriz_2);
  free(matriz_saida);
  free(args);
  free(tid);
  GET_TIME(fim);   
  delta = fim - inicio;
  printf("Tempo finalizacao:%lf\n", delta);

  return 0;
}
