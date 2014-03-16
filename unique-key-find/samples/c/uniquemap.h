#ifndef UNIQUEMAP_H
#define UNIQUEMAP_H

struct UniqueMap; 

struct UniqueMapIterator; 

struct UniqueMap* create_unique_map(int min, int max);

void destroy_unique_map(struct UniqueMap* map);

int map_put_unique(struct UniqueMap* map, void* value);

void* map_get(struct UniqueMap* map, int key); 

int map_contains(struct UniqueMap* map, int key); 

void* map_remove(struct UniqueMap* map, int key);

int map_size(struct UniqueMap* map);

int map_space(struct UniqueMap* map);

#endif