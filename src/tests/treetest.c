#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "codingtree.h"

void test_tree(){
	TreeNode *tree=TreeNode_create(2);
	TreeNode *layer2=TreeNode_create(4);
	TreeNode_attach(tree, layer2, 1);
	TreeNode_attach(tree, TreeNode_create_leaf('a'), 0);
	TreeNode_attach(layer2, TreeNode_create_leaf('b'), 0);
	TreeNode_attach(layer2, TreeNode_create_leaf('c'), 1);
	TreeNode_attach(layer2, TreeNode_create_leaf('d'), 2);
	TreeNode_attach(layer2, TreeNode_create_leaf('e'), 3);

	printf("tree is_leaf=%i\n", TreeNode_leaf(tree));
	printf("tree@0 is_leaf=%i\n", TreeNode_leaf(TreeNode_traverse(tree, 0)));
	printf("tree@0=%c\n", TreeNode_traverse(tree, 0)->value);

	printf("tree@1 is_leaf=%i\n", TreeNode_leaf(TreeNode_traverse(tree, 1)));
	printf("tree@1@0 is_leaf=%i\n", TreeNode_leaf(TreeNode_traverse(TreeNode_traverse(tree, 1), 0)));
	printf("tree@1@0=%c\n", TreeNode_traverse(TreeNode_traverse(tree, 1), 0)->value);
	printf("tree@1@1=%c\n", TreeNode_traverse(TreeNode_traverse(tree, 1), 1)->value);
	printf("tree@1@2=%c\n", TreeNode_traverse(TreeNode_traverse(tree, 1), 2)->value);
	printf("tree@1@3=%c\n", TreeNode_traverse(TreeNode_traverse(tree, 1), 3)->value);
}