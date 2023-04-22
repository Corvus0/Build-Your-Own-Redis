#pragma once

#include <stddef.h>
#include <stdint.h>

struct AVLNode {
    uint32_t depth = 0;
    uint32_t cnt = 0;
    AVLNode *left = NULL;
    AVLNode *right = NULL;
    AVLNode *parent = NULL;
};

inline void avl_init(AVLNode *node) {
    node->depth = 1;
    node->cnt = 1;
    node->left = node->right = node->parent = NULL;
}

inline uint32_t avl_depth(AVLNode *node) {
    return node ? node->depth : 0;
}

inline uint32_t avl_cnt(AVLNode *node) {
    return node ? node->cnt : 0;
}

inline uint32_t max(uint32_t lhs, uint32_t rhs) {
    return lhs < rhs ? rhs : lhs;
}

AVLNode *avl_fix(AVLNode *node);
AVLNode *avl_del(AVLNode *node);
AVLNode *avl_offset(AVLNode *node, int64_t offset);
uint64_t avl_rank(AVLNode *node);
