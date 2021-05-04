/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Implementação do problema de soma de prefixos utilizando barreira, ou, sincronização coletiva */
/* Condição lógica: dado a entrada um vetor de inteiros de tamanho 2^n, realizar o somatório de todos os elementos anteriores a posição incluindo a própria posição, e guardar na posição tal valor. Isto é:
a{i} = somatório de (a{k}) de 0 até i, sendo a{i} um elemento de um vetor de tamanho t = 2^n, e i variando de 0 a t
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
  int id;
  int qtd_passos;
  int *prefixos;
} Argumento;

/* Variaveis globais */
int bloqueadas = 0;
int qtd_threads;

pthread_mutex_t barr_leit_mutex;
pthread_cond_t barr_leit_cond;
pthread_mutex_t barr_escr_mutex;
pthread_cond_t barr_escr_cond;

//funcao barreira
void barreira_leitura() {
  pthread_mutex_lock(&barr_leit_mutex); //inicio secao critica
  if (bloqueadas == (qtd_threads-1)) {
    //ultima thread a chegar na barreira
    pthread_cond_broadcast(&barr_leit_cond);
    bloqueadas=0;
  } else {
    bloqueadas++;
    pthread_cond_wait(&barr_leit_cond, &barr_leit_mutex);
  }
  pthread_mutex_unlock(&barr_leit_mutex); //fim secao critica
}

void barreira_escrita() {
  pthread_mutex_lock(&barr_escr_mutex); //inicio secao critica
  if (bloqueadas == (qtd_threads-1)) {
    //ultima thread a chegar na barreira
    pthread_cond_broadcast(&barr_escr_cond);
    bloqueadas=0;
  } else {
    bloqueadas++;
    pthread_cond_wait(&barr_escr_cond, &barr_escr_mutex);
  }
  pthread_mutex_unlock(&barr_escr_mutex); //fim secao critica
}

//funcao das threads
void *soma_prefixo (void *container) {
  Argumento *arg = (Argumento *) container;
  int offset, aux, i;
  for(i = 0; i < arg->qtd_passos; i++) {
    offset = (int) pow(2, i);
    if(arg->id - offset >= 0) {
      aux = arg->prefixos[arg->id - offset];
    }
    barreira_leitura();
    if(arg->id - offset >= 0) {
      arg->prefixos[arg->id] += aux;
    }
    barreira_escrita();
  }
  pthread_exit(NULL);
}

void print_vetor(int* vetor, int tam) {
  int i;
  printf("[");
  for(i = 0 ; i < tam ; i++) {
    printf("%d", vetor[i]);
    if(i < tam - 1) {
      printf(", ");
    }
  }
  printf("]");
}

void como_usar() {
  printf("\n Soma de prefixos");
  printf("\n\t sum_pre <potencia do tamanho do vetor> [<vetor>]");
  printf("\n\t <potencia do tamanho do vetor>:");
  printf("\n\t\t Valor que será o indice i em 2^i para definir o tamanho do vetor.");
  printf("\n\t");
  printf("\n\t[<vetor>]:");
  printf("\n\t\t Vetor de inteiros a serem somados pelo algoritmo de soma de prefixos. As posições não definidas na sequência serão inicializadas com valor 1. Valores extra serão desconsiderados.");
  printf("\n\tExemplo:");
  printf("\n\t\tsum_pre 3 1 2 3 4 5 6");
  printf("\n\t\t[1 2 3 4 5 6 1 1]");
  printf("\n\t\tsum_pre 4 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
  printf("\n\t\t[1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16]");
  printf("\n");
}

/* Funcao principal */
int main(int argc, char *argv[]) {
  int i;
  int qtd_passos;
  int *prefixos;
  Argumento *args;
  pthread_t *threads;

  if(argc > 1) {
    qtd_passos = atoi(argv[1]);
    qtd_threads = (int) pow(2, qtd_passos);
    args = (Argumento *) malloc(qtd_threads*sizeof(Argumento));
    threads = (pthread_t *) malloc(qtd_threads*sizeof(pthread_t));
    prefixos = (int *) malloc(qtd_threads*sizeof(int));
    if(prefixos == NULL || threads == NULL || args == NULL) {
      printf("\nErro ao alocar memória!\n");
      return 1;
    }
    for(i = 0; i < qtd_threads; i++) {
      args[i].id = i;
      args[i].prefixos = prefixos;
      args[i].qtd_passos = qtd_passos;
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
  pthread_mutex_init(&barr_leit_mutex, NULL);
  pthread_cond_init (&barr_leit_cond, NULL);
  pthread_mutex_init(&barr_escr_mutex, NULL);
  pthread_cond_init (&barr_escr_cond, NULL);
  printf("\nVetor antes = ");
  print_vetor(prefixos, qtd_threads);
  printf("\n");
  /* Cria as threads */
  for(i = 0; i < qtd_threads; i++) {
     pthread_create(&threads[i], NULL, soma_prefixo, (void *) &args[i]);
  }

  /* Espera todas as threads completarem */
  for (i = 0; i < qtd_threads; i++) {
    pthread_join(threads[i], NULL);
  }
  printf("\nVetor depois = ");
  print_vetor(prefixos, qtd_threads);
  printf("\n");

  printf ("FIM.\n");

  /* Desaloca variaveis e termina */
  free(args);
  free(threads);
  free(prefixos);
  pthread_mutex_destroy(&barr_leit_mutex);
  pthread_cond_destroy(&barr_leit_cond);
  pthread_mutex_destroy(&barr_escr_mutex);
  pthread_cond_destroy(&barr_escr_cond);
  return 0;
}
