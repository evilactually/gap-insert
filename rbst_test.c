#include "rbst.h"
#include "unity_fixture.h"
#include "string.h" // memcpy
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// exposed privite functions
struct RBSTNode* create_node(int key, void* value);

struct RBST* stdtree;
char stdtree_inord[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};
char stdtree_postord[] = {'A', 'C', 'E', 'D', 'B', 'H', 'I', 'G', 'F'};
char stdtree_preord[] = {'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H'};

void init_stdtree()
{
  stdtree = rbst_create(stdtree_inord[0], stdtree_inord[8]);
  stdtree->root = create_node(stdtree_inord[5], &stdtree_inord[5]);

  stdtree->root->count = 9;
  stdtree->root->links[0] = create_node(stdtree_inord[1], &stdtree_inord[1]);
  stdtree->root->links[1] = create_node(stdtree_inord[6], &stdtree_inord[6]);

  stdtree->root->links[0]->count = 5;
  stdtree->root->links[0]->links[0] = create_node(stdtree_inord[0], &stdtree_inord[0]);
  stdtree->root->links[0]->links[1] = create_node(stdtree_inord[3], &stdtree_inord[3]);

  stdtree->root->links[0]->links[1]->count = 3;
  stdtree->root->links[0]->links[1]->links[0] = create_node(stdtree_inord[2], &stdtree_inord[2]);
  stdtree->root->links[0]->links[1]->links[1] = create_node(stdtree_inord[4], &stdtree_inord[4]);

  stdtree->root->links[1]->count = 3;
  stdtree->root->links[1]->links[1] = create_node(stdtree_inord[8], &stdtree_inord[8]);
  stdtree->root->links[1]->links[1]->links[0] = create_node(stdtree_inord[7], &stdtree_inord[7]);
}

void free_stdtree()
{
  rbst_free(stdtree);
}

void fill_up_partial(struct RBST* tree, float fraction, void* value)
{
  int k;
  while(rbst_count(tree) < (int)(((tree->max-tree->min)-1)*fraction)) {
    rbst_gap_insert(tree, value, &k);
  }
}

void verify_order(struct RBST* rbst)
{
  void node_verify(struct RBSTNode* node)
  {
    if(node->links[0] != NULL)
      TEST_ASSERT_TRUE(node->links[0]->key < node->key);

    if(node->links[1] != NULL)
      TEST_ASSERT_TRUE(node->links[1]->key > node->key);
  }
  rbst_preord_apply(rbst, node_verify);
}

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
  
  // two gaps, breaking the tie
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

  verify_order(tree);
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

  verify_order(tree);
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

  verify_order(tree);
}

TEST(RBST, GapInsertion)
{
  const int A = 5;
  const int B = 100;
  const int SIZE = B - A - 1;
  
  struct RBST* tree = rbst_create(A, B);

  int values[SIZE];
  int i = 0;
  int k = 0;
  int gaps = SIZE;
  int size = 0;

  while(rbst_gap_count(tree) > 0) {

    TEST_ASSERT_EQUAL_INT(gaps, rbst_gap_count(tree));
    TEST_ASSERT_EQUAL_INT(size, rbst_count(tree));

    values[i] = i*1000;
    rbst_gap_insert(tree, &values[i], &k);

    gaps--;
    size++;

    TEST_ASSERT_TRUE(k > A && k < B);
    TEST_ASSERT_EQUAL_INT(values[i], *(int*)rbst_find(tree, k));

    TEST_ASSERT_EQUAL_INT(gaps, rbst_gap_count(tree));
    TEST_ASSERT_EQUAL_INT(size, rbst_count(tree));

    verify_order(tree);

    i++;
  }
}

TEST(RBST, ForEach)
{
  const int A = 0;
  const int B = 15; 
  struct RBST* tree = rbst_create(A, B);
  int value = 0xFF;
  
  fill_up_partial(tree, 1.0f, &value);
  
  int k = A;
  void test_pair(int key, void* value)
  { 
    TEST_ASSERT_EQUAL_INT(++k, key);
  }

  rbst_for_each(tree, test_pair);
}

TEST(RBST, InOrdTraversal)
{
  init_stdtree();

  int i = 0;
  void verify_node(struct RBSTNode* node)
  {
    TEST_ASSERT_EQUAL_INT(stdtree_inord[i++], node->key);
  }

  rbst_inord_apply(stdtree, verify_node);

  free_stdtree();
}

TEST(RBST, PostOrdTraversal)
{
  init_stdtree();

  int i = 0;
  void verify_node(struct RBSTNode* node)
  {
    TEST_ASSERT_EQUAL_INT(stdtree_postord[i++], node->key);
  }

  rbst_postord_apply(stdtree, verify_node);

  free_stdtree();
}

TEST(RBST, PreOrdTraversal)
{
  init_stdtree();

  int i = 0;
  void verify_node(struct RBSTNode* node)
  {
    TEST_ASSERT_EQUAL_INT(stdtree_preord[i++], node->key);
  }

  rbst_preord_apply(stdtree, verify_node);

  free_stdtree();
}

#define NEGATIVE(x) (~(x)+1)


#include <limits.h>
TEST(RBST, IntegerLimits)
{
  int a = 5;
  int b = 8;
  //unsigned int difference = (unsigned int)b + NEGATIVE((unsigned int)a);
  unsigned int difference = (unsigned int)b + (unsigned int)(-a);
                                                          // ^
                                                          // |_ flip sign before casting to unsigned

  printf("======%u\n", difference);
  printf("%u %u\n", 1231 >> 1, 1231 / 2);

  // for (int i = 0; i < 50000; ++i)
  // {
  //    TEST_ASSERT_EQUAL_INT(i >> 1, i/2);
  // }



  //printf("******%u\n", keys_between(2,3));

  struct RBST* tree = rbst_create(INT_MIN, INT_MAX);

  unsigned long inserted_count = 0;
  
  const unsigned long expected_inserted_count = (unsigned long)INT_MAX + NEGATIVE((unsigned long)INT_MIN) + 1;
  printf("%lu\n", expected_inserted_count);

  printf("Integer limits: %d %d\n", INT_MIN, INT_MAX);
  printf("Number of keys possible: %lu\n", expected_inserted_count);

  int key = 0;
  while(rbst_gap_insert(tree, &key, &key) != 0) {
    printf("%d\n", key);
    inserted_count++;
  }

  // for (int i = INT_MIN; i <= INT_MAX; ++i)
  // {
  //   //rbst_gap_insert(tree);
  //   printf("%d\n", i);
  // }

  rbst_free(tree);
}

#include <limits.h>
int middle(int a, int b);

TEST(RBST, Middle) {
  TEST_ASSERT_EQUAL_UINT((unsigned int)INT_MAX-(unsigned int)INT_MIN,UINT_MAX);
  TEST_ASSERT_EQUAL_INT(middle(INT_MIN, INT_MAX), -1);

  // Explanation:
  // 2147483647 -(-2147483648) = 4294967295
  // INT_MAX      INT_MIN         UINT_MAX 
  // 4294967295 >> 1 = 2147483647
  // (-2147483648) + 2147483647 = -1

  TEST_ASSERT_EQUAL_INT(middle(1, 5), 3);
  TEST_ASSERT_EQUAL_INT(middle(1, 4), 2);
  TEST_ASSERT_EQUAL_INT(middle(-5, -1), -3);
  TEST_ASSERT_EQUAL_INT(middle(-4, -1), -3);
  TEST_ASSERT_EQUAL_INT(middle(-8, -1), -5);

}

unsigned int keys_between(int a, int b);

TEST(RBST, KeysBetween) {
  TEST_ASSERT_EQUAL_INT(14, keys_between(-8,7));
  TEST_ASSERT_EQUAL_INT(3, keys_between(1,5));
  TEST_ASSERT_EQUAL_UINT(UINT_MAX-1, keys_between(INT_MIN,INT_MAX));
}

TEST_GROUP_RUNNER(RBST)
{
  // RUN_TEST_CASE(RBST, DirectionSelection);
  // RUN_TEST_CASE(RBST, GapInsertion);
  RUN_TEST_CASE(RBST, RemoveHardRoot);
  RUN_TEST_CASE(RBST, RemoveHardChildLeft);
  RUN_TEST_CASE(RBST, RemoveHardChildRight);
  // RUN_TEST_CASE(RBST, ForEach);
  // RUN_TEST_CASE(RBST, InOrdTraversal);
  // RUN_TEST_CASE(RBST, PostOrdTraversal);
  // RUN_TEST_CASE(RBST, PreOrdTraversal);
  // RUN_TEST_CASE(RBST, IntegerLimits);
  // RUN_TEST_CASE(RBST, Middle);
  RUN_TEST_CASE(RBST, KeysBetween);
}