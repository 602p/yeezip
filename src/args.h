#ifndef __ARGS_H__
#define __ARGS_H__

#define INTENT_HELP 1
#define INTENT_COMPRESS 2
#define INTENT_DECOMPRESS 3
#define INTENT_TEST 4

#include "map.h"
#include <stdio.h>

typedef struct Intent_struct{
	int intent;
	//ID code for what we want to do. Refer to INTENT_*

	bool invalid;
	//Is it a valid intent? There is no situation in which this is garbage data,
	//but if it is set to true, then other values may be

	char *infile;
	//Path for the file to read for whatever operation
	//(i.e. for compression the file to compress and for decompression the compressed file)

	bool do_compress;
	//Actually do the compression or just build the tree (and save it if outfile is set)
	char *outfile;
	//Path for the file to write for whatever operation
	//(i.e. for compression the compressed file and for decompression the output paintext)

	char *savefile;
	//File to save the tree data to. If we don't want to save it is empty (a null byte)
	bool skip_save_tree;
	//Save the tree to outfile?

	char *importfile;
	//Path to file to load the tree to use for compression/decompression to/from

	char *algorithm;
	//Name of algorithm to use for compression. If nil, multiple will be tried and the best picked

	char *testname;
} Intent;

typedef struct AppConfig_struct{
	int loglevel;
	FILE *logfile;
	Map *config;
} AppConfig;

AppConfig *app_config;

Intent *parse_args(int argc, char *argv[]);
//Create and fill an intent from args specified by argc and argv
//Always returns an intent that provides at least `invalid`
//other arguments only filled as needed

void display_help();
//Print out the help message

int Arg_get_int(char *name, int fallback);
bool Arg_has(char *name);

#endif