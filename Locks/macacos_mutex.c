/**
 * Maria Eduarda Machado de Holanda - 190043725 
**/

/**
 * Exercicio:
 * 
 * Suponha que há macacos em dois morros A e B, ligados por uma ponte de corda. De tempos em tempos
 * os macacos decidem passar para o outro morro à procura de comida. Esta passagem é feita através 
 * da ponte de corda. Mais de um macaco pode atravessar a ponte ao mesmo tempo, mas isso só é possível 
 * se eles estiverem indo na mesma direção. Implemente um programa que faça o controle da passagem de macacos
 * pela ponte usando locks. 
**/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_vez = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_travessia = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_mAB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_mBA = PTHREAD_MUTEX_INITIALIZER;

int macacos_AB = 0;
int macacos_BA = 0;

void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){

        // Lock garante que seja a vez dos macacos de BA ou de AB ou do gorila
        pthread_mutex_lock(&lock_vez);
            //Procedimentos para acessar a corda
            pthread_mutex_lock(&lock_mAB);
                macacos_AB++;
                if (macacos_AB == 1){
                    pthread_mutex_lock(&lock_travessia);
                }
            pthread_mutex_unlock(&lock_mAB);
        pthread_mutex_unlock(&lock_vez);

        printf("Macaco %d passado de A para B \n",i);   // travessia
        sleep(1);

        //Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_mAB);
            macacos_AB--;
            if (macacos_AB == 0){
                pthread_mutex_unlock(&lock_travessia);
            }
        pthread_mutex_unlock(&lock_mAB);
    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){

        // Lock garante que seja a vez dos macacos de BA ou de AB ou do gorila
        pthread_mutex_lock(&lock_vez);
            //Procedimentos para acessar a corda
            pthread_mutex_lock(&lock_mBA);
                macacos_BA++;
                if (macacos_BA == 1){
                    pthread_mutex_lock(&lock_travessia);
                }
            pthread_mutex_unlock(&lock_mBA);
        pthread_mutex_unlock(&lock_vez);

        printf("Macaco %d passado de B para A \n",i);   // travessia
        sleep(1);

        //Procedimentos para quando sair da corda
        pthread_mutex_lock(&lock_mBA);
            macacos_BA--;
            if (macacos_BA == 0){
                pthread_mutex_unlock(&lock_travessia);
            }
        pthread_mutex_unlock(&lock_mBA);
    }
    pthread_exit(0);
}

void * gorila(void * a){
    while(1){
        
        // Lock vez garante que sera a vez dos macacos ou dos gorilas
        pthread_mutex_lock(&lock_vez);
            pthread_mutex_lock(&lock_travessia);    //Procedimentos para acessar a corda
        pthread_mutex_unlock(&lock_vez);
              
            printf("Gorila passado de A para B \n");    // travessia
            sleep(5);
            
        //Procedimentos para quando sair da corda
        pthread_mutex_unlock(&lock_travessia);
        
    }
    pthread_exit(0);
}

int main(int argc, char * argv[]){
    
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
        if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
        }
        }
        else{
        if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
        }
        }
    }
    pthread_t g;
    pthread_create(&g, NULL, &gorila, NULL);

    pthread_join(macacos[0], NULL);
    return 0;
}

