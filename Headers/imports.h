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
#include "Espia.h"

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
#define SEM_EGOISTAS    "/Egoistas"
#define SEM_FILE_WRITERS    "/Egoistas"
#define ID_MEM_FILE     "id_memory.txt"
#define EGOISTAS       	"egoistas.txt"
#define BITACORA       	"bitacora.txt"
#define MEM_EGOISTAS    "procesos_Memoria_Egoista.txt"
#define MEM_READERS     "procesos_Memoria_Reader.txt"
#define MEM_WRITERS     "procesos_Memoria_Writer.txt"
#define DORM_WRITERS    "procesos_Dormidos_Writer.txt"
#define BLOQ_WRITERS    "procesos_Bloqueados_Writer.txt"
#define BLOQ_READERS    "procesos_Bloqueados_Reader.txt"
#define DORM_READERS    "procesos_Dormidos_Reader.txt"
#define DORM_EGOISTA    "procesos_Dormidos_Egoista.txt"
#define BLOQ_EGOISTA    "procesos_Bloqueados_Egoista.txt"




#endif
