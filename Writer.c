#include "Headers/Writer.h"
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

int cantidadWriters;

int estados[100]; 	//Arreglo con el estado el writer. 3 memoria, 2 dormido, 1 bloqueado, 0 desbloqueado.

//Compile project
//gcc -w Writer.c -o Writer -pthread
//./Writer



struct Writer
{
  int id;
  char *tiempo;
  int escribe;
  int dormido;
} Writer;

char* timestamp(){
  time_t ltime; /* tiempo calendario */
  ltime = time(NULL); /* obtener tiempo actual */
  return asctime(localtime(&ltime));
}

char *Create_Buffer(struct Writer *writer, int linea, char *buffer){
	writer->tiempo[strlen(writer->tiempo)-1]=0;
	sprintf(buffer, "%d-%s-%d",writer->id, writer->tiempo, linea);
	return buffer;
}

void Cargar_bloqueados(){
	FILE *fptr;
	bloquear_sem_sencillo(semF);
    fptr = fopen("procesos_Bloqueados_Writer.txt", "w");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }

    int i;
    int flag = 0;
    for (i = 0; i <= cantidadWriters; i++){
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
    fptr = fopen("procesos_Dormidos_Writer.txt", "w");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }

    int i;
    int flag = 0;
    for (i = 0; i <= cantidadWriters; i++){
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

void Write_line(void * writer2){
	struct Writer * writer1 = (struct Writer*) writer2;
	
	//Se lee la memoria del archivo de texto
	int key = read_int(ID_MEM_FILE);
	//Se pide la memoria
	int shmid  = shmget (key,MENSAJE, 0777);
	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}

	while(true){
	    //Bloqueo la memoria
		estados[writer1->id] = 1;
		Cargar_bloqueados();
		Cargar_dormidos();
		bool resp = bloquear_sem(sem, 'W');
		if (resp){
			//Guarda en el archivo "procesos_Memoria_Writer.txt" el id del proceso que esta en memoria
			char bufferWriter[1];
			sprintf(bufferWriter, "%d\n",writer1->id);
			save_chain(bufferWriter, MEM_WRITERS, "w");

			// Para usar la memoria hay que hacerle attached
			char *buffer; /* shared buffer */
			buffer = shmat (shmid , (char *)0 , 0);
			estados[writer1->id] = 3;
			Cargar_bloqueados();
			writer1->tiempo = timestamp();
			Write_line_aux(writer1, buffer);
			printf("%s%d\n", "ESCRIBIENDO WRITER ",writer1->id);
			sleep(writer1->escribe);
			
			//Se desbloquea la memoria
			desbloquear_sem(sem);
			
			//Guarda en el archivo "procesos_Memoria_Writer.txt" que salio el proceso
			save_chain(" ", MEM_WRITERS, "w");

			//Duerme el thread
			estados[writer1->id] = 2;
			Cargar_dormidos();
			printf("%s%d\n", "DURMIENDO WRITER ",writer1->id);
			sleep(writer1->dormido);
			estados[writer1->id] = 0;
			//Cargar_bloqueados();
		}

		else{
			estados[writer1->id] = 2;
			Cargar_bloqueados();
			Cargar_dormidos();
			printf("No se puede entrar\n");
			printf("%s%d\n", "DURMIENDO WRITER ",writer1->id);
			sleep(writer1->dormido);
			estados[writer1->id] = 0;
		}
	}
}

void Write_line_aux(struct Writer *writer, char *buffer){
	if (buffer == NULL) {
		printf("Error!!!  No se puede hacer attached\n");
		exit(1);
	}
	char linea[34];
	Create_Buffer(writer, 10, linea);
	
	//Se escribe en memoria
	int i;
	int cont = 0;
	bool flag = true;
	int num_linea = 0;
	for (i = 0; i < strlen (buffer); i++){
		if (buffer[i] == 'X' && cont < strlen(linea)){
			if (flag){
				flag = false;
				Create_Buffer(writer, num_linea, linea);
				registrar_accion(BITACORA, writer->id, linea);
			}
			buffer[i] = linea[cont];
			cont++;
		}
		else if (cont > 0){
			if (buffer[i]==','){
				break;
			}
			buffer[i]='*';
			cont++;
		}
		else if (buffer[i] == ','){
			num_linea++;
		}
	}
}

void registrar_accion(char * file_name, int id, char * registro){
    FILE *fptr;

    //Se abre con "a" para realizar un append en el archivo
    fptr = fopen(file_name, "a");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }

    fprintf(fptr,"%s%d%s%s\n", "Writer PID: ", id ,", escribió: ", registro);
    fclose(fptr);
}

void Creador_Writers(int cantidad, int escritura, int dormido){
	cantidadWriters = cantidad;
	char *shm, *s;
	//Se crea el semaforo
	sem = (sem_t *) solicitar_sem(SEM_NAME);
	semF = (sem_t *) solicitar_sem(SEM_FILE_WRITERS);
	pthread_t thread1;
	int i;
	for (i = 0; i < cantidad; i++){
		estados[i] = 0;
		struct Writer *writer1;
		writer1 = malloc(sizeof(Writer));
		writer1->id = i+1;
		writer1->tiempo = timestamp();
		writer1->escribe = escritura;
		writer1->dormido = dormido;
		printf("%d\n",i);
		pthread_create(&thread1, NULL, Write_line, (void*)writer1);
	}

	pthread_join(thread1, NULL);
}

int main(int argc, char const *argv[])
{
	int cantidad;
	int escritura;
	int dormido;
	printf("Ingrese la cantidad de writers que desea: ");
	scanf("%d",&cantidad);
	printf("Ingrese la cantidad de tiempo que tarda escribiendo: ");
	scanf("%d",&escritura);
	printf("Ingrese la cantidad de tiempo que tarda durmiendo: ");
	scanf("%d",&dormido);
	Creador_Writers(cantidad, escritura, dormido);

	return 0;
}
