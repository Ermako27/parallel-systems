#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "matrix.h"
#include "tree.h"
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

void print_array(int* array, int n) {
    printf("\n----- Array -----\n");

    for (int i = 0; i < n; i++) {
        printf("\n %d", array[i]);
    }

    printf("\n----- End Array -----\n");
}

Node* create_node(matrix_t matrix, int border, int is_included, Node* parent, Node* left, Node* right) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->matrix = copy_matrix(matrix);
    node->estimate.pos.i = -1;
    node->estimate.pos.j = -1;
    node->estimate.value = -1;

    node->border = border;
    node->is_included = is_included;

    node->parent = parent;
    node->left_exclude = left;
    node->right_include = right;
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
Node* create_left_exclude(Node* parent) {
    // Node* node;
    // node = create_node(parent->matrix, 0, 0, parent, NULL, NULL);
    // calc_left_exclude_border(node);
    // parent->left_exclude = node;

    Node* node;
    node = create_node(parent->matrix, 0, 0, parent, NULL, NULL);
    calc_left_exclude_border(node);
    return node;
}

// метод для создания ноды ключающей путь, найденный в estimate.pos в методе lit_leaves
Node* create_right_include(Node* parent) {
    // Node* node;
    // node = create_node(parent->matrix, 0, 1, parent, NULL, NULL);
    // calc_right_include_border(node);
    // parent->right_include = node;

    Node* node;
    node = create_node(parent->matrix, 0, 1, parent, NULL, NULL);
    calc_right_include_border(node);
    return node;
}


void split_leaves(Node* node) {
    max_zero_estimate_t estimate;

    if (node->is_included == 1) {
        // считаем оценку нулей и находим ноль с наибольшей оценкой
        // выставляем эту оценку ноде, которая будет являться
        // родительской для нод, которые будут создаваться ниже
        estimate = find_max_zero_estimate(node->matrix);
        node->estimate = estimate;

        int array_from_node_size;
        int* array_from_node = node_to_array(node, &array_from_node_size);
        MPI_Send(array_from_node, array_from_node_size, MPI_INT, LEFT_PID, 0, MPI_COMM_WORLD);
        MPI_Send(array_from_node, array_from_node_size, MPI_INT, RIGHT_PID, 0, MPI_COMM_WORLD);

        // create_left_exclude(node);
        // create_right_include(node);
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

        int array_from_node_size;
        int* array_from_node = node_to_array(node, &array_from_node_size);
        MPI_Send(array_from_node, array_from_node_size, MPI_INT, LEFT_PID, 0, MPI_COMM_WORLD);
        MPI_Send(array_from_node, array_from_node_size, MPI_INT, RIGHT_PID, 0, MPI_COMM_WORLD);

        // create_left_exclude(node);
        // create_right_include(node);
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

void create_tree() {
    MPI_Init(NULL,NULL);
	int pid, num;
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &num);

    if (pid == ROOT_PID) {
        printf("process: %d\n", pid);
        MPI_Status status;
        int array_from_node_el_cnt;

        int idle_proc_count = PARALLEL_PROC_COUNT;

        FILE *fp;
        char fileName[] = "test1.txt";
        char ch;
        fp = fopen(fileName, "r");

        matrix_t matrix;
        Node* root;
        Node* node_with_min_border;
        Node* node_from_array;
        int is_one_element_left = 0;

        // считаем матрицу
        matrix = create_matrix(fp);

        // создаем корень, считаем его границу и выставляем нужную матрицу
        root = create_node(matrix, 0, 1, NULL, NULL, NULL);
        calc_root_border(root);

        // чтобы начать цикл находим ноду с наименьшей границей (на данный момент это root) и проверяем кол-во элементов в ее матрице
        node_with_min_border = find_node_with_min_border(root);
        is_one_element_left = is_one_element_matrix(node_with_min_border->matrix);

        // пока не найдено решение
        while (is_one_element_left == 0) {
            // выполняем разделение ноды с минимальной границей
            split_leaves(node_with_min_border);
            // обнуляем кол-во свободных процессов так как оба сейчас заняты разделением
            idle_proc_count = 0;
            // printf("idle_proc_count: %d\n", idle_proc_count);

            // ждем пока не освободятся оба процесса
            while (idle_proc_count != 2) {
                // получаем от двух других процессов массивы с данными новых нод
                MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                MPI_Get_count(&status, MPI_INT, &array_from_node_el_cnt);
                int* array_from_node = malloc(array_from_node_el_cnt * sizeof(int));
                MPI_Recv(array_from_node, array_from_node_el_cnt, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

                // создаем ноду из массива
                node_from_array = array_to_node(array_from_node);

                // если это нода включает путь, то это правый потомок разделяемой на данном шаге ноды
                if (node_from_array->is_included == 1) {
                    node_with_min_border->right_include = node_from_array;
                    node_from_array->parent = node_with_min_border;
                } else { // иначе левый
                    node_with_min_border->left_exclude = node_from_array;
                    node_from_array->parent = node_with_min_border;
                }

                // увеличиваем кол-во свободных процессов
                idle_proc_count++;
                // printf("idle_proc_count: %d\n", idle_proc_count);
            }

            // когда вычислили и установили новые ноды, снова ищем лист с наименьшей границей и проверяем является ли он решением
            node_with_min_border = find_node_with_min_border(root);
            is_one_element_left = is_one_element_matrix(node_with_min_border->matrix);
        }

        // когда решение найдено отправляем всем процессам сообщение о том что нужно остановить while(1)
        int* end_array;
        end_array[0] = -1;
        MPI_Send(end_array, 1, MPI_INT, LEFT_PID, 0, MPI_COMM_WORLD);
        MPI_Send(end_array, 1, MPI_INT, RIGHT_PID, 0, MPI_COMM_WORLD);

        print_node(node_with_min_border);
    } else if (pid == LEFT_PID) {
        printf("process: %d\n", pid);
        MPI_Status status;
        int array_from_node_el_cnt;
        Node* left_node, *parent_node_from_array;
        int array_from_left_node_el_cnt;
        int* array_from_left_node;

        while (1) {
            // получаем от root процесса массив с данными родительской ноды
            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &array_from_node_el_cnt);

            int* array_from_node = malloc(array_from_node_el_cnt * sizeof(int));
            MPI_Recv(array_from_node, array_from_node_el_cnt, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            // print_array(array_from_node, array_from_node_el_cnt);

            if (array_from_node[0] == -1) {
                break;
            }

            // создаем ноду из массива
            parent_node_from_array = array_to_node(array_from_node);
            
            // создаем новую ноду, исключающую путь, вычисляем для нее все данные
            left_node = create_left_exclude(parent_node_from_array);
            // print_node(left_node);

            // кастим новую ноду в массив
            array_from_left_node = node_to_array(left_node, &array_from_left_node_el_cnt);

            // print_array(array_from_left_node, array_from_node_el_cnt);

            // отправляем этот массив root процессу
            MPI_Send(array_from_left_node, array_from_left_node_el_cnt, MPI_INT, ROOT_PID, 0, MPI_COMM_WORLD);
        }
    } else if (pid == RIGHT_PID) {
        printf("process: %d\n", pid);
        MPI_Status status;
        int array_from_node_el_cnt;
        Node* right_node, *parent_node_from_array;
        int array_from_right_node_el_cnt;
        int* array_from_right_node;

        while (1) {
            // получаем от root процесса массив с данными родительской ноды
            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &array_from_node_el_cnt);

            int* array_from_node = malloc(array_from_node_el_cnt * sizeof(int));
            MPI_Recv(array_from_node, array_from_node_el_cnt, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            if (array_from_node[0] == -1) {
                break;
            }

            // создаем ноду из массива
            parent_node_from_array = array_to_node(array_from_node);
            
            // создаем новую ноду, исключающую путь, вычисляем для нее все данные
            right_node = create_right_include(parent_node_from_array);

            // кастим новую ноду в массив
            array_from_right_node = node_to_array(right_node, &array_from_right_node_el_cnt);
            
            // print_array(array_from_right_node, array_from_right_node_el_cnt);

            // отправляем этот массив root процессу
            MPI_Send(array_from_right_node, array_from_right_node_el_cnt, MPI_INT, ROOT_PID, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();

















    // // считаем матрицу
    // matrix = create_matrix(fp);

    // // создаем корень, считаем его границу и выставляем нужную матрицу
    // root = create_node(matrix, 0, 1, NULL, NULL, NULL);
    // calc_root_border(root);



    // // чтобы начать цикл находим ноду с наименьшей границей (на данный момент это root) и проверяем кол-во элементов в ее матрице
    // node_with_min_border = find_node_with_min_border(root);
    // is_one_element_left = is_one_element_matrix(node_with_min_border->matrix);

    // while (is_one_element_left == 0) {
    //     split_leaves(node_with_min_border);
    //     node_with_min_border = find_node_with_min_border(root);
    //     is_one_element_left = is_one_element_matrix(node_with_min_border->matrix);
    //     // printf("\n is_one_element_left: %d", is_one_element_left);
    // }

    // print_node(node_with_min_border);
    MPI_Finalize();

















    // if (pid == 0) {
    //     while (is_one_element_left == 0) {
    //         split_leaves(node_with_min_border);

    //         while (cnt_of_idle_proc != 2) {
    //             recv(from_any)
    //             node = array_to_node()
    //             if (node.is_included) {
    //                 node_with_min_border.right = node;
    //             } else {
    //                 node_with_min_border.left = node;
    //             }
    //             cnt_of_idle_proc++;
    //         }

    //         node_with_min_border = find_node_with_min_border(root);
    //         is_one_element_left = is_one_element_matrix(node_with_min_border->matrix);
    //         printf("\n is_one_element_left: %d", is_one_element_left);
    //     }
    // } else if (pid == 1) {
    //     while (1) {
    //         recv()

    //         if (размер переданного массива == 1) {
    //             break
    //         } else {
    //             parent_node = array_to_node()
    //             left_node = create_left_exclude(parent_node)
    //             result_arr = node_to_array(left_node)
    //             send(result_arr);
    //         }
    //     }

    // } else if (pid == 2) {
    //     while (1) {
    //         recv()

    //         if (размер переданного массива == 1) {
    //             break
    //         } else {
    //             parent_node = array_to_node()
    //             right_node = create_right_include(parent_node)
    //             result_arr = node_to_array(right_node)
    //             send(result_arr);
    //         }
    //     }
    // }
}







    // print_node(root);
    // print_matrix_ways(root->matrix);
    // int size;
    // int* array_from_node = node_to_array(root, &size);
    // print_array(array_from_node, size);

    // Node* new_node;
    // new_node = array_to_node(array_from_node);
    // print_node(new_node);
    // print_matrix_ways(new_node->matrix);



















































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

    // MPI_Finalize();

    // node_with_min_border = find_node_with_min_border(root);
    // printf("\n\n~~~~~~~node with min border~~~~~~~\n");
    // print_node(root->left_exclude->right_include->right_include);
