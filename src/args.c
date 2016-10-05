#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "main.h"
#include "args.h"
#include "map.h"

#define LOG_REGION "args"

char *consume_next(int argc, char *argv[], int index){
	++index;
	if(index!=argc){
		return argv[index];
	}
	return 0;
}

Intent *parse_args(int argc, char *argv[]){
	Intent *intent=malloc(sizeof(Intent));

	intent->intent=0;
	intent->invalid=false;
	intent->infile="";
	intent->outfile="";
	intent->do_compress=true;
	intent->savefile="";
	intent->importfile="";
	intent->algorithm="";
	intent->testname="";
	intent->skip_save_tree=false;

	char *equals_position;
	char *name_temp;
	int name_length;
	char *next;
	int arglen;

	int current_arg=1;
	while (current_arg<argc){
		if(argv[current_arg][0]!='-'){
			LOG_WARN("Invalid option (ignored): %s\n", argv[current_arg]);
		}else{
			switch(argv[current_arg][1]){
				case 'h':
					if(intent->intent) goto multiple_intents;
					LOG_SPAM("intent=INTENT_HELP\n");
					intent->intent=INTENT_HELP;
					break;

				case 'c':
					if(intent->intent) goto multiple_intents;
					LOG_SPAM("intent=INTENT_COMPRESS\n");
					intent->intent=INTENT_COMPRESS;

					next=consume_next(argc, argv, current_arg);
					if(next) intent->infile=next;
					else goto no_file;
					current_arg++;
					LOG_SPAM("infile=%s\n", intent->infile);

					break;

				case 'o':
					next=consume_next(argc, argv, current_arg);
					if(next) intent->outfile=next;
					else goto no_file;
					current_arg++;
					LOG_SPAM("outfile=%s\n", intent->outfile);

					break;

				case 'e':
				case 'x':
					if(intent->intent) goto multiple_intents;
					LOG_SPAM("intent=INTENT_DECOMPRESS\n");
					intent->intent=INTENT_DECOMPRESS;

					next=consume_next(argc, argv, current_arg);
					if(next) intent->infile=next;
					else goto no_file;
					current_arg++;
					LOG_SPAM("infile=%s\n", intent->infile);

					break;

				case 't':
					if(intent->intent) goto multiple_intents;
					LOG_SPAM("intent=INTENT_TEST\n");
					intent->intent=INTENT_TEST;
					intent->testname=argv[current_arg]+2;
					LOG_SPAM("testname=%s\n", intent->testname);
					break;

				case 'a':
					intent->algorithm=argv[current_arg]+2;
					LOG_SPAM("algorithm=%s\n", intent->algorithm);
					break;

				case 'v':
					app_config->loglevel-=strlen(argv[current_arg])-1;
					LOG_SPAM("loglevel=%i\n", app_config->loglevel);
					break;

				case 'q':
					app_config->loglevel=999;
					break;

				case 'f':
					app_config->loglevel=atoi(argv[current_arg]+2);
					LOG_SPAM("loglevel=%i\n", app_config->loglevel);
					break;

				case 's':
					next=consume_next(argc, argv, current_arg);
					if(next) intent->savefile=next;
					else goto no_file;
					current_arg++;
					LOG_SPAM("savefile=%s\n", intent->savefile);

					break;

				case 'O':
					intent->skip_save_tree=true;
					LOG_SPAM("skip_save_tree=true\n");
					break;

				case 'l':
					next=consume_next(argc, argv, current_arg);
					if(next) intent->importfile=next;
					else goto no_file;
					current_arg++;
					LOG_SPAM("importfile=%s\n", intent->importfile);

					break;

				case 'd':
					intent->do_compress=false;
					LOG_SPAM("do_compress=false\n");
					break;

				case 'p':
					name_length=0;
					arglen=strlen(argv[current_arg]);
					while(argv[current_arg][2+name_length]!='='){
						++name_length;
						if(name_length>=arglen){
							goto invalid_parameter;
						}
					}
					name_temp=malloc(name_length+1);
					strncpy(name_temp, argv[current_arg]+2, name_length);
					Map_setstr(app_config->config, name_temp, argv[current_arg]+2+name_length+1);
					LOG_SPAM("Set parameter `%s` -> `%s`\n", name_temp, argv[current_arg]+2+name_length+1);
					break;

				case 'P':
					Map_setstr(app_config->config, argv[current_arg]+2, "1");
					LOG_SPAM("Set parameter `%s` -> `1` (-P directive)\n", name_temp);
					break;

				default:
					LOG_WARN("Invalid option (ignored): %s\n", argv[current_arg]);
			}
		}
		++current_arg;
	}

	LOG_SPAM("intent->intent -> `%i`\n (0=INTENT_NONE, 1=INTENT_HELP, 2=INTENT_COMPRESS, 3=INTENT_DECOMPRESS, 4=INTENT_TEST)\n", intent->intent);
	LOG_SPAM("intent->invalid -> `%i`\n", intent->invalid);
	LOG_SPAM("intent->infile -> `%s`\n", intent->infile);
	LOG_SPAM("intent->outfile -> `%s`\n", intent->outfile);
	LOG_SPAM("intent->do_compress -> `%i`\n", intent->do_compress);
	LOG_SPAM("intent->savefile -> `%s`\n", intent->savefile);
	LOG_SPAM("intent->skip_save_tree -> `%i`\n", intent->skip_save_tree);
	LOG_SPAM("intent->importfile -> `%s`\n", intent->importfile);
	LOG_SPAM("intent->algorithm -> `%s`\n", intent->algorithm);
	LOG_SPAM("intent->testname -> `%s`\n", intent->testname);

	//Sanity checks
	if(intent->intent==INTENT_TEST && strlen(intent->testname)==0){
		LOG_FAIL("Didn't give a testname!\n");
		goto fail;
	}

	if(intent->intent==INTENT_COMPRESS || intent->intent==INTENT_DECOMPRESS){
		if((strlen(intent->infile)==0 || (strlen(intent->outfile)==0) &&((intent->intent==INTENT_COMPRESS && intent->do_compress) || intent->intent==INTENT_DECOMPRESS))){
			LOG_FAIL("Specified to compress (without -d) or decompress but did not provide both an input and output file\n");
			goto fail;
		}
	}

	if(intent->intent==0){
		LOG_FAIL("No operation\n")
		goto fail;
	}

	return intent;

	multiple_intents:
	LOG_FAIL("Cannot specify more than one Intent\n");
	goto fail;

	no_file:
	LOG_FAIL("Option requiring filename (-c, -e, -x, -s, -o) passed without filename\n");
	goto fail;

	invalid_parameter:
	LOG_FAIL("Invalid -p option\n");
	goto fail;

	fail:
	intent->invalid=true;
	return intent;
}

void display_help(){
	printf(
		"compress - A huffman coding codec + tree generator\n"
		"\n"
		"Base Usage: compress [-v|-vv|-vvv|-vvv|-f<n>|-q] [-p<key>=<value> ...] [-h] {command}\n"
		"  -v              : Set loglevel to STATUS\n"
		"  -vv             : Set loglevel to DEBUG\n"
		"  -vvv            : Set loglevel to SPAM (requires having been compiled with ENABLE_SPAM)\n"
		"  -vvvv           : Set loglevel to ALLOC (requires having been compiled with ENABLE_SPAM)\n"
		"  -f<n>           : Sets loglevel to <n> where INFO=5, ALLOC=1, FAIL=8\n"
		"  -q              : Disables logging. Equivilent to -f0\n"
		"  -p<key>=<value> : Sets configuration parameter <key> to <value>. Both are strings. An = is required.\n"
		"  -P<key>         : Sets configuration parameter <key> to \"1\"\n"
		"  -h              : Show this fustercluck\n"
		"\n"
		"Commands:\n"
		"Compress: -c INFILE (-a<algorithm>|-l IMPORTFILE) [-s SAVEFILE] (-d|-o OUTFILE)\n"
		"  -c INFILE       : Read from INFILE and do compression on this\n"
		"  -a<algorithm>   : Use <algorithm> (loaded from extensions) to generate a tree. If unspecified, use some number and choose best\n"
		"  -l IMPORTFILE   : Read in tree from IMPORTFILE\n"
		"  -s SAVEFILE     : Save generated tree to SAVEFILE\n"
		"  -O              : Don't save the generated tree into OUTFILE, just the data. Probably unrecoverable without using -s to save a copy\n"
		"  -d              : Don't actually compress, just generate tree. For use with -s\n"
		"  -o OUTFILE      : Save output to OUTFILE\n"
		"\n"
		"Decompress: -d FILE [-l IMPORTFILE] -o OUTFILE\n"
		"  -d FILE         : Decompress FILE\n"
		"  -l IMPORTFILE   : Source tree for decompression from IMPORTFILE (for if file was compressed with -O)\n"
		"  -o OUTFILE      : Save to OUTFILE\n"
		"\n"
		"Test: -t<name>\n"
		"  -t<name>        : Run the test name <name>\n"
		"\n"
		"By Louis Goessling. Good luck!\n"
	);
}

int Arg_get_int(char *name, int fallback){
	if(Map_has(app_config->config, name)==false){
		return fallback;
	}
	char *res = Map_getstr(app_config->config, name);
	int val=atoi(res);
	char buf[20];
	sprintf(buf,"%d",val);
	if(strcmp(res, buf)==0){
		return val;
	}else{
		LOG_ERROR("Invalid option for parameter %s, must be int. Falling back to %i\n", name, fallback);
		return fallback;
	}
}

bool Arg_has(char *name){
	return Map_has(app_config->config, name);
}

#undef LOG_REGION