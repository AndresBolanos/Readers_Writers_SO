#include "Headers/Inicializador.h"
#include <stdio.h>
#include <sys/ipc.h>
#include  <sys/shm.h>
#include <string.h>
#include <stdlib.h>

//Compile project
//gcc -w Inicializador.c Funciones.c  -o Inicializador -pthread
//./Inicializador
//Clean Memory
//ipcrm -M 0x4101001a
//See memory keys
//ipcs

void Create_Memory(char * memory_name, int memory_key, int lines_memory){
	//Crea la llave unica
	// buffer[lines_memory][5];
	sem_t * sem = NULL;

	int key = ftok(memory_name, memory_key);
	printf("La llave de la memoria es %d\n",key );
	if(key == -1){
		printf("Error!!!  con la llave\n");
		exit(1);
	}

	//Se crea el semaforo
	sem = (sem_t *) solicitar_sem(SEM_NAME);

	printf("Creando la memoria compartida\n");
	// Crea la memoria compartida
	int shmid  = shmget (key,lines_memory, 0777 | IPC_CREAT);
	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}

	printf("Memoria creada con el id:  %d\n", shmid );

	//Bloqueo la memoria
	bloquear_sem(sem);

	// Para usar la memoria hay que hacerle attached
	char *buffer; /* shared buffer */
	buffer = shmat (shmid , (char *)0 , 0);
	if (buffer == NULL) {
		printf("Error!!!  reservando la memoria compartida\n");
		exit(1);
	}

	//Limpio el buffer de la memoria
	int i;
	int cont = 0;
	for (i = 0; i < lines_memory; i++){
		if (cont == 34){
			buffer[i] = ',';
			cont = 0;
		}
		else{
			buffer[i] = 'X';
			cont++;
		}
	}	
	
	for (i = 0; i < lines_memory; i++){
		printf("segment contains: \"%c\"\n", buffer[i]);
	};
	//Se desbloquea la memoria
	desbloquear_sem(sem);	
}

int main(int argc, char const *argv[])
{
	//Deben limpiarce los archivos antes de todo

	//solicita la memoria o lineasrequerida 
	int lines_memory;
	int largo_mensaje =34;
	printf("Ingrese la cantidad de lineas que desea reservar\n");
	scanf("%d",&lines_memory);

	Create_Memory("/bin/cat", 65, lines_memory * largo_mensaje);
	return 0;
}
