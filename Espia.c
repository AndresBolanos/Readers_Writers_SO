#include "Headers/Espia.h"
#include <stdio.h>
#include <sys/ipc.h>
#include  <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MENSAJE 34

sem_t * sem = NULL; 	//Definimos el semaforo
char *buffer;

void memory_State(){
	sem = (sem_t *) solicitar_sem(SEM_NAME);

	//Se lee la memoria del archivo de texto
	int key = read_int(ID_MEM_FILE);

	int shmid  = shmget (key,MENSAJE, 0777);
	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}
	bloquear_sem_sencillo(sem);
	
	printf("Entra aqui\n");
	 /* shared buffer */
	buffer = shmat (shmid , (char *)0 , 0);
	printf("%d\n", strlen(buffer));
	int i;
	for (i = 0; i < strlen(buffer); i++){
		if (buffer[i] == ','){
			printf("\n");
		}
		else if (buffer[i] != '*'){
			printf("%c",buffer[i]);
		}
	}
	printf("\n");
	desbloquear_sem(sem);
}

int main(int argc, char const *argv[])
{
	int option;
	printf("Que desea consultar\n");
	printf("1. Estado del archivo\n");
	printf("2. Estado de los Writers\n");
	printf("2. Estado de los Readers\n");
	printf("2. Estado de los Readers Egoistas\n");
	scanf("%d",&option);
	if (option == 1){
		memory_State();
	}
	//Creador_Readers(cantidad, lectura, dormido);
	return 0;
}