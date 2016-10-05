#include <stdio.h>
#include <string.h>
#include "codingtree.h"
#include "map.h"
#include "log.h"
#include "args.h"

#define LOG_REGION "static"

char *ext_get_name(){
	return "static";
}

TreeNode *ext_build_tree(AppConfig *parent_config, char *text){
	app_config=parent_config;
	char *treename=Map_GETDEFAULTSTR(app_config->config, "tree", "flat");
	LOG_DEBUG("Returning tree: %s\n", treename);
	

	if(strcmp(treename, "flat")==0){
		TreeNode *tree=TreeNode_create(256);
		int i=0;
		while(i<256){
			TreeNode_attach(tree, TreeNode_create_leaf(i), i);
			i++;
		}
		return tree;
	}else if(strcmp(treename, "hex")==0){
		TreeNode *tree=TreeNode_create(16);
		TreeNode_attach(tree, TreeNode_create_leaf('0'), 0);
		TreeNode_attach(tree, TreeNode_create_leaf('1'), 1);
		TreeNode_attach(tree, TreeNode_create_leaf('2'), 2);
		TreeNode_attach(tree, TreeNode_create_leaf('3'), 3);
		TreeNode_attach(tree, TreeNode_create_leaf('4'), 4);
		TreeNode_attach(tree, TreeNode_create_leaf('5'), 5);
		TreeNode_attach(tree, TreeNode_create_leaf('6'), 6);
		TreeNode_attach(tree, TreeNode_create_leaf('7'), 7);
		TreeNode_attach(tree, TreeNode_create_leaf('8'), 8);
		TreeNode_attach(tree, TreeNode_create_leaf('9'), 9);
		TreeNode_attach(tree, TreeNode_create_leaf('a'), 10);
		TreeNode_attach(tree, TreeNode_create_leaf('b'), 11);
		TreeNode_attach(tree, TreeNode_create_leaf('c'), 12);
		TreeNode_attach(tree, TreeNode_create_leaf('d'), 13);
		TreeNode_attach(tree, TreeNode_create_leaf('f'), 14);

		TreeNode *l2=TreeNode_create(4);
		TreeNode_attach(l2, TreeNode_create_leaf('e'), 0);
		TreeNode_attach(l2, TreeNode_create_leaf('x'), 1);
		TreeNode_attach(l2, TreeNode_create_leaf(' '), 2);
		TreeNode_attach(l2, TreeNode_create_leaf('\n'), 3);

		TreeNode_attach(tree, l2, 15);
		return tree;
	}else if(strcmp(treename, "hex2")==0){
		TreeNode *root=TreeNode_create(2);
		TreeNode_attach(root, TreeNode_create_leaf('0'), 0);

		TreeNode *tree=TreeNode_create(16);
		TreeNode_attach(tree, TreeNode_create_leaf('e'), 0);
		TreeNode_attach(tree, TreeNode_create_leaf('1'), 1);
		TreeNode_attach(tree, TreeNode_create_leaf('2'), 2);
		TreeNode_attach(tree, TreeNode_create_leaf('3'), 3);
		TreeNode_attach(tree, TreeNode_create_leaf('4'), 4);
		TreeNode_attach(tree, TreeNode_create_leaf('5'), 5);
		TreeNode_attach(tree, TreeNode_create_leaf('6'), 6);
		TreeNode_attach(tree, TreeNode_create_leaf('7'), 7);
		TreeNode_attach(tree, TreeNode_create_leaf('8'), 8);
		TreeNode_attach(tree, TreeNode_create_leaf('9'), 9);
		TreeNode_attach(tree, TreeNode_create_leaf('a'), 10);
		TreeNode_attach(tree, TreeNode_create_leaf('b'), 11);
		TreeNode_attach(tree, TreeNode_create_leaf('c'), 12);
		TreeNode_attach(tree, TreeNode_create_leaf('d'), 13);
		TreeNode_attach(tree, TreeNode_create_leaf('f'), 14);

		TreeNode *l2=TreeNode_create(4);
		
		TreeNode_attach(l2, TreeNode_create_leaf('x'), 1);
		TreeNode_attach(l2, TreeNode_create_leaf(' '), 2);
		TreeNode_attach(l2, TreeNode_create_leaf('\n'), 3);
		TreeNode_attach(l2, TreeNode_create_leaf('\t'), 0);

		TreeNode_attach(tree, l2, 15);
		TreeNode_attach(root, tree, 1);
		return root;
	}else if(strcmp(treename, "abc")==0){
		TreeNode *tree=TreeNode_create(2);
		TreeNode_attach(tree, TreeNode_create_leaf('a'), 0);

		TreeNode *l2=TreeNode_create(2);
		TreeNode_attach(l2, TreeNode_create_leaf('b'), 0);
		TreeNode_attach(l2, TreeNode_create_leaf('c'), 1);

		TreeNode_attach(tree, l2, 1);

		return tree;
	}else{
		LOG_ERROR("Requested bad tree from testtree: %s\n", treename);
		return 0;
	}
}

#undef LOG_REGION