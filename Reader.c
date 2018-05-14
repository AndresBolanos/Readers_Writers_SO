#include <stdio.h>
#include <sys/ipc.h>
#include  <sys/shm.h>
#include <string.h>
#include <stdlib.h>

void ReadMemory(int memory_key, int lines_memory){
	char *shm, *s;

	int shmid  = shmget (memory_key,lines_memory, 0777);
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
	for (i = 0; i < lines_memory; i++){
		printf("segment contains: \"%c\"\n", buffer[i]);
	};	

}


int main(int argc, char const *argv[])
{
	int lines_to_read = 10;
	int memory_key = 1090584602;
	ReadMemory(memory_key, lines_to_read);
	return 0;
}