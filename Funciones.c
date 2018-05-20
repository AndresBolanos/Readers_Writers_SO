#include "Headers/Funciones.h"
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <pthread.h>

//Tipos de semaforos E=Egoista, R = Reader, W = Writer y I = Inicializador

void * solicitar_sem(char * nombre_sem)
{
    sem_t * sem;

    printf("Solicitando semaforo...");
    sem = sem_open (nombre_sem , O_CREAT|O_EXCL , S_IRUSR|S_IWUSR , 1);

    if (!sem == SEM_FAILED)
        printf("creado...");

    else if (errno == EEXIST)
    {
        sem = sem_open (nombre_sem, 0);
        printf("abierto...");
    }
    printf("OK\n");
    return (void *) sem;
}

/*--------------------------------------------------------------------------------------------------------------------*/

bool bloquear_sem(void * sem_ref, char tipo)
{
    sem_t * sem = (sem_t *) sem_ref;
    int result = 0;

    printf("Bloqueando semaforo...");

    int egoistas = read_int(EGOISTAS);
    if (tipo == 'E'){
        
        egoistas += 1;
        //Se guarda en el archivo el id de la memoria
        save_int(egoistas,EGOISTAS);
    }
    else{
        save_int(0,EGOISTAS);
    }
    if (egoistas <= 2){
        //if (read_state("estado.txt") != 'R'){
        result = sem_wait(sem);
        if (result == -1)
        {
            printf("ERROR\n");
            exit(2);
            return false;
        }
        printf("Semaforo tipo %c\n", tipo);
        printf("OK\n");
        return true;
    }
    else{
         printf("Hay mas de dos readers egoistas seguidos\n");  
         return false; 
    }
    
}

/*--------------------------------------------------------------------------------------------------------------------*/

void desbloquear_sem(void * sem_ref)
{
    sem_t * sem = (sem_t *) sem_ref;
    int result;

    printf("Desbloqueando semaforo...");

    result = sem_post(sem);
    if (result == -1)
    {
        printf("ERROR\n");
        exit(2);
    }
    printf("OK\n");
}

/*--------------------------------------------------------------------------------------------------------------------*/

void cerrar_sem(void * sem_ref)
{
    sem_t * sem = (sem_t *) sem_ref;
    int result = 0;

    printf("Cerrando semaforo...");

    result = sem_close (sem);
    if (result == -1)
    {
        printf("ERROR\n");
        exit(2);
    }

    printf("clausurado...");

    result = sem_unlink (SEM_NAME);
    if (result == -1)
    {
        printf("ERROR\n");
        exit(2);
    }
    printf("OK\n");
}


//Estas dos funciones las tengo ahi para manejar masomenos lo de sincronizar
//Guardar el estado de cual nodo esta ocupando la memoria
//Temporal para prueba
void save_state(char sem, char * file_name)
{
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(file, "%c", sem);
    fclose(file);
}



char read_state(char * file_name)
{
    FILE *file;
    char buff;

    file = fopen(file_name, "r");
    if (file){
        buff = (char)fgetc(file);
        fclose(file);
        printf("Bffer%c\n",buff );
        return buff;
    }else{
       return 'N';
    }
}

//Guardar el id de la memoria
void save_int(int num, char * file_name)
{
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(file, "%d", num);
    fclose(file);
}

/*--------------------------------------------------------------------------------------------------------------------*/

int read_int(char * file_name)
{
    FILE *file;
    char buff[255];

    file = fopen(file_name, "r");
    if (file){
        fscanf(file, "%s", buff);
        fclose(file);

        return atoi(buff);
    }else{
       return 0;
    }
    
}