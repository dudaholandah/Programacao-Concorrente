/**
 * Maria Eduarda Machado de Holanda - 190043725 
**/

/**
 * Exercicio:
 * 
 * Uma tribo de N canibais selvagens está jantando os missionários capturados. Eles ficam ao 
 * redor de um grande caldeirão contendo X porções de ensopado de missionário cozido. Quando 
 * um selvagem tem fome, ele se serve no caldeirão (pega 1 porção), a menos que esteja vazio 
 * (neste caso, o canibal deve aguardar/dormir até que o cozinheiro forneça mais porções). 
 * Quando um canibal pegar a última porção, o mesmo deve acordar o cozinheiro para que ele 
 * prepare mais X porções de ensopado e encha o caldeirão. Após encher a tigela, o cozinheiro 
 * acorda os canibais selvagens que estavam esperando o ensopado ficar pronto e volta a dormir.
**/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

pthread_cond_t canibais_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cozinheiro_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t canibais_lock = PTHREAD_MUTEX_INITIALIZER;

int comida = 0;

void *canibal(void*meuid);
void *cozinheiro(int m);

void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);

}

void * canibal (void* pi){
  
  while(1) {

    pthread_mutex_lock(&lock);
    
      // se esta sem comida vai dormir
      while(comida == 0){
        pthread_cond_wait(&canibais_cond, &lock);
      }

      // pega sua porcao de comida
      comida--;

      // acorda o cozinheiro se as porções acabaram
      if(comida == 0){
        printf("canibais estamos sem comida!\n");
        pthread_cond_signal(&cozinheiro_cond);
      }

    pthread_mutex_unlock(&lock);

    // todos comem juntos
    printf("canibal %d: vou comer a porcao que peguei\n", *(int *)(pi));
    sleep(5);

  }
  
}

void *cozinheiro (int m){
 
  while(1){

    pthread_mutex_lock(&lock);
    
      // dormir enquanto tiver comida
      while(comida > 0) pthread_cond_wait(&cozinheiro_cond, &lock);

      // cozinhando 
      printf("cozinheiro: vou cozinhar\n");
      sleep(5);
      comida = m;

      // acordar os canibais
      if(comida == m){
        printf("cozinheiro: %d porcoes de comida na mesa!\n", m);
        pthread_cond_broadcast(&canibais_cond); // acorda todos os canibais
      } 

    pthread_mutex_unlock(&lock);

  }

}
