#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "main.h"
#include "codingtree.h"
#include "map.h"
#include "extension.h"
#include "log.h"
#include "args.h"
#include "tests/dispatcher.h"
#include "compress.h"
#include "io.h"

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

	if(intent->intent==INTENT_COMPRESS){
		LOG_DEBUG("Building LookupTable\n");

		LookupTable *table=create_table(tree);

		if(Arg_has("print_table")) LookupTable_print(table);

		if(intent->do_compress){
			LOG_INFO("Compressing %s -> %s ...\n", intent->infile, intent->outfile);

			FILE *file_in=fopen(intent->infile, "r");
			BitFile *file_out=BitFile_open(intent->outfile, false);

			struct stat st;
			stat(intent->infile, &st);

			compress_file(file_in, file_out, table, st.st_size);

			fclose(file_in);
			BitFile_close(file_out);

			free_table(table);
		}
	}else if(intent->intent==INTENT_DECOMPRESS){
		LOG_INFO("Decompressing %s -> %s ...\n", intent->infile, intent->outfile);

		BitFile *file_in=BitFile_open(intent->infile, true);
		FILE *file_out = fopen(intent->outfile, "w");

		decompress_file(file_in, file_out, tree);

		BitFile_close(file_in);
		fclose(file_out);
	}

	exit(0);
}

#undef LOG_REGION