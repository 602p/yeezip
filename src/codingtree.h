#ifndef __CODINGTREE_H__
#define __CODINGTREE_H__

#include "main.h"

typedef struct TreeNode_struct TreeNode;

typedef struct TreeNode_struct* TreeNode_linklist[];

typedef struct TreeNode_struct {
	byte value;
	int children;
	TreeNode_linklist *links;
} TreeNode;

typedef struct TreePosition_struct{
	int len;
	int steps[];
} TreePosition;

TreeNode *TreeNode_create(int count);
TreeNode *TreeNode_create_leaf(byte value);
TreeNode *TreeNode_traverse(TreeNode* node, int value);
bool TreeNode_leaf(TreeNode* node);
void TreeNode_destroy(TreeNode* node);
void TreeNode_attach(TreeNode* parent, TreeNode* child, int pos);
void TreeNode_print_tree(TreeNode* tree);
int TreeNode_count(TreeNode* node);

char *Tree_savetobuf(TreeNode* root);
TreeNode *Tree_loadfrombuf(byte* buf);

#endif