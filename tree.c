#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "tree.h"

void print_node(Node* node) {
    printf("\n--------NODE-------\n");
    print_matrix(node->matrix);
    printf("\nborder: %d", node->border);
    printf("\nis_included: %d", node->is_included);
    printf("\n--------END NODE-------\n");
}

Node* create_node(matrix_t matrix, int border, int is_included, Node* parent, Node* left, Node* right) {
    Node* node;
    node->matrix = matrix;
    node->parent = parent;
    node->left_not_include = left;
    node->right_include = right;
    node->border = border;
    node->is_included = is_included;
    return node;
}

void calc_root_border(Node *root) {
    int border; 
    reduced_rows_matrix_t reduced_rows_matrix;
    reduced_rows_matrix = reduce_rows(root->matrix);
    printf("\nreduced_rows_matrix \n");
    print_matrix(reduced_rows_matrix.matrix);
    printf("sum of min element in every row %d\n\n", reduced_rows_matrix.sum_rows_min);

    reduced_cols_matrix_t reduced_columns_matrix;
    reduced_columns_matrix = reduce_columns(reduced_rows_matrix.matrix);
    printf("\nreduced_columns_matrix \n");
    print_matrix(reduced_columns_matrix.matrix);
    printf("sum of min element in every column %d\n", reduced_columns_matrix.sum_column_min);

    border = reduced_rows_matrix.sum_rows_min + reduced_columns_matrix.sum_column_min;
    printf("\nroot border: %d \n\n", border);

    root->border = border;
    root->matrix = reduced_columns_matrix.matrix;
}

void create_tree(FILE *fp) {
    matrix_t matrix;
    Node* root;

    // считаем матрицу
    matrix = create_matrix(fp);

    // создаем корень, считаем его границу и выставляем нужную матрицу
    root = create_node(matrix, 0, 1, NULL, NULL, NULL);
    calc_root_border(root);
    print_node(root);
}