#ifdef ENABLE_TESTS
#include <criterion/criterion.h>
#include "map.h"

Test(demo, simple_success){
	cr_assert(1, "Hello World!");
}

Test(map, map_has){
	Map *map=Map_create();
	int i=42;
	Map_set(map, "test", &i);
	cr_assert(Map_has(map, "test"), "Map did not contain inserted item.");
	Map_destroy(map);
}

Test(map, map_getset_raw){
	Map *map=Map_create();
	int i=42;
	Map_set(map, "test", &i);
	int j=*((int*)Map_get(map, "test"));
	cr_assert(i==j, "Retrieved item != saved item");
	Map_destroy(map);
}

Test(map, map_delete){
	Map *map=Map_create();
	int i=42;
	Map_set(map, "test", &i);
	Map_del(map, "test");
	cr_assert(!Map_has(map, "test"), "Deleting item not destroying it!");
	cr_assert(i==42, "Deleting item modified value of it");
	Map_destroy(map);
}

Test(map, map_getset_string){
	Map *map=Map_create();
	Map_setstr(map, "test", "test value");
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(strcmp(Map_getstr(map, "test"), "test value")==0, "Retrieved!=Inserted");
	Map_destroy(map);
}

Test(map, map_getset_int){
	Map *map=Map_create();
	Map_setint(map, "test", 42);
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(Map_getint(map, "test")==42, "Retrieved!=Inserted");
	Map_destroy(map);
}

Test(map, map_getset_float){
	Map *map=Map_create();
	Map_setfloat(map, "test", 4.2f);
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(Map_getfloat(map, "test")==4.2f, "Retrieved!=Inserted");
	Map_destroy(map);
}

Test(map, map_getset_bool){
	Map *map=Map_create();
	Map_setbool(map, "test", false);
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(Map_getbool(map, "test")==false, "Retrieved!=Inserted");
	Map_destroy(map);
}

typedef int map_getset_func_sig(void);

int map_getset_func_testfunc(){
	return 1234;
}

Test(map, map_getset_func){
	Map *map=Map_create();
	Map_SETFUNC(map, "test", map_getset_func_sig, map_getset_func_testfunc);
	cr_assert(Map_has(map, "test"), "Item did not insert");
	cr_assert(Map_GETFUNC(map, "test", map_getset_func_sig)()==1234, "Called func did not return correct value");
	Map_destroy(map);
}

#endif