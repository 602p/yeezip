#include "main.h"
#include "codingtree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

// void _TreeNode_print_node(TreeNode* node, char *prefix, bool drawLine){
// 	char *new=malloc(strlen(prefix)+4);
// 	strcpy(new, prefix);
// 	if(drawLine){
// 		memcpy(new+strlen(prefix), "|   ", 4);
// 	}else{
// 		memcpy(new+strlen(prefix), "    ", 4);
// 	}
// 	if(TreeNode_leaf(node)){
// 		printf(prefix);
// 		printf("|---%c\n", node->value);
// 		if(drawLine){
// 			printf(prefix);
// 			printf("|\n");
// 		}
// 	}else{
// 		printf(prefix);
// 		printf("+---*\n");
// 		_TreeNode_print_node(node->left, new, true);
// 		_TreeNode_print_node(node->right, new, false);
// 		if(drawLine){
// 			printf(prefix);
// 			printf("|\n");
// 		}
// 	}
// 	free(new);
// }

// void TreeNode_print_tree(TreeNode* tree){
// 	char blank[]="";
// 	_TreeNode_print_node(tree, blank, false);
// }