#ifndef __MAP_H__
#define __MAP_H__

#include "main.h"

typedef struct MapElement_struct{
	char *key;
	void *value;
	bool owns_value;
	struct MapElement_struct *next;
} MapElement;

typedef struct Map_struct{
	struct MapElement_struct *head;
} Map;

Map *Map_create();
MapElement *Map_gete(Map *map, char *key);
void Map_destroy(Map *map);
void *Map_get(Map *map, char *key);
void Map_del(Map *map, char *key);
void Map_set(Map *map, char *key, void *value);
bool Map_has(Map *map, char *key);

void Map_setstr(Map *map, char *key, char *value);
char *Map_getstr(Map *map, char *key);

#define _MAP_TYPEGETSET_HEADER(TYPE, NAME) \
	void Map_set##NAME(Map *map, char *key, TYPE value);\
	TYPE Map_get##NAME(Map *map, char *key);

#define _MAP_TYPEGETSET_IMPL(TYPE, NAME) \
	void Map_set##NAME(Map *map, char *key, TYPE value){\
		TYPE *temp=malloc(sizeof(TYPE));\
		*temp=value;\
		Map_set(map, key, (void*)temp);\
		Map_gete(map, key)->owns_value=true;\
	}\
	TYPE Map_get##NAME(Map *map, char *key){\
		return *((TYPE*)Map_get(map, key));\
	}

_MAP_TYPEGETSET_HEADER(int, int)
_MAP_TYPEGETSET_HEADER(bool, bool)
_MAP_TYPEGETSET_HEADER(float, float)

#define Map_SETFUNC(map, key, SIG, FUNC) SIG *_func##FUNC##SIG = malloc(sizeof(SIG*));\
	_func##FUNC##SIG = &FUNC;\
	Map_set(map, key, _func##FUNC##SIG);

#define Map_GETFUNC(map, key, SIG) ((SIG*)Map_get(map, key))

#define Map_GETDEFAULT(map, key, default, type) (Map_has(map, key)?Map_get##type(map, key):default)

#define Map_GETDEFAULTSTR(map, key, default) Map_GETDEFAULT(map, key, default, str)
#define Map_GETDEFAULTINT(map, key, default) Map_GETDEFAULT(map, key, default, int)
#define Map_GETDEFAULTBOOL(map, key, default) Map_GETDEFAULT(map, key, default, bool)
#define Map_GETDEFAULTFLOAT(map, key, default) Map_GETDEFAULT(map, key, default, float) 

#endif