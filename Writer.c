#include <stdio.h>
#include <sys/ipc.h>
#include  <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

#define KEY 1090584602
#define MENSAJE 34

struct Writer
{
  int id;
  char *tiempo;
  int escribe;
  int dormido;
};

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

void Write_line(struct Writer *writer){
	char *shm, *s;

	int shmid  = shmget (KEY,MENSAJE, 0777);
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
	char linea[34];
	Create_Buffer(writer, 10, linea);
	printf("Memoria lista para escribir\n");
	//Limpio el buffer de la memoria
	int i;
	int cont = 0;
	bool flag = true;
	int num_linea = 0;
	for (i = 0; i < strlen (buffer); i++){
		if (buffer[i] == 'X' && cont < strlen(linea)){
			if (flag){
				flag = false;
				Create_Buffer(writer, num_linea, linea);
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
	for (i = 0; i < strlen (buffer); i++){
		printf("segment contains: \"%c\"\n", buffer[i]);
	};
}

void Creador_Writers(int cantidad, int escritura, int dormido){
	for (int i = 0; i < cantidad; i++){
		pthread_t thread1;
		struct Writer writer1; // = {i,0, timestamp(), escritura, dormido};
		writer1.id = i;
		writer1.tiempo = timestamp();
		writer1.escribe = escritura;
		writer1.dormido = dormido;
		printf("%d\n",i);
		pthread_create (&thread1, NULL, (void*)Write_line, (void*)&writer1);

		pthread_join(thread1, NULL);
	}
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