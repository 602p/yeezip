#include <stdio.h>
#include "codingtree.h"
#include "map.h"
#include "args.h"
#include "log.h"

#define LOG_REGION "ext2"

char *ext_get_name(){
	return "foobar";
}

TreeNode *ext_build_tree(int loglevel_in, Map *options, int *freqtable){
	loglevel=loglevel_in;
	LOG_INFO("***Test 2 called***\n");
	LOG_INFO("options[test]=%s\n",Map_getstr(options, "test"));
	LOG_INFO("options[int]+1=%i\n",atoi(Map_getstr(options, "int"))+1);
	return 0;
}

#undef LOG_REGION