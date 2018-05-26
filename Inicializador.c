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
	sem_t * semF = NULL;

	int key = ftok(memory_name, memory_key);
	printf("La llave de la memoria es %d\n",key );

	//Se guarda en el archivo el id de la memoria
	save_int(key,ID_MEM_FILE);

	if(key == -1){
		printf("Error!!!  con la llave\n");
		exit(1);
	}

	//Se crea el semaforo
	sem = (sem_t *) solicitar_sem(SEM_NAME);
	semF = (sem_t *) solicitar_sem(SEM_FILE_WRITERS);
	//Se guarda que hay cero egoistas en memoria
	save_int(0,EGOISTAS);

	printf("Creando la memoria compartida\n");
	// Crea la memoria compartida
	int shmid  = shmget (key,lines_memory, 0777 | IPC_CREAT);
	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}

	printf("Memoria creada con el id:  %d\n", shmid );

	//Bloqueo la memoria
	bool resp = bloquear_sem(sem, 'I');

	if (resp){
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
			if (cont == 33){
				buffer[i] = ',';
				cont = 0;
			}
			else{
				buffer[i] = 'X';
				cont++;
			}
		}	
		//Se desbloquea la memoria
		//Se limpian los archivos
		//Se borra q hay un egoista en memoria
		save_chain(" ", MEM_EGOISTAS,"w");
		save_chain(" ", MEM_READERS,"w");
		save_chain(" ", MEM_WRITERS ,"w");
		save_chain(" ", BITACORA ,"w");
		save_chain(" ", DORM_WRITERS ,"w");
		save_chain(" ", BLOQ_WRITERS ,"w");
		save_chain(" ", BLOQ_READERS ,"w");
		save_chain(" ", DORM_READERS ,"w");
		save_chain(" ", DORM_EGOISTA ,"w");
		save_chain(" ", BLOQ_EGOISTA ,"w");
		save_int(0,EGOISTAS);

		desbloquear_sem(sem);	
	}
	else{
		printf("Error!! No se puede escribir la memoria\n");
	}
	
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