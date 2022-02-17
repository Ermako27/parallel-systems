#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "tree.h"

void print_node(Node* node) {
    printf("\n--------NODE-------\n");
    if (node == NULL) {
        printf("NULL LEAF");
    } else {
        print_matrix(node->matrix);
        printf("\nborder: %d", node->border);
        printf("\nis_included: %d", node->is_included);
    }
    printf("\n-----END NODE------\n");
}

Node* create_node(matrix_t matrix, int border, int is_included, Node* parent, Node* left, Node* right) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->matrix = copy_matrix(matrix);
    node->estimate.pos.i = -1;
    node->estimate.pos.j = -1;
    node->estimate.value = -1;
    node->parent = parent;
    node->left_exclude = left;
    node->right_include = right;
    node->border = border;
    node->is_included = is_included;
    return node;
}

void calc_root_border(Node *root) {
    int border; 
    reduced_rows_matrix_t reduced_rows_matrix;
    reduced_rows_matrix = reduce_rows(root->matrix);

    reduced_cols_matrix_t reduced_columns_matrix;
    reduced_columns_matrix = reduce_columns(reduced_rows_matrix.matrix);

    border = reduced_rows_matrix.sum_rows_min + reduced_columns_matrix.sum_column_min;

    root->border = border;
    root->matrix = reduced_columns_matrix.matrix;
}

void calc_left_exclude_border(Node* node) {
    int i, j;
    i = node->parent->estimate.pos.i;
    j = node->parent->estimate.pos.j;

    // считаем границу для ноды
    node->border = node->parent->border + node->parent->estimate.value;
    // так как нода описывает исключающий путь, то на позиции нуля с наибольшей оценкой ставим INF
    node->matrix.data[i][j].weight = INF;
}

void calc_right_include_border(Node* node) {
    matrix_t reduced_matrix;
    reduced_rows_matrix_t reduced_rows_matrix;
    reduced_cols_matrix_t reduced_columns_matrix;
    int border;

    // проводим редукцию матрицы
    reduced_matrix = reduce_matrix(node->matrix, node->parent->estimate.pos);

    // проводим редукцию строк
    reduced_rows_matrix = reduce_rows(reduced_matrix);

    // проводим редукцию столбцов
    reduced_columns_matrix = reduce_columns(reduced_rows_matrix.matrix);

    // считаем границу для ноды
    border = node->parent->border + reduced_rows_matrix.sum_rows_min + reduced_columns_matrix.sum_column_min;

    // устанавливаем border и преобразованную матрицу в node
    node->border = border;
    node->matrix = reduced_columns_matrix.matrix;
}

// метод для создания ноды исключающей путь, найденный в estimate.pos в методе split_leaves
void create_left_exclude(Node* parent) {
    Node* node;
    node = create_node(parent->matrix, 0, 0, parent, NULL, NULL);
    calc_left_exclude_border(node);
    parent->left_exclude = node;
}

// метод для создания ноды ключающей путь, найденный в estimate.pos в методе lit_leaves
void create_right_include(Node* parent) {
    Node* node;
    node = create_node(parent->matrix, 0, 1, parent, NULL, NULL);
    calc_right_include_border(node);
    parent->right_include = node;
}


void split_leaves(Node* node) {
    max_zero_estimate_t estimate;

    if (node->is_included == 1) {
        // считаем оценку нулей и находим ноль с наибольшей оценкой
        // выставляем эту оценку ноде, которая будет являться
        // родительской для нод, которые будут создаваться ниже
        estimate = find_max_zero_estimate(node->matrix);
        node->estimate = estimate;
        // создаем левый лист - лист в котором путь не учитывается 
        create_left_exclude(node);
        create_right_include(node);
    } else {
        // 1 считаем редукцию строк матрицы в node
        reduced_rows_matrix_t reduced_rows_matrix;
        reduced_rows_matrix = reduce_rows(node->matrix);


        // 2 считаем редукцию столбцов матрицы в node
        reduced_cols_matrix_t reduced_columns_matrix;
        reduced_columns_matrix = reduce_columns(reduced_rows_matrix.matrix);

        // сохраняем преобразованную матрицу в ноду 
        node->matrix = copy_matrix(reduced_columns_matrix.matrix);

        // 4 считаем оценку нулей и находим ноль с наибольшей оценкой
        // выставляем эту оценку ноде, которая будет являться
        // родительской для нод, которые будут создаваться ниже
        estimate = find_max_zero_estimate(node->matrix);
        node->estimate = estimate;

        create_left_exclude(node);
        create_right_include(node);
    }
}

int is_leaf(Node* node) {
    if (node->left_exclude == NULL && node->right_include == NULL) {
        return 1;
    } else {
        return 0;
    }
}

Node* find_node_with_min_border(Node* node) {
    Node* left_node;
    Node* right_node;
    int is_node_leaf;
    is_node_leaf = is_leaf(node);

    if (is_node_leaf == 1) {
        return node;
    } else {
        left_node = find_node_with_min_border(node->left_exclude);
        right_node = find_node_with_min_border(node->right_include);

        if (left_node->border < right_node->border) {
            return left_node;
        } else {
            return right_node;
        }
    }
}

void create_tree(FILE *fp) {
    matrix_t matrix;
    Node* root;
    Node* node_with_min_border;
    int is_one_element_left = 0;

    // считаем матрицу
    matrix = create_matrix(fp);

    // создаем корень, считаем его границу и выставляем нужную матрицу
    root = create_node(matrix, 0, 1, NULL, NULL, NULL);
    calc_root_border(root);

    // чтобы начать цикл находим ноду с наименьшей границей (на данный момент это root) и проверяем кол-во элементов в ее матрице
    node_with_min_border = find_node_with_min_border(root);
    is_one_element_left = is_one_element_matrix(node_with_min_border->matrix);

    while (is_one_element_left == 0) {
        split_leaves(node_with_min_border);
        node_with_min_border = find_node_with_min_border(root);
        is_one_element_left = is_one_element_matrix(node_with_min_border->matrix);
        printf("\n is_one_element_left: %d", is_one_element_left);
    }


    printf("\n~~~~~~~LAYER 0~~~~~~~\n");
    print_node(root);

    printf("\n\n~~~~~~~LAYER 1~~~~~~~\n");
    print_node(root->left_exclude);
    print_node(root->right_include);
    
    printf("\n\n~~~~~~~LAYER 2~~~~~~~\n");
    print_node(root->left_exclude->left_exclude);
    print_node(root->left_exclude->right_include);
    print_node(root->right_include->left_exclude);
    print_node(root->right_include->right_include);

    printf("\n\n~~~~~~~LAYER 3~~~~~~~\n");
    print_node(root->left_exclude->right_include->left_exclude);
    print_node(root->left_exclude->right_include->right_include);

    printf("\n\n~~~~~~~LAYER 4~~~~~~~\n");
    print_node(root->left_exclude->right_include->right_include->left_exclude);
    print_node(root->left_exclude->right_include->right_include->right_include);
    
    printf("\n\n~~~~~~~LAYER 5~~~~~~~\n");
    print_node(root->left_exclude->right_include->right_include->right_include->left_exclude);
    print_node(root->left_exclude->right_include->right_include->right_include->right_include);

    // node_with_min_border = find_node_with_min_border(root);
    // printf("\n\n~~~~~~~node with min border~~~~~~~\n");
    // print_node(root->left_exclude->right_include->right_include);

}