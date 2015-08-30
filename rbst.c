#include "common.h"
#include "rbst.h"

#include <assert.h>
#include <stdlib.h> // NULL
#include <math.h>   // log, rand, ceil, ...

// allowable deviation from log N depth (used to allocate stack) 
#define MAX_DEPTH_FACTOR 1.2

#define NEGATIVE(x) (~(x)+1)

// INT_MAX-INT_MIN = UINT_MAX

/* helper functions */

int rnd_bool()
{
  unsigned int base_random = rand(); /* in [0, RAND_MAX] */
  return (base_random & (1)) > 0;    /* is odd? */
}

int middle(int a, int b)           // abs(a) < abs(b) => b - a > 0
{
  unsigned int difference = b - a; // has to be unsigned
                                   // since result is potentialy UINT_MAX number of a = INT_MIN and b = INT_MAX
  return (a + (difference >> 1));  // "divide" by 2 and offset by a
}

unsigned int keys_between(int a, int b)
{
  if(a == b) return 0;
  return  (b - a) - 1; // abs(a) < abs(b) => b - a > 0
}                      // Maximum UINT_MAX-1

unsigned int count(const struct RBSTNode* const node)
{
  if(!node) return 0;
  return node->count;  // Maximum UINT_MAX-1, i.e keys between min and max
}

#define IN_ORDER   0x01
#define POST_ORDER 0x02
#define PRE_ORDER  0x03

void apply_for_each(struct RBST* rbst, void (*f)(struct RBSTNode*), int order)
{
  if(!rbst->root)
    return;

  struct RBSTNode* stack[rbst->MAX_DEPTH];
  int sp = 0;
  stack[0] = rbst->root;
  int from = -1;
  do {
    // prefer left links if they are not visited
    int dir = from == 0 || stack[sp]->links[0] == NULL;
    
    // if going to right and not back from right
    if(order == IN_ORDER && dir && from < 1)
      f(stack[sp]); // in ord
    
    // if haven't visited any sub nodes yet
    if(order == PRE_ORDER && from < 0) 
      f(stack[sp]); // pre order
    
    struct RBSTNode* next = stack[sp]->links[dir];

    // if back from right or no right
    if(!next || from == 1) { 
      // record relation to parent we are returning to
      from = sp > 0 ? stack[sp]->key > stack[sp-1]->key : -1;
      
      if(order == POST_ORDER)
        f(stack[sp]); // post order
      
      // return to parent
      sp = sp - 1;
    } else {
      // not returning from anywhere, going down
      from = -1;
      stack[++sp] = next;
    }
  } while(sp > -1); // until stack is empty
}

struct RBSTNode* create_node(int key, void* value)
{
  struct RBSTNode* node = new(struct RBSTNode);
  node->links[0] = NULL;
  node->links[1] = NULL;
  node->value = value;
  node->count = 1;
  node->key = key;
  return node;
}

void remove_node(struct RBSTNode* node)
{
  free(node);
}

struct RBST* rbst_create(int min, int max)
{
  struct RBST* tree = new(struct RBST);
  tree->min = min;
  tree->max = max;
  tree->root = NULL;
  // maximum expected tree depth (used to allocate stack)
  tree->MAX_DEPTH = ceil(MAX_DEPTH_FACTOR*(log(max-min)/log(2)));
  return tree;
}

void rbst_free(struct RBST* rbst)
{
  rbst_postord_apply(rbst, remove_node);
  free(rbst);
}

/* Find a gap and insert value there returning it's key, O(log N) time
 * Return 0 if no gap was available, constant time */
int rbst_gap_insert(struct RBST* tree, void* value, int* key)
{
  assert(tree);

  // initial boundaries
  int min = tree->min;
  int max = tree->max;

  // check if tree has any gaps at all
  if((keys_between(min, max) - count(tree->root)) == 0)
  {
    return 0;
  }
  
  // handle special case when root is NULL
  if(tree->root == NULL)
  {
    *key = middle(min, max);
    tree->root = create_node(*key, value);
    return 1;
  }

  // find any node adjacent to a gap
  // store node at "it" and gap boundaries in "min", "max"
  int dir;
  struct RBSTNode* it = tree->root;
  for(;;) {
    int bias = rnd_bool();
    int gaps_left = keys_between(min, it->key) - count(it->links[0]);
    int gaps_right = keys_between(it->key, max) - count(it->links[1]);

    // select any branch having gaps, select randomly if both available
    dir = (!bias && !(gaps_left > 0)) || (bias && (gaps_right > 0));

    // apply boundary propagation rules
    if(!dir) 
      max = it->key; 
    else
      min = it->key;

    // increment actual size, since we know gap is available somewhere
    // and insertion is guaranteed to succeed along this path
    it->count++;

    // check if current node has open link on the side of gap
    if(it->links[dir] == NULL)
      break;

    // continue along branch where gaps are
    it = it->links[dir];
  }

  // select any key from the gap and create node there
  *key = middle(min, max);
  it->links[dir] = create_node(*key, value);

  return 1;
}

void* rbst_find(const struct RBST* const tree, int key)
{
  struct RBSTNode* it = tree->root;
  while(it != NULL) {
    if(it->key == key)
      return it->value;
    else
    {
      int dir = key > it->key;
      it = it->links[dir];
    }
  }
  return NULL;
}

struct stack_node_t {
  void *value;
  struct stack_node_t* next;
};

void* rbst_remove(struct RBST* tree, int key)
{
  assert(tree);

  int depth_count = 0;

  // for deletion, stack is required to postpone count updates
  // since we don't know in advance if key is in the tree
  struct RBSTNode* stack[tree->MAX_DEPTH];
  int sp = 0;
  void** stack_head = NULL;

  struct RBSTNode* target;
  
  // nothing to delete
  if(!tree->root)
   return NULL;
    
  stack[0] = tree->root;

  stack_head = alloca(sizeof(void*)*2);
  *stack_head = tree->root;
  *(stack_head+1) = NULL;

  // find node to be deleted, store it at the top of the stack
  while(stack[sp] != NULL) {
    


    if(stack[sp]->key == key)
      break;

    struct RBSTNode* next = stack[sp]->links[key > stack[sp]->key];
    stack[++sp] = next;

    void** new_stack_head = alloca(sizeof(void*)*2);
    *new_stack_head = next;
    *(new_stack_head+1) = stack_head;
    stack_head = new_stack_head;
    assert(next->key == ((struct RBSTNode*)stack_head)->key);
  }

  // nothing to remove, key not found
  if(stack[sp] == NULL)
    return NULL;
  
  // save target for later
  target = stack[sp];

  // node is considered hard case if it has two children
  int hard_case = stack[sp]->links[0] && stack[sp]->links[1];

  // find successor/predecessor for hard-case nodes and place it on top of the stack
  if(hard_case)
  {
    struct RBSTNode* replaced = stack[sp];
    int dir = rnd_bool();
    stack[++sp] = replaced->links[dir];
    while(stack[sp]->links[!dir] != NULL) {
      assert(++depth_count <= tree->MAX_DEPTH);
      struct RBSTNode* next = stack[sp]->links[!dir];
      stack[++sp] = next;
    }
  }
  
  // inherit a link from deleted node, if at has any
  struct RBSTNode* inheritance = stack[sp]->links[stack[sp]->links[1] != NULL];
  
  // if there's parent update it's link
  if(sp > 0) 
    stack[sp - 1]->links[stack[sp]->key > stack[sp - 1]->key] = inheritance;
  else // otherwise update root
    tree->root = inheritance; 
  
  // save value to return later
  void* value = target->value;

  // replace target's key for hard cases with successor or predecessor
  if(hard_case)
  {
    target->key = stack[sp]->key;
    target->value = stack[sp]->value;
  }

  // delete whatever is at the top
  free(stack[sp]);
  
  // if traversed path is past the root (sp > 0) 
  // update counts along traversed path down to stack[0],
  // but not including stack[sp], the deleted node
  while(sp--) {
    stack[sp]->count--;
  }

  return value;
}

void rbst_for_each(struct RBST* rbst, void (*f)(int, void*))
{
  void pair_apply(struct RBSTNode* node)
  {
    f(node->key, node->value);
  }

  rbst_inord_apply(rbst, pair_apply);
}

void rbst_preord_apply(struct RBST* rbst, void (*f)(struct RBSTNode*))
{
  apply_for_each(rbst, f, PRE_ORDER);
}

void rbst_postord_apply(struct RBST* rbst, void (*f)(struct RBSTNode*))
{
  apply_for_each(rbst, f, POST_ORDER);
}

void rbst_inord_apply(struct RBST* rbst, void (*f)(struct RBSTNode*))
{
  apply_for_each(rbst, f, IN_ORDER);
}

int rbst_count(const struct RBST* const tree)
{
  return count(tree->root);
}

int rbst_gap_count(const struct RBST* const tree)
{
  return keys_between(tree->min, tree->max) - rbst_count(tree);
}
