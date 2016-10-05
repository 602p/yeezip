#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "map.h"

typedef int test_sig(int);

int test_foo(int i){
	printf("Foo called, i=%i\n", i);
	return 7;
}

int test_bar(int i){
	printf("bar called, i=%i\n", i);
	return 12;
}

int test_baz(int i){
	printf("baz called, i=%i\n", i);
	return -5;
}

void test_map_functions(){
	printf("Creating map...\n");

	Map *map=Map_create();

	Map_SETFUNC(map, "foo", test_sig, test_foo);
	Map_SETFUNC(map, "bar", test_sig, test_bar);
	Map_SETFUNC(map, "baz", test_sig, test_baz);

	printf("Calling foo...\n");
	int i = Map_GETFUNC(map, "foo", test_sig)(5);
	printf("Value returned:%i\n", i);

	printf("Calling bar...\n");
	int j = Map_GETFUNC(map, "bar", test_sig)(-55);
	printf("Value returned:%i\n", j);

	printf("Calling baz...\n");
	int k = Map_GETFUNC(map, "baz", test_sig)(24);
	printf("Value returned:%i\n", k);
}