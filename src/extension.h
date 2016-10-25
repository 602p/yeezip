#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include "codingtree.h"
#include "map.h"
#include "log.h"
#include "main.h"
#include "io.h"

#define EXTENSION_PATH "./extentions"

//Extension interface definitions
typedef TreeNode* treebuilder_sig(int loglevel, Map* options, freqtable*); //config, pointer to 256 element array of byte freqencies
typedef char* get_name_sig(); //Return the name of the extension

typedef struct ExtensionData_struct{
	treebuilder_sig* builder; //Pointer to a treebuilder_sig function for doing actual work
	char* name; //Name of the extension owning ->builder
} ExtensionData;

ExtensionData* import_lib(char *path); //Load the SO at path and return a ExtensionData constructed from it
void load_extensions(char *folder, Map* treebuilder_extensions); //Load all the extensions

#endif