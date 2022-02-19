#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "tree.h"
#include "mpi.h"
#include "defines.h"


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

int* node_to_array(Node* node, int* size) {
    int matrix_size_weight = 2; // в структуре matrix_size_t 2 элемента типа int;
    int matrix_el_cnt = node->matrix.size.n * node->matrix.size.m; // количество элементов в матрице
    int total_matrix_weight = matrix_el_cnt * MATRIX_EL_WEIGHT; // общее количество int которое занимает матрица

    /**
     * [0-1] (size.n, size.m);
     * [2-total_matrix_weight] (matrix)
     *      3 числа массива - 1 элмент матрицы: по порядку следования
     *      * el.start
     *      * el.end
     *      * el.weight
     */
    int size_off_array =  MATRIX_SIZE_WEIGHT + total_matrix_weight + MATRIX_ESTIMATE_WEIGHT + MATRIX_BORDER_WEIGHT + MATRIX_IS_INCLUDED_WEIGHT;
    *size = size_off_array;

    int* arr_from_node = malloc(size_off_array*sizeof(int));

    // кладем в массив размеры матрицы
    arr_from_node[0] = node->matrix.size.n;
    arr_from_node[1] = node->matrix.size.m;

    // кладем в массив все элементы матрицы
    int step = MATRIX_SIZE_WEIGHT;
    for (int i = 0; i < node->matrix.size.n; i++) {
        for (int j = 0; j < node->matrix.size.m; j++) {
            arr_from_node[step] = node->matrix.data[i][j].start;
            arr_from_node[step + 1] = node->matrix.data[i][j].end;
            arr_from_node[step + 2] = node->matrix.data[i][j].weight;

            step += MATRIX_EL_WEIGHT;
        }
    }

    // кладем в массив estimate;
    int estimate_start_pos = MATRIX_SIZE_WEIGHT + total_matrix_weight;
    arr_from_node[estimate_start_pos] = node->estimate.pos.i;
    arr_from_node[estimate_start_pos + 1] = node->estimate.pos.j;
    arr_from_node[estimate_start_pos + 2] = node->estimate.value;

    // кладем в массив значение border
    int border_pos = estimate_start_pos + MATRIX_ESTIMATE_WEIGHT;
    arr_from_node[border_pos] = node->border;

    // кладем в массив значение 
    int is_included_pos = border_pos + MATRIX_BORDER_WEIGHT;
    arr_from_node[is_included_pos] = node->is_included;

    return arr_from_node;
}

Node* array_to_node(int* array) {
    // достаем размеры матрицы
    matrix_size_t size;
    size.n = array[0];
    size.m = array[1];

    // достаем из массива саму матрицу
    matrix_t matrix;

    matrix_el_t** matrix_data;
    matrix_data = allocate_matrix(size);
    int row_step;

    for (int i = 0; i < size.n; i++) {
        row_step = MATRIX_SIZE_WEIGHT + i * size.m * MATRIX_EL_WEIGHT;
        for (int j = 0; j < size.m; j++) {
            matrix_data[i][j].start = array[j * MATRIX_EL_WEIGHT + row_step]; // 0 * 3 + 0 + 2 = 2 ; 1 * 3 + 0 + 2 = 5 ; 0 * 3 + 0 + 8 = 8 ; 1 * 3 + 0 + 8 = 11
            matrix_data[i][j].end = array[j * MATRIX_EL_WEIGHT + 1 + row_step]; // 0 * 3 + 1 + 2 = 3 ; 1 * 3 + 1 + 2 = 6 ; 0 * 3 + 1 + 8 = 9 ; 1 * 3 + 1 + 8 = 12
            matrix_data[i][j].weight = array[j * MATRIX_EL_WEIGHT + 2 + row_step]; // 0 * 3 + 2 + 2 = 4 ; 1 * 3 + 2 + 2 = 7 ; 0 * 3 + 2 + 8 = 10 ; 1 * 3 + 2 + 8 = 13
        }
    }
    matrix.size = size;
    matrix.data = matrix_data;


    // достаем из массива estimate
    max_zero_estimate_t estimate;
    int matrix_el_cnt = size.n * size.m;
    int total_matrix_weight = matrix_el_cnt * MATRIX_EL_WEIGHT; // общее количество int которое занимает матрица
    int estimate_start_pos = MATRIX_SIZE_WEIGHT + total_matrix_weight;
    estimate.pos.i = array[estimate_start_pos];
    estimate.pos.j = array[estimate_start_pos + 1];
    estimate.value = array[estimate_start_pos + 2];

    // достаем из массива border
    int border;
    int border_pos = estimate_start_pos + MATRIX_ESTIMATE_WEIGHT;
    border = array[border_pos];

    // достаем из массива _included
    int is_included;
    int is_included_pos = border_pos + MATRIX_BORDER_WEIGHT;
    is_included = array[is_included_pos];

    // создаем саму ноду
    Node* node = create_node(matrix,border,is_included,NULL,NULL,NULL);
    node->estimate.pos.i = estimate.pos.i;
    node->estimate.pos.j = estimate.pos.j;
    node->estimate.value = estimate.value;

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
    int *arr1;
    int size_arr1;
    Node *tmp_node1;

    Node* node;
    node = create_node(parent->matrix, 0, 0, parent, NULL, NULL);
    calc_left_exclude_border(node);
    parent->left_exclude = node;

    arr1 = node_to_array(node, &size_arr1);
    tmp_node1 = array_to_node(arr1);
}

// метод для создания ноды ключающей путь, найденный в estimate.pos в методе lit_leaves
void create_right_include(Node* parent) {
    int *arr1;
    int size_arr1;
    Node *tmp_node1;

    Node* node;
    node = create_node(parent->matrix, 0, 1, parent, NULL, NULL);
    calc_right_include_border(node);
    parent->right_include = node;

    arr1 = node_to_array(node, &size_arr1);
    tmp_node1 = array_to_node(arr1);
}


void split_leaves(Node* node) {
    max_zero_estimate_t estimate;


    int *arr1, *arr2;
    int size_arr1, size_arr2;
    Node *tmp_node1, *tmp_node2;

    if (node->is_included == 1) {
        // считаем оценку нулей и находим ноль с наибольшей оценкой
        // выставляем эту оценку ноде, которая будет являться
        // родительской для нод, которые будут создаваться ниже
        estimate = find_max_zero_estimate(node->matrix);
        node->estimate = estimate;
        // создаем левый лист - лист в котором путь не учитывается 

        arr1 = node_to_array(node, &size_arr1);
        tmp_node1 = array_to_node(arr1);
        create_left_exclude(node);

        arr2 = node_to_array(node, &size_arr2);
        tmp_node2 = array_to_node(arr2);
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

        arr1 = node_to_array(node, &size_arr1);
        tmp_node1 = array_to_node(arr1);
        create_left_exclude(node);

        arr2 = node_to_array(node, &size_arr2);
        tmp_node2 = array_to_node(arr2);
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

    MPI_Init(NULL,NULL);
	int pid, num;
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
    double start_time, end_time;
    
    start_time = MPI_Wtime();

    printf("\n-------PROGRAMM INFO-------\n");
    printf("pid: %d\n", pid);
    printf("count of procs: %d\n\n", num);
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
        // printf("\n is_one_element_left: %d", is_one_element_left);
    }

    print_node(node_with_min_border);
    end_time = MPI_Wtime();
    printf("\npid %d, time: %lf\n", pid, end_time - start_time);

    MPI_Finalize();
}


    // printf("\n~~~~~~~LAYER 0~~~~~~~\n");
    // print_node(root);

    // printf("\n\n~~~~~~~LAYER 1~~~~~~~\n");
    // print_node(root->left_exclude);
    // print_node(root->right_include);
    
    // printf("\n\n~~~~~~~LAYER 2~~~~~~~\n");
    // print_node(root->left_exclude->left_exclude);
    // print_node(root->left_exclude->right_include);
    // print_node(root->right_include->left_exclude);
    // print_node(root->right_include->right_include);

    // printf("\n\n~~~~~~~LAYER 3~~~~~~~\n");
    // print_node(root->left_exclude->right_include->left_exclude);
    // print_node(root->left_exclude->right_include->right_include);

    // printf("\n\n~~~~~~~LAYER 4~~~~~~~\n");
    // print_node(root->left_exclude->right_include->right_include->left_exclude);
    // print_node(root->left_exclude->right_include->right_include->right_include);
    
    // printf("\n\n~~~~~~~LAYER 5~~~~~~~\n");
    // print_node(root->left_exclude->right_include->right_include->right_include->left_exclude);
    // print_node(root->left_exclude->right_include->right_include->right_include->right_include);

    // node_with_min_border = find_node_with_min_border(root);
    // printf("\n\n~~~~~~~node with min border~~~~~~~\n");
    // print_node(root->left_exclude->right_include->right_include);