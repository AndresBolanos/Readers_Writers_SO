#include "Headers/Reader.h"
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

sem_t * semF = NULL; 	//Definimos el semaforo

int cantidadReaders =0;
char *buffer; // shared buffer

int totalReaders;
int estados[100]; 	//Arreglo con el estado el reader. 3 memoria, 2 dormido, 1 bloqueado, 0 desbloqueado.


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

void Cargar_bloqueados(){
	FILE *fptr;
	bloquear_sem_sencillo(semF);
    fptr = fopen(BLOQ_READERS, "w");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }

    int i;
    int flag = 0;
    for (i = 0; i <= totalReaders; i++){
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
    fptr = fopen(DORM_READERS, "w");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }

    int i;
    int flag = 0;
    for (i = 0; i <= totalReaders; i++){
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

bool writer_bloqueados(char * nombre_archivo){
	bool respuesta = true;
	bloquear_sem_sencillo(semF);
	FILE *file;
    char buff;
    file = fopen(nombre_archivo, "r");
    if (file){
        buff = (char)fgetc(file);
        fclose(file);
        if (buff == '*'){
        	respuesta = false;
        }
    }
	desbloquear_sem(semF);
	return respuesta;
}


void ReadMemory(void * reader2){

	struct Reader * reader1 = (struct Reader*) reader2;
	
	//Se lee la memoria del archivo de texto
	int key = read_int(ID_MEM_FILE);

	//Se pide la memoria
	int shmid  = shmget (key,MENSAJE, 0777);

	if (shmid  == -1) {
		printf("Error!!!  creando la memoria compartida \n");
		exit(1);
	}

    while (true){
    	 
    	//true si hay bloqueados, false lo contrario
    	bool respuesta = writer_bloqueados("procesos_Bloqueados_Writer.txt");
    	bool respuesta2 = writer_bloqueados("procesos_Bloqueados_Egoista.txt");
    	
    	//Bloqueo la memoria
    	bool resp = false;

    	if ((cantidadReaders ==0) || (respuesta || respuesta2)){
    		estados[reader1->id] = 1; //bloqueado
    		Cargar_dormidos();
    		Cargar_bloqueados();
    		resp = bloquear_sem(sem,'R');
    		buffer = shmat (shmid , (char *)0 , 0);
    	}

    	while(buffer == NULL){

    	}

    	cantidadReaders= cantidadReaders+1;
    	estados[reader1->id] = 3; //memoria
    	Cargar_dormidos();
    	Cargar_bloqueados();

		//Guarda en el archivo "procesos_Memoria_Writer.txt" el id del proceso que esta en memoria
		if (cantidadReaders > 1){
			//Hace append
			char bufferFile[1];
			sprintf(bufferFile, "%d\n",reader1->id);
			save_chain(bufferFile, MEM_READERS,"a");
		}
		else{
			//Sobre escribe
			char bufferFile[1];
			sprintf(bufferFile, "%d\n",reader1->id);
			save_chain(bufferFile, MEM_READERS,"w");
		}
		//Hace un append al archivo
		
		reader1->tiempo = timestamp();
		ReadMemory_Aux(reader1, buffer);
		printf("%s%d\n", "LEYENDO READER ",reader1->id);
		sleep(reader1->lectura);
		
		if(cantidadReaders ==1){
			//Se desbloquea la memoria
			desbloquear_sem(sem);
		}
		//Aqui hay q borrar del archivo un reader
		save_chain_Delete(MEM_READERS,cantidadReaders,reader1->id);
		cantidadReaders = cantidadReaders-1;

	}
	
	estados[reader1->id] = 2; //dormido
	Cargar_dormidos();
	Cargar_bloqueados();

	printf("%s%d\n", "DURMIENDO READER ",reader1->id);
	sleep(reader1->dormido);
}

void ReadMemory_Aux(struct Reader *reader, char *buffer){
	if (buffer == NULL) {
		printf("Error!!!  No se puede hacer attached\n");
		exit(1);
	}

	int i;
	int linea = 0;
	int primerChar =0;
	int lineas = strlen(buffer) / MENSAJE;
	char lineaMsj[34];
	int contadorMsj =0;
	for (i = 0; i < strlen(buffer); i++){
		if (buffer[i] == ','){
			if (buffer[i-1] != 'X' && primerChar != 0){
				printf("\n");
				registrar_accion(BITACORA, reader->id, lineaMsj, linea);
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
						registrar_accion(BITACORA, reader->id, lineaMsj, linea);
						lineaMsj[contadorMsj] = buffer[i];
						contadorMsj= contadorMsj +1;

						reader->linea=0;
						break;
					}
					if (primerChar == 0){
						primerChar = 1;
					}
					printf("%c", buffer[i]);
					lineaMsj[contadorMsj] = buffer[i];
					contadorMsj= contadorMsj +1;
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

void registrar_accion(char * file_name, int id, char * registro, int linea){
    FILE *fptr;

    //Se abre con "a" para realizar un append en el archivo
    fptr = fopen(file_name, "a");
    if(fptr == NULL)
    {
        printf("Error opening file!");
        exit(1);
    }
    fprintf(fptr,"%s%d%s%d%s%s\n", "Reader PID: ", id , " en la línea ", linea,", leyó: ", registro);
    fclose(fptr);
}

void Creador_Readers(int cantidad, int lectura, int dormido){
	totalReaders=cantidad;
	char *shm, *s;
	//Se crea el semaforo
	sem = (sem_t *) solicitar_sem(SEM_NAME);
	semF = (sem_t *) solicitar_sem(SEM_FILE_WRITERS);
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
		sleep(10);

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