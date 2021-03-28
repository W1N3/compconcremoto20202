/*

Disciplina: Computacao Concorrente
Prof.: Silvana Rossetto
Módulo 1 - Laboratório: 1
Codigo: Atividade 5 
Objetivo: Implementar um programa concorrente, com duas threads (além da thread principal) para incrementar de 1 cada elemento de um vetor de 10000 elementos.

*/

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2
#define TAM_VETOR 10000

// Cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
  int id;
  int *inicio_vetor;
  int *fim_vetor;
} Argumento;

// Função para incrementar um pedaço de vetor dado
void *incrementaVetor (void *container) {
  Argumento *arg = (Argumento *) container;
  while(arg->inicio_vetor != arg->fim_vetor) {
    // Somo o valor que está guardado na posição
    arg->inicio_vetor[0]++;
    // Adianto o apontador de memória
    arg->inicio_vetor++;
  }
  pthread_exit(NULL);
}

int main() {
  int t, tam_fatia_thread;
  int *vetor;
  Argumento *args;
  pthread_t *tid_sistema;
  char somou_certo = 's';

  printf("Computação Concorrente - Laboratório 1\n");

  // Aloca dinamicamente a memória para o vetor, os argumentos e os identificadores das threads, respectivamente
  vetor = (int *) malloc(sizeof(int)*TAM_VETOR);
  args = (Argumento *) malloc(sizeof(Argumento)*NTHREADS);
  tid_sistema = (pthread_t *) malloc(sizeof(pthread_t)*NTHREADS);
  if(vetor == NULL || args == NULL || tid_sistema == NULL) {
    printf("\t!#ERRO: malloc()\n");
    exit(-1);
  }

  printf("\tAntes da inicialização: vetor[t] == %d.\n", vetor[0]);
  // Inicializa o vetor com um número
  for(t = 0 ; t < TAM_VETOR ; t++) {
    vetor[t] = 23;
  }
  printf("\tAntes: vetor[t] == %d.\n", vetor[0]);

  // Estabeleço o tamanho da fatia do vetor a ser passada para a thread
  tam_fatia_thread = TAM_VETOR / NTHREADS;

  for(t = 0; t < NTHREADS; t++) {
    // Inicializa os argumentos a serem passados para as threads
    args[t].id = t+1;
    args[t].inicio_vetor = (vetor+t*(tam_fatia_thread));
    // Caso já seja a última fatia, considerar logo o fim do vetor
    if(t == (NTHREADS-1)) {
      args[t].fim_vetor = (vetor+TAM_VETOR);
    }
    else {
      args[t].fim_vetor = (vetor+(t+1)*(tam_fatia_thread));
    }
    
    // Dispara as threads para execução
    if (pthread_create(&tid_sistema[t], NULL, incrementaVetor, (void*) (args+t))) {
      printf("\t!#ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  // Espera as threads terminarem
  for (t = 0; t < NTHREADS; t++) {
    if (pthread_join(tid_sistema[t], NULL)) {
      printf("\t!#ERRO: pthread_join()\n");
      exit(-1); 
    } 
  }

  // Verifica se o vetor foi alterado corretamente
  for(t = 0; t< TAM_VETOR; t++) {
    if(vetor[t] != 24) {
      printf("\t!# ERRO: vetor[%d] == %d!\n", t, vetor[t]);
      somou_certo = 'n';
    }
  }

  printf("\tDepois: vetor[t] == %d!\n", vetor[0]);
  if(somou_certo == 's') {
    printf("As threads somaram o vetor com sucesso!\n");
  }

  // Libera a memória das variavéis dinamicas alocadas
  free(vetor);
  free(args);
  free(tid_sistema);
  pthread_exit(NULL);
}
