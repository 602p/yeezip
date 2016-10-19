#include "compress.h"
#include "log.h"
#include "map.h"
#include "args.h"
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "codingtree.h"

#define LOG_REGION "cmprs"

void LookupTable_print(LookupTable *table){

	LOG_INFO("===LookupTable_print starts===\n");

	int omissionstart=-1;
	int i=0;
	int j=0;
	int wsum=0;
	printf("table[xxx]\tINT\tHEX\tASCII\tPLEN\tWIDTH\tPOSITION\n");
	while(i<256){
		if((*table)[i].len!=0){
			if(omissionstart!=-1){
				printf("\t\t\t\t\t\t\t\t\t\t\tOmitted %i blank rows %i-%i\n", i-omissionstart, omissionstart, i);
				omissionstart=-1;
			}
			printf("table[%03i]\t%i\t%02x\t%c\t%i\t", i, i, i, (i>31&&i<127)?(char)i:' ', (*table)[i].len);
			
			j=0;
			wsum=0;
			while(j<(*table)[i].len){
				wsum+=(*table)[i].widths[j];
				j++;
			}

			printf("%i\t", wsum);

			j=0;
			while(j<(*table)[i].len){
				printf("%i", (*table)[i].values[j]);
				if(j+1!=(*table)[i].len){
					printf(" -> ");
				}
				j++;
			}
			
				printf("\n");
			
			
		}else{
			if(omissionstart==-1){
				omissionstart=i;
			}
		}
		i++;
	}
	if(omissionstart!=-1){
		printf("\t\t\t\t\t\t\t\t\t\t\tOmitted %i blank rows %i-%i\n", i-omissionstart-1, omissionstart, i-1);
		omissionstart=-1;
	}

	LOG_INFO("===LookupTable_print ends===\n");
}

void fill_table(LookupTable *table, TreeNode *node, int depth, int *path, int *widths){
	if(TreeNode_leaf(node)){
		// LOG_SPAM("Traversing to leaf, v=%c\n", node->value);
		(*table)[(int)node->value].len=depth;
		(*table)[(int)node->value].values=malloc(sizeof(int)*depth);
		(*table)[(int)node->value].widths=malloc(sizeof(int)*depth);
		int i=0;
		while(i<depth){
			(*table)[(int)node->value].values[i]=path[i];
			i++;
		}

		i=0;
		while(i<depth){
			(*table)[(int)node->value].widths[i]=widths[i];
			i++;
		}
	}else{
		LOG_SPAM("Traversing to node, children=%i\n", node->children);
		int i=0;
		while(i<node->children){
			path[depth]=i;
			widths[depth]=min_bits_to_represent(node->children);
			fill_table(table, TreeNode_traverse(node, i), depth+1, path, widths);
			i++;
		}
	}
}

LookupTable *create_table(TreeNode *root){
	LookupTable *table=malloc(sizeof(LookupTable));
	int search_depth=Arg_get_int("max_search_depth", 50);
	LOG_SPAM("create_table search_depth=%i\n", search_depth);
	int *path=malloc(sizeof(int)*search_depth);
	int *widths=malloc(sizeof(int)*search_depth);

	int i=0;
	while(i<256){
		(*table)[i].len=0;
		i++;
	}

	fill_table(table, root, 0, path, widths);

	return table;
}

void free_table(LookupTable *table){
	int i = 0;
	while(i<256){
		if((*table)[i].len!=0){
			free((*table)[i].values);
			free((*table)[i].widths);
		}
		i++;
	}
	free(table);
}

void compress_file(FILE *in, BitFile *out, LookupTable *table, int size){
	int position=0;
	byte currbyte;
	int write_pos;
	int bit_pos;
	while(position<size){
		currbyte=fgetc(in);
		if((*table)[currbyte].len==0){
			LOG_WARN("Char %c was found in file but not in LookupTable, ignoring\n", currbyte);
		}else{
			write_pos=0;
			while(write_pos<(*table)[currbyte].len){
				BitFile_writeint(out, (*table)[currbyte].values[write_pos], (*table)[currbyte].widths[write_pos]);
				write_pos++;
			}
		}
		position++;
	}
}

void decompress_file(BitFile *in, FILE *out, TreeNode *tree, int size){
	TreeNode *pos=tree;
	int bytes_read=0;
	int val;
	int width;
	while(bytes_read!=size){
		width=min_bits_to_represent(pos->children);
		val=BitFile_readint(in, width);
		pos=TreeNode_traverse(pos, val);
		// printf("%i:%i->(%i)%i\t", in->buffer, in->position, width, val);
		// printf("val=%i, pos->value=%c, pos->children=%i\n", val, pos->value, pos->children);
		if(TreeNode_leaf(pos)){
			fprintf(out, "%c", pos->value);
			pos=tree;
			bytes_read+=1;
		}
	}
	printf("\n");
}

#define MAGIC_STRING "YEE"
#define MAGIC_STRING_SIZE 3
#define FILE_VERSION 0

//Header structure (v0)
//BYTES		TYPE	THING
//3 		char 	Literal string 'YEE'
//1 		byte 	Version (Currently: 0)
//1 		byte	Flags (see compress.h:HF_*)
//4			int 	Length of contents in bytes
//* 		?	 	If HF_NOTREE, nothing
//					If HF_FLAT, nothing
//					If HF_PTREE, a PTree_HeaderInfo as described:
//						BYTES 	TYPE 	THING
//	 					4 		int 	Size of following ptree data (incl name)
//						?		string 	The algorithm name followed by a null byte
//	 					? 		void 	Arbitrary data as specified by the algorithm
//					If no flags (default behavior: Tree stored here), a tree serialized as follows
//						BYTES 	TYPE 	THING
//						4		int 	Total number of nodes
//						? 		tree 	Tree saved as follows:
//						For each node starting with the root node, the following is written:
//							BYTES 	TYPE 	THING
//							1 		char 	The character represented
// 							1 		byte 	The number of children. 0=Leaf
// 						Then following this is written all of the children serialized as above(left-hand traversal)

HeaderInfo *HeaderInfo_create_notree(unsigned int fsize){
	HeaderInfo *hi=malloc(sizeof(HeaderInfo));
	hi->size=fsize;
	hi->flags=HF_NOTREE;
	hi->version=FILE_VERSION;
	return hi;
}

HeaderInfo *HeaderInfo_create_tree(unsigned int fsize, TreeNode *tree){
	HeaderInfo *hi=HeaderInfo_create_notree(fsize);
	hi->flags=HF_NONE;
	hi->tree=tree;
	return hi;
}

HeaderInfo *HeaderInfo_create_ptree(unsigned int fsize, char *algname, int data_size, void *data){
	HeaderInfo *hi=HeaderInfo_create_notree(fsize);
	hi->flags=HF_PTREE | HF_NOTREE;
	hi->ptree_name=algname;
	hi->ptree_data_size=data_size;
	hi->ptree_data=data;
	return hi;
}

void HeaderInfo_destroy(HeaderInfo *header){
	free(header);
}

int HeaderInfo_get_base_size(){
	return 
		sizeof(byte)+			//Version
		sizeof(byte)+			//Flags
		sizeof(unsigned int)+	//Size
		MAGIC_STRING_SIZE;	 	//Magic String
}

byte *HeaderInfo_save(HeaderInfo *header, int *size_out){
	int hdr_size=HeaderInfo_get_base_size();

	LOG_SPAM("HeaderInfo_save: hdr_size=%i\n", hdr_size);

	void *tree_buf;
	int   tree_buf_size;
	int   tree_node_count;

	if(!(header->flags & HF_NOTREE)){
		//We need to save the tree too...
		tree_buf_size=Tree_savetobuf_size(TreeNode_count(header->tree));
		tree_buf=Tree_savetobuf(header->tree);
		tree_node_count=TreeNode_count(header->tree);

		hdr_size+=tree_buf_size;
		hdr_size+=sizeof(int);
	}

	if(header->flags & HF_PTREE){
		//We need to include ptree data
		hdr_size+=
			strlen(header->ptree_name)+		//Algorithm Name
			1+								//Null-terminator for algorithm name
			sizeof(unsigned int)+			//Data size
			header->ptree_data_size; 		//PTree data
	}

	byte *buffer=malloc(hdr_size);
	*size_out=hdr_size;
	int write_pos=0;
	LOG_SPAM("HeaderInfo_save: allocated %ib\n", hdr_size);

	memcpy(buffer, &MAGIC_STRING, MAGIC_STRING_SIZE);
	write_pos+=MAGIC_STRING_SIZE;
	LOG_SPAM("HeaderInfo_save: wrote MAGIC_STRING (%s), write_pos=%i\n", MAGIC_STRING, write_pos);

	memcpy(buffer+write_pos, &header->version, sizeof(byte));
	write_pos+=sizeof(byte);
	LOG_SPAM("HeaderInfo_save: wrote version, write_pos=%i\n", write_pos);

	memcpy(buffer+write_pos, &header->flags, sizeof(byte));
	write_pos+=sizeof(byte);
	LOG_SPAM("HeaderInfo_save: wrote flags, write_pos=%i\n", write_pos);

	int size_nbo=htonl(header->size);
	memcpy(buffer+write_pos, &size_nbo, sizeof(unsigned int));
	write_pos+=sizeof(unsigned int);
	LOG_SPAM("HeaderInfo_save: wrote size, write_pos=%i\n", write_pos);

	if(!(header->flags & HF_NOTREE)){
		int tree_node_count_nbo=htonl(tree_node_count);
		memcpy(buffer+write_pos, &tree_node_count_nbo, sizeof(int));
		write_pos+=sizeof(int);

		memcpy(buffer+write_pos, tree_buf, tree_buf_size);
	}else if(header->flags & HF_PTREE){
		int ptree_data_size_nbo=htonl(header->ptree_data_size);
		memcpy(buffer+write_pos, &ptree_data_size_nbo, sizeof(int));
		write_pos+=sizeof(int);

		memcpy(buffer+write_pos, header->ptree_name, strlen(header->ptree_name)+1);
		write_pos+=strlen(header->ptree_name)+1;

		memcpy(buffer+write_pos, header->ptree_data, header->ptree_data_size);
	}

	return buffer;
}

HeaderInfo *HeaderInfo_load_base(byte *buffer){
	HeaderInfo *hdr=malloc(sizeof(HeaderInfo));

	int read_pos=0;

	while(read_pos<MAGIC_STRING_SIZE){
		if(buffer[read_pos]!=MAGIC_STRING[read_pos]){
			LOG_ERROR("Invalid MAGIC_STRING, corrupt/incorrect file\n");
			goto fail;
		}
		read_pos++;
	}

	LOG_SPAM("HeaderInfo_load_base: Magic String validated, read_pos=%i\n", read_pos);

	memcpy(&hdr->version, buffer+read_pos, sizeof(byte));
	read_pos+=sizeof(byte);

	LOG_SPAM("HeaderInfo_load_base: Version read, read_pos=%i\n", read_pos);

	if(hdr->version!=FILE_VERSION){
		LOG_ERROR("Invalid file version, Mine=%i, File's=%i\n", (int)FILE_VERSION, (int)hdr->version);
		goto fail;
	}

	memcpy(&hdr->flags, buffer+read_pos, sizeof(byte));
	read_pos+=sizeof(byte);

	int size_nbo;
	memcpy(&size_nbo, buffer+read_pos, sizeof(int));
	read_pos+=sizeof(int);
	hdr->size=ntohl(size_nbo);

	return hdr;

	fail:
	free(hdr);
	return 0;
}

int HeaderInfo_load_ptree_size(HeaderInfo *hdr, byte *buffer);
void HeaderInfo_load_ptree(HeaderInfo *hdr, byte *buffer);

int HeaderInfo_load_tree_size(HeaderInfo *hdr, byte *buffer){
	int nodecount_nbo;
	memcpy(&nodecount_nbo, buffer, sizeof(int));
	int nodes = htonl(nodecount_nbo);
	hdr->_tree_node_count=nodes;
	return nodes*(sizeof(byte)+sizeof(byte));
}
void HeaderInfo_load_tree(HeaderInfo *hdr, byte *buffer){
	hdr->tree = Tree_loadfrombuf(buffer);
}

HeaderInfo *HeaderInfo_load_fd(FILE *fd){
	int hdr_base_size=HeaderInfo_get_base_size();
	byte *base_hdr_buffer=malloc(hdr_base_size);
	fread(base_hdr_buffer, hdr_base_size, 1, fd);
	HeaderInfo *hdr=HeaderInfo_load_base(base_hdr_buffer);
	free(base_hdr_buffer);

	LOG_SPAM("Base header loaded. Version=%i, Flags=%i, Len=%i\n", (int)hdr->version, (int)hdr->flags, (int)hdr->size);

	if(!(hdr->flags & HF_NOTREE)){
		LOG_SPAM("Header includes tree, loading tree header\n");

		char *hdr_tree_hdr_buffer=malloc(sizeof(int));
		fread(hdr_tree_hdr_buffer, sizeof(int), 1, fd);
		int size=HeaderInfo_load_tree_size(hdr, hdr_tree_hdr_buffer);
		free(hdr_tree_hdr_buffer);

		LOG_SPAM("HeaderInfo_load_tree_size returned %i\n", size);

		char *hdr_tree_tree_buffer=malloc(size);
		fread(hdr_tree_tree_buffer, size, 1, fd);
		HeaderInfo_load_tree(hdr, hdr_tree_tree_buffer);
		free(hdr_tree_tree_buffer);

		LOG_SPAM("Tree header loaded. Nodes=%i\n", TreeNode_count(hdr->tree));
	}

	return hdr;
}

HeaderInfo *HeaderInfo_load_file(char *path){
	FILE *fd=fopen(path, "r");
	HeaderInfo *hdr=HeaderInfo_load_fd(fd);
	fclose(fd);
	return hdr;
}

#undef LOG_REGION