#include "Headers/ReaderEgoista.h"
#include <stdio.h>
#include <sys/ipc.h>
#include  <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

#define MENSAJE 34

sem_t * sem = NULL; 	//Definimos el semaforo
sem_t * semF = NULL; 	//Definimos el semaforo

int cantidadEgoistas;
int estados[100]; 	//Arreglo con el estado del reader. 3 memoria, 2 dormido, 1 bloqueado, 0 desbloqueado.

struct Reader
{
  int id;
  char *tiempo;
  int dormido; 
  int lectura;
}Reader;

char* timestamp(){
  time_t ltime; /* tiempo calendario */
  ltime = time(NULL); /* obtener tiempo actual */
  return asctime(localtime(&ltime));
}

void Cargar_bloqueados(){
	FILE *fptr;
	bloquear_sem_sencillo(semF);
    fptr = fopen("procesos_Bloqueados_Egoista.txt", "w");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }

    int i;
    int flag = 0;
    for (i = 0; i <= cantidadEgoistas; i++){
    	if(estados[i] == 1){
    		fprintf(fptr,"%d\n", i);
    		flag = 1;
    	}
    }
    if (flag == 0){
    	fprintf(fptr,"%s", "*");
    }
    fclose(fptr);
    desbloquear_sem(semF);
}

void Cargar_dormidos(){
	FILE *fptr;
	bloquear_sem_sencillo(semF);
    fptr = fopen("procesos_Dormidos_Egoista.txt", "w");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }

    int i;
    int flag = 0;
    for (i = 0; i <= cantidadEgoistas; i++){
    	if(estados[i] == 2){
    		fprintf(fptr,"%d\n", i);
    		flag = 1;
    	}
    }
    if (flag == 0){
    	fprintf(fptr,"%s", "*");
    }
    fclose(fptr);
    desbloquear_sem(semF);
}

void ReadMemory(void * reader2){
	struct Reader * reader1 = (struct Reader*) reader2;

	//Se lee la memoria del archivo de texto
	int key = read_int(ID_MEM_FILE );

	int shmid  = shmget (key,MENSAJE, 0777);
	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}

    while(true){

    	estados[reader1->id] = 1; //bloqueado
		Cargar_bloqueados();
		Cargar_dormidos();

    	//Bloqueo la memoria
		bool resp = bloquear_sem(sem, 'E');
		if (resp){
			printf("%s%d\n", "Memoria accesada por el reader\n", reader1->id);
			//Se guarda q hay un egoista en memoria
	    	char bufferFile[1];
			sprintf(bufferFile, "%d\n",reader1->id);
			save_chain(bufferFile, "procesos_Memoria_Egoista.txt","w");

			// Para usar la memoria hay que hacerle attached
			char *buffer; /* shared buffer */
			buffer = shmat (shmid , (char *)0 , 0);

			estados[reader1->id] = 3; //en memoria
			Cargar_bloqueados();

			reader1->tiempo = timestamp();
			ReadMemory_aux(reader1, buffer);
			
			printf("%s%d\n", "LEYENDO READER",reader1->id);
			sleep(reader1->lectura);
			
			//Se borra q hay un egoista en memoria
			save_chain(" ", "procesos_Memoria_Egoista.txt","w");

			//Se desbloquea la memoria
			desbloquear_sem(sem);

			estados[reader1->id] = 2; //durmiendo
			Cargar_dormidos();

			printf("%s%d\n", "DURMIENDO READER",reader1->id);
			sleep(reader1->dormido);
		}
		else{
			estados[reader1->id] = 2; //durmiendo
			Cargar_dormidos();

			printf("No se puede entrar\n");
			printf("%s%d\n", "DURMIENDO READER",reader1->id);
			sleep(reader1->dormido);
		}
		
    }
}

void ReadMemory_aux(struct Reader *reader, char *buffer){
	if (buffer == NULL) {
		printf("Error!!!  No se puede hacer attached\n");
		exit(1);
	}
	int lineas = strlen(buffer) / MENSAJE;
	int posicion = (rand() % lineas);
	int i;
	int linea = 0;
	registrar_accion(BITACORA, reader->id, "",0, posicion);
	for (i = 0; i < strlen(buffer); i++){
		if (buffer[i] == ','){
			linea++;
		}
		else if (linea == posicion && buffer[i] != 'X'){
			registrar_accion(BITACORA, reader->id, buffer[i], 1, posicion);
			buffer[i] = 'X';
		}
	}
	registrar_accion(BITACORA, reader->id, "",2, posicion);
}


void registrar_accion(char * file_name, int id, char * registro, int cont, int linea){
    FILE *fptr;

    //Se abre con "a" para realizar un append en el archivo
    fptr = fopen(file_name, "a");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }
    if(cont ==0){
    	fprintf(fptr,"%s%d%s%d%s", "Egoista PID: ", id , " en la línea ", linea,", leyó y borró: ");
    }
    else if (cont ==1){
    	fprintf(fptr,"%c",registro);
    }
    else{
    	fprintf(fptr,"\n");
    }
    
    fclose(fptr);
}

void Creador_Readers_Egoistas(int cantidad, int lectura, int dormido){ 
	cantidadEgoistas = cantidad;
	char *shm, *s;
	sem = (sem_t *) solicitar_sem(SEM_NAME);
	semF = (sem_t *) solicitar_sem(SEM_FILE_WRITERS);

	pthread_t thread1;
	int i;
	for (i = 0; i < cantidad; i++){
		struct Reader *reader1; // = {i,0, timestamp(), escritura, dormido};
		reader1 = malloc(sizeof(Reader));
		reader1->id = i+1;
		reader1->tiempo = timestamp();
		reader1->lectura = lectura;
		reader1->dormido = dormido;
		pthread_create (&thread1, NULL, ReadMemory, (void*)reader1);
	}
	pthread_join(thread1, NULL);
}

int main(int argc, char const *argv[])
{
	int cantidad;
	int lectura;
	int dormido;
	printf("Ingrese la cantidad de readers egoistas que desea: ");
	scanf("%d",&cantidad);
	printf("Ingrese la cantidad de tiempo que tarda leyendo: ");
	scanf("%d",&lectura);
	printf("Ingrese la cantidad de tiempo que tarda durmiendo: ");
	scanf("%d",&dormido);
	Creador_Readers_Egoistas(cantidad, lectura, dormido);
	return 0;
}