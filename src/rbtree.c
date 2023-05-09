// valgrind --leak-check=full ./test/test-rbtree
#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    // TODO: initialize struct if needed

    // create nil node 
    node_t *nil = (node_t *)malloc(sizeof(node_t));
    nil->color = RBTREE_BLACK;
    
    // 설정
    p->root = nil;
    p->nil = nil;
    return p;
}

void recursive(rbtree *t, node_t *x) {
    if (x != t->nil) {
        recursive(t, x->left);
        recursive(t, x->right);
        free(x);
    }
}
void delete_rbtree(rbtree *t) {
    recursive(t, t->root);
    free(t->nil);
    free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
    node_t *z = (node_t *)malloc(sizeof(node_t));
    z->key = key;

    node_t *y = t->nil;
    node_t *x = t->root;
    while (x != t->nil) {
        y = x;  // y의 값을 x로 계속 최신화 해줌
        if (z->key < x->key) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == t->nil) {
        t->root = z;
    }
    else if (z->key < y->key) {
        y->left = z;
    }
    else {
        y->right = z;
    }
    
    z->left = t->nil;
    z->right = t->nil;
    z->color = RBTREE_RED;
    rbtree_insert_fix(t, z);
    return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    node_t *x = t->root;
    while (x != t->nil) {
        if (key > x->key) {
            x = x->right;
        }
        else if (key < x->key) {
            x = x->left;
        }
        else {
            return x;
        }
    }
    return NULL;
}

node_t *rbtree_min(const rbtree *t) {
    node_t *x = t->root;
    while (x->left != t->nil) {
        x = x->left;
    }
    return x;
}

node_t *rbtree_max(const rbtree *t) {
    node_t *x = t->root;
    while (x->right != t->nil) {
        x = x->right;
    }
    return x;
}

int rbtree_erase(rbtree *t, node_t *p) {
    node_t *y = p;
    node_t *x = p;
    color_t y_original_color = y->color;
    if (p->left == t->nil) {
        x = p->right;
        rb_transplant(t, p, p->right);
    }
    else if (p->right == t->nil) {
        x = p->left;
        rb_transplant(t, p, p->left);
    }
    else {
        y = subtree_min(t, p->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == p) {
            x->parent = y;
        }
        else {
            rb_transplant(t, y, y->right);
            y->right = p->right;
            y->right->parent = y;
        }
        rb_transplant(t, p, y);
        y->left = p->left;
        y->left->parent = y;
        y->color = p->color;
    }
    
    if (y_original_color == RBTREE_BLACK) {
        rb_delete_fix(t, x);
    }
    free(p);
    return 0;
}
void inorder(const rbtree *t, node_t *x, key_t *arr, int *i) {
    if (x != t->nil) {
        inorder(t, x->left, arr, i);
        arr[*i] = x->key;
        *i += 1;
        inorder(t, x->right, arr, i);
    }
}
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    // need sort
    // arr에 배열 넣기
    int i = 0;
    
    inorder(t, t->root, arr, &i);

    return 0;
}

// add function
void rbtree_insert_fix(rbtree *t, node_t *z) {
    // 부모가 빨간색일 때
    while (z->parent->color == RBTREE_RED) {
        if (z->parent == z->parent->parent->left) {
            node_t *y = z->parent->parent->right;
            if (y->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                right_rotate(t, z->parent->parent);
            }
        }
        else {
            node_t *y = z->parent->parent->left;
            if (y->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                left_rotate(t, z->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}

void left_rotate(rbtree *t, node_t *x) {
    node_t *y = x->right;
    x->right = y->left;
    if (y->left != t->nil) {
        y->left->parent = x;
    }
    // y의 부모를 x의 부모로 설정
    y->parent = x->parent;
    if (x->parent == t->nil) {
        t->root = y;
    }
    else if (x->parent->left == x) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
    node_t *y = x->left;
    x->left = y->right;
    if (y->right != t->nil) {
        y->right->parent = x;
    }
    // 부모 설정
    y->parent = x->parent;
    if (x->parent == t->nil) {
        t->root = y;
    }
    else if (x->parent->left == x) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

void rb_transplant(rbtree *t, node_t *delete, node_t *replace) {
    if (delete->parent == t->nil) {
        t->root = replace;
    }
    else if (delete == delete->parent->left) {
        delete->parent->left = replace;
    }
    else {
        delete->parent->right = replace;
    }
    replace->parent = delete->parent;
}

node_t *subtree_min(rbtree *t, node_t *x) {
    while (x->left != t->nil) {
        x = x->left;
    }
    return x;
}

void rb_delete_fix(rbtree *t, node_t *x) {
    // x가 root node가 아니고, x의 색상이 검정색일 때
    while (x != t->root && x->color == RBTREE_BLACK) {
        // x가 왼쪽 자식일 때
        if (x == x->parent->left) {
            // y는 x의 형제 노드
            node_t *y = x->parent->right;
            // y의 색상이 레드 면 case. 1
            if (y->color == RBTREE_RED) {
                // y와 부모의 색상 교환 후 부모 기준 왼쪽 회전
                y->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                left_rotate(t, x->parent);
                // 트리가 바뀌어서 y를 다시 x의 형제노드로 설정 및 case. 2, 3, 4 중 하나로 해결
                y = x->parent->right;
            }
            //case. 2
            if (y->left->color == RBTREE_BLACK && y->right->color == RBTREE_BLACK) {
                y->color = RBTREE_RED;
                x = x->parent;
            }
            else {
                if (y->right->color == RBTREE_BLACK) {
                    y->left->color = RBTREE_BLACK;
                    y->color = RBTREE_RED;
                    right_rotate(t, y);
                    y = x->parent->right;
                }
                y->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                y->right->color = RBTREE_BLACK;
                left_rotate(t, x->parent);
                x = t->root;
            }
        }
        else {
            node_t *y = x->parent->left;
            // y의 색상이 레드 면 case. 1
            if (y->color == RBTREE_RED) {
                // y와 부모의 색상 교환 후 부모 기준 왼쪽 회전
                y->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t, x->parent);
                // 트리가 바뀌어서 y를 다시 x의 형제노드로 설정 및 case. 2, 3, 4 중 하나로 해결
                y = x->parent->left;
            }
            //case. 2
            if (y->left->color == RBTREE_BLACK && y->right->color == RBTREE_BLACK) {
                y->color = RBTREE_RED;
                x = x->parent;
            }
            else {
                if (y->left->color == RBTREE_BLACK) {
                    y->right->color = RBTREE_BLACK;
                    y->color = RBTREE_RED;
                    left_rotate(t, y);
                    y = x->parent->left;
                }
                y->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                y->left->color = RBTREE_BLACK;
                right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = RBTREE_BLACK;
}

void print_rbtree(const rbtree *t, const node_t *x) {
  if (x == t->nil) {
    printf("nil\n");
    return;
  }
  printf("key = %d\n", x->key);
  printf("left: ");
  print_rbtree(t, x->left);
  printf("right: ");
  print_rbtree(t, x->right);
}