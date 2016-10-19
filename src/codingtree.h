#ifndef __CODINGTREE_H__
#define __CODINGTREE_H__

#include "main.h"

typedef struct TreeNode_struct TreeNode;
//Foreward typedef so that we can define TreeNode_linklist for use in TreeNode_struct

typedef struct TreeNode_struct* TreeNode_linklist[];
//Array of pointers to TreeNodes, used for child managment

typedef struct TreeNode_struct {
	byte value; //Actual value of the node. If children!=0, this is unused
	int children; //Number of children this node has. If 0, it is a leaf (value-holder)
	TreeNode_linklist *links; //Pointers to children
} TreeNode;

TreeNode *TreeNode_create(int count); //Create a node with <count> children
TreeNode *TreeNode_create_leaf(byte value); //Create a node with 0 children and this value
TreeNode *TreeNode_traverse(TreeNode* node, int value); //Get the child at pos <value> of the node
bool TreeNode_leaf(TreeNode* node); //Is this node a leaf (i.e. node->children==0?)
void TreeNode_destroy(TreeNode* node); //Wreck house on this node AND IT'S CHILDREN RECURSIVLEY
void TreeNode_attach(TreeNode* parent, TreeNode* child, int pos); //Attach child to parent at position pos
void TreeNode_print_tree(TreeNode* tree); //Print a tree (kinda broken) with tree as it's root node
int TreeNode_count(TreeNode* node); //Count up all the nodes in the tree with node as it's root

int Tree_savetobuf_size(int nodes); //Get the size (in bytes) it will take to represent this tree
                                    // using Tree_savetobuf
char *Tree_savetobuf(TreeNode* root); //Create and return a buffer from root
TreeNode *Tree_loadfrombuf(byte* buf); //Create and return a tree from buf

#endif