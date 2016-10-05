#include "compress.h"
#include "log.h"
#include "map.h"
#include "args.h"

#define LOG_REGION "compress"

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
		LOG_SPAM("Traversing to leaf, v=%c\n", node->value);
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
				bit_pos=0;
				while(bit_pos<(*table)[currbyte].widths[write_pos]){
					BitFile_write(out, Byte_getbit((*table)[currbyte].values[write_pos], bit_pos));
					bit_pos++;
				}
				write_pos++;
			}
		}
		position++;
	}
}

void decompress_file(BitFile *in, FILE *out, TreeNode *tree){
	TreeNode *pos=tree;
	while(BitFile_has_more(in)){
		pos=TreeNode_traverse(pos, BitFile_readint(in, min_bits_to_represent(pos->children)));
		if(TreeNode_leaf(pos)){
			fprintf(out, "%c", pos->value);
			pos=tree;
		}
	}
}

#undef LOG_REGION