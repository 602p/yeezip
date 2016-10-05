#include <stdio.h>
#include <string.h>
#include "log.h"

#define ASSOCIATE_TEST(X, Y) if(strcmp(name, X)==0)Y();

void dispatch_test(char *name){
	ASSOCIATE_TEST("extension", test_extensions)
	ASSOCIATE_TEST("map", test_map)
	ASSOCIATE_TEST("tree", test_tree)
	ASSOCIATE_TEST("mapfunc", test_map_functions)
	ASSOCIATE_TEST("mmap", test_mmap)
	ASSOCIATE_TEST("bit", test_bitmanipulation)
}