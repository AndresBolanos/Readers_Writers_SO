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

    printf("Bloqueando semaforo...\n");

    //Bloqueo cuando escribo con semaforo
    char *shm_egoista;

    //Se crea el semaforo para el archivo
    shm_egoista = (sem_t *) solicitar_sem(SEM_EGOISTAS);

    //Se bloquea el semaforo del archivo
    bloquear_sem_sencillo(shm_egoista);
    int egoistas = read_int(EGOISTAS);
    if (tipo == 'E'){
        egoistas += 1;
        //Se guarda en el archivo el id de la memoria
        save_int(egoistas,EGOISTAS);
    }
    else{
        egoistas = 0;
        save_int(0,EGOISTAS);
    }
    //Se desbloquea el semaforo de entrada al archivo
    desbloquear_sem(shm_egoista);

    if (egoistas <= 2){
        //if (read_state("estado.txt") != 'R'){
        result = sem_wait(sem);
        if (result == -1)
        {
            printf("ERROR\n");
            exit(2);
            return false;
        }
        printf("Semaforo tipo %c bloqueado\n", tipo);
        return true;
    }
    else{
         printf("Hay mas de dos readers egoistas seguidos\n");  
         return false; 
    }    
}

//Semaforo sencillo, no verfica, solo bloquea
void bloquear_sem_sencillo(void * sem_ref)
{
    sem_t * sem = (sem_t *) sem_ref;
    int result = 0;
    //printf("Bloqueando semaforo...");

    result = sem_wait(sem);
        if (result == -1)
    {
        //printf("ERROR\n");
        exit(2);
    }
    //printf("OK\n");
}

/*--------------------------------------------------------------------------------------------------------------------*/
//Desbloquea un semafoto
void desbloquear_sem(void * sem_ref)
{
    sem_t * sem = (sem_t *) sem_ref;
    int result;

    printf("Desbloqueando semaforo...\n");

    result = sem_post(sem);
    if (result == -1)
    {
        printf("ERROR\n");
        exit(2);
    }
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

//Guarda un entero
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
//Lee un entero
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

//Guarda una cadena de caracteres
void save_chain(char * cadena, char * file_name, char * tipo_escritura)
{
    FILE *file = fopen(file_name, tipo_escritura);
    if (file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fscanf(file, "This is testing for fprintf...\n");
    fprintf(file, "%s", cadena);
    fclose(file);
}

/*--------------------------------------------------------------------------------------------------------------------*/
//Lee una cadena de caracteres
int read_chain(char * file_name)
{
    FILE *file;
    char buff[255];

    file = fopen(file_name, "r");
    if (file){
        fscanf(file, "%s", buff);
        fclose(file);

        return buff;
    }else{
       return "";
    }
    
}