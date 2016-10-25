#ifndef __COMPRESS_H__
#define __COMPRESS_H__

#include "main.h"
#include "codingtree.h"
#include "io.h"

//Struct composing the compression lookup table.
//Represents the path thru the tree for any given character (the steps (traversals) and widths
// (bit widths) of those steps)
typedef struct LookupTableElement_struct{
	int len;     //Nr. of items in values and widths. If len=0 then the char cannot be represented
	int *values; //Array of length len of ints representing the ordered traversals required
	             //  to get from the root node to the character for this element
	int *widths; //Array of length len of ints representing the number of bits required to represent
	             //  the traversal specified at the same position in values.
	             //  In other terms, widths[x]=min_bits_to_represent({{node we are traversing in
	             //  values[x]}}->children)
} LookupTableElement;

enum {
	HF_NONE   = 0, //Default. Tree follows
	HF_NOTREE = 1, //Treedata not included
	HF_FLAT   = 2, //Tree used was a flat (i.e. no-op) tree
	HF_PTREE  = 4  //Tree used can be procedurally generated from the contents of the header
	               // (name of algorithm and some amount of algorithm-supplied data)
	               // (this dosen't work/isn't used)
};

typedef struct HeaderInfo_struct{
	unsigned int  size;           //File size in bytes (i.e. what you should end up with)
	byte version;                 //Protocol version (0)
	byte flags;                   //Bitfield for some flags. See enum above
	TreeNode *tree;               //The actual tree stored (garbage if HF_NOTREE set)
	int _tree_node_count;         //Count of nodes in the tree
	char *ptree_name;             // (unused) algorithm to send ptree data to
	unsigned int  ptree_data_size;// (unused) size ofptree data
	byte *ptree_data;             // (unused) ptree data
} HeaderInfo;

typedef LookupTableElement LookupTable[256];
//Typedef for the lookup table
//When we want to compress, we don't really care about the position of the character we want to
//write in the tree, we only care about the path to it (because this is what we write.) Therefore
//we build this cache mapping of character (array index) -> [steps...] and [step widths...]
//so that we can lookup into this and write to our output BitFile appropriatley.

LookupTable *create_table(TreeNode *root);
//Create and populate a new lookuptable from the tree where <root> is the root node
LookupTable *create_table_d(TreeNode *root, int depth);
//Same as above, but specify the <depth> of the search stack construct
void free_table(LookupTable *table);
//Free the lookuptable and the associated linked data

void compress_file(FILE *in, BitFile *out, LookupTable *table, int size);
//Compress size bytes of data from in to out, using the lookuptable table
//Contents out the output file will be padded to the next byte for storage in the FS once done
void decompress_file(BitFile *in, FILE *out, TreeNode *tree, int size);
//Decompress size bytes of (decompressed) data from in to out, using tree tree
// NOTE: size referes to the number of bytes once decoded, therefore generally len(in)<size
//       (although this isnt always true.)
// Size is required here because compress_file always pads out to the next byte (otherwise you
//  can't stick it in the FS)

void LookupTable_print(LookupTable *table);
//Dump out a lookup table

HeaderInfo *HeaderInfo_create_notree(unsigned int fsize);
//Create a headerinfo without a tree in it (i.e HF_NOTREE set)
HeaderInfo *HeaderInfo_create_tree(unsigned int fsize, TreeNode *tree);
//Create a headerinfo holding the tree tree
HeaderInfo *HeaderInfo_create_ptree(unsigned int fsize, char *algname, int data_size, void *data);
//[UNSUPPORTED]
void HeaderInfo_destroy(HeaderInfo *header);
//Free a headerinfo (but not the associated tree or ptree data)

byte *HeaderInfo_save(HeaderInfo *header, int *size_out);
//Save the header (incl tree/ptree data or whatever) to a buffer and return that. Int pointed to by
// size_out will be set to the size of said buffer

int HeaderInfo_get_base_size();
//Get the size of the base (version, flags, size) header for use with HeaderInfo_load_base
HeaderInfo *HeaderInfo_load_base(byte *buffer);
//Load just the base (version, flags, size) of a header from the buffer buffer
//Should be called with a buffer of at least size HeaderInfo_get_base_size()

//UNIMPLEMENTED:
int HeaderInfo_load_ptree_size(HeaderInfo *hdr, byte *buffer);
void HeaderInfo_load_ptree(HeaderInfo *hdr, byte *buffer);
//END UNIMPLEMENTED

int HeaderInfo_load_tree_size(HeaderInfo *hdr, byte *buffer);
//Get the size of the tree data stored in buffer (buffer should be the 4 bytes immeadetly after the
// base header)
void HeaderInfo_load_tree(HeaderInfo *hdr, byte *buffer);
//Load the tree stored in buffer (where buffer is HeaderInfo_load_tree_size bytes following the
// 4-byte int following the base header)

HeaderInfo *HeaderInfo_load_fd(FILE *fd);
//Load a complete header (including tree) from the file fd, advancing the read cursor to the
// first byte of actual compressed content
HeaderInfo *HeaderInfo_load_file(char *path);
//Load a complete header (including tree) from the file at the path path then close it

int get_compressed_size(LookupTable *ltbl, freqtable *ftbl);
//Get the size (in bits) to compress the file represented by ftbl using the tree represented by ltbl

#endif