/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 6 */
/* Código: Implementar um programa concorrente em Java para incrementar de 1 todos os elementos de um vetor. Considere que o número de threads é menor ou igual ao número de elementos do vetor. Divida a tarefa entre as threads de forma balanceada. */
/* -------------------------------------------------------------------*/


//classe que estende Thread e implementa a tarefa de cada thread do programa 
class T extends Thread {
  // variáveis necessárias
  private int id;
  private int qtd_threads;
  private int[] vetor;

  // inicialização das variáveis da thread
  public T(int tid, int qtdt, int[] vet) { 
    this.id = tid;
    this.qtd_threads = qtdt;
    this.vetor = vet;
  }

  //metodo main da thread
  public void run() {
    int inicio, fim, i;
    // começo da fatia que a thread soma
    inicio = this.id*(this.vetor.length/this.qtd_threads);
    // fim da fatia que a thread soma
    fim = (this.id+1)*(this.vetor.length/this.qtd_threads);
    // incremento da fatia do vetor em 1 unidade
    for (i = inicio; i < fim; i++) {
      this.vetor[i] = this.vetor[i] + 1;
    }
  }
}

//classe da aplicacao
class TIncrementoVetor {
  public static void como_usar() {
    System.out.println("Incrementa Vetor");
    System.out.println("\tComo usar:");
    System.out.println("\t\tjava TIncrementoVetor <tamanho do vetor> <quantidade de threads>");
  }

  public static void main (String[] args) {
    // verifica a quantidade de argumentos
    if(args.length < 2) {
      como_usar();
      System.exit(1);
    }
    // inicializa as variáveis com os argumentos passados
    int qtd_threads, tam_vetor, i;
    tam_vetor = Integer.parseInt(args[0]);
    qtd_threads = Integer.parseInt(args[1]);
    // caso passem uma quantidade de threads superior ao tamanho do vetor
    if(tam_vetor < qtd_threads) {
      qtd_threads = tam_vetor;
    }
    // depois de ter os valores obtidos pelos argumentos cria o vetor de inteiros e de threads
    int[] vetor = new int[tam_vetor];
    Thread[] threads = new Thread[qtd_threads];
    
    // inicializa o vetor com zero
    for(i = 0; i < vetor.length ; i++) {
      vetor[i] = 0;
    }
    //cria as threads da aplicacao
    for (i = 0; i < threads.length; i++) {
      threads[i] = new T(i, qtd_threads, vetor);
    }
    //inicia as threads
    for (i = 0; i < threads.length; i++) {
      threads[i].start();
    }
    // espera pelo termino de todas as threads
    for (i = 0; i < threads.length; i++) {
      try { threads[i].join(); }
      catch (InterruptedException e) { return; }
    }

    // verifica se a soma deu certo
    for(i = 0; i < vetor.length ; i++) {
      if(vetor[i] != 1) {
        System.out.println("Erro ao somar todos os valores, posição "+ String.valueOf(i) + " valor " + String.valueOf(vetor[i]));
        System.exit(2);
      }
    }
    System.out.println("Soma realizada com sucesso!");
  }
}
