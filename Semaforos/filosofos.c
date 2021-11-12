/* Maria Eduarda Holanda - 190043725*/

/**
 * Cinco filósofos estão sentados em uma mesa redonda para jantar. Cada filósofo
 * tem um prato com espaguete à sua frente. Cada prato possui um garfo para pegar 
 * o espaguete. O espaguete está muito escorregadio e, para que um filósofo consiga 
 * comer, será necessário utilizar dois garfos. Cada filósofo alterna entre duas 
 * tarefas: comer ou pensar. Quando um filósofo fica com fome, ele tenta pegar os
 * garfos à sua esquerda e à sua direita; um de cada vez, independente da ordem.
 * Caso ele consiga pegar dois garfos, ele come durante um determinado tempo e 
 * depois recoloca os garfos na mesa. Em seguida ele volta a pensar. O objetivo é
 * criar um algoritmo para esse problema sem gerar um Deadlock.
**/ 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id+N-1)%N
#define DIR(id)	(id+1)%N

#define PENSANDO 0
#define FAMINTO 1
#define COMENDO 2

sem_t lock;
sem_t s[N];	// indica quem esta com talheres

int estado[N]; // todos comecam pensando

void *filosofos (void *arg);
void pega_talher (int n);
void devolve_talher (int n);
void comer (int n);

int main () {
	int i;
	int *id;

	sem_init(&lock, 0, 1);

	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 0);
	}
	pthread_t r[N]; 

	//criacao das threads de filosofos
	for (i = 0; i < N ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

    pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
		
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);

		pega_talher(n);
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);

    	printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	} 
}

void pega_talher (int n) {
	
	// regiao critica - define a vez
	sem_wait(&lock);

		// filosofo esta faminto entao tenta comer
		estado[n] = FAMINTO;
		comer(n);

	sem_post(&lock);

	sem_wait(&s[n]); // bloqueia se nao pegou os talheres
	
}

void devolve_talher (int n) {

	// regiao critica - define a vez
	sem_wait(&lock);

		// ja comeu entao volta a pensar
		estado[n] = PENSANDO;
		
		// tenta chamar os vizinhos para comerem
		comer(ESQ(n)); 
		comer(DIR(n));

	sem_post(&lock);
}

void comer (int n) {

	// se o filosofo estiver faminto e os filosofos ao lado nao estiverem comendo
	// entao o filosofo pega dois talheres e comeca a comer
	if(estado[n] == FAMINTO && estado[ESQ(n)] != COMENDO && estado[DIR(n)] != COMENDO){

		estado[n] = COMENDO;
		sem_post(&s[n]); // pega talheres
	}

}

