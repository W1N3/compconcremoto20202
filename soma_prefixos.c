/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Implementação do problema de soma de prefixos utilizando barreira, ou, sincronização coletiva */
/* Condição lógica: dado a entrada um vetor de inteiros de tamanho 2^n, realizar o somatório de todos os elementos anteriores a posição incluindo a própria posição, e guardar na posição tal valor. Isto é:
a{i} = somatório de (a{k}) de 0 até i, sendo a{i} um elemento de um vetor de tamanho t = 2^n, e i variando de 0 a t
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  5
#define PASSOS  5

//funcao barreira
void barreira(int nthreads) {
    pthread_mutex_lock(&x_mutex); //inicio secao critica
    if (bloqueadas == (nthreads-1)) { 
      //ultima thread a chegar na barreira
      pthread_cond_broadcast(&x_cond);
      bloqueadas=0;
    } else {
      bloqueadas++;
      pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); //fim secao critica
}

//funcao das threads
void *soma_prefixo (void *arg) {
  int id = *(int*)arg;
  int boba1, boba2;

  for (int passo=0; passo < PASSOS; passo++) {
    printf("Thread %d: passo=%d\n", id, passo);

    /* faz alguma coisa... */
    boba1=100; boba2=-100; while (boba2 < boba1) boba2++;

    //sincronizacao condicional
    barreira(NTHREADS);
  }
  pthread_exit(NULL);
}

void como_usar() {
  printf("\n");
}

typedef struct {
  int id;
  int *prefixos;
  int *threads_bloqueadas;
  pthread_mutex_t *bastao_leitura;
  pthread_cond_t *condicao_leitura;
  pthread_mutex_t *bastao_escrita;
  pthread_cond_t *condicao_escrita;
} Argumento;

/* Funcao principal */
int main(int argc, char *argv[]) {
  int *id;
  int *prefixos;
  Argumento *args;
  pthread_t *threads;
  int qtd_threads;
  int i;
  /* Variaveis globais */
  int threads_bloqueadas = 0;
  pthread_mutex_t bastao_leitura;
  pthread_cond_t condicao_leitura;
  pthread_mutex_t bastao_escrita;
  pthread_cond_t condicao_escrita;

  if(argc > 1) {
    qtd_threads = (int) pow(2, atoi(argv[1]));
    id = (int *) malloc(qtd_threads*sizeof(int));
    args = (Argumento *) malloc(qtd_threads*sizeof(Argumento));
    threads = (pthread_t *) malloc(qtd_threads*sizeof(pthread_t));
    prefixos = (int *) malloc(qtd_threads*sizeof(int));
    if(prefixos == NULL || threads = NULL || id == NULL || args == NULL) {
      printf("\nErro ao alocar memória!\n");
      return 1;
    }
    for(i = 0; i < qtd_threads; i++) {
      args[i]->id = id;
      args[i]->prefixos = prefixos;
      args[i]->threads_bloqueadas = &threads_bloqueadas;
      args[i]->bastao_leitura = &bastao_leitura;
      args[i]->condicao_leitura = &condicao_leitura;
      args[i]->bastao_escrita = &bastao_escrita;
      args[i]->condicao_escrita = &condicao_escrita;
      if((i+2) < argc) {
        prefixos[i] = atoi(argv[i+2]);
      } else {
        prefixos[i] = 1;
      }
    }
  } else {
    como_usar();
    return 2;
  }
  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&bastao_leitura, NULL);
  pthread_cond_init (&condicao_leitura, NULL);
  pthread_mutex_init(&bastao_escrita, NULL);
  pthread_cond_init (&condicao_escrita, NULL);

  /* Cria as threads */
  for(int i=0;i<NTHREADS;i++) {
     id[i]=i;
     pthread_create(&threads[i], NULL, soma_prefixo, (void *) &args[i]);
  }

  /* Espera todas as threads completarem */
  for (int i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  printf ("FIM.\n");

  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&bastao_leitura);
  pthread_cond_destroy(&condicao_leitura);
  pthread_mutex_destroy(&bastao_escrita);
  pthread_cond_destroy(&condicao_escrita);
  return 0;
}
