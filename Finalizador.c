
#include "Headers/imports.h"

#define KEY 1091108217 //Deberia guardarse en un archivo

//Compile the program 
//gcc -w Finalizador.c Funciones.c  -o Finalizador -pthread
//./Finalizador


void finalizar()
{
    int shm_id = KEY;

    /*Eliminacion de los procesos del productor*/
    system("killall Writer");
    printf("\nProcesos Writer eliminados\n");

    /*Eliminacion de los procesos del productor*/
    system("killall Reader");
    printf("\nProcesos Reader eliminados\n");

    /*Eliminacion de los procesos del productor*/
    system("killall ReaderEgoista");
    printf("\nProcesos Egoista eliminados\n");

    /*Eliminacion de la memoria compartida simulada*/
    if (shmctl(shm_id, IPC_RMID, 0) != -1)
        printf("\nMemoria compartida liberada\n");

    /*Eliminacion del semaforo de la memoria simulada*/
    if (sem_unlink(SEM_NAME) == 0)
        printf("\nSemaforo de memoria compartido eliminado\n");

    /*Eliminacion del semaforo de los egoistas*/
    if (sem_unlink(SEM_EGOISTAS) == 0)
        printf("\nSemaforo de memoria compartido eliminado\n");

    /*Eliminacion del semaforo de los egoistas*/
    if (sem_unlink(SEM_FILE_WRITERS) == 0)
        printf("\nSemaforo de memoria compartido eliminado\n");

}


int main(int argc, char const *argv[])
{
	finalizar();
	return 0;
}