#include "log.h"
#include "map.h"
#include "extension.h"

#define LOG_REGION "extest"

void test_extensions(){
	Map_setstr(app_config->config, "cool", "Sick, duuude!");
	Map_setstr(app_config->config, "test", "test_value");

	LOG_STATUS("app_config->config has cool:%i\n", Map_has(app_config->config, "cool"));
	LOG_STATUS("options[cool]=%s\n",Map_getstr(app_config->config, "cool"));

	LOG_DEBUG("===Calling test===\n");

	Map_GETFUNC(treebuilder_extensions, "test", treebuilder_sig)(app_config, "test string");
}

#undef LOG_REGION