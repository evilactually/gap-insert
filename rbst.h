#ifndef RBST_H
#define RBST_H

#include "common.h"

struct RBST {
  struct RBSTNode* root;
  int min;
  int max;
  int MAX_DEPTH;
};

struct RBSTNode {
  int key;
  void* value;
  struct RBSTNode* links[2];
  unsigned int count;
};

struct RBST* rbst_create(int min, int max);

void rbst_free(struct RBST* rbst);

int rbst_gap_insert(struct RBST* tree, void* value, int* key);

void* rbst_find(const struct RBST* const tree, int key);

void* rbst_remove(struct RBST* tree, int key);

void rbst_for_each(struct RBST* rbst, void (*f)(int, void*));

void rbst_preord_apply(struct RBST* rbst, void (*f)(struct RBSTNode*));

void rbst_postord_apply(struct RBST* rbst, void (*f)(struct RBSTNode*));

void rbst_inord_apply(struct RBST* rbst, void (*f)(struct RBSTNode*));

int rbst_count(const struct RBST* const tree);

int rbst_gap_count(const struct RBST* const tree);

#endif