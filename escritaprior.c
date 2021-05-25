#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "escritaprior.h"

// Funcao para inicializar a estrutura de controle , os mecanismos de sincronização de threads e variaveis auxiliares para as devidas condicoes
void InicializaCtrlLeituraEscrita(Controle** ctrl) {
    (*ctrl) = (Controle *) malloc(sizeof(Controle));
    if(ctrl == NULL) {
        printf("Erro: malloc - Controle");
        exit(-1);
    }
    (*ctrl)->qtd_leitoras = 0;
    (*ctrl)->qtd_escritoras = 0;
    (*ctrl)->querem_escrever = 0;
    //inicializa as variaveis de sincronizacao
    pthread_mutex_init(&((*ctrl)->totem), NULL);
    pthread_cond_init(&((*ctrl)->cond_leit), NULL);
    pthread_cond_init(&((*ctrl)->cond_escr), NULL);
}
// Funcao para desalocar a estrutura de controle e liberar os mecanismos de sincronizacao de threads
void TerminaCtrlLeituraEscrita(Controle** ctrl) {
    pthread_mutex_destroy(&((*ctrl)->totem));
    pthread_cond_destroy(&((*ctrl)->cond_leit));
    pthread_cond_destroy(&((*ctrl)->cond_escr));
    free(*(ctrl));
}
//Funcoes para leitura, no caso eh o atuador, recebem ponteiro para a struct controle 
void EntraLeitura(Controle* ctrl) {
    pthread_mutex_lock(&(ctrl->totem));           // totem sera utilizado como essa variavel
    // bloqueia também caso tenha threads querendo escrever, assim implementando prioridade para escrita
    while(ctrl->qtd_escritoras > 0 || ctrl->querem_escrever > 0) {
        pthread_cond_wait(&(ctrl->cond_leit), &(ctrl->totem));
    }
    ctrl->qtd_leitoras++;
    pthread_mutex_unlock(&(ctrl->totem));
}
void SaiLeitura(Controle* ctrl) {
    pthread_mutex_lock(&(ctrl->totem));
    ctrl->qtd_leitoras--;
    if(ctrl->qtd_leitoras == 0) pthread_cond_signal(&(ctrl->cond_escr));
    pthread_mutex_unlock(&(ctrl->totem));
}
//Funcoes para escrita, no caso eh o sensor, recebem ponteiro para a struct controle 
void EntraEscrita(Controle* ctrl) {
    pthread_mutex_lock(&(ctrl->totem));
    // antes de entrar no while sinaliza aumentando a quantidade de threads querendo escrever
    ctrl->querem_escrever++;
    while((ctrl->qtd_leitoras > 0) || (ctrl->qtd_escritoras > 0)) {
        pthread_cond_wait(&(ctrl->cond_escr), &(ctrl->totem));
    }
    ctrl->qtd_escritoras++;
    // ao sair do while diminui a quantidade de threads querendo escrever, uma vez que foi liberada para escrita
    ctrl->querem_escrever--;
    pthread_mutex_unlock(&(ctrl->totem));
}
void SaiEscrita(Controle* ctrl) {
    pthread_mutex_lock(&(ctrl->totem));
    ctrl->qtd_escritoras--;
    // se não possui nenhuma thread querendo escrever no momento da saída da escrita, libera para as threads que querem ler, ou seja, forçando que uma prioridade para escrita a partir da liberação para leitura apenas se não tiver threads querendo escrever
    if(ctrl->querem_escrever == 0) {
      pthread_cond_broadcast(&(ctrl->cond_leit));
    } else {
    // libera para escrita enquanto tiver threads querendo escrever, assim sendo uma prioridade para escrita
      pthread_cond_signal(&(ctrl->cond_escr));
    }
    pthread_mutex_unlock(&(ctrl->totem));
}

