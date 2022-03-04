#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define NUM_THREADS	2
#define ITER 		10

/*Mutex y variables condicionales para proteger la variable*/
pthread_mutex_t mutex;
int copiado = false;
pthread_cond_t condicion;

void funcion(int *id) {
	int j;
	int s;
	double k;

	pthread_mutex_lock(&mutex);
	//Aqui esta la seccion critica, esta asignacion:
	int mid = *id;  // cada thread recibe un número (0 o 1)

	copiado = true;

	for(j=0 ; j < ITER; j++) {
		k = (double) rand_r((unsigned int *) &s) / RAND_MAX;	
		usleep((int) (k * 100000)); // duerme entre 0 y 100 ms
		printf("Ejecuta el thread %d iteracion %d \n", mid, j );
	}
	pthread_cond_signal(&condicion);

	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);

}
int main(int argc, char *argv[])
{
	//El main es un PROCESO LIGERO!!! 
	//Y como tal tiene que bloquearse(por eso hace un lock con su correspondiente unlock) para dar paso
	//a los threads

	int j;
	pthread_attr_t attr;
	pthread_t thid[NUM_THREADS];
	struct timeval t;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condicion, NULL);
	pthread_attr_init(&attr);

	/*Atributos de los threads, que son independientes*/
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	

	gettimeofday(&t, NULL);
	srand(t.tv_sec);	// se inicializa la semilla de nª pseudoaleatorios

	pthread_attr_init(&attr);

	for (j = 0; j < NUM_THREADS; j++)
		if (pthread_create(&thid[j], NULL, (void *) funcion, &j) == 0){
			//Seccion critica
			pthread_mutex_lock(&mutex);
			while(copiado == false){
				pthread_cond_wait(&condicion, &mutex);//El wait libera el mutex, se bloquea en el mutex de void*
				//el wait tiene implicito un unlock y un lock (en ese orden)
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