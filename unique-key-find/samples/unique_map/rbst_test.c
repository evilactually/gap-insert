#include "rbst.h"
#include "unity_fixture.h"
#include "string.h" // memcpy
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

TEST_GROUP(RBST);

TEST_SETUP(RBST)
{

}

TEST_TEAR_DOWN(RBST)
{

};


// When only one gap available, it is chosen.
// When gaps available on both sides, the tie is resolved by random bias
// 1 - signifies right, 0 left
TEST(RBST, DirectionSelection)
{
  // this boolean function is used inside gap insert implementation 
  #define DIRECTION(bias, gaps_left, gaps_right) (!bias && !(gaps_left > 0)) || (bias && (gaps_right > 0))

  // it's behaviour is defined here:

  // one gap, bias ignored
  TEST_ASSERT_EQUAL_INT(DIRECTION(0,0,1),1);
  TEST_ASSERT_EQUAL_INT(DIRECTION(0,1,0),0);
  TEST_ASSERT_EQUAL_INT(DIRECTION(1,0,1),1);
  TEST_ASSERT_EQUAL_INT(DIRECTION(1,1,0),0);
  
  // two gaps, braking the tie
  TEST_ASSERT_EQUAL_INT(DIRECTION(0,1,1),0);
  TEST_ASSERT_EQUAL_INT(DIRECTION(1,1,1),1);

  // this case shouldn't happen, but it's defined
  TEST_ASSERT_EQUAL_INT(DIRECTION(1,0,0),0); 
}

TEST(RBST, TreeCreate)
{
  struct RBST* tree = rbst_create(0, 10);
  TEST_ASSERT_EQUAL_INT(tree->min, 0);
  TEST_ASSERT_EQUAL_INT(tree->max, 10);
  TEST_ASSERT_EQUAL_INT(tree->root, NULL); 
}

struct RBSTNode* create_node(int key, void* value);


// case #1 - remove hard root
//       6(x)      
//      / \      
//     4   8    
TEST(RBST, RemoveHardRoot)
{
  const int A = 0;
  const int B = 15; 
  
  int values[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};

  struct RBST* tree = rbst_create(A, B);

  tree->root = create_node(6, &values[5]);
  tree->root->count = 3;
  tree->root->links[0] = create_node(4, &values[3]);
  tree->root->links[1] = create_node(8, &values[7]);
  rbst_remove(tree, 6);

  TEST_ASSERT_EQUAL_INT(2, rbst_count(tree));

  // one of the links got deleted, but not both
  TEST_ASSERT_TRUE(tree->root->links[0] == NULL ^ tree->root->links[1] == NULL);

  // counts have been updated and nodes have expected values
  if(tree->root->links[0])
  {
  	TEST_ASSERT_EQUAL_INT(8, tree->root->key);
    TEST_ASSERT_EQUAL_INT(4, tree->root->links[0]->key);
    TEST_ASSERT_EQUAL_INT(1, tree->root->links[0]->count);
  } else
  {
  	TEST_ASSERT_EQUAL_INT(4, tree->root->key);
  	TEST_ASSERT_EQUAL_INT(8, tree->root->links[1]->key);
    TEST_ASSERT_EQUAL_INT(1, tree->root->links[1]->count);
  }
}

// case #2 - remove hard right sub-tree
//       6      
//      / \      
//     4   8(x)
//        / \
//       7   9
TEST(RBST, RemoveHardChildRight)
{
  const int A = 0;
  const int B = 15; 
  
  int values[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};

  struct RBST* tree = rbst_create(A, B);

  tree->root = create_node(6, &values[5]);
  tree->root->count = 5;
  tree->root->links[0] = create_node(4, &values[3]);
  tree->root->links[1] = create_node(8, &values[7]);
  tree->root->links[1]->count = 2; 
  tree->root->links[1]->links[0] = create_node(7, &values[6]);
  tree->root->links[1]->links[1] = create_node(9, &values[8]);
  rbst_remove(tree, 8);

  struct RBSTNode* eight = tree->root->links[1];
  
  TEST_ASSERT_EQUAL_INT(4, rbst_count(tree));
  TEST_ASSERT_EQUAL_INT(1, eight->count);

  TEST_ASSERT_TRUE(eight->links[0] == NULL ^ eight->links[1] == NULL);

  if(eight->links[0])
  {
   	TEST_ASSERT_EQUAL_INT(9, eight->key);
    TEST_ASSERT_EQUAL_INT(7, eight->links[0]->key);
    TEST_ASSERT_EQUAL_INT(1, eight->links[0]->count);
    TEST_ASSERT_NULL(eight->links[1]);
  } else
  {
    TEST_ASSERT_EQUAL_INT(7, eight->key);
    TEST_ASSERT_EQUAL_INT(9, eight->links[1]->key);
    TEST_ASSERT_EQUAL_INT(1, eight->links[1]->count);
    TEST_ASSERT_NULL(eight->links[0]);
  }
}

// case #3 - remove hard left sub-tree 
//     11   
//    /  \  
//   8(x)13
//  / \
// 7   9
TEST(RBST, RemoveHardChildLeft)
{
  const int A = 0;
  const int B = 15; 
  
  int values[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};

  struct RBST* tree = rbst_create(A, B);


  tree->root = create_node(11, &values[10]);
  tree->root->count = 5;
  tree->root->links[0] = create_node(8, &values[7]);
  tree->root->links[1] = create_node(13, &values[12]);
  tree->root->links[0]->count = 2; 
  tree->root->links[0]->links[0] = create_node(7, &values[6]);
  tree->root->links[0]->links[1] = create_node(9, &values[8]);
  rbst_remove(tree, 8);

  struct RBSTNode* eight = tree->root->links[0];
  
  TEST_ASSERT_EQUAL_INT(4, rbst_count(tree));
  TEST_ASSERT_EQUAL_INT(1, eight->count);

  TEST_ASSERT_TRUE(eight->links[0] == NULL ^ eight->links[1] == NULL);

  if(eight->links[0])
  {
   	TEST_ASSERT_EQUAL_INT(9, eight->key);
    TEST_ASSERT_EQUAL_INT(7, eight->links[0]->key);
    TEST_ASSERT_EQUAL_INT(1, eight->links[0]->count);
    TEST_ASSERT_NULL(eight->links[1]);
  } else
  {
    TEST_ASSERT_EQUAL_INT(7, eight->key);
    TEST_ASSERT_EQUAL_INT(9, eight->links[1]->key);
    TEST_ASSERT_EQUAL_INT(1, eight->links[1]->count);
    TEST_ASSERT_NULL(eight->links[0]);
  }
}

TEST(RBST, GapInsertion)
{
  const int A = 0;
  const int B = 5; 
  
  struct RBST* tree = rbst_create(A, B);

  int k;

  int values[] = {1,2,3,4};
 
  // insert 4 elements
  for (int i = A+1; i < B; i++)
  {
    TEST_ASSERT_EQUAL_INT((B-A) - i, rbst_gap_count(tree));
    TEST_ASSERT_EQUAL_INT(i-1, rbst_count(tree));
  	k = rbst_gap_insert(tree, &values[i]);
    TEST_ASSERT_TRUE(k > A && k < B);
    TEST_ASSERT_TRUE(rbst_find(tree, k) == &values[i]);
    TEST_ASSERT_EQUAL_INT((B-A) - i - 1, rbst_gap_count(tree));
    TEST_ASSERT_EQUAL_INT(i, rbst_count(tree));
  }
}

int* global_keys;

TEST(RBST, ForEach)
{
  const int A = 0;
  const int B = 15; 
  struct RBST* tree = rbst_create(A, B);
  int value = 0xFF;
  
  int k = 0;
  while(rbst_gap_count(tree) > 0) {
    rbst_gap_insert(tree, &value);
    k++;
  }
  
  k = A;
  void test_pair(int key, void* value)
  { 
    TEST_ASSERT_EQUAL_INT(++k, key);
  }

  rbst_for_each(tree, test_pair);
}

// TODO: test traversal functions

TEST_GROUP_RUNNER(RBST)
{
  RUN_TEST_CASE(RBST, DirectionSelection);
  RUN_TEST_CASE(RBST, GapInsertion);
  RUN_TEST_CASE(RBST, RemoveHardRoot);
  RUN_TEST_CASE(RBST, RemoveHardChildLeft);
  RUN_TEST_CASE(RBST, RemoveHardChildRight);
  RUN_TEST_CASE(RBST, ForEach);
}