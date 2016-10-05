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

	bool invalid;

	char *infile;

	bool do_compress;
	char *outfile;

	char *savefile;
	bool skip_save_tree;

	char *importfile;

	char *algorithm;

	char *testname;
} Intent;

typedef struct AppConfig_struct{
	int loglevel;
	FILE *logfile;
	Map *config;
} AppConfig;

AppConfig *app_config;

Intent *parse_args(int argc, char *argv[]);
void display_help();

int Arg_get_int(char *name, int fallback);
bool Arg_has(char *name);

#endif