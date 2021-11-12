/**
 * Maria Eduarda Holanda - 190043725
 * Faça um programa para somar os valores dos elementos de uma matriz utilizando várias threads.
**/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXSIZE 10000  /* maximum matrix size */

pthread_barrier_t barrier;  
int size;

int matrix1[MAXSIZE][MAXSIZE];
int parcial[MAXSIZE];

void *Worker(void *);
void calcula_soma();
void mostra_matriz();

int main(int argc, char *argv[]) {
    int i, j;
  
    size = atoi(argv[1]);
  
    if(size > MAXSIZE){
        printf("Tamanho muito grande!\n");
        return 0;	
    }

    pthread_t workerid[size];

    pthread_barrier_init(&barrier, NULL, size);

    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            matrix1[i][j] = 1;

    int * id;
    for (i = 0; i < size; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&workerid[i], NULL, Worker, (void *) (id));
    }

    for (i = 0; i < size; i++){
        if(pthread_join(workerid[i], NULL)){
            printf("\n ERROR joining thread");
            exit(1);
        }
    }
    
    printf("Bye!\n");
}

void *Worker(void *arg){

    int myid = *(int *)(arg);
    int j, k;

    int self = pthread_self();
    
    printf("worker %d (pthread id %d) has started\n", myid, self);

    for(j = 0; j < size; j++){
        parcial[myid] += matrix1[myid][j]; 
    }

    pthread_barrier_wait(&barrier);

    if (myid == 0) {
        mostra_matriz();
        calcula_soma();
    }
}

void mostra_matriz(){
    printf("\nMatriz:\n");
    for(int j = 0; j < size; j++){
        for(int k = 0; k < size; k++){
            printf("%d ",matrix1[j][k]);  
            
        }
        printf("\n");
    }
}

void calcula_soma(){
    int soma = 0;

    printf("\n");
    for(int i=0; i<size; i++){
        printf(" Parcial %d: %d\n", i, parcial[i]);
        soma+= parcial[i];
    }

    printf("\nSoma total = %d\n\n", soma); 
}