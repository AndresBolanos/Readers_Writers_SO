#ifndef SHAREDMEMORY_FUNCIONES_H
#define SHAREDMEMORY_FUNCIONES_H

#include "imports.h"

// Semaforos
void * solicitar_sem(char * nombre_sem);
void bloquear_sem(void * sem_ref);
void desbloquear_sem(void * sem_ref);
void cerrar_sem(void * sem_ref);


#endif //SHAREDMEMORY_HERRAMIENTAS_H