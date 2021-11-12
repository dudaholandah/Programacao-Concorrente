/* Maria Eduarda Holanda - 190043725*/

/**
 * Solução usando semáforos:
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

sem_t sem_pombo;  
sem_t sem_cartas;   
sem_t sem_voo;
sem_t sem_lock;

int cartas_mochila = 0;

void * f_usuario(void *arg);
void * f_pombo(void *arg);
void viajando();

int main(int argc, char **argv){
    int i;

    sem_init(&sem_pombo, 0, 1);
	sem_init(&sem_cartas, 0, CARTAS);
    sem_init(&sem_voo, 0, 0);
    sem_init(&sem_lock, 0, 1);

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

        sem_wait(&sem_voo); // ve se pode voar ou nao
        sem_wait(&sem_pombo); // comeca a voar

            viajando();

            printf("Pombo chegou ao destino B e ira esvaziar a mochila!\n");
            sleep(2);
            for(int i=0; i<CARTAS; i++){
                // entrega as cartas
                sem_post(&sem_cartas); 
                cartas_mochila--;
            }
            printf("Mochila vazia! Pombo ira retornar ao local A!\n");

            viajando();

            printf("Pombo retornou ao local A!\n");


        sem_post(&sem_pombo); // para de voar

    }
}

void * f_usuario(void *arg){
    
    while(1){
	
        sem_wait(&sem_cartas); // escreve a carta ou dorme esperando o pombo chegar
        sem_wait(&sem_pombo); // o pombo espera ate poder voar de novo

            // adiciona uma carta na mochila
            cartas_mochila++;
            printf("Adicionando cartas na mochila, quantidade de cartas: %d\n", cartas_mochila);
            sleep(1);

            if(cartas_mochila == CARTAS){
                sem_post(&sem_voo); // o pombo pode voar
                printf("Mochila cheia, agora o pombo pode viajar!\n");
            }

        sem_post(&sem_pombo); // libera o pombo
        //sleep(3);
    }
}

void viajando(){
    printf("Pombo esta viajando!\n");
    sleep(3);
}
