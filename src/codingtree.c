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
	node->links=malloc(sizeof(TreeNode*)*count); //Allocate space for the linklist
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
	//node->links is a pointer to an array, so dereference it before retreiving the pointer from it
	return (*node->links)[value];
}

void TreeNode_attach(TreeNode* parent, TreeNode* child, int pos){
	//ditto
	(*parent->links)[pos]=child;
}

int TreeNode_count(TreeNode* node){
	if(TreeNode_leaf(node)){
		return 1;
	}

	int sum = 1; //For this node
	int i = 0;
	while(i<node->children){
		sum+=TreeNode_count((*node->links)[i]);
		i++;
	}

	return sum;
}

int Tree_savetobuf_size(int nodes){
	int size=((sizeof(byte)+sizeof(byte))*nodes);
	return size;
}

void TreeNode_write(TreeNode *node, char *buffer, int *buf_pos){
	LOG_SPAM("Writing node where value=%c children=%i buf_pos=%i memaddr=%i\n", node->value, node->children, *buf_pos, buffer+(*buf_pos));
	
	memcpy(buffer+(*buf_pos), &(node->value), sizeof(byte));
	(*buf_pos)+=sizeof(byte);
	//Write the value and move forward one byte

	byte children_as_byte=node->children;
	memcpy(buffer+(*buf_pos), &children_as_byte, sizeof(byte));
	(*buf_pos)+=sizeof(byte);
	//Write the children (cast to byte from int) and move forward one byte

	int i=0;
	while(i<node->children){
		TreeNode_write((*node->links)[i], buffer, buf_pos);
		//Write all of this node's children (passing them buf_pos (the pointer))
		i++;
	}
}

TreeNode *TreeNode_load(byte *buffer, int *buf_pos){
	byte value;
	memcpy(&value, buffer+(*buf_pos), sizeof(byte));
	TreeNode *node=TreeNode_create_leaf(value);
	//Snarf out the value of the node and create a "leaf" (just a node w/ 0 children) from it

	*buf_pos+=sizeof(byte);
	//Move forward in the buffer one byte

	byte children_as_byte;
	memcpy(&children_as_byte, buffer+(*buf_pos), sizeof(byte));
	node->children=children_as_byte;
	*buf_pos+=sizeof(byte);
	//Snarf out the child count as a byte, and implicitly cast to int for storage in the new struct
	// (this ovverwrites the value set in TreeNode_create_leaf)

	LOG_SPAM("Loaded node value=%c children=%i, buf_pos=%i\n", node->value, node->children, *buf_pos);

	if(node->children>0){
		node->links=malloc(sizeof(TreeNode*)*node->children);
		//Re-malloc the links item to hold the number of children.
		// (No need to free as TreeNode_create_leaf never sets it to anything)
		int child_idx=0;
		while(child_idx<node->children){
			(*node->links)[child_idx]=TreeNode_load(buffer, buf_pos);
			child_idx+=1;
		}
	}

	return node;
}

char *Tree_savetobuf(TreeNode* root){
	int size=Tree_savetobuf_size(TreeNode_count(root));
	LOG_SPAM("Allocating %i bytes for Tree_savetobuf\n", size);
	char *buffer=malloc(size);
	//Allocate the size of the buffer to store tree data

	int buf_pos=0;
	TreeNode_write(root, buffer, &buf_pos);
	//Pass along buf_pos as a shared state for whatever pattern of recursion ends up happening

	return buffer;
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