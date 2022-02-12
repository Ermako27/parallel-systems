#include <stdio.h>
#include "matrix.h"

#ifndef __TREE__H__
#define __TREE__H__

typedef struct Node {
    matrix_t matrix;
    int lower_border;
    bool is_included;
    struct Node *right_include;
    struct Node *left_not_include;
    struct Node *parent;
} Node;

#endif