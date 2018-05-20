#ifndef SHAREDMEMORY_BIBLIOTECAS_H
#define SHAREDMEMORY_BIBLIOTECAS_H


// Generales
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>

// Otros headers
#include "Funciones.h"
#include "Inicializador.h"
#include "Writer.h"

#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

// Memoria Compartida
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Semaforos
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <fcntl.h>

//Variables globales
#define SEM_NAME        "/Shared_memori"
#define ID_MEM_FILE     "id_memory.txt"
#define EGOISTAS       	"egoistas.txt"


#endif
