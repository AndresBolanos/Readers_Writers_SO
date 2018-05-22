#ifndef SHAREDMEMORY_READEREGOISTA_H
#define SHAREDMEMORY_READEREGOISTA_H

#include "imports.h"

struct Reader;
char* timestamp();char *Create_Buffer(struct Writer *writer, int linea, char *buffer);
void ReadMemory(void * reader2);
void Creador_Readers_Egoistas(int cantidad, int lectura, int dormido);

#endif 