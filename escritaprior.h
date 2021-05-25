#ifndef ESCRITAPRIOR_H
#define ESCRITAPRIOR_H
#include <pthread.h>

// paradigma leitura e escrita, com prioridade para escrita

typedef struct {
    pthread_mutex_t totem;
    pthread_cond_t cond_leit;
    pthread_cond_t cond_escr;
    int qtd_leitoras;
    int qtd_escritoras;
    // se igual a 1 entraLeitura bloqueia novas threads de entrarem
    // se igual a 0 entraLeitura permite entrarem para leitura normalmente
    int querem_escrever;
} Controle;

void InicializaCtrlLeituraEscrita(Controle** ctrl);
void TerminaCtrlLeituraEscrita(Controle** ctrl);
void EntraEscrita(Controle* ctrl);
void SaiEscrita(Controle* ctrl);
void EntraLeitura(Controle* ctrl);
void SaiLeitura(Controle* ctrl);

#endif
