#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include "codingtree.h"
#include "map.h"
#include "log.h"
#include "main.h"

#define EXTENSION_PATH "extentions"

typedef TreeNode* treebuilder_sig(AppConfig *, char*); //config, char* text, Map* options
typedef char* get_name_sig();

Map *treebuilder_extensions;

void init_extension_manager();
void load_extensions();

#endif