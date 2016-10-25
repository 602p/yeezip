#include <stdio.h>
#include "log.h"
#include "args.h"

#define LOG_REGION "log"

int loglevel=STATUS;

void *malloc_hook(size_t size, char *file, int line){
	if(loglevel<=1){
		LOG_ALLOC("malloc-ing %i bytes at %s:%i\n",size,file,line);
	}
	return MALLOC_NOLOG(size);
}

#undef LOG_REGION