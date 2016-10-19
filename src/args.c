#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "main.h"
#include "args.h"
#include "map.h"

#define LOG_REGION "args"

char *consume_next(int argc, char *argv[], int index){
	//Consume the next value (starting at index) from the command line in argc, argv
	//The idiom for usage of this helper is
	//		char *value=consume_next(argc, argv, current_arg)
	//		if(value==0) <fail>
	// 		else: <succeed>
	// 		current_arg++;
	++index;
	if(index!=argc){
		return argv[index];
	}
	return 0;
}

Intent *parse_args(int argc, char *argv[]){
	Intent *intent=malloc(sizeof(Intent));
	//Initilize an Intent, this holds the options passed to the program

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

	char *name_temp; //Will hold name of key=value args
	int name_length; //Will hold length of ^
	char *next; //Will hold result of invocations of consume_next
	int arglen; //Whole arg length used in key=value args

	int current_arg=1;
	while (current_arg<argc){
		if(argv[current_arg][0]!='-'){
			LOG_WARN("Invalid option (ignored): %s\n", argv[current_arg]);
			//Warn on random crap in input.
			//Won't trip on filenames because functions using a space-delimited parameter
			//jump current_arg right over it after calling consume_next
		}else{
			switch(argv[current_arg][1]){
				//Switch on the first character of the arg (after the -)
				//There are no "long" args so this is enough
				case 'h': //Help command
					if(intent->intent) goto multiple_intents; //GOTO??? OH GOD!!
					LOG_SPAM("intent=INTENT_HELP\n");
					intent->intent=INTENT_HELP;
					break;

				case 'c': //Compress command. Filename to compress follows
					if(intent->intent) goto multiple_intents;
					LOG_SPAM("intent=INTENT_COMPRESS\n");
					intent->intent=INTENT_COMPRESS;

					next=consume_next(argc, argv, current_arg);
					if(next) intent->infile=next;
					else goto no_file;
					current_arg++;
					LOG_SPAM("infile=%s\n", intent->infile);

					break;

				case 'o': //Set output file
					next=consume_next(argc, argv, current_arg);
					if(next) intent->outfile=next;
					else goto no_file;
					current_arg++;
					LOG_SPAM("outfile=%s\n", intent->outfile);

					break;

				case 'e': //Fall through...
				case 'x': //Decompress operation. Filename to decompress follows
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

				case 'a': //Set algorithm (form is -a<value> , no space)
					intent->algorithm=argv[current_arg]+2; //Skip first two bytes, as they are -a
					LOG_SPAM("algorithm=%s\n", intent->algorithm);
					break;

				case 'v': //Verbose mode
					app_config->loglevel-=strlen(argv[current_arg])-1; //Decrement by # of vs
					LOG_SPAM("loglevel=%i\n", app_config->loglevel);
					break;

				case 'q': //Quiet mode
				          //(Suppresses only stuff generated thru normal logging facilities, won't
				          // suppress stuff coming out thru printfs used for debugging
				          // (e.g. when -Pprint_table is used to debug the table transformation))
					app_config->loglevel=999;
					break;

				case 'f': //Set log level to the value (passed by -f<value>)
					app_config->loglevel=atoi(argv[current_arg]+2); //Skip first two bytes, as they are -f
					LOG_SPAM("loglevel=%i\n", app_config->loglevel);
					break;

				case 's': //Save tree to file passed next
					next=consume_next(argc, argv, current_arg);
					if(next) intent->savefile=next;
					else goto no_file;
					current_arg++;
					LOG_SPAM("savefile=%s\n", intent->savefile);

					break;

				case 'O': //Omit the tree from the output compressed file
					intent->skip_save_tree=true;
					LOG_SPAM("skip_save_tree=true\n");
					break;

				case 'l': //Load the tree to use from the following arg
					next=consume_next(argc, argv, current_arg);
					if(next) intent->importfile=next;
					else goto no_file;
					current_arg++;
					LOG_SPAM("importfile=%s\n", intent->importfile);

					break;

				case 'd': //Dont do the actual compression, just generate a tree
					intent->do_compress=false;
					LOG_SPAM("do_compress=false\n");
					break;

				case 'p': //Save a value to the app_config config mapping via the form
				          // -p<key>=<value>. <key> and <value> can contain any string of bytes
				          // you manage to cram into argv (excluding null, and in the case
				          // of the key =, as that is the terminator)

					name_length=0;
					arglen=strlen(argv[current_arg]); //Get the length of the whole argument
					while(argv[current_arg][2+name_length]!='='){
						//Iterate over the current argument, starting at 2 (first two bytes are -p)
						//and count the length of the key portion of the <key>=<value> pair
						++name_length;
						if(name_length>=arglen){
							//Skip out on this if there isn't a = anywhere
							//NOTE: it is entirely value to set a key to an empty string, and this
							// would be accomplished with -pfoo= , and you'd end up with the mapping
							// foo => ''
							goto invalid_parameter;
						}
					}

					name_temp=malloc(name_length+1);
					//Create the storage to store just the name (+ a null byte) for in the map

					strncpy(name_temp, argv[current_arg]+2, name_length);
					//Copy the name into the new buffer (incl trailing null), starting at +2 as
					// first two bytes are -p

					Map_setstr(app_config->config, name_temp, argv[current_arg]+2+name_length+1);
					//Do the actual argument set. Offset for the value is...
					// argv[current_value]	:	Base position of the whole string
					// +2 					:	Skip the -p at the start of the arg
					// +name_length 		:	Skip forward past the name, to the value
					// +1					:	Skip the = sign
					//NOTE: No type transformation is preformed, and all values are stored as strings
					// (this means that setting `-pfoo=1`, and then doing 
					//  Map_getint(app_config->config, 'foo') DOSENT EQUAL (int)1
					//  and is probably segfault-causing)

					LOG_SPAM("Set parameter `%s` -> `%s`\n", name_temp, argv[current_arg]+2+name_length+1);
					break;

				case 'P': //Set a flag to the literal string '1' (for usage as flags)
					Map_setstr(app_config->config, argv[current_arg]+2, "1");
					//Set the argument. Offset is +2 to skip the -P at the start of the arg

					LOG_SPAM("Set parameter `%s` -> `1` (-P directive)\n", argv[current_arg]+2);
					break;

				default:
					LOG_WARN("Invalid option (ignored): %s\n", argv[current_arg]);
			}
		}
		++current_arg;
		//Advance an argument. NOTE: This counter may actually move forward during the loop body
		// as well as here, as a result of a consume_next
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

	//Check that we have both an input and output when we're operating
	//exception being that if we are compressing with -d set (don't save)
	//it's ok to lack an output

	if(intent->intent==INTENT_COMPRESS || intent->intent==INTENT_DECOMPRESS){
		if(strlen(intent->infile)==0){
			LOG_ERROR("Specified to compress/decompress without an input file.\n")
			goto fail;
		}

		if (!(intent->intent==INTENT_COMPRESS && !intent->do_compress)){
			if(strlen(intent->outfile)==0){
				LOG_ERROR("Specified to compress (w/o -d) /decompress without an output file.\n")
				goto fail;
			}
		}
	}

	if(intent->intent==0){
		LOG_FAIL("No operation\n")
		goto fail;
	}

	return intent;

	//Now, before y'all knock the goto I think that the flow of this (horribly large) function
	// is massivley simplified by this little construct here, as opposed to having the error,
	// intent invalidation and return stuff in every possible failure point

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

void display_help(Map *extensions){
	printf(
		"compress - A huffman coding codec + tree generator\n"
		"\n"
		"Base Usage: compress [-v[v...]|-f<n>|-q] [-p<key>=<value> ...] [-P<key>] (-h|{command})\n"
		"  -v[v][v][v][v]  : Set loglevel to lower levels depending on the number of vs\n"
		"                        (i.e. v=STATUS, vv=DEBUG, vvv=SPAM, vvvv=ALLOC)\n"
		"  -f<n>           : Sets loglevel to <n> where INFO=5, ALLOC=1, FAIL=8\n"
		"  -q              : Disables logging. Equivilent to -f0\n"
		"  -p<key>=<value> : Sets configuration parameter <key> to <value>. Both are strings. An = is required.\n"
		"  -P<key>         : Sets configuration parameter <key> to \"1\". Equivilent to -p<key>=1\n"
		"  -h              : Show this fustercluck\n"
		"\n"
		"Commands:\n"
		"Compress: -c INFILE (-a<algorithm>|-l IMPORTFILE) [-s SAVEFILE] [-O] (-d|-o OUTFILE)\n"
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
		"Installed algorithms: "
	);
	if(extensions->head!=0){
		MapElement *ele=extensions->head;
		while(ele->next!=0){
			printf(ele->key);
			if(ele->next->next!=0){
				printf(", ");
			}
			ele=ele->next;
		}
	}else{
		printf("<NONE!>");
	}
	printf(
		"\n\n(>) Copyleft Louis Goessling 2016. Good luck!\n"
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