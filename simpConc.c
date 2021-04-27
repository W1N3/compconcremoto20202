#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"


typedef struct {
    int id;                         // identificador da thread
    char metodo; // escolha entre metodo do trapézio ou de simpson
    int tipo_funcao;                // identificador do tipo de funcao a ser utilizada
    int qtd_termos;                 // numero de termos da funcao
    double* termos_funcao;          // os termos da função
    int qtd_threads;              //numero de threads
    long int qtd_iteracoes;               // numero de divisoes da regiao de integracao
    long double limInf;
    long double limSup;    // limites inferior e superior da regiao de integracao
} Argumento;


/*  Aqui o grau serve para formatar a funcao de acordo com o modelo [a0]*x^0 + [a1]*x^1 + ... + [an]*x^n 
    Os fatores no vetor em ordem sao a0,a1,..,an de acordo com o modelo citado.
    X diz respeito ao valor da variavel que sera usada no calculo da funcao. 
*/
double funcPoli(int grau, double* fator, double x) {
    double func = 0;

    for(int i=0;i<grau;i++){
        func += fator[i]* pow(x,i); 
    }
    return func;
}
/*  Aqui o numero de termos serve para confirmar que o modelo A*( e^(B*x) ) + C esta sendo respeitado.
    Os fatores no vetor em ordem sao A,B e C de acordo com o modelo citado.
    X diz respeito ao valor da variavel que sera usada no calculo da funcao.
*/
double funcExp(int termos, double* fator, double x) {
    if(termos == 3) {
        return fator[0]*exp(fator[1]*x) + fator[2];
    } else {
        return NAN;
    }
}
/*  Aqui o numero de termos serve para confirmar que o modelo A*( log(B*x) ) + C esta sendo respeitado.
    Os fatores no vetor em ordem sao A,B e C de acordo com o modelo citado.
    X diz respeito ao valor da variavel que sera usada no calculo da funcao.
*/
double funcLog(int termos, double* fator, double x) {
    if(termos == 3) {
        return fator[0]*log(fator[1]*x) + fator[2];
    } else {
        return NAN;
    }
}
/*  Aqui o numero de termos serve para confirmar que o modelo A*( Sen(B*x) ) + C esta sendo respeitado.
    Os fatores no vetor em ordem sao A,B e C de acordo com o modelo citado.
    X diz respeito ao valor da variavel que sera usada no calculo da funcao.
*/
double funcSen(int termos, double* fator, double x) {
    // a*sin(b*x);
    if(termos == 3) {
        return fator[0]*sin(fator[1]*x) + fator[2];
    } else {
        return NAN;
    }
}
/*  Aqui o numero de termos serve para confirmar que o modelo A*( Cos(B*x) ) + C esta sendo respeitado.
    Os fatores no vetor em ordem sao A,B e C de acordo com o modelo citado.
    X diz respeito ao valor da variavel que sera usada no calculo da funcao.
*/
double funcCos(int termos, double* fator, double x) {
    if(termos == 3) {
        return fator[0]*cos(fator[1]*x) + fator[2];
    } else {
        return NAN;
    }
}

void* tarefa(void* container){
    Argumento* args = (Argumento*) container;
    long int id=(long int) args->id;        // numero da thread
    long double* somaLocal;                 // soma local feita por cada thread e eh o retorno
    long int tamBloco = (args->qtd_iteracoes - 2)/args->qtd_threads;  // separacao dos termos para blocos das threads, menos o primeiro e ultimo termos
    long double altura = (args->limSup - args->limInf)/((long double) args->qtd_iteracoes); // h=(a-b)/n 
    long int ini = (id * tamBloco)+1;       // definicao do inicio para cada thread
    long int fim ;                          // definicao do fim para cada thread
    double x;
    int i;
    if(id == args->qtd_threads-1) fim = args->qtd_iteracoes-1;
    else fim = ((id+1) * tamBloco)+1;
    double (*func) (int, double*, double);  // declara um ponteiro para uma funcao generica
    somaLocal = (long double*) malloc(sizeof(long double));     // aloca espaco para a variavel e zera os espacos
    somaLocal[0] = 0;
    
    /*O ponteiro para funcao generica func passa a apontar para um tipo especifico que foi escolhido */
    switch(args->tipo_funcao) {
        case 1:
            func = funcPoli;
            break;
        case 2:
            func = funcExp;
            break;
        case 3:
            func = funcLog;
            break;
        case 4:
            func = funcSen;
            break;
        case 5:
            func = funcCos;
            break;
        default:
            printf("ERRO -> tipo da função passada não existente! falhou na validação inicial da main!");
            exit(2);
            break;
    }
    /*Realizamos parcialmente o calculo da integral pelo metodo do trapezio, visto que ainda faltam o primeiro e ultimo termos
    na soma da serie, alem da divisao por 2 do resultado obtido. Esses tratamentos serao feitos na funcao main. */
    for(i = ini;i < fim; i++) {
        x = ((double) args->limInf)+(((double) i)*((double) altura));

        if(args->metodo == 's') {
          if(i%2==0) {
              somaLocal[0]+=(long double) 2*func(args->qtd_termos, args->termos_funcao, x);
          }
          else {
              somaLocal[0]+=(long double) 4*func(args->qtd_termos, args->termos_funcao, x);
          }
        } else if(args->metodo == 't') {
            somaLocal[0]+=(long double) 2*func(args->qtd_termos, args->termos_funcao, x);
        }
    }
    pthread_exit((void*) somaLocal); // melhor ser NULL 
    // temos que dar cast para void* aqui, pegamos esse valor com o join

}

void como_usar() {
    printf("\n### Programa de análise de desempenho comparando os Métodos de Integração, Trapézios e 1/3 de Simpson ###");
    printf("\n\nComo usar: simp <tipo da função> <número iterações> <limite inferior> <limite superior> <número de threads> [<argumentos que definem a função>]");
    printf("\n<tipo da função>:");
    printf("\n\t 1 - Função Polinomial ([a0]*x^0 + [a1]*x^1 + ... + [an]*x^n , [i, f])");
    printf("\n\t 2 - Função Exponencial ([a0]*e^([a1] * x) + [a2], [i, f])");
    printf("\n\t 3 - Função Logarítmica ([a0] * ln([a1] * x) + [a2], [a1] > 0.0, [i >= 1.0 , f > = 1.0])");
    printf("\n\t 4 - Função Seno ([a0] * sin([a1] * x) + [a2] , [i, f])");
    printf("\n\t 5 - Função Cosseno ([a0] * cos([a1] * x) + [a2] , [i, f])");
    printf("\n");
    printf("\n<número iterações>:");
    printf("\n\t(int) - [> 3] Quantidade de iterações da integração numérica a ser efetuada.");
    printf("\n");
    printf("\n<limite inferior>:");
    printf("\n\t(float) [< limite superior] - Limite inferior da integração.");
    printf("\n");
    printf("\n<limite superior>:");
    printf("\n\t(float) [> limite inferior] - Limite superior da integração.");
    printf("\n");
    printf("\n<número de threads>:");
    printf("\n\t(int) [> 0] - Quantidade de threads a serem disparadas para divisão de tarefa e execução da integração numérica.");
    printf("\n");
    printf("\n[<argumentos que definem a função>]:");
    printf("\n\t(float) - Os valores a{n} que definem a função. Sendo o primeiro valor a{0}, o segundo a{1}, e assim sucessivamente.");
    printf("\n");
}

int main(int argc, char** argv) {
    int i;
    char* avulso;
    int tipo_funcao;                // identificador do tipo de funcao a ser utilizada
    int qtd_termos;                 // numero de termos da funcao
    double* termos_funcao;          // os termos da função
    int qtd_threads;           //numero de threads
    long int qtd_iteracoes;         // numero de divisoes da regiao de integracao
    long double limInf;
    long double limSup;
    long double* somaLocal;
    Argumento* args;
    long double somaConc=0; 
    double ini,fim,delta;          // variaveis de tempo
    pthread_t* tid;                // identificacao das threads
    long double* integralControle; // valor esperado da integral
    double (*func) (int, double*, double);  // declara um ponteiro para uma funcao generica
    
    // simp <tipo da função> <número iterações> <limite inferior> <limite superior> <número de threads> [<argumentos que definem a função>]

    GET_TIME(ini);
    if(argc < 7) {
        como_usar();
        exit(1);
    } else {
        tipo_funcao = atoi(argv[1]);
        if(tipo_funcao < 1 || tipo_funcao > 5) {
            como_usar();
            exit(1);
        }
        
        qtd_iteracoes = atol(argv[2]);
        if(qtd_iteracoes < 4) {
            como_usar();
            exit(1);
        }

        limInf = strtold(argv[3], &avulso);
        limSup = strtold(argv[4], &avulso);
        if(limInf >= limSup || limInf == NAN || limSup == NAN) {
            como_usar();
            exit(1);
        }
        qtd_threads = atoi(argv[5]);
        if(qtd_threads < 1) {
            como_usar();
            exit(1);
        }
        switch(tipo_funcao) {
            case 3: // verifica se os valores passados para a função logarítmica são coerentes
              if(limInf <= 0.0 || limSup <= 0.0 || strtod(argv[7], &avulso) <= 0.0) {
                como_usar();
                exit(1);
              }
            case 2:
            case 4:
            case 5:
                if(argc != 9) {
                    como_usar();
                    exit(1);
                }
            case 1:
                i = 6;
                termos_funcao = (double *) malloc((argc-6)*sizeof(double));
                if(termos_funcao == NULL ) {
                    printf("\nERRO : malloc");
                    exit(2);
                }
                while(i < argc) {
                    termos_funcao[i-6] = strtod(argv[i], &avulso);
                    i++;
                    if(termos_funcao[i-6] == NAN) {
                        como_usar();
                        exit(1);
                    }
                }
                qtd_termos = i - 6;
                break;
            default:
                printf("ERRO -> tipo da função passada não existente!");
                exit(2);
                break;
        }
    }

    switch(tipo_funcao) {
        case 1:
            func = funcPoli;
            break;
        case 2:
            func = funcExp;
            break;
        case 3:
            func = funcLog;
            break;
        case 4:
            func = funcSen;
            break;
        case 5:
            func = funcCos;
            break;
        default:
            printf("ERRO -> tipo da função passada não existente! falhou na validação inicial da main!");
            exit(2);
            break;
    }
    args = (Argumento *) malloc(sizeof(Argumento)*qtd_threads);
    tid = (pthread_t *) malloc(sizeof(pthread_t)*qtd_threads);
    if(args == NULL || tid == NULL) {
        printf("\nERRO : malloc");
        exit(2);
    }

    GET_TIME(fim);
    delta = fim - ini;
    printf("Inicialização: %lf\n",delta);

    /*  parte concorrente
        criação das threads
    */
    // simpson
    GET_TIME(ini);
    for(int i=0; i<qtd_threads; i++){
        (args+i)->id = i;
        (args+i)->metodo = 's';
        (args+i)->tipo_funcao = tipo_funcao;
        (args+i)->qtd_termos = qtd_termos;
        (args+i)->termos_funcao = termos_funcao;
        (args+i)->qtd_threads = qtd_threads;
        (args+i)->qtd_iteracoes = qtd_iteracoes;
        (args+i)->limInf = limInf;
        (args+i)->limSup = limSup;
        if( pthread_create(tid+i, NULL, tarefa, (void*) (args+i)) ) {
             puts("ERRO--pthread_create\n"); return 3;
        }
    }
    // espera pelo termino das threads 
    for(int i=0; i<qtd_threads; i++){
        pthread_join(*(tid+i), (void **) &somaLocal); 
        somaConc += *somaLocal;
        free(somaLocal);
    }
    somaConc += func(qtd_termos, termos_funcao, limInf);
    somaConc += func(qtd_termos, termos_funcao, limSup);
    somaConc = somaConc*((limSup - limInf)/qtd_iteracoes)/3;
    GET_TIME(fim);
    delta = fim - ini;
    printf("Resultado da integração com Simpson = %Lf\n", somaConc);
    printf("Tempo da execução com [%d] threads: %lf\n", qtd_threads, delta);

    // trapezio
    GET_TIME(ini);
    for(int i=0; i<qtd_threads; i++){
        (args+i)->metodo = 't';
        if( pthread_create(tid+i, NULL, tarefa, (void*) (args+i)) ) {
             puts("ERRO--pthread_create\n"); return 3;
        }
    }
    // espera pelo termino das threads 
    for(int i=0; i<qtd_threads; i++){
        pthread_join(*(tid+i), (void **) &somaLocal); 
        somaConc += *somaLocal;
        free(somaLocal);
    }
    somaConc += func(qtd_termos, termos_funcao, limInf);
    somaConc += func(qtd_termos, termos_funcao, limSup);
    somaConc = somaConc*((limSup - limInf)/qtd_iteracoes)/2;
    GET_TIME(fim);
    delta = fim - ini;
    printf("Resultado da integração com Trapézio = %Lf\n", somaConc);
    printf("Tempo da execução com [%d] threads: %lf\n", qtd_threads, delta); 

    GET_TIME(ini);
    

    free(tid);
    free(args);
    free(termos_funcao);

    GET_TIME(fim);
    delta = fim - ini;
    printf("Finalização: %lf\n",delta);
    return 0;
}
