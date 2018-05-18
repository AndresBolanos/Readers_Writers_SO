#ifndef SHAREDMEMORY_READER_H
#define SHAREDMEMORY_READER_H

#include "imports.h"

struct Reader;
char* timestamp();
void ReadMemory(int memory_key, int lines_memory);
void Creador_Readers(int cantidad, int lectura, int dormido);

#endif 