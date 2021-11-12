/**
 * Maria Eduarda Holanda - 190043725
**/

/**
 * Problema do Barbeiro Dorminhoco:
 * 
 * Um outro problema clássico de IPC acontece em uma barbearia. Na barbearia há um barbeiro, uma cadeira de 
 * barbeiro e N cadeiras para eventuais clientes esperarem a vez. Quando não há clientes, o barbeiro senta-se
 * e cai no sono (fica bloqueado). Quando chega um cliente, ele precisa acordar o barbeiro. Se outros clientes 
 * chegarem enquanto o barbeiro estiver cortando o cabelo de um cliente, eles se sentarão (se houver cadeiras 
 * vazias) ou sairão da barbearia (se todas as cadeiras estiverem ocupadas). O problema é programar o barbeiro 
 * e os M clientes sem cair em condições de disputa. Esse problema é semelhante a situações com várias filas, 
 * como uma mesa de atendimento de telemarketing com diversos atendentes e com um sistema computadorizado de 
 * chamadas em espera, atendendo a um número limitado de chamadas que chegam.
**/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CLIENTES 50
#define N_CADEIRAS 5

sem_t sem_cadeiras;   // cadeiras de esperas vazias
sem_t sem_barbeiro;   // barbeiro atende o cliente
sem_t sem_cliente;    // cliente eh atendido
pthread_mutex_t lock_cadeira = PTHREAD_MUTEX_INITIALIZER; // lock para pegar a cadeira do barbeiro

void * f_barbeiro(void *v) {

  while(1) {
    
    //...Esperar/dormindo algum cliente sentar na cadeira do barbeiro
    sem_wait(&sem_barbeiro);

      sleep(1); //Cortar o cabelo do cliente 
      printf("Barbeiro cortou o cabelo de um cliente\n"); 
    
    //...Liberar/desbloquear o cliente
    sem_post(&sem_cliente);
  }
  pthread_exit(0);
}

void* f_cliente(void* v) {
  int id = *(int*) v;
  sleep(id%3);
  
  if(sem_trywait(&sem_cadeiras) == 0){ //conseguiu pegar uma cadeira de espera

    printf("Cliente %d entrou na barbearia \n",id);
    //... pegar/sentar a cadeira do barbeiro (entra na regiao critica - so é possivel um cliente por vez cortar o cabelo)
    pthread_mutex_lock(&lock_cadeira);

      //... liberar a sua cadeira de espera (incrementa cadeiras vazias)
      sem_post(&sem_cadeiras);
      //... acordar o barbeiro para cortar seu cabelo (agora o cliente esta na cadeira do barbeiro)
      sem_post(&sem_barbeiro);
      //... aguardar o corte do seu cabelo (bloqueia o cliente para fazer o seu corte)
      sem_wait(&sem_cliente);
    
    //... liberar a cadeira do barbeiro (sai da regiao critica)
    pthread_mutex_unlock(&lock_cadeira);
    printf("Cliente %d cortou o cabelo e foi embora \n",id);

  }
  else{ //barbearia cheia
	  printf("Barbearia cheia, cliente %d indo embora\n",id);
  }

  pthread_exit(0);
}

int main() {
  pthread_t thr_clientes[N_CLIENTES], thr_barbeiro;
  int i, id[N_CLIENTES];

  sem_init(&sem_cadeiras, 0, N_CADEIRAS);
  sem_init(&sem_barbeiro, 0, 0);
  sem_init(&sem_cliente, 0, 0);
  
  for (i = 0; i < N_CLIENTES; i++) {
    id[i] = i;
    pthread_create(&thr_clientes[i], NULL, f_cliente, (void*) &id[i]);
  }

  pthread_create(&thr_barbeiro, NULL, f_barbeiro, NULL);
  
  for (i = 0; i < N_CLIENTES; i++) 
    pthread_join(thr_clientes[i], NULL);

  /* Barbeiro assassinado */
  
  return 0;
}

