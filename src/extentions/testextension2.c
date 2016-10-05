#include <stdio.h>
#include "codingtree.h"
#include "map.h"
#include "args.h"
#include "log.h"

#define LOG_REGION "ext2"

char *ext_get_name(){
	return "foobar";
}

TreeNode *ext_build_tree(AppConfig *parent_config, char *text){
	app_config=parent_config;
	LOG_INFO("***Test 2 called***\n");
	LOG_INFO("options[test]=%s\n",Map_getstr(app_config->config, "test"));
	LOG_INFO("options[int]+1=%i\n",atoi(Map_getstr(app_config->config, "int"))+1);
	return 0;
}

#undef LOG_REGION