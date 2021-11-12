/* Maria Eduarda Holanda - 190043725*/

/**
 * Solução usando variáveis condicionais:
 * 
 * Um pombo correio leva cartas entre os sites A e B, mas só quando o número de cartas
 * acumuladas em sua mochila chegar a 20. Inicialmente, o pombo fica em A, esperando 
 * que existam 20 cartas para carregar, e dormindo enquanto não houver. Quando as 
 * cartas chegam a 20, o pombo deve levar todas as 20 cartas de A para B, e em seguida 
 * voltar para A, repetindo este ciclo. As cartas são escritas por uma quantidade qualquer 
 * de usuários. Cada usuário, quando tem uma carta pronta, coloca sua carta na mochila do 
 * pombo e volta a escrever uma nova carta. Caso o pombo tenha partido, ele deve esperar o 
 * seu retorno para colar a carta na mochila.
**/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

#define TRUE 1
#define FALSE 0

int pombo_disponivel = TRUE;
int cartas_mochila = 0;

pthread_cond_t usuario_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pombo_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * f_usuario(void *arg);
void * f_pombo(void *arg);
void viajando();

int main(int argc, char **argv){
    int i;

    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }
    
    pthread_t pombo;
    id = (int *) malloc(sizeof(int));
    *id = 0;
    pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

    pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
  
    while(1){

        // regiao critica
        pthread_mutex_lock(&lock);

            while (cartas_mochila < CARTAS) {
                pthread_cond_wait(&pombo_cond, &lock);
            }

            viajando();

            printf("Pombo chegou ao destino B e ira esvaziar a mochila!\n");
            sleep(3);
            cartas_mochila = 0;
            printf("Mochila vazia! Pombo ira retornar ao local A!\n");

            viajando();

            pombo_disponivel = TRUE;
            printf("Pombo retornou ao local A! Agora ele esta disponivel!\n");
            pthread_cond_broadcast(&usuario_cond);


        pthread_mutex_unlock(&lock);
          
    }
}

void * f_usuario(void *arg){
    
    while(1){

        // regiao critica
        pthread_mutex_lock(&lock);

            // se o pombo nao estiver disponivel ou a mochila estiver cheia, o usuario vai dormir
            while(!pombo_disponivel || cartas_mochila == CARTAS){
                pthread_cond_wait(&usuario_cond, &lock);
            }

            // adiciona cartas na mochila
            cartas_mochila++;
            printf("Adicionando cartas na mochila, quantidade de cartas: %d\n", cartas_mochila);
            sleep(1);

            // quando a mochila estiver cheia o pombo ira viajar
            if(cartas_mochila == CARTAS){
                printf("Mochila cheia, agora o pombo pode viajar!\n");
                pthread_cond_signal(&pombo_cond);
            }

        pthread_mutex_unlock(&lock);
	
    }
}

void viajando(){
    printf("Pombo esta viajando!\n");
    sleep(3);
}