#ifndef __IO_H__
#define __IO_H__

#include <sys/stat.h>
#include <stdio.h>
#include "main.h"

typedef struct MemoryMapping_struct{
	void *ptr;
	int size;
	struct stat filestat;
	int fd;
}MemoryMapping;

typedef struct BitFile_struct{
	FILE *file;
	bool read;
	int size;

	int position; //position reading/writing within buffer (bits)
	int read_position;
	byte buffer;
}BitFile;

MemoryMapping *MemoryMapping_open(char *path);
void MemoryMapping_close(MemoryMapping* mapping);

void Byte_setbit(byte *ref, int pos, bool value);
bool Byte_getbit(byte b, int pos);

BitFile *BitFile_open(char *path, bool read);
void BitFile_close(BitFile *file);
void BitFile_write(BitFile *file, bool bit);
bool BitFile_readbit(BitFile *file);
bool BitFile_has_more(BitFile *file);
int BitFile_readint(BitFile *file, int size);
void BitFile_writeint(BitFile *file, int value, int size);

typedef int freqtable[256];

freqtable *FreqTable_create(FILE* file);

int min_bits_to_represent(int n);

#endif