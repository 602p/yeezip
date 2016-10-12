#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include "io.h"
#include "log.h"

#define LOG_REGION "io"

MemoryMapping *MemoryMapping_open(char *path){
	LOG_DEBUG("Building MemoryMapping for `%s`\n", path);
	MemoryMapping *mapping=malloc(sizeof(MemoryMapping));

	LOG_SPAM("Opening\n");
	mapping->fd=open(path, O_RDONLY);

	if(mapping->fd==-1){
		LOG_ERROR("Couldn't open() %s\n", path);
		free(mapping);
		goto fail;
	}

	LOG_SPAM("Running stat\n");
	if (stat(path, &mapping->filestat) == -1) {
		LOG_ERROR("Couldn't stat() %s\n", path);
		free(mapping);
		goto fail;
	}

	mapping->size=mapping->filestat.st_size;
	
	LOG_SPAM("Running mmap\n");
	mapping->ptr = mmap(0, mapping->filestat.st_size, PROT_READ, MAP_SHARED, mapping->fd, 0);
	if (mapping->ptr == (void*)-1) {
		LOG_ERROR("Couldn't mmap() for %s\n", path);
		free(mapping);
		goto fail;
	}

	return mapping;

	fail:
	mapping=0;
	return mapping;
}

void MemoryMapping_close(MemoryMapping* mapping){
	munmap(mapping->ptr, mapping->size);
	free(mapping);
}

void Byte_setbit(byte *ref, int pos, bool value){
	*ref ^= (-value ^ *ref) & (1 << pos);
}

bool Byte_getbit(byte b, int pos){
	return (b >> pos) & 1;
}

BitFile *BitFile_open(char *path, bool read){
	BitFile *file=malloc(sizeof(BitFile));
	file->read=read;

	char *mode;
	if(read){
		mode="r";
		file->position=-1;
		file->read_position=-1;
	}
	else{
		mode="w";
		file->buffer=0;
		file->position=0;
	}

	file->file = fopen(path, mode);
	if((void*)file->file==0){
		LOG_ERROR("Failed to open %s as BitFile, fopen failed.\n", path);
		free(file);
		file=0;
		return file;
	}

	if(read){
		struct stat st;
		stat(path, &st);
		file->size=st.st_size;
	}

	return file;
}

void BitFile_write(BitFile *file, bool bit){
	if(file->read){
		LOG_ERROR("Tried to write to read-mode BitFile\n");
	}

	Byte_setbit(&file->buffer, file->position, bit);
	file->position++;
	if(file->position==8){
		fprintf(file->file, "%c", file->buffer);
		file->buffer=0;
		file->position=0;
	}
}

bool BitFile_readbit(BitFile *file){
	if(file->position==8 || file->position==-1){
		file->position=0;
		file->read_position++;
		file->buffer=fgetc(file->file);
	}

	bool value=Byte_getbit(file->buffer, file->position);
	file->position++;
	return value;
}

int BitFile_readint(BitFile *file, int size){
	int i=0;
	int val=0;
	while(i<size && BitFile_has_more(file)){
		val ^= (-BitFile_readbit(file) ^ val) & (1 << i);
		i++;
	}
	return val;
}

bool BitFile_has_more(BitFile *file){
	return !(file->position==8 && file->read_position==file->size-1);
}

void BitFile_close(BitFile *file){
	if(file->read==0){
		if(file->position!=0){
			int i=0;
			int req_fill=8-file->position;
			LOG_SPAM("Padding end of BitFile on BitFile_close (position=%i, req_fill=%i)\n", file->position, req_fill);
			while(i<req_fill){
				BitFile_write(file, false);
				i++;
			}
		}
	}
	fclose(file->file);
	free(file);
}

int min_bits_to_represent(int n){
	if(n<0){
		LOG_ERROR("Call to min_bits_to_represent with n<0 (n=%i)\n", n);
		return -1;
	}
	float bitwidth=1;
	while(true){
		if(n<=pow(2.0, bitwidth))
			return (int)bitwidth;
		bitwidth++;
	}
}

#undef LOG_REGION