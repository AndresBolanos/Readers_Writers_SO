#ifndef SHAREDMEMORY_WRITER_H
#define SHAREDMEMORY_WRITER_H

#include "imports.h"

struct Writer;
char* timestamp();
char *Create_Buffer(struct Writer *writer, int linea, char *buffer);
void Write_line(struct Writer *writer);
void Creador_Writers(int cantidad, int escritura, int dormido);

#endif 
