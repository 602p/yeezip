#ifndef __COMPRESS_H__
#define __COMPRESS_H__

#include "main.h"
#include "codingtree.h"
#include "io.h"

typedef struct LookupTableElement_struct{
	int len;
	int *values;
	int *widths;
} LookupTableElement;

enum {
	HF_NONE   = 0, //Default. Tree follows
	HF_NOTREE = 1, //Treedata not included
	HF_FLAT   = 2, //Tree used was a flat (i.e. no-op) tree
	HF_PTREE  = 4  //Tree used can be procedurally generated from the contents of the following
};

typedef struct HeaderInfo_struct{
	unsigned int  size;
	byte version;
	byte flags;
	TreeNode *tree;
	int _tree_node_count;
	char *ptree_name;
	unsigned int  ptree_data_size;
	byte *ptree_data;
} HeaderInfo;

typedef LookupTableElement LookupTable[256];

LookupTable *create_table(TreeNode *root);
void free_table(LookupTable *table);
void compress_file(FILE *in, BitFile *out, LookupTable *table, int size);
void decompress_file(BitFile *in, FILE *out, TreeNode *tree, int size);

void LookupTable_print(LookupTable *table);

HeaderInfo *HeaderInfo_create_notree(unsigned int fsize);
HeaderInfo *HeaderInfo_create_tree(unsigned int fsize, TreeNode *tree);
HeaderInfo *HeaderInfo_create_ptree(unsigned int fsize, char *algname, int data_size, void *data);
void HeaderInfo_destroy(HeaderInfo *header);

byte *HeaderInfo_save(HeaderInfo *header, int *size_out);
HeaderInfo *HeaderInfo_load_base(byte *buffer);
int HeaderInfo_load_ptree_size(HeaderInfo *hdr, byte *buffer);
void HeaderInfo_load_ptree(HeaderInfo *hdr, byte *buffer);
int HeaderInfo_load_tree_size(HeaderInfo *hdr, byte *buffer);
void HeaderInfo_load_tree(HeaderInfo *hdr, byte *buffer);

HeaderInfo *HeaderInfo_load_fd(FILE *fd);
HeaderInfo *HeaderInfo_load_file(char *path);

#endif