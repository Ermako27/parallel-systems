#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

void print_string(char *str) {
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        printf("%c", str[i]);
    }
}

void print_matrix(matrix_t matrix) {
    for (int i = 0; i < matrix.size.n; i++) {
        for (int j = 0; j < matrix.size.m; j++) {
            printf("%d ", matrix.data[i][j].weight);
        }
        printf("\n");
    }
}

void print_matrix_ways(matrix_t matrix) {
    for (int i = 0; i < matrix.size.n; i++) {
        for (int j = 0; j < matrix.size.m; j++) {
            printf("%d-%d ", matrix.data[i][j].start, matrix.data[i][j].end);
        }
        printf("\n");
    }
}

matrix_el_t** allocate_matrix(matrix_size_t size) {
    matrix_el_t **matrix = calloc(size.n, sizeof(matrix_el_t*));

    if (!matrix) {
        return NULL;
    }

    for (int i = 0; i < size.n; i++) {
        matrix[i] = malloc(size.m * sizeof(matrix_el_t));

        if (!matrix[i]) {
            return NULL;
        }
    }

    return matrix;
}

matrix_t copy_matrix(matrix_t matrix_to_copy) {
    matrix_t result;
    matrix_el_t** matrix;

    matrix = allocate_matrix(matrix_to_copy.size);

    for (int i = 0; i < matrix_to_copy.size.n; i++) {
        for (int j = 0; j < matrix_to_copy.size.m; j++) {
            matrix[i][j].start = matrix_to_copy.data[i][j].start;
            matrix[i][j].end = matrix_to_copy.data[i][j].end;
            matrix[i][j].weight = matrix_to_copy.data[i][j].weight;

        }
    }

    result.data = matrix;
    result.size.n = matrix_to_copy.size.n;
    result.size.m = matrix_to_copy.size.m;

    return result;
}


matrix_t create_matrix(FILE *fp) {
    matrix_t matrix;
    char ch;
    int num, n, m;
    char nm[128];

    // читаем размерность матрицы
    fscanf(fp, "%d", &num);
    n = num;
    fscanf(fp, "%d", &num);
    m = num;

    matrix_size_t size = {n, m};
    // выделяем память для матрицы
    matrix.data = allocate_matrix(size);
    matrix.size = size;

    // заполняем матрицу
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fscanf(fp, "%d", &num);
            matrix.data[i][j].weight = num;
            matrix.data[i][j].start = i;
            matrix.data[i][j].end = j;
        }
    }

    return matrix;
}

int find_column_min(matrix_t matrix, int n, int column) {
    int result = -1;

    for (int i = 0; i < n; i++) {
        // printf("result %d matrix[i][column] %d \n", result, matrix[i][column]);
        if (result == -1 || matrix.data[i][column].weight < result) {
            result = matrix.data[i][column].weight;
        }
    }

    return result;
}

reduced_cols_matrix_t reduce_columns(matrix_t matrix) {
    reduced_cols_matrix_t result;
    matrix_t new_matrix;
    int column_min;
    int sum_column_min = 0;

    matrix_el_t** reduced_columns_matrix = allocate_matrix(matrix.size);

    // идем по столбцам и находим минимальный элемент в каждом столбце
    for (int j = 0; j < matrix.size.m; j++) {
        column_min = find_column_min(matrix, matrix.size.n, j);
        sum_column_min += column_min;
        // printf("column %d min: %d \n", j, column_min);

        for (int i = 0; i < matrix.size.n; i++) {
            if (matrix.data[i][j].weight == INF) {
                reduced_columns_matrix[i][j].weight = matrix.data[i][j].weight;
            } else {
                reduced_columns_matrix[i][j].weight = matrix.data[i][j].weight - column_min;
            }

            reduced_columns_matrix[i][j].start = matrix.data[i][j].start;
            reduced_columns_matrix[i][j].end = matrix.data[i][j].end;
        }
    }

    // в каждую ячейку матрицы устанавливаем откуда куда это путь
    for (int i = 0; i < matrix.size.n; i++) {
        for (int j = 0; j < matrix.size.m; j++) {
            reduced_columns_matrix[i][j].start = matrix.data[i][j].start;
            reduced_columns_matrix[i][j].end = matrix.data[i][j].end;
        }
    }

    new_matrix.data = reduced_columns_matrix;
    new_matrix.size = matrix.size;

    result.matrix = new_matrix;
    result.sum_column_min = sum_column_min;
    return result;
}

int find_row_min(matrix_t matrix, int row, int m) {
    int result = -1;

    // фиксируем строку и идем по столбцам, находя в строке минимальный элемент
    for (int j = 0; j < m; j++) {
        if (result == -1 || matrix.data[row][j].weight < result) {
            result = matrix.data[row][j].weight;
        }
    }

    return result;
}

reduced_rows_matrix_t reduce_rows(matrix_t matrix) {
    reduced_rows_matrix_t result;
    matrix_t new_matrix;
    int row_min;
    int sum_row_min = 0;

    matrix_el_t** reduced_rows_matrix = allocate_matrix(matrix.size);
    // идем по строкам и находим минимальный элемент в каждой строке
    for (int i = 0; i < matrix.size.n; i++) {
        row_min = find_row_min(matrix, i, matrix.size.m);
        sum_row_min += row_min;
        // printf("row %d min: %d \n", i, row_min);

        for (int j = 0; j < matrix.size.m; j++) {
            if (matrix.data[i][j].weight == INF) {
                reduced_rows_matrix[i][j].weight = matrix.data[i][j].weight;
            } else {
                reduced_rows_matrix[i][j].weight = matrix.data[i][j].weight - row_min;
            }

            // в каждую ячейку матрицы устанавливаем откуда куда это путь
            reduced_rows_matrix[i][j].start = matrix.data[i][j].start;
            reduced_rows_matrix[i][j].end = matrix.data[i][j].end;
        }
    }

    new_matrix.data = reduced_rows_matrix;
    new_matrix.size = matrix.size;

    result.matrix = new_matrix;
    result.sum_rows_min = sum_row_min;
    return result;
}

int find_row_min_for_estimate(matrix_t matrix, int row, int m, int column) {
    int result = -1;

    for (int j = 0; j < m; j++) {
        // фиксируем строку и идем по столбцам, находя в строке минимальный элемент
        // не учитывая 0 для которого считается оценка
        if ((result == -1 || matrix.data[row][j].weight < result) && j != column) {
            result = matrix.data[row][j].weight;
        }
    }
    return result;
}

int find_column_min_for_estimate(matrix_t matrix, int n, int column, int row) {
    int result = -1;

    for (int i = 0; i < n; i++) {
        // фиксируем столбец и идем по строкам, находя в столбце минимальный элемент
        // не учитывая 0 для которого считается оценка
        if ((result == -1 || matrix.data[i][column].weight < result) && i != row) {
            result = matrix.data[i][column].weight;
        }
    }
    return result;
}

max_zero_estimate_t find_max_zero_estimate(matrix_t matrix) {
    int min_row;
    int min_column;
    int estimate;

    max_zero_estimate_t zero_estimate;
    zero_estimate.value = -1;
    zero_estimate.pos.i = -1;
    zero_estimate.pos.j = -1;

    for (int i = 0; i < matrix.size.n; i++) {
        for (int j = 0; j < matrix.size.m; j++) {
            if (matrix.data[i][j].weight == 0) {
                min_row = find_row_min_for_estimate(matrix, i , matrix.size.m, j);
                min_column = find_column_min_for_estimate(matrix, matrix.size.n, j, i);

                // printf("min in row %d: %d\n", i, min_row);
                // printf("min in column %d: %d", j, min_column);

                // printf("\n--------\n\n");
                estimate = min_row + min_column;
                if (zero_estimate.value == -1 || zero_estimate.value < estimate) { 
                    zero_estimate.value = estimate;
                    zero_estimate.pos.i = i;
                    zero_estimate.pos.j = j;
                }
            }
        }
    }

    printf("\nestimate: %d, i: %d, j: %d", zero_estimate.value, zero_estimate.pos.i, zero_estimate.pos.j);
    return zero_estimate;
}

pos_t find_way_back(matrix_t matrix, pos_t estimate_position) {
    pos_t result;
    int stop = 0;
    int way_back_start = matrix.data[estimate_position.i][estimate_position.j].end;
    int way_back_end = matrix.data[estimate_position.i][estimate_position.j].start;
    int value = matrix.data[estimate_position.i][estimate_position.j].weight;

    for (int i = 0; i < matrix.size.n; i++) {
        for (int j = 0; j < matrix.size.m; j++) {
            if (matrix.data[i][j].start == way_back_start && matrix.data[i][j].end == way_back_end) {
                result.i = i;
                result.j = j;
                stop = 1;
                break;
            }     
        }

        if (stop == 1) {
            break;
        }
    }

    return result;
}

matrix_t reduce_matrix(matrix_t matrix, pos_t pos) {
    matrix_t result;
    matrix_size_t new_size;
    pos_t way_back_position;
    new_size.n = matrix.size.n - 1;
    new_size.m = matrix.size.m - 1;

    int row_offset = 0;
    int column_offset = 0;

    matrix_el_t** new_matrix_data = allocate_matrix(new_size);

    // при удалении строки и столбца из матрицы ставим на обратный путь значек INF
    way_back_position = find_way_back(matrix, pos);
    matrix.data[way_back_position.i][way_back_position.j].weight = INF;

    // проходимся по элементам старрый матрицы
    for (int i = 0; i < matrix.size.n; i++) {
        if (i == pos.i) { // когда доходим до строки которую хотим удалить
            row_offset = 1; // в новой матрице пятая строка должна стать четвертой поэтому нужно смещение
        } else {
            for (int j = 0; j < matrix.size.m; j++) {
                if (j == pos.j) { // такая же логика когда доходим до столбца который хотим удалить
                    column_offset = 1;
                } else {
                    new_matrix_data[i - row_offset][j - column_offset] = matrix.data[i][j];
                }
            }
            column_offset = 0;
        }
    }

    result.data = new_matrix_data;
    result.size = new_size;
    return result;
}