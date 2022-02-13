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

    printf("\n!4!\n");
    node->estimate.pos.i = -1;
    node->estimate.pos.j = -1;
    node->estimate.value = -1;
    printf("\n!5!\n");
    node->parent = parent;
    node->left_exclude = left;
    node->right_include = right;
    node->border = border;
    node->is_included = is_included;
    printf("\n!6!\n");
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

void calc_left_exclude_border(Node* node) {
    int i, j;
    i = node->parent->estimate.pos.i;
    j = node->parent->estimate.pos.j;

    printf("\n!8!\n");
    // считаем границу для ноды
    node->border = node->parent->border + node->parent->estimate.value;
    printf("\n!9!\n");
    // так как нода описывает исключающий путь, то на позиции нуля с наибольшей оценкой ставим INF
    node->matrix.data[i][j].weight = INF;
    print_node(node);
    printf("\n!10!\n");
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

// метод для создания ноды исключающей путь, найденный в estimate.pos в методе lit_leaves
void create_left_exclude(Node* parent) {
    Node* node;
    printf("\n!3!\n");
    node = create_node(parent->matrix, 0, 0, NULL, NULL, NULL);
    printf("\n!7!\n");
    // calc_left_exclude_border(node);
    printf("\n!11!\n");
    // parent->left_exclude = node;
    printf("\n!12!\n");
}

// метод для создания ноды ключающей путь, найденный в estimate.pos в методе lit_leaves
void create_right_include(Node* parent) {
    printf("\n!13\n");
    Node* node;
    printf("\n!14!\n");
    node = create_node(parent->matrix, 0, 1, parent, NULL, NULL);
    calc_right_include_border(node);
    parent->right_include = node;
    printf("\n!15!\n");
}


void split_leaves(Node* node) {
    max_zero_estimate_t estimate;

    if (node->is_included == 1) {
        // считаем оценку нулей и находим ноль с наибольшей оценкой
        // выставляем эту оценку ноде, которая будет являться
        // родительской для нод, которые будут создаваться ниже
        estimate = find_max_zero_estimate(node->matrix);
        node->estimate = estimate;
        printf("\n!1!\n");
        // создаем левый лист - лист в котором путь не учитывается 
        create_left_exclude(node);
        printf("\n!2!\n");
        create_right_include(node);
    } else {
        // 1 считаем редукцию строк матрицы в node


        // 2 считаем редукцию столбцов матрицы в node


        // 3 считаем оценку нулей и находим ноль с наибольшей оценкой
        // выставляем эту оценку ноде, которая будет являться
        // родительской для нод, которые будут создаваться ниже

        // матрицу полученную в результате 1 и 2 сетим в node
    }
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

    

    // сплитим корень на две другие ноды
    split_leaves(root);
}