#include <stdio.h>
#include "codingtree.h"
#include "map.h"
#include "args.h"
#include "log.h"
#include <python3.4/Python.h>

#define LOG_REGION "pyiface"

char *ext_get_name(){
	return "pyiface";
}

TreeNode *ext_build_tree(AppConfig *parent_config, int *freqtable){
	app_config=parent_config;
	LOG_STATUS("***Starting up the python interpreter***\n");

	Py_Initialize();

	LOG_DEBUG("Building app_arguments python object\n");
	PyObject *app_arguments = PyDict_New();
	LOG_SPAM("Entering mapping loop\n");
	MapElement *elem;
	if(app_config->config->head!=0){
		elem=app_config->config->head;
		while(elem!=0){
			LOG_SPAM("Looping...\n");
			LOG_SPAM("Setting %s=%s\n", elem->key, (char*)elem->value);
			PyDict_SetItemString(app_arguments, elem->key, Py_BuildValue("s", (char*)elem->value));
			elem=elem->next;
		}
	}
	PyObject *main_module = PyImport_ImportModule("__main__");
    PyObject_SetAttrString(main_module, "yee_extension_parameters", app_arguments);

    //RUN SOME CODE

	FILE *fp_hdr=fopen("./extentions/yeezip.py", "r");
	if(fp_hdr==0){
		LOG_FAIL("Failed to open extentions interface header for the python interpreter\n");
		return 0;
	}
	PyRun_SimpleFile(fp_hdr, "./extentions/yeezip.py");
	fclose(fp_hdr);
	FILE *fp=fopen("./extentions/python_test.py", "r");
	if(fp==0){
		LOG_FAIL("Failed to open extention file\n");
		return 0;
	}
	PyRun_SimpleFile(fp, "./extentions/python_test.py");
	fclose(fp);

	//END RUNNING CODE

	PyObject *bytes_obj=PyObject_GetAttrString(main_module, "yee_extension_exported_serialized_tree");
	TreeNode *tree=0;
	if(PyBytes_Check(bytes_obj)){
    	char *buf=PyBytes_AsString(bytes_obj);
    	LOG_SPAM("Building tree...\n");
    	tree=Tree_loadfrombuf((byte*)buf);
    }else{
    	LOG_FAIL("yee_extension_exported_serialized_tree is not a bytes\n");
    }

    printf("\n");

	Py_Finalize();

	return tree;
}

#undef LOG_REGION