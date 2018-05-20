#include "Headers/Reader.h"
#include <stdio.h>
#include <sys/ipc.h>
#include  <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define KEY 1091108217
#define MENSAJE 34

sem_t * sem = NULL; 	//Definimos el semaforo

//Compile the code
//gcc -w Reader.c Funciones.c  -o Reader -pthread
//./Reader

struct Reader
{
  int id;
  char *tiempo;
  int dormido; 
  int lectura;
  int linea;
} Reader;

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

    printf("Memoria accesada\n");

    while (true){
    	//Bloqueo la memoria
    	bloquear_sem(sem);
    	char *buffer; // shared buffer 
		buffer = shmat (shmid , (char *)0 , 0);
		reader1->tiempo = timestamp();
		ReadMemory_Aux(reader1, buffer);
		printf("%s%d\n", "LEYENDO ",reader1->id);
		sleep(reader1->lectura);
		
		//Se desbloquea la memoria
		desbloquear_sem(sem);
	
		printf("%s%d\n", "DURMIENDO ",reader1->id);
		sleep(reader1->dormido);
    }
	//save_state('R', "estado.txt"); //Se guarda en un archivo que un semaforo esta en memori

    // Para usar la memoria hay que hacerle attached
	
	//Se desbloquea la memoria
}

void ReadMemory_Aux(struct Reader *reader, char *buffer){
	if (buffer == NULL) {
		printf("Error!!!  No se puede hacer attached\n");
		exit(1);
	}

	printf("Memoria lista para leer\n");
	int i;
	int linea = 0;
	int primerChar =0;
	int lineas = strlen(buffer) / MENSAJE;
	for (i = 0; i < strlen(buffer); i++){
		if (buffer[i] == ','){
			if (buffer[i-1] != 'X' && primerChar != 0){
				printf("\n");
				registrar_accion("bitacora.txt", reader->id, buffer[i], 2, linea);
				primerChar =0;
				reader->linea = linea+1;
				if(reader->linea == lineas){
					reader->linea=0;
				}
				break;
			}
			linea++;
		}
		else{
			if (reader->linea == linea){
				if (buffer[i] != 'X'){
					if (i+1 == strlen(buffer)){
						printf("%s\n", "YES");
						registrar_accion("bitacora.txt", reader->id, buffer[i], 1, linea);
						registrar_accion("bitacora.txt", reader->id, buffer[i], 2, linea);
						reader->linea=0;
						break;
					}
					if (primerChar == 0){
						primerChar = 1;
						registrar_accion("bitacora.txt", reader->id, "",0, linea);
					}
					registrar_accion("bitacora.txt", reader->id, buffer[i], 1, linea);
					printf("%c", buffer[i]);
				}
				else{
					reader->linea = linea+1;
					if(reader->linea == lineas){
					reader->linea=0;
				}
				}
			}
		}
	}
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
    	fprintf(fptr,"%s%d%s%d%s", "Reader PID: ", id , " en la línea ", linea,", leyó: ");
    }
    else if (cont ==1){
    	fprintf(fptr,"%c",registro);
    }
    else{
    	fprintf(fptr,"\n");
    }
    
    fclose(fptr);
}

void Creador_Readers(int cantidad, int lectura, int dormido){
	char *shm, *s;
	//Se crea el semaforo
	sem = (sem_t *) solicitar_sem(SEM_NAME);
	pthread_t thread1;
	int i;
	for (i = 0; i < cantidad; i++){
		
		struct Reader *reader1; // = {i,0, timestamp(), escritura, dormido};
		reader1 = malloc(sizeof(Reader));
		reader1->id = i;
		printf("%d\n", reader1->id);
		reader1->tiempo = timestamp();
		reader1->lectura = lectura;
		reader1->dormido = dormido;
		reader1->linea=0;
		pthread_create (&thread1, NULL, ReadMemory, (void*)reader1);

	}
	pthread_join(thread1, NULL);
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