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

void ReadMemory(int memory_key, int lines_memory){
	char *shm, *s;

	int shmid  = shmget (KEY,MENSAJE, 0777);

	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}

    printf("Memoria accesada\n");

    // Para usar la memoria hay que hacerle attached
	char *buffer; /* shared buffer */
	buffer = shmat (shmid , (char *)0 , 0);
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
	for (int i = 0; i < cantidad; i++){
		pthread_t thread1;
		struct Reader reader1; // = {i,0, timestamp(), escritura, dormido};
		reader1.id = i;
		reader1.tiempo = timestamp();
		reader1.lectura = lectura;
		reader1.dormido = dormido;
		pthread_create (&thread1, NULL, (void*)ReadMemory, (void*)&reader1);

		pthread_join(thread1, NULL);
	}
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