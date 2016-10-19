#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include "extension.h"
#include "map.h"
#include "log.h"

#define LOG_REGION "extmgr"

ExtensionData* import_lib(char *path){
	LOG_DEBUG("Loading extension `%s` -> \n",path);
	void* lib = dlopen(path, RTLD_NOW);
	if(lib == NULL){
		LOG_DEBUG(":(\n");
		LOG_ERROR("Cannot load library `%s`: %s\n", path, dlerror());
		return;
	}

	treebuilder_sig *build_tree = dlsym(lib, "ext_build_tree");
	get_name_sig *get_name = dlsym(lib, "ext_get_name");
	if(build_tree && get_name){
		char *temp=get_name();
		char *name=malloc(strlen(temp));
		strcpy(name, temp);
		LOG_DEBUG("Loaded Extension `%s`\n", name);
		ExtensionData *ret=malloc(sizeof(ExtensionData));
		ret->name=temp;
		ret->builder=build_tree;
		return ret;
	}else{
		LOG_DEBUG(":(\n");
		LOG_ERROR("Invalid library %s\n", path);
	}
	// dlclose(lib); //Unloads functions if called
}

void load_extensions(char *folder, Map* treebuilder_extensions){
	DIR *d;
	struct dirent *dir;
	d = opendir(folder);
	if (d){
		while ((dir = readdir(d)) != NULL){
			if(dir->d_name[strlen(dir->d_name)-1]=='o'){
				char *fullname=malloc(strlen(folder)+strlen(dir->d_name)+2);
				strcpy(fullname, folder);
				strcat(fullname, "/");
				strcat(fullname, dir->d_name);
				ExtensionData *d=import_lib(fullname);
				Map_set(treebuilder_extensions, d->name, d->builder);
				free(d);
			}
		}
		closedir(d);
	}
}

#undef LOG_REGION