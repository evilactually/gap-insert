#include <assert.h>
#include <stdlib.h>
#include "common.h"
#include "uniquemap.h"
#include "rbst.h"

struct UniqueMap {
  struct RBST* rbst;
  void* cached_value;
  int cached_key;
};

struct UniqueMapIterator; 

static inline void cache_reset(struct UniqueMap* map)
{
  map->cached_key = map->rbst->min;
}

struct UniqueMap* create_unique_map(int min, int max)
{
  struct UniqueMap* new_map = new(struct UniqueMap);
  new_map->rbst = rbst_create(min, max);
  new_map->cached_key = min;
  return new_map;
}

void destroy_unique_map(struct UniqueMap* map)
{
  rbst_free(map->rbst);
  free(map);
}

int map_put_unique(struct UniqueMap* map, void* value)
{
  int key = 0;
  if(rbst_gap_insert(map->rbst, value, &key) > 0)
  {
    map->cached_key = key;
    map->cached_value = value;
  }
  return key;
}

void* map_get(struct UniqueMap* map, int key)
{
  assert(key > map->rbst->min && key < map->rbst->max);

  if(map->cached_key == key)
  {
    return map->cached_value;
  } else
  {
    void* found_value = rbst_find(map->rbst, key);
    if(found_value)
    {
      map->cached_key = key;
      map->cached_value = found_value;
    }
    return found_value;
  }
}

int map_contains(struct UniqueMap* map, int key)
{
  assert(key > map->rbst->min && key < map->rbst->max);

  if(map->cached_key == key)
  {
    return 1;
  } else
  {
    void* found_value = rbst_find(map->rbst, key);
    if(found_value)
    {
      map->cached_key = key;
      map->cached_value = found_value;
    }
    return found_value != NULL;
  }
}

void* map_remove(struct UniqueMap* map, int key)
{
  assert(key > map->rbst->min && key < map->rbst->max);

  void* removed_value = rbst_remove(map->rbst, key);

  if(key == map->cached_key && removed_value)
  {
    cache_reset(map);
  }

  return removed_value;
}

int map_size(struct UniqueMap* map)
{
  return rbst_count(map->rbst);
}

int map_space(struct UniqueMap* map)
{
  return rbst_gap_count(map->rbst);
}
