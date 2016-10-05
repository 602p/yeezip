#include <stdio.h>
#include <string.h>
#include "main.h"
#include "codingtree.h"
#include "map.h"
#include "extension.h"
#include "log.h"
#include "args.h"
#include "tests/dispatcher.h"
#include "compress.h"

#define LOG_REGION "main"

int main(int argc, char *argv[]) {
	app_config=MALLOC_NOLOG(sizeof(AppConfig));
	setup_logging();
	app_config->config=Map_create();

	Intent *intent=parse_args(argc, argv);

	if(intent->invalid){
		exit(1);
	}

	if(intent->intent==INTENT_HELP){
		display_help();
		exit(0);
	}

	LOG_DEBUG("Logging initilized\n");

	LOG_DEBUG("Loading Extensions...\n");
	init_extension_manager();
	load_extensions();

	LOG_STATUS("Extentions loaded.\n");

	if(intent->intent==INTENT_TEST){
		dispatch_test(intent->testname);
		exit(0);
	}

	if(strlen(intent->algorithm)==0){
		LOG_ERROR("No algorithm specified\n");
		exit(1);
	}

	char *extention_name=intent->algorithm;

	if(!Map_has(treebuilder_extensions, extention_name)){
		LOG_ERROR("Extention %s not found\n", extention_name);
		exit(1);
	}

	treebuilder_sig *extension = Map_GETFUNC(treebuilder_extensions, extention_name, treebuilder_sig);

	LOG_STATUS("Extention handle acquired\n");
	LOG_STATUS("Calling extension %s...\n", extention_name);

	TreeNode *tree=extension(app_config, 0);

	if((void*)tree==0){
		LOG_FAIL("Specified algorithm did not return a tree\n");
		exit(1);
	}

	LOG_DEBUG("Building LookupTable\n");

	LookupTable *table=create_table(tree);

	if(Arg_has("print_table")) LookupTable_print(table);

	free_table(table);

	exit(0);
}

#undef LOG_REGION