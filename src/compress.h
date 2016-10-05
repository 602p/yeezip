#ifndef __COMPRESS_H__
#define __COMPRESS_H__

#include "main.h"
#include "codingtree.h"
#include "io.h"

struct LookupTableElement_struct{
	int len;
	int *values;
	int *widths;
} LookupTableElement;

typedef struct LookupTableElement_struct LookupTable[256];

LookupTable *create_table(TreeNode *root);
void free_table(LookupTable *table);
void compress_file(MemoryMapping *in, BitFile *out);

void LookupTable_print(LookupTable *table);

#endif