#include <stdio.h>
#include "codingtree.h"
#include "map.h"
#include "args.h"
#include "log.h"
#include <python3.4/Python.h>

#define LOG_REGION "extpy"

char *ext_get_name(){
	return "pytest";
}

TreeNode *ext_build_tree(AppConfig *parent_config, int *freqtable){
	app_config=parent_config;
	LOG_INFO("***Trying python!***\n");

	Py_Initialize();
	PyObject *main_module = PyImport_ImportModule("__main__");
    PyObject *main_dict   = PyModule_GetDict(main_module);
    long i_value = 5;
    PyObject *i_obj=PyLong_FromLong(i_value);
    PyDict_SetItemString(main_dict, "i", i_obj);
	PyRun_SimpleString("print('I=%i' % i)\n"
					   "i+=1\n"
					   "print('I=%i' % i)\n"
					   "k=[7,8,9]\n");
	PyObject *k_obj = PyMapping_GetItemString(main_dict, "k");
    long k = PyLong_AsLong(k_obj);
    Py_XDECREF(k_obj);
    Py_XDECREF(i_obj);
    if(PySequence_Check(k_obj)){
    	int i=0;
    	while(i<PySequence_Length(k_obj)){
    		PyObject *item=PySequence_GetItem(k_obj, i);
    		printf("%d ", PyLong_AsLong(item));
    		Py_XDECREF(item);
    		i++;
    	}
    }
    printf("\n");
	Py_Finalize();

	TreeNode *tree=TreeNode_create(256);
	int i=0;
	while(i<256){
		TreeNode_attach(tree, TreeNode_create_leaf(i), i);
		i++;
	}
	return tree;
}

#undef LOG_REGION