/**
 * Maria Eduarda Machado de Holanda - 190043725 
**/

/**
 * Exercicio:
 * 
 * O problema do produtor e consumidor (também conhecido como o problema do buffer limitado), 
 * consiste em um conjunto de processos que compartilham um mesmo buffer. Os processos chamados 
 * produtores põem informação no buffer. Os processos chamados consumidores retiram informação 
 * deste buffer. Quando o buffer estiver cheio, os produtores devem aguardar por posições 
 * disponíveis para inserir os dados produzidos. Da mesma forma, quando o buffer estiver vazio
 * os consumidores devem aguardar por dados para serem consumidos.
**/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 10 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t prod_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cons_cond = PTHREAD_COND_INITIALIZER;

int cont = 0; // contador de itens do buffer

void * produtor(void * meuid);
void * consumidor (void * meuid);

void main(argc, argv)
int argc;
char *argv[];{

  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];
   
  for (i = 0; i < PR; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
 
  pthread_join(tid[0],NULL);

}

void * produtor (void* pi){
 
  while(1){

    // gera o proximo item
    printf("Produtor esta gerando o proximo item!\n");

    // lock para o produtor e consumidor nao acessarem a memoria ao mesmo tempo
    pthread_mutex_lock(&lock);
      sleep(2);
      // se o buffer ta cheio -> dormir
      while(cont == N){
        printf("Produtor dormindo - buffer cheio\n");
        pthread_cond_wait(&prod_cond, &lock);
      }

      // increma o contador de itens do buffer
      cont++;
      // insere item no buffer
      printf("Produtor esta inserindo item no buffer, tamanho = %d\n", cont);

      // se cont == 1 (buffer estava vazio) -> acorda
      if(cont == 1){
        pthread_cond_signal(&cons_cond);
      }

      sleep(2);
    pthread_mutex_unlock(&lock);
  }
  pthread_exit(0);
  
}

void * consumidor (void* pi){

  while(1){

    pthread_mutex_lock(&lock);
      sleep(2);

      // se buffer ta vazio -> dormir
      while(cont == 0){
        printf("Consumidor dormindo - buffer vazio\n");
        pthread_cond_wait(&cons_cond, &lock);
      }

      // decrementa o contador
      cont--;
      // remove item
      printf("Consumidor esta removendo item no buffer, tamanho = %d\n", cont);

      // se cont==n-1 (buffer estava cheio) -> acorda produtor
      if(cont == N-1){
        pthread_cond_signal(&prod_cond);
      }
    
      sleep(2);
    pthread_mutex_unlock(&lock);
  }
  pthread_exit(0); 

}
