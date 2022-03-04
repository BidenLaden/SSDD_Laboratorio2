#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>


#define NUM_THREADS	2
#define ITER 		10

pthread_mutex_t mutex;
pthread_mutex_t mutex1;

int copiado = false;
int turno = 0;
pthread_cond_t condicion;
pthread_cond_t condicion1;



void funcion(int *id) {
	int j;
	int s;
	double k;

	pthread_mutex_lock(&mutex);
	int mid = *id;  // cada thread recibe un número (0 o 1)
	copiado = true;
	pthread_cond_signal(&condicion);
	pthread_mutex_unlock(&mutex);
	
	


	for(j=0 ; j < ITER; j++) {
		pthread_mutex_lock(&mutex1);
		while(turno != mid){
			pthread_cond_wait(&condicion1, &mutex1);
		}
		turno = (mid + 1)%NUM_THREADS; //Para que salga 0 o 1
		pthread_mutex_unlock(&mutex1);

		k = (double) rand_r((unsigned int *) &s) / RAND_MAX;	
		usleep((int) (k * 100000)); // duerme entre 0 y 100 ms
		printf("Ejecuta el thread %d iteracion %d \n", mid, j );

		pthread_cond_signal(&condicion1);

		
	}
	pthread_exit(NULL);

}

int main(int argc, char *argv[])
{
	int j;
	pthread_attr_t attr;
	pthread_t thid[NUM_THREADS];
	struct timeval t;

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex1, NULL);
	pthread_cond_init(&condicion, NULL);
	pthread_cond_init(&condicion1, NULL);
	pthread_attr_init(&attr);

	gettimeofday(&t, NULL);
	srand(t.tv_sec);	// se inicializa la semilla de nª pseudoaleatorios

	pthread_attr_init(&attr);



	for (j = 0; j < NUM_THREADS; j++)
		if (pthread_create(&thid[j], NULL, (void *) funcion, &j) == 0){
			pthread_mutex_lock(&mutex); //Bloqueamos el main
			while(copiado == false){
				pthread_cond_wait(&condicion, &mutex);
			}
			copiado = false;
			pthread_mutex_unlock(&mutex);

		}else{
			printf("Error al crear los threads\n");
			exit(0);
		}

	for (j = 0; j < NUM_THREADS; j++)
		pthread_join(thid[j], NULL);

	exit(0);

}