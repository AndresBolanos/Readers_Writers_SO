#include <stdio.h>
#include <sys/ipc.h>
#include  <sys/shm.h>
#include <string.h>
#include <stdlib.h>

void Create_Memory(char * memory_name, int memory_key, int memory_size){
	//Crea la llave unica
	int key = ftok(memory_name, memory_key);
	if(key == -1){
		printf("Error!!!  con la llave\n");
		exit(1);
	}

	printf("Creando la memoria compartida\n");
	// Crea la memoria compartida
	int shmid  = shmget (key, sizeof(int)*memory_size, 0777 | IPC_CREAT);
	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}

	printf("Memoria creada con el id:  %d\n", shmid );

	int *buffer;

	// Para usar la memoria hay que hacerle attached
	buffer = shmat (shmid , (char *)0, 0);
	if (buffer == NULL) {
		printf("Error!!!  reservando la memoria compartida\n");
		exit(1);
	}

	//Limpio el buffer de la memoria
	int i;
	for (i = 0; i < memory_size; i++){
		buffer[i] = -1;
	};	
}

int main(int argc, char const *argv[])
{
	//Deben limpiarce los archivos antes de todo
	//Deben crearce los semaforos antes de todo

	//solicita la memoria o lineasrequerida 
	int size_memory;
	printf("Ingrese la cantidad de memoria que desea reservar \n");
	scanf("%d",&size_memory);

	Create_Memory("/bin/cat", 65, size_memory);
	return 0;
}