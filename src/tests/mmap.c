#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include "io.h"

void test_mmap(){
	MemoryMapping *fmap=MemoryMapping_open("build.py");

	FILE *f = fopen("out.txt", "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	int i=0;
	char c;
	while(i<fmap->size){
		c=((char*)fmap->ptr)[i];
		if(c=='#'){
			fprintf(f, "#-");
		}else{
			fprintf(f, "%c", c);
		}
		++i;
	}

	fclose(f);
	MemoryMapping_close(fmap);
}