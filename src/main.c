#ifndef ENABLE_TESTS
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

	LOG_DEBUG("Logging initilized\n");

	LOG_DEBUG("Loading Extensions...\n");
	init_extension_manager();
	load_extensions();

	LOG_STATUS("Extentions loaded.\n");

	if(intent->intent==INTENT_HELP){
		display_help(treebuilder_extensions);
		exit(0);
	}

	if(intent->intent==INTENT_TEST){
		dispatch_test(intent->testname);
		exit(0);
	}

	TreeNode *tree;

	if(intent->intent==INTENT_COMPRESS){
		if(strlen(intent->algorithm)==0){
			LOG_ERROR("No algorithm specified\n");
			exit(1);
		}

		LOG_STATUS("Building Freqtable\n");
		FILE *file_in_ft=fopen(intent->infile, "r");
		freqtable *ftbl=FreqTable_create(file_in_ft);
		fclose(file_in_ft);

		if(Arg_has("print_freqtable")){
			int fidx=0;
			printf("[");
			while(fidx<256){
				printf("%d, ", (*ftbl)[fidx]);
				fidx++;
			}
			printf("]\n");
		}

		char *extention_name=intent->algorithm;

		if(!Map_has(treebuilder_extensions, extention_name)){
			LOG_ERROR("Extention %s not found\n", extention_name);
			exit(1);
		}

		treebuilder_sig *extension = Map_GETFUNC(treebuilder_extensions, extention_name, treebuilder_sig);

		LOG_DEBUG("Extention handle acquired\n");
		LOG_STATUS("Calling extension %s...\n", extention_name);

		tree=extension(app_config, ftbl);

		if((void*)tree==0){
			LOG_FAIL("Specified algorithm did not return a tree\n");
			exit(1);
		}

		LOG_STATUS("Building LookupTable\n");

		LookupTable *table=create_table(tree);

		if(Arg_has("print_table")) LookupTable_print(table);

		if(intent->do_compress){
			FILE *file_in=fopen(intent->infile, "r");

			if(strlen(intent->savefile)!=0){
				LOG_INFO("Saving tree to `%s`\n", intent->savefile);

				HeaderInfo *export_hdr=HeaderInfo_create_tree(0, tree);
				FILE *file_export=fopen(intent->savefile, "w");
				int export_hdr_size;
				void *export_hdr_buffer=HeaderInfo_save(export_hdr, &export_hdr_size);
				fwrite(export_hdr_buffer, export_hdr_size, 1, file_export);
				free(export_hdr_buffer);
				HeaderInfo_destroy(export_hdr);
			}

			if(intent->do_compress){
				LOG_INFO("Compressing `%s` -> `%s` ...\n", intent->infile, intent->outfile);
				BitFile *file_out=BitFile_open(intent->outfile, false);

				struct stat st;
				stat(intent->infile, &st);

				HeaderInfo *hdr;
				if(intent->skip_save_tree){
					hdr=HeaderInfo_create_notree((unsigned int)st.st_size);
				}else{
					hdr=HeaderInfo_create_tree((unsigned int)st.st_size, tree);
				}

				int hdr_size;

				void *buffer=HeaderInfo_save(hdr, &hdr_size);
				LOG_SPAM("Writing header...\n");
				fwrite(buffer, hdr_size, 1, file_out->file);

				free(hdr);
				free(buffer);

				compress_file(file_in, file_out, table, st.st_size);
				BitFile_close(file_out);
			}

			fclose(file_in);
		}

		free_table(table);
	}else if(intent->intent==INTENT_DECOMPRESS){
		LOG_INFO("Decompressing %s -> %s ...\n", intent->infile, intent->outfile);

		BitFile *file_in=BitFile_open(intent->infile, true);

		HeaderInfo *hdr=HeaderInfo_load_fd(file_in->file);

		if(hdr->flags & HF_NOTREE){
			if(strlen(intent->importfile)!=0){
				LOG_STATUS("Loading tree from `%s`\n", intent->importfile);
				
				HeaderInfo *imp_hdr=HeaderInfo_load_file(intent->importfile);

				LOG_SPAM("Tree (imported) header loaded. Nodes=%i\n", TreeNode_count(imp_hdr->tree));

				tree=imp_hdr->tree;
			}else{
				LOG_FAIL("No treedata in input file (HF_NOTREE set) and no treefile provided\n");
				exit(1);
			}
		}else{
			tree=hdr->tree;
		}

		FILE *file_out = fopen(intent->outfile, "w");

		if(Arg_has("print_tree")) Tree_print(tree);

		decompress_file(file_in, file_out, tree, hdr->size);

		BitFile_close(file_in);
		fclose(file_out);
	}

	exit(0);
}

#undef LOG_REGION
#endif