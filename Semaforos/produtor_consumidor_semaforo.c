/**
 * Maria Eduarda Holanda - 190043725
**/

/**
 * Problema Produtor-Consumidor com Semáforos
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
#include <semaphore.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

// semaforos e lock utilizados
sem_t empty;    
sem_t full;     
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;    

int buffer = 0;

void * produtor (void * meuid);
void * consumidor (void * meuid);

void main(argc, argv)
int argc;
char *argv[];
{

    int erro;
    int i, n, m;
    int *id;

    // a gnt precisa primeiro inicializar os semaforos
    sem_init(&empty, 0, N);     
    sem_init(&full, 0, 0);

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

        // gera item pra por no buffer
        printf("produtor: criando um novo item.\n");

        sem_wait(&empty);   // decresce o contador empty
        pthread_mutex_lock(&lock);      // entra na regiao critica
            sleep(3);
            buffer++;       // adiciona novo item ao buffer
            printf("produtor: novo item adicionado ao buffer, total = %d\n", buffer);
        pthread_mutex_unlock(&lock);    // sai da regiao criticia
        sem_post(&full);    // incrementa o contador full

    }
    pthread_exit(0);
  
}

void * consumidor (void* pi){

    while(1){

        sem_wait(&full);    // descresce o contador full
        pthread_mutex_lock(&lock);      // entra na regiao critica
            sleep(3);
            buffer--;       // pega item no buffer
            printf("consumidor: item retirado do buffer, total = %d\n", buffer);
        pthread_mutex_unlock(&lock);    // sai da regiao criticia
        sem_post(&empty);   // incrementa o contador empty

        // consome o item que tirou do buffer
        printf("consumidor: consumindo o item\n");
    
    }
    pthread_exit(0);
  
}
