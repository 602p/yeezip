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
	// LOG_SPAM("Writing node where value=%c children=%i buf_pos=%i memaddr=%i\n", node->value, node->children, *buf_pos, buffer+(*buf_pos));
	memcpy(buffer+(*buf_pos), &(node->value), sizeof(byte));
	(*buf_pos)+=sizeof(byte);

	int children_nbo=htonl(node->children);
	memcpy(buffer+(*buf_pos), &children_nbo, sizeof(int));
	(*buf_pos)+=sizeof(int);

	int i=0;
	while(i<node->children){
		TreeNode_write((*node->links)[i], buffer, buf_pos);
		i++;
	}
}

int Tree_savetobuf_size(TreeNode *root){
	int nodes=TreeNode_count(root);
	int size=((sizeof(int)+sizeof(byte))*nodes);
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

	int children_nbo;
	memcpy(&children_nbo, buffer+(*buf_pos), sizeof(int));
	node->children=ntohl(children_nbo);
	*buf_pos+=sizeof(int);

	// LOG_SPAM("Loaded node value=%c children=%i\n", node->value, node->children);

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

#undef LOG_REGION