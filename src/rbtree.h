#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
    color_t color;
    key_t key;
    struct node_t *parent, *left, *right;
} node_t;

typedef struct {
    node_t *root;
    node_t *nil;  // for sentinel
} rbtree;

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);
// add function
void rbtree_insert_fix(rbtree *t, node_t *z);
void left_rotate(rbtree *t, node_t *x);
void right_rotate(rbtree *t, node_t *x);
void rb_transplant(rbtree *t, node_t *delete, node_t *replace);
node_t *subtree_min(rbtree *t, node_t *x);
void rb_delete_fix(rbtree *t, node_t *x);
void print_rbtree(const rbtree *t, const node_t *x);

#endif  // _RBTREE_H_
