#include <stdlib.h>
#include <stdio.h>
#include "io.h"

#define I 1,
#define O 0,

bool test_bitmanipulation_array[]={
	O O O O O O O O
	I O O O O O O O
	O I O O O O O O
	O O I I I I O O
	O O O I I O O O
	O O I I I O O O 
	O O O I I I O O 
	O O O O O O O I
	O I O I O I
};

#undef I
#undef O

void test_bitmanipulation(){
	BitFile *f=BitFile_open("out.txt", false);

	int i=0;
	int sz=sizeof(test_bitmanipulation_array)/sizeof(test_bitmanipulation_array[0]);
	while(i<sz){
		BitFile_write(f, test_bitmanipulation_array[i]);
		i++;
	}

	printf("BTR 16: %i\n", min_bits_to_represent(16));
	printf("BTR 17: %i\n", min_bits_to_represent(17));
	printf("BTR 15: %i\n", min_bits_to_represent(15));
	printf("BTR 4: %i\n", min_bits_to_represent(4));
	printf("BTR 2: %i\n", min_bits_to_represent(2));
	printf("BTR 12: %i\n", min_bits_to_represent(12));
	printf("BTR 123123123: %i\n", min_bits_to_represent(123123123));

	BitFile_close(f);

	printf("Testing reading...\n");

	f=BitFile_open("out.txt", true);
	i=0;

	printf("Starting loop...\n");

	while(BitFile_has_more(f)){
		if(BitFile_readbit(f)){
			printf("1");
		}else{
			printf("0");
		}
		i++;
		if(i==8){
			printf("\n");
			i=0;
		}
	}

	printf("Closing\n");

	BitFile_close(f);
}