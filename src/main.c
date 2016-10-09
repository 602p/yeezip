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

	TreeNode *tree;

	if(intent->intent==INTENT_COMPRESS){
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

		tree=extension(app_config, 0);

		if((void*)tree==0){
			LOG_FAIL("Specified algorithm did not return a tree\n");
			exit(1);
		}

		LOG_DEBUG("Building LookupTable\n");

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

		int hdr_base_size=HeaderInfo_get_base_size();
		byte *base_hdr_buffer=malloc(hdr_base_size);
		fread(base_hdr_buffer, hdr_base_size, 1, file_in->file);
		HeaderInfo *hdr=HeaderInfo_load_base(base_hdr_buffer);
		free(base_hdr_buffer);

		LOG_SPAM("Base header loaded. Version=%i, Flags=%i, Len=%i\n", (int)hdr->version, (int)hdr->flags, (int)hdr->size);

		if(!(hdr->flags & HF_NOTREE)){
			LOG_SPAM("Header includes tree, loading tree header\n");

			char *hdr_tree_hdr_buffer=malloc(sizeof(int));
			fread(hdr_tree_hdr_buffer, sizeof(int), 1, file_in->file);
			int size=HeaderInfo_load_tree_size(hdr, hdr_tree_hdr_buffer);
			free(hdr_tree_hdr_buffer);

			LOG_SPAM("HeaderInfo_load_tree_size returned %i\n", size);

			char *hdr_tree_tree_buffer=malloc(size);
			fread(hdr_tree_tree_buffer, size, 1, file_in->file);
			HeaderInfo_load_tree(hdr, hdr_tree_tree_buffer);
			free(hdr_tree_tree_buffer);

			LOG_SPAM("Tree header loaded. Nodes=%i\n", TreeNode_count(hdr->tree));

			tree=hdr->tree;
		}else{
			if(strlen(intent->importfile)!=0){
				LOG_STATUS("Loading tree from `%s`\n", intent->importfile);
				FILE *imp_file=fopen(intent->importfile, "r");

				int imp_hdr_base_size=HeaderInfo_get_base_size();
				byte *imp_base_hdr_buffer=malloc(imp_hdr_base_size);
				fread(imp_base_hdr_buffer, imp_hdr_base_size, 1, imp_file);
				HeaderInfo *imp_hdr=HeaderInfo_load_base(imp_base_hdr_buffer);
				free(imp_base_hdr_buffer);

				if(imp_hdr->flags & HF_NOTREE){
					LOG_FAIL("Specified importfile did not contain a tree (HF_NOTREE set)\n");
					exit(1);
				}

				char *imp_hdr_tree_hdr_buffer=malloc(sizeof(int));
				fread(imp_hdr_tree_hdr_buffer, sizeof(int), 1, imp_file);
				int imp_size=HeaderInfo_load_tree_size(imp_hdr, imp_hdr_tree_hdr_buffer);
				free(imp_hdr_tree_hdr_buffer);

				LOG_SPAM("HeaderInfo_load_tree_size (on imp_hdr) returned %i\n", imp_size);

				char *imp_hdr_tree_tree_buffer=malloc(imp_size);
				fread(imp_hdr_tree_tree_buffer, imp_size, 1, imp_file);
				HeaderInfo_load_tree(imp_hdr, imp_hdr_tree_tree_buffer);
				free(imp_hdr_tree_tree_buffer);

				LOG_SPAM("Tree (imported) header loaded. Nodes=%i\n", TreeNode_count(imp_hdr->tree));

				tree=imp_hdr->tree;
			}else{
				LOG_FAIL("No treedata in input file (HF_NOTREE set) and no treefile provided\n");
				exit(1);
			}
		}

		FILE *file_out = fopen(intent->outfile, "w");

		decompress_file(file_in, file_out, tree, hdr->size);

		BitFile_close(file_in);
		fclose(file_out);
	}

	exit(0);
}

#undef LOG_REGION
#endif