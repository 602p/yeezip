#include <stdio.h>
#include "codingtree.h"
#include "map.h"
#include "log.h"
#include "args.h"

#define LOG_REGION "testext"

char *ext_get_name(){
	return "test";
}

TreeNode *ext_build_tree(AppConfig *parent_config, char *text){
	app_config=parent_config;
	LOG_INFO("Extention called\n");
	LOG_ALLOC("Boutta malloc...\n");
	free(malloc(1));
	LOG_STATUS("app_config->config has cool:%i\n", Map_has(app_config->config, "cool"));
	LOG_STATUS("options[cool]=%s\n",Map_getstr(app_config->config, "cool"));
	return 0;
}

#undef LOG_REGION