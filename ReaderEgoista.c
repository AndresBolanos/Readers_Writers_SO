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

#define KEY 1091108217
#define MENSAJE 34

sem_t * sem = NULL; 	//Definimos el semaforo

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

void ReadMemory(void * reader2){
	struct Reader * reader1 = (struct Reader*) reader2;
	int shmid  = shmget (KEY,MENSAJE, 0777);
	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}
	printf("%s%d\n", "Memoria accesada ", reader1->id);

    while(true){
    	//Bloqueo la memoria
		bloquear_sem(sem);
		// Para usar la memoria hay que hacerle attached
		char *buffer; /* shared buffer */
		buffer = shmat (shmid , (char *)0 , 0);

		reader1->tiempo = timestamp();
		ReadMemory_aux(reader1, buffer);
		
		printf("%s%d\n", "LEYENDO ",reader1->id);
		sleep(reader1->lectura);
		
		//Se desbloquea la memoria
		desbloquear_sem(sem);
	
		printf("%s%d\n", "DURMIENDO ",reader1->id);
		sleep(reader1->dormido);
    }
}

void ReadMemory_aux(struct Reader *reader, char *buffer){
	if (buffer == NULL) {
		printf("Error!!!  No se puede hacer attached\n");
		exit(1);
	}
	printf("Memoria lista para leer\n");
	int lineas = strlen(buffer) / MENSAJE;
	int posicion = (rand() % lineas);
	printf("%d\n", posicion);
	int i;
	int linea = 0;
	registrar_accion("bitacora.txt", reader->id, "",0, posicion);
	for (i = 0; i < strlen(buffer); i++){
		if (buffer[i] == ','){
			linea++;
		}
		else if (linea == posicion && buffer[i] != 'X'){
			registrar_accion("bitacora.txt", reader->id, buffer[i], 1, posicion);
			buffer[i] = 'X';
		}
	}
	registrar_accion("bitacora.txt", reader->id, "",2, posicion);
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
	char *shm, *s;
	//Se crea el semaforo
	sem = (sem_t *) solicitar_sem(SEM_NAME);
	pthread_t thread1;
	int i;
	for (i = 0; i < cantidad; i++){
		struct Reader *reader1; // = {i,0, timestamp(), escritura, dormido};
		reader1 = malloc(sizeof(Reader));
		reader1->id = i;
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