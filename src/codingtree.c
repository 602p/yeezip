#include "main.h"
#include "codingtree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "log.h"

#define LOG_REGION "tree"

bool TreeNode_leaf(TreeNode* node){
	if(node->children==0){
		return true;
	}
	return false;
}

TreeNode *TreeNode_create_leaf(byte value){
	TreeNode *node = malloc(sizeof(TreeNode));
	node->children=0;
	node->value=value;
	return node;
}

TreeNode *TreeNode_create(int count){
	TreeNode *node = TreeNode_create_leaf(0);
	node->children=count;
	node->links=malloc(sizeof(TreeNode*)*count);
	return node;
}

void TreeNode_destroy(TreeNode* node){
	int i=0;
	while(i<node->children){
		TreeNode_destroy((*node->links)[i]);
		++i;
	}
	free(node->links);
	free(node);
}

TreeNode *TreeNode_traverse(TreeNode* node, int value){
	return (*node->links)[value];
}

void TreeNode_attach(TreeNode* parent, TreeNode* child, int pos){
	(*parent->links)[pos]=child;
}

int TreeNode_count(TreeNode* node){
	if(TreeNode_leaf(node)){
		return 1;
	}

	int sum = 0;
	int i = 0;
	while(i<node->children){
		sum+=TreeNode_count((*node->links)[i]);
		i++;
	}

	return sum+1;
}

void TreeNode_write(TreeNode *node, char *buffer, int *buf_pos){
	if(Arg_has("debug_tree_save"))
		LOG_SPAM("Writing node where value=%c children=%i buf_pos=%i memaddr=%i\n", node->value, node->children, *buf_pos, buffer+(*buf_pos));
	memcpy(buffer+(*buf_pos), &(node->value), sizeof(byte));
	(*buf_pos)+=sizeof(byte);

	byte children_as_byte=node->children;
	memcpy(buffer+(*buf_pos), &children_as_byte, sizeof(byte));
	(*buf_pos)+=sizeof(byte);

	int i=0;
	while(i<node->children){
		TreeNode_write((*node->links)[i], buffer, buf_pos);
		i++;
	}
}

int Tree_savetobuf_size(TreeNode *root){
	int nodes=TreeNode_count(root);
	int size=((sizeof(byte)+sizeof(byte))*nodes);
	return size;
}

char *Tree_savetobuf(TreeNode* root){
	int size=Tree_savetobuf_size(root);
	LOG_SPAM("Allocating %i bytes for Tree_savetobuf\n", size);
	char *buffer=malloc(size);

	int buf_pos=0;
	TreeNode_write(root, buffer, &buf_pos);

	return buffer;
}

TreeNode *TreeNode_load(byte *buffer, int *buf_pos){
	byte value;
	memcpy(&value, buffer+(*buf_pos), sizeof(byte));
	TreeNode *node=TreeNode_create_leaf(value);
	*buf_pos+=sizeof(byte);

	byte children_as_byte;
	memcpy(&children_as_byte, buffer+(*buf_pos), sizeof(byte));
	node->children=children_as_byte;
	*buf_pos+=sizeof(byte);

	if(Arg_has("debug_tree_load"))
		LOG_SPAM("Loaded node value=%c children=%i, buf_pos=%i\n", node->value, node->children, *buf_pos);

	if(node->children>0){
		node->links=malloc(sizeof(TreeNode*)*node->children);
		int child_idx=0;
		while(child_idx<node->children){
			(*node->links)[child_idx]=TreeNode_load(buffer, buf_pos);
			child_idx+=1;
		}
	}

	return node;
}

TreeNode *Tree_loadfrombuf(byte* buf){
	int buf_pos=0;
	return TreeNode_load(buf, &buf_pos);
}

void Tree_print(TreeNode *root){
	TreeNode *pos=root;
	bool run = true;
	int depth=0;
	int dp;
	int cp;
	while(run){
		cp=0;
		while(cp<pos->children){
			dp=0;
			while(dp<depth){printf("\t");dp++;}
			printf(" o ");
			printf("Value = `%c`, Children=%i", (*pos->links)[cp]->value, (*pos->links)[cp]->children);
			if((*pos->links)[cp]->children!=0){
				printf("   <CONTAINER>\n");
				pos=(*pos->links)[cp];
				depth++;
				break;
			}
			if(cp==pos->children-1){
				run=false;
			}
			printf("\n");
			cp++;
		}
	}
}

#undef LOG_REGION