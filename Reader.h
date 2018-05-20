#ifndef SHAREDMEMORY_READER_H
#define SHAREDMEMORY_READER_H

#include "imports.h"

struct Reader;
char* timestamp();
void ReadMemory(void * reader2);
void ReadMemory_Aux(struct Reader *reader, char *buffer);
void Creador_Readers(int cantidad, int lectura, int dormido);

#endif 