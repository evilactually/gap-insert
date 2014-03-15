#include "uniquemap.h"

#include "unity_fixture.h"
#include "string.h" // memcpy
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

TEST_GROUP(UniqueMap);

TEST_SETUP(UniqueMap)
{

}

TEST_TEAR_DOWN(UniqueMap)
{

};

TEST(UniqueMap, MapCreate)
{
  struct UniqueMap* map = create_unique_map(0, 10);
  TEST_ASSERT_NOT_NULL(map);
  destroy_unique_map(map);
}

TEST(UniqueMap, MapPutUnique)
{
  const int A = 0;
  const int B = 4;

  struct UniqueMap* map = create_unique_map(A, B);
  int values[] = {1,2,3};
  int k[3];
  k[0] = map_put_unique(map, &values[0]);
  TEST_ASSERT_TRUE(k[0] > A && k[0] < B);

  k[1] = map_put_unique(map, &values[1]);
  TEST_ASSERT_TRUE(k[1] > A && k[1] < B);
  TEST_ASSERT_NOT_EQUAL(k[0], k[1]);
  
  k[2] = map_put_unique(map, &values[2]);
  TEST_ASSERT_TRUE(k[2] > A && k[2] < B);
  TEST_ASSERT_NOT_EQUAL(k[0], k[2]);
  TEST_ASSERT_NOT_EQUAL(k[1], k[2]);

  TEST_ASSERT_EQUAL_INT(0, map_put_unique(map, &values[2]));

  destroy_unique_map(map);
}

TEST(UniqueMap, MapGet)
{
  struct UniqueMap* map = create_unique_map(0, 10);
  int values[] = {1,2,3};
  int k[3];
  k[0] = map_put_unique(map, &values[0]);
  k[1] = map_put_unique(map, &values[1]);
  k[2] = map_put_unique(map, &values[2]);

  TEST_ASSERT_TRUE(map_get(map, k[0]) == &values[0]);
  TEST_ASSERT_TRUE(map_get(map, k[1]) == &values[1]);
  TEST_ASSERT_TRUE(map_get(map, k[2]) == &values[2]);
  TEST_ASSERT_TRUE(map_get(map, k[2]) == &values[2]); // cached result

  destroy_unique_map(map);
}

TEST(UniqueMap, MapContains)
{
  const int A = 0;
  const int B = 4;
  int value = 0xFF;
  struct UniqueMap* map = create_unique_map(A, B);
  int k[3];

  TEST_ASSERT_FALSE(map_contains(map, 1));
  TEST_ASSERT_FALSE(map_contains(map, 2));
  TEST_ASSERT_FALSE(map_contains(map, 3));

  k[0] = map_put_unique(map, &value);
  k[1] = map_put_unique(map, &value);
  k[2] = map_put_unique(map, &value);
  
  TEST_ASSERT_TRUE(map_contains(map, k[0]));
  TEST_ASSERT_TRUE(map_contains(map, k[1]));
  TEST_ASSERT_TRUE(map_contains(map, k[2]));
  TEST_ASSERT_TRUE(map_contains(map, k[2])); // cached result

  destroy_unique_map(map);
}

TEST(UniqueMap, MapRemove)
{
  struct UniqueMap* map = create_unique_map(0, 10);
  int values[] = {1,2,3};
  int k[3];

  k[0] = map_put_unique(map, &values[0]);
  k[1] = map_put_unique(map, &values[1]);
  k[2] = map_put_unique(map, &values[2]);

  TEST_ASSERT_TRUE(map_contains(map, k[2])); // hit cache
  TEST_ASSERT_TRUE(map_remove(map, k[2]) == &values[2]);
  TEST_ASSERT_FALSE(map_contains(map, k[2]));

  TEST_ASSERT_TRUE(map_remove(map, k[0]) == &values[0]); // uncached
  TEST_ASSERT_FALSE(map_contains(map, k[0]));

  TEST_ASSERT_NULL(map_remove(map, k[0])); // superfluous remove

  destroy_unique_map(map);
}

TEST(UniqueMap, MapSize)
{
  struct UniqueMap* map = create_unique_map(0, 10);
  int value = 0xFF;
    
  TEST_ASSERT_EQUAL_INT(0, map_size(map));
  map_put_unique(map, &value);
  TEST_ASSERT_EQUAL_INT(1, map_size(map));
  map_put_unique(map, &value);
  TEST_ASSERT_EQUAL_INT(2, map_size(map));
  map_put_unique(map, &value);
  TEST_ASSERT_EQUAL_INT(3, map_size(map));

  destroy_unique_map(map);
}

TEST(UniqueMap, MapSpace)
{
  struct UniqueMap* map = create_unique_map(0, 5);
  int value = 0xFF;

  TEST_ASSERT_EQUAL_INT(4, map_space(map));
  map_put_unique(map, &value);
  TEST_ASSERT_EQUAL_INT(3, map_space(map));
  map_put_unique(map, &value);
  TEST_ASSERT_EQUAL_INT(2, map_space(map));
  map_put_unique(map, &value);
  TEST_ASSERT_EQUAL_INT(1, map_space(map));
  map_put_unique(map, &value);
  TEST_ASSERT_EQUAL_INT(0, map_space(map));

  destroy_unique_map(map);
}

TEST_GROUP_RUNNER(UniqueMap)
{
  RUN_TEST_CASE(UniqueMap, MapCreate);
  RUN_TEST_CASE(UniqueMap, MapPutUnique);
  RUN_TEST_CASE(UniqueMap, MapGet);
  RUN_TEST_CASE(UniqueMap, MapContains);
  RUN_TEST_CASE(UniqueMap, MapRemove);
  RUN_TEST_CASE(UniqueMap, MapSize);
  RUN_TEST_CASE(UniqueMap, MapSpace);
}