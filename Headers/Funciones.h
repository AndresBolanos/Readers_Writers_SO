#ifndef SHAREDMEMORY_FUNCIONES_H
#define SHAREDMEMORY_FUNCIONES_H

#include "imports.h"

// Semaforos
void * solicitar_sem(char * nombre_sem);
bool bloquear_sem(void * sem_ref, char tipo);
void bloquear_sem_sencillo(void * sem_ref);
void desbloquear_sem(void * sem_ref);
void cerrar_sem(void * sem_ref);

//Manejo de archivos
void save_state(char sem, char * file_name); //Escribe el semaforo que esta en ese momento
char read_state(char * file_name);
void save_int(int num, char * file_name);
int read_int(char * file_name);
void save_chain(char * cadena, char * file_name, char * tipo_escritura);
int read_chain(char * file_name);


#endif 