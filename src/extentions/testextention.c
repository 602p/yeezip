#include <stdio.h>
#include "codingtree.h"
#include "map.h"
#include "log.h"
#include "args.h"

#define LOG_REGION "testext"

char *ext_get_name(){
	return "test";
}

TreeNode *ext_build_tree(int loglevel_in, Map *options, int *freqtable){
	loglevel=loglevel_in;
	LOG_INFO("Extention called\n");
	LOG_ALLOC("Boutta malloc...\n");
	free(malloc(1));
	LOG_STATUS("options has cool:%i\n", Map_has(options, "cool"));
	LOG_STATUS("options[cool]=%s\n",Map_getstr(options, "cool"));
	return 0;
}

#undef LOG_REGION