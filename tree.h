#include <stdio.h>
#include "matrix.h"

#ifndef __TREE__H__
#define __TREE__H__

typedef struct Node {
    matrix_t matrix;
    int border;
    int is_included;
    struct Node *right_include;
    struct Node *left_not_include;
    struct Node *parent;
} Node;

Node* create_node(matrix_t matrix, int border, int is_included, Node* parent, Node* left, Node* right);

void create_tree(FILE *fp);

#endif