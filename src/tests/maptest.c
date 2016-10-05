#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "map.h"

void test_map(){
	printf("Creating map...\n");

	Map *map=Map_create();

	printf("Map has name? %i\n", Map_has(map, "name"));

	printf("Inserting...\n");
	Map_setstr(map, "name", "Louis Goessling");
	Map_setstr(map, "homecity", "Minneapolis");
	Map_setstr(map, "foo", "bar");

	int *test=malloc(sizeof(int));
	*test=3;
	printf("Test=%i\n", *test);

	Map_set(map, "test", test);

	printf("Map has name? %i\n", Map_has(map, "name"));
	printf("Map has name_bad? %i\n", Map_has(map, "name_bad"));

	printf("Map @ name: %s\n", Map_getstr(map, "name"));
	printf("Map @ homecity: %s\n", Map_getstr(map, "homecity"));
	printf("Map @ foo: %s\n", Map_getstr(map, "foo"));
	printf("Map @ test: %d\n", *(int*)Map_get(map, "test"));

	printf("Setting name=Bob Fibnitz\n");
	Map_setstr(map, "name", "Bob Fibnitz");

	printf("Setting foo=baz\n");
	Map_setstr(map, "foo", "baz");

	printf("Map @ name: %s\n", Map_getstr(map, "name"));
	printf("Map @ homecity: %s\n", Map_getstr(map, "homecity"));
	printf("Map @ foo: %s\n", Map_getstr(map, "foo"));
	printf("Map @ test: %d\n", *(int*)Map_get(map, "test"));

	printf("Deleting foo\n");
	Map_del(map, "foo");
	printf("Has foo? %i\n", Map_has(map, "foo"));
	printf("Map @ test: %d\n", *(int*)Map_get(map, "test"));

	printf("Deleting test\n");
	Map_del(map, "test");
	printf("Has test? %i\n", Map_has(map, "test"));

	printf("Deleting name\n");
	Map_del(map, "name");
	printf("Has name? %i\n", Map_has(map, "name"));
	printf("Map @ homecity: %s\n", Map_getstr(map, "homecity"));

	Map_setint(map, "int", 3);
	printf("int=%i\n", Map_getint(map, "int"));
	Map_del(map, "int");
	printf("int freed\n");
}