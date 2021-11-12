/**
 * Maria Eduarda Machado de Holanda - 190043725 
**/

/**
 * Exercicio:
 * 
 * Este problema é uma abstração do acesso à base de dados, onde não existe o perigo em termos
 * diversos processos lendo concorrentemente (leitores), mas escrevendo ou mudando os dados 
 * (escritores) deve ser feito sob exclusão mútua para garantir consistência.
 * Leitores e escritores é uma família de modelos de controle de concorrência em que leitores 
 * (entes que não alteram conteúdo) pode acessar concorrentemente os recursos (por exemplo, 
 * um banco de dados) e escritores (entes que alteram conteúdo) requerem acesso exclusivo.
 * 
 * Usando como base o código disponibilizado neste tópico, resolva o problema de starvation dos escritores.
**/

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1
#define NE 3 	//numero de escritores
#define NL 10 	//numero de leitores

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_vez = PTHREAD_MUTEX_INITIALIZER;

int num_leitores = 0;

void * reader(void *arg);
void * writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {

	pthread_t r[NL], w[NE];
	int i;
	int *id;
	/* criando leitores */
	for (i = 0; i < NL ; i++){
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, reader, (void *) (id));
	}
	/* criando escritores */
	for (i = 0; i< NE; i++){
	   	id = (int *) malloc(sizeof(int));
        *id = i;
		pthread_create(&w[i], NULL, writer, (void *) (id));
	}
	pthread_join(r[0],NULL);
	return 0;
}

void * reader(void *arg) {
	int i = *((int *) arg);

	while(TRUE){

		// lock que determina se eh a vez dos leitores ou escritores
		pthread_mutex_lock(&lock_vez);		
			pthread_mutex_lock(&lock_nl);
				num_leitores++;
				if(num_leitores == 1){
					pthread_mutex_lock(&lock_bd);
				}
			pthread_mutex_unlock(&lock_nl);
		pthread_mutex_unlock(&lock_vez);

		read_data_base(i);	/* acesso aos dados */
		
		pthread_mutex_lock(&lock_nl);
			num_leitores--;
			if(num_leitores == 0){ 
				pthread_mutex_unlock(&lock_bd);
			}
		pthread_mutex_unlock(&lock_nl);
		use_data_read(i);	/* região não crítica */		
		
	}
	pthread_exit(0);
}

void * writer(void *arg) {
	int i = *((int *) arg);
	while(TRUE) {            
		think_up_data(i);       /* região não crítica */

		// lock para adicionar escritor na fila da vez
		// se existir 1 escritor na fila, sera a vez dele e nao sera adicionado mais leitores na fila
		pthread_mutex_lock(&lock_vez);	
			pthread_mutex_lock(&lock_bd);
				write_data_base(i);			/* atualiza os dados */
			pthread_mutex_unlock(&lock_bd);
		pthread_mutex_unlock(&lock_vez);


	}
	pthread_exit(0);
}

void read_data_base(int i) {
	printf("Leitor %d está lendo os dados! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 5 + 8);
}

void use_data_read(int i) {
	printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 5);
}

void think_up_data(int i) {
	printf("Escritor %d está pensando no que escrever!\n", i);
	sleep(rand() % 5);
}

void write_data_base(int i) {
	printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 5 + 8);
}
