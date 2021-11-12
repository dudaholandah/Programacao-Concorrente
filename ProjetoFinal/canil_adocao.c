/**
 * Trabalho Final Programacao Concorrente 2021/1
 * Implementacao de um canil
 * Maria Eduarda Holanda - 190043725
**/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define MAX_CLIENTES 5 // número de clientes
#define MAX_CARROCINHAS 10  // número de carrocinhas
#define MAX_CANIL 20 // capacidade maxima de animais
#define MAX_ESPERA 3 // capacidade maxima de pessoas esperando

// variável compartilhada
int canil = 0; // canil comeca vazio

pthread_cond_t clientes_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t carrocinhas_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

sem_t sem_funcionario_atende;     
sem_t sem_cliente_atendido;  
sem_t sem_sala_espera;  // clientes esperando para adotar

void * funcionario(void *arg);
void * cliente(void *arg);
void * carrocinha(void *arg);

void procura_cachorro(int id);
void achou_cachorro(int id);
void adiciona_ao_canil(int id);
void cliente_chega(int id);
void cliente_vai_embora(int id);
void cliente_adotou(int id);
void atendendo_cliente();

int main(int argc, char **argv){

    pthread_t func;
    pthread_t cli[MAX_CLIENTES];
    pthread_t car[MAX_CARROCINHAS];
    int *id;

    sem_init(&sem_sala_espera, 0, MAX_ESPERA);  // a sala de espera inicializa cheia
    sem_init(&sem_funcionario_atende, 0, 0);    // inicializa com 0, ja que a ação não está sendo feita ainda
    sem_init(&sem_cliente_atendido, 0, 0);

    for (int i=0; i<MAX_CARROCINHAS; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(car[i]), NULL, carrocinha, (void *) (id));
    }

    for (int i=0; i<MAX_CLIENTES; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(cli[i]), NULL, cliente, (void *) (id));
    }

    pthread_create(&func, NULL, funcionario, NULL);

    pthread_join(car[0],NULL);

    return 0;
}

void * funcionario(void *arg){

    while(1){
        // funcionario comeca a atender o cliente
        sem_wait(&sem_funcionario_atende);
            atendendo_cliente();
        sem_post(&sem_cliente_atendido);
        // funcionario termina de atender o cliente e libera ele
    }

}

void * cliente(void *arg){

    int id = *(int*) arg;

    while(1){

        cliente_chega(id);

        // tenta entrar no canil
        if(sem_trywait(&sem_sala_espera) == 0){

            // zona critica quando tenta adotar o cachorrinho
            pthread_mutex_lock(&lock);

                // se o canil estiver vazio, o cliente vai esperar chegar o proximo cachorro
                while(canil == 0){
                    pthread_cond_wait(&clientes_cond, &lock);
                }

                sem_post(&sem_sala_espera);         // sai da sala de espera
                sem_post(&sem_funcionario_atende);  // vai para sala onde o funcionario atende
                sem_wait(&sem_cliente_atendido);    // espera ate ser atendido e escolher seu novo cachorro 

                cliente_adotou(id);

                // chama as carrocinha sempre que o canil não estiver cheio
                pthread_cond_broadcast(&carrocinhas_cond);

            pthread_mutex_unlock(&lock);

        }
        else{
            // se a sala de espera estiver cheia, a pessoa vai embora
            cliente_vai_embora(id);
        }

    }

}

void * carrocinha(void *arg){

    int id = *(int*) arg;

    while(1){

        // cada carrocinha vai atrás de um cachorro
        procura_cachorro(id);
        achou_cachorro(id);
        
        // zona critica quando chega ao canil
        pthread_mutex_lock(&lock);

            // se o canil estiver cheio, as carrocinhas irao parar de procurar outros cachorros
            while(canil == MAX_CANIL){
                pthread_cond_wait(&carrocinhas_cond, &lock);
            }
            
            // adiciona mais um caozinho ao canil
            adiciona_ao_canil(id);

            // chama os clientes sempre que o canil não estiver mais vazio
            if(canil == 1){
                pthread_cond_broadcast(&clientes_cond);
            }   

        pthread_mutex_unlock(&lock);

    }

}

void procura_cachorro(int id){
    printf("Carrocinha %d esta indo procurar um cachorro!\n\n", id);
    sleep(rand() % 40);
}

void achou_cachorro(int id){
    printf("Carrocinha %d acabou de encontrar mais um cachorrinho perdido! Voltando para o canil\n\n", id);
    sleep(rand() % 40);
}

void adiciona_ao_canil(int id){
    canil++;
    printf("Carrocinha %d retornou ao canil com mais um cachorro!\n", id);
    printf("Cachorro adicionado ao canil com sucesso!\n");
    printf("Quantidade de cachorros no canil: %d\n\n", canil);
}

void cliente_chega(int id){
    sleep(rand() % 40);
    printf("\tCliente %d chegou para adotar um cachorrinho!\n\n", id);
}

void cliente_vai_embora(int id){
    printf("\tInfelizmente, a sala de espera do canil está cheia!\n");
    printf("\tCliente %d está indo embora!\n\n", id);
}

void cliente_adotou(int id){
    canil--;
    printf("\tCliente %d adotou um cachorrinho!\n", id);
    printf("\tQuantidade de cachorros restantes: %d\n\n", canil);
}

void atendendo_cliente(){
    sleep(rand() % 5);
    printf("\tFuncionario esta atendendo um cliente! Mais um caozinho sera adotado!\n");
}
