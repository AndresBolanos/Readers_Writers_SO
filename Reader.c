#include "Headers/Reader.h"
#include <stdio.h>
#include <sys/ipc.h>
#include  <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define KEY 1090584602
#define MENSAJE 34

sem_t * sem = NULL; 	//Definimos el semaforo

//Compile the code
//gcc -w Reader.c Funciones.c  -o Reader -pthread
//./Reader

struct Reader
{
  int id;
  char *tiempo;
  int dormido; 
  int lectura;
};

char* timestamp(){
  time_t ltime; /* tiempo calendario */
  ltime = time(NULL); /* obtener tiempo actual */
  return asctime(localtime(&ltime));
}

void ReadMemory(void * reader2){

	struct Reader * reader1 = (struct Reader*) reader2;
	
	int shmid  = shmget (KEY,MENSAJE, 0777);

	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}

    printf("Memoria accesada\n");

    while (true){
    	//Bloqueo la memoria
    	bloquear_sem(sem);
    	char *buffer; // shared buffer 
		buffer = shmat (shmid , (char *)0 , 0);
		reader1->tiempo = timestamp();
		printf("%s%d\n", "LEYENDO ",reader1->id);
		sleep(reader1->lectura);
		
		//Se desbloquea la memoria
		desbloquear_sem(sem);
	
		printf("%s%d\n", "DURMIENDO ",reader1->id);
		sleep(reader1->dormido);
    }
	//save_state('R', "estado.txt"); //Se guarda en un archivo que un semaforo esta en memori

    // Para usar la memoria hay que hacerle attached
	
	//Se desbloquea la memoria
}

void ReadMemory_Aux(struct Reader *reader, char *buffer){
	if (buffer == NULL) {
		printf("Error!!!  No se puede hacer attached\n");
		exit(1);
	}

	printf("Memoria lista para leer\n");
	int i;
	int linea = 0;

	for (i = 0; i < strlen(buffer); i++){
		if (buffer[i] == ','){
			if (buffer[i-1] != 'X'){
				printf("\n");
			}
			linea++;
		}
		else{
			if (buffer[i] != 'X'){
				printf("%c", buffer[i]);
			}
		}
	}
}

void Creador_Readers(int cantidad, int lectura, int dormido){
	char *shm, *s;
	//Se crea el semaforo
	sem = (sem_t *) solicitar_sem(SEM_NAME);
	pthread_t thread1;
	int i;
	for (int i = 0; i < cantidad; i++){
		
		struct Reader reader1; // = {i,0, timestamp(), escritura, dormido};
		reader1.id = i;
		reader1.tiempo = timestamp();
		reader1.lectura = lectura;
		reader1.dormido = dormido;
		pthread_create (&thread1, NULL, (void*)ReadMemory, (void*)&reader1);

	}
	pthread_join(thread1, NULL);
}

int main(int argc, char const *argv[])
{
	int cantidad;
	int lectura;
	int dormido;
	printf("Ingrese la cantidad de readers que desea: ");
	scanf("%d",&cantidad);
	printf("Ingrese la cantidad de tiempo que tarda leyendo: ");
	scanf("%d",&lectura);
	printf("Ingrese la cantidad de tiempo que tarda durmiendo: ");
	scanf("%d",&dormido);
	Creador_Readers(cantidad, lectura, dormido);
	return 0;
}