#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include "extension.h"
#include "map.h"
#include "log.h"

#define LOG_REGION "extmgr"

void import_lib(char *path){
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
		Map_set(treebuilder_extensions, name, build_tree);
		LOG_DEBUG("Registered extension `%s`\n", name);
	}else{
		LOG_DEBUG(":(\n");
		LOG_ERROR("Invalid library %s\n", path);
	}
	// dlclose(lib); //Unloads functions if called
}

void init_extension_manager(){
	treebuilder_extensions=Map_create();
}

void load_extensions(){
	DIR *d;
	struct dirent *dir;
	d = opendir(EXTENSION_PATH);
	if (d){
		while ((dir = readdir(d)) != NULL){
			if(dir->d_name[0]!='.'){
				char *fullname=malloc(strlen(EXTENSION_PATH)+strlen(dir->d_name)+2);
				strcpy(fullname, EXTENSION_PATH);
				strcat(fullname, "/");
				strcat(fullname, dir->d_name);
				import_lib(fullname);
				// free(fullname);
			}
		}
		closedir(d);
	}
}

#undef LOG_REGION