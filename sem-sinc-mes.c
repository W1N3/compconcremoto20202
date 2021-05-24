/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Projetar e implementar um programa concorrente onde a ordem de execução das threads é controlada no programa */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 4

// Variaveis globais

//semaforos para sincronizar a ordem de execucao das threads
sem_t condt23, condt_final1, condt_final2;

//funcao executada pela thread 1
void *t1 (void *container) {
  sem_wait(&condt_final1);
  sem_wait(&condt_final2);
  printf("Volte sempre!\n");
  pthread_exit(NULL);
}

//funcao executada pela thread 2
void *t2 (void *container) {
  sem_wait(&condt23);
  printf("Fique a vontade.\n");
  sem_post(&condt_final1);
  pthread_exit(NULL);
}

//funcao executada pela thread 3
void *t3 (void *container) {
  sem_wait(&condt23);
  printf("Sente-se por favor.\n");
  sem_post(&condt_final2);
  pthread_exit(NULL);
}
//funcao executada pela thread 4
void *t4 (void *container) {
  printf("Seja Bem-vindo!\n");
  sem_post(&condt23);
  sem_post(&condt23);
  pthread_exit(NULL);
}

//funcao principal
int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];
  int t;

  //inicia os semaforos
  sem_init(&condt23, 0, 0);
  sem_init(&condt_final1, 0, 0);
  sem_init(&condt_final2, 0, 0);

  //cria as tres threads
  if (pthread_create(&tid[0], NULL, t1, NULL)) { 
    printf("--ERRO: pthread_create()\n");
    exit(-1); 
  }
  if (pthread_create(&tid[1], NULL, t2, NULL)) { 
    printf("--ERRO: pthread_create()\n");
    exit(-1); 
  }
  if (pthread_create(&tid[2], NULL, t3, NULL)) { 
    printf("--ERRO: pthread_create()\n");
    exit(-1); 
  }
  if (pthread_create(&tid[3], NULL, t4, NULL)) {
    printf("--ERRO: pthread_create()\n");
    exit(-1);
  }

  //--espera todas as threads terminarem
  for (t=0; t<NTHREADS; t++) {
    if (pthread_join(tid[t], NULL)) {
      printf("--ERRO: pthread_join() \n");
      exit(-1);
    }
  }
  sem_destroy(&condt23);
  sem_destroy(&condt_final1);
  sem_destroy(&condt_final2);
  pthread_exit(NULL);
}

