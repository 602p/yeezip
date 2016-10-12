#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "main.h"
#include "log.h"

#define LOG_REGION "map"

Map *Map_create(){
	Map *map = malloc(sizeof(Map));
	map->head=0;
	return map;
}

void _MapElement_destroy(MapElement *ele){
	if(ele->next){
		_MapElement_destroy(ele->next);
	}
	free(ele);
}

void Map_destroy(Map *map){
	if(map->head){
		_MapElement_destroy(map->head);
	}
	free(map);
}

MapElement *_MapElement_create(char *key, void *value){
	MapElement *ele=malloc(sizeof(MapElement));
	ele->key=key;
	ele->value=value;
	ele->next=0;
	ele->owns_value=false;
	return ele;
}

MapElement *Map_gete(Map *map, char *key){
	if(map->head==0){
		return 0;
	}

	MapElement *current=map->head;
	while(strcmp(current->key, key)!=0){
		if(current->next){
			current=current->next;
		}else{
			return 0;
		}
	}

	return current;
}

void *Map_get(Map *map, char *key){
	return Map_gete(map, key)->value;
}

bool Map_has(Map *map, char *key){
	if(Map_gete(map, key)){
		return true;
	}else{
		return false;
	}
}

void Map_set(Map *map, char *key, void *value){
	MapElement *ele = Map_gete(map, key);
	if(ele){
		ele->value=value;
	}else{
		if(map->head){
			MapElement *temp = map->head;
			map->head=_MapElement_create(key, value);
			map->head->next=temp;
		}else{
			map->head=_MapElement_create(key, value);
		}
	}
}

void Map_del(Map *map, char *key){
	MapElement *current=map->head;
	MapElement *last=0;
	while(strcmp(current->key, key)!=0){
		if(current->next){
			last=current;
			current=current->next;
		}else{
			return;
		}
	}

	if(current->next){
		if(last){
			last->next=current->next;
		}else{
			map->head=current->next;
		}
	}

	if(current->owns_value){
		free(current->value);
	}

	if(map->head == current){
		map->head=0;
	}

	free(current);
}

void Map_setstr(Map *map, char *key, char *value){
	Map_set(map, key, (void*)value);
}

char *Map_getstr(Map *map, char *key){
	return (char*)Map_get(map, key);
}

_MAP_TYPEGETSET_IMPL(int, int)
_MAP_TYPEGETSET_IMPL(bool, bool)
_MAP_TYPEGETSET_IMPL(float, float)

#undef LOG_REGION