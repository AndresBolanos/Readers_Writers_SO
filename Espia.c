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

sem_t * semF = NULL; 	//Definimos el semaforo

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

void bloqueados(char * nombre_archivo){
	bloquear_sem_sencillo(semF);
	printf("\n");
	char ch;
   	FILE *fp;
 	fp = fopen(nombre_archivo, "r"); // read mode
 
	if (fp == NULL){
		printf("%s\n", "Error");
		return;
	}
	printf("%s\n", "*Bloqueados*");
	while((ch = fgetc(fp)) != EOF)
	    printf("%c", ch);
	fclose(fp);
	printf("\n");
	desbloquear_sem(semF);
}

void dormidos(char * nombre_archivo){
	bloquear_sem_sencillo(semF);
	printf("\n");
	char ch;
   	FILE *fp;
	fp = fopen(nombre_archivo, "r"); // read mode
	if (fp == NULL){
		printf("%s\n", "Error");
		return;
	}
	printf("%s\n", "*Dormidos*");
	while((ch = fgetc(fp)) != EOF)
	    printf("%c", ch);
	fclose(fp);
	printf("\n");
	desbloquear_sem(semF);
}

void memoria(char * nombre_archivo){
	bloquear_sem_sencillo(semF);
	printf("\n");
	char ch;
   	FILE *fp;
	fp = fopen(nombre_archivo, "r"); // read mode
	if (fp == NULL){
		printf("%s\n", "Error");
		return;
	}
	printf("%s\n", "*Memoria*");
	while((ch = fgetc(fp)) != EOF)
	    printf("%c", ch);
	fclose(fp);
	printf("\n");
	desbloquear_sem(semF);
}


int main(int argc, char const *argv[])
{
	semF = (sem_t *) solicitar_sem(SEM_FILE_WRITERS);
	int option;
	printf("Que desea consultar\n");
	printf("1. Estado del archivo\n");
	printf("2. Estado de los Writers\n");
	printf("3. Estado de los Readers\n");
	printf("4. Estado de los Readers Egoistas\n");
	scanf("%d",&option);
	if (option == 1){
		memory_State();
	}
	else if(option ==2){
		memoria("procesos_Memoria_Writer.txt");
		bloqueados("procesos_Bloqueados_Writer.txt");
		dormidos("procesos_Dormidos_Writer.txt");
	}
	else if (option ==3){
		memoria("procesos_Memoria_Reader.txt");
		bloqueados("procesos_Bloqueados_Reader.txt");
		dormidos("procesos_Dormidos_Reader.txt");
	}
	else{
		memoria("procesos_Memoria_Egoista.txt");
		bloqueados("procesos_Bloqueados_Egoista.txt");
		dormidos("procesos_Dormidos_Egoista.txt");
	}
	return 0;
}