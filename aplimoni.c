#include <stdio.h>
#include <stdlib.h>
#include "escritaprior.h"
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t bastaoPrint;

typedef struct {
  float temperatura; // varia entre [25, 40]
  int id_sensor;
  int id_leitura; // inicia com 0
} Medida;

typedef struct {
  Controle *ctrl;
  Medida *buffer; // fixo com valor 60
  int *onde_ler;
  int *onde_escrever;
  int id;
} Argumento;

void *sensores(void* container) {
  Argumento *arg = (Argumento *) container;
  int id_leitura = 0;
  while(1) {
    EntraEscrita(arg->ctrl);
    id_leitura++;
    arg->buffer[*(arg->onde_escrever)].temperatura = (float) 25.0 + (float)rand()/(float)(RAND_MAX/15);
    //if (arg->buffer[*(arg->onde_escrever)].temperatura > 35.0) {
      //printf("\t%d, %f\n",arg->id, arg->buffer[*(arg->onde_escrever)].temperatura);
    //}
    arg->buffer[*(arg->onde_escrever)].id_sensor = arg->id;
    arg->buffer[*(arg->onde_escrever)].id_leitura = id_leitura;
    *(arg->onde_escrever) = (*(arg->onde_escrever)+1)%60;
    SaiEscrita(arg->ctrl);
    sleep(1);
  }
  pthread_exit(NULL);
}
void *atuadores(void* container) {
  Argumento *arg = (Argumento *) container;
  int i=0, id, leitura, sequencia_maior_que_35 = 0;
  char ultimas_15[15];
  int p_15 = 0;
  int conta_ultimas_15;
  float temperatura;
  while(1) {
    /*Regiao de leitura*/
    EntraLeitura(arg->ctrl);
    id = arg->buffer[*(arg->onde_ler)].id_sensor;
    leitura = arg->buffer[*(arg->onde_ler)].id_leitura;
    temperatura = arg->buffer[*(arg->onde_ler)].temperatura;
    *(arg->onde_ler) = (*(arg->onde_ler)+1) % 60;
    SaiLeitura(arg->ctrl);
    /*Fim da regiao de leitura*/
    if (arg->id == id) {
      // lógica de análise para disparar o alerta vermelho
      if (temperatura > 35.0) {
        sequencia_maior_que_35++;
        ultimas_15[p_15] = 's';
        p_15 = (p_15+1)%15;
      } else {
        sequencia_maior_que_35 = 0;
        ultimas_15[p_15] = 'n';
        p_15 = (p_15+1)%15;
      }
      // disparo do alerta vermelho
      if(sequencia_maior_que_35 >= 5) {
        pthread_mutex_lock(&bastaoPrint);
        printf("\033[0;31m"); 
        printf("Atuador %d: Alerta vermelho!\n", arg->id);
        printf("\033[0m"); 
        pthread_mutex_unlock(&bastaoPrint);
      }
      // lógica de análise para disparar o alerta amarelo
      if(sequencia_maior_que_35 < 5 && leitura >= 15) {
        conta_ultimas_15 = 0;
        for(i = 0 ; i < 15 ; i++) {
          if(ultimas_15[i] == 's') {
            conta_ultimas_15++;
          }
          // disparo do alerta amarelo
          if(conta_ultimas_15 >= 5) {
            pthread_mutex_lock(&bastaoPrint);
            printf("\033[0;33m"); 
            printf("Atuador %d: Alerta amarelo!\n", arg->id);
            printf("\033[0m"); 
            pthread_mutex_unlock(&bastaoPrint);
            break;
          }
        }
      // disparo do alerta verde (condição normal)
      } else {
        pthread_mutex_lock(&bastaoPrint);
        printf("\033[0;32m"); 
        printf("Atuador %d: Normal.\n", arg->id);
        printf("\033[0m");
        pthread_mutex_unlock(&bastaoPrint);
      }
    }
    sleep(2);
  }
  pthread_exit(NULL);
}

int main(int argc, char** argv) {
    int qtd_threads, i, *onde_ler, *onde_escrever;
    Controle *ctrl;
    Medida *buffer;
    pthread_t *tid;
    Argumento *arg;
    
    if(argc > 1) {
      qtd_threads = atoi(argv[1]);
      if(qtd_threads < 1) {
        fprintf(stderr,"\nDigite: %s <numero de threads>\n \n", argv[0]);
        printf("O numero de threads deve ser maior ou igual a 1\n \n");
        exit(-1);
      }
    } else {
      fprintf(stderr,"\nDigite: %s <numero de threads>\n \n", argv[0]);
      exit(-1);
    }
    tid = (pthread_t *) malloc(sizeof(pthread_t)*qtd_threads*2);
    arg = (Argumento *) malloc(sizeof(Argumento)*qtd_threads*2);
    if (arg == NULL || tid == NULL) {
      printf("Erro: malloc - Controle");
      exit(-1);
    }
    onde_ler = (int *) malloc(sizeof(int));
    onde_escrever = (int *) malloc(sizeof(int));
    *(onde_ler) = 0;
    *(onde_escrever) = 0;
    buffer = (Medida *) malloc(60*sizeof(Medida)); // fixo com valor 60
    pthread_mutex_init(&bastaoPrint, NULL);
    InicializaCtrlLeituraEscrita(&ctrl);
    for(i = 0 ; i < (qtd_threads*2); i++) {
      arg[i].ctrl = ctrl;
      arg[i].onde_ler = onde_ler;
      arg[i].onde_escrever = onde_escrever;
      arg[i].buffer = buffer;
      arg[i].id = i/2;
      if((i%2) == 0) {
        if (pthread_create(&tid[i], NULL, sensores, (void *) &arg[i])) {
          printf("Erro: pthread_create\n");
          exit(-1);
        }
      } else {
        if (pthread_create(&tid[i], NULL, atuadores, (void *) &arg[i])) {
          printf("Erro: pthread_create\n");
          exit(-1);
        }
      }
    }
    for (i = 0 ;i < (qtd_threads*2); i++ ) {
      if(pthread_join(tid[i], NULL)) {
        printf("Erro: pthread_join");
        exit(-1);
      }
    }
    TerminaCtrlLeituraEscrita(&ctrl);
    pthread_mutex_destroy(&bastaoPrint);
    free(tid);
    free(arg);
    free(onde_ler);
    free(onde_escrever);
    free(buffer);
    return 0;
}