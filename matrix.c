#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

#define INF 9999

void print_string(char *str) {
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        printf("%c", str[i]);
    }
}

void print_matrix(int** matrix, matrix_size_t size) {
    for (int i = 0; i < size.n; i++) {
        for (int j = 0; j < size.m; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int** allocate_matrix(matrix_size_t size) {
    int **matrix = calloc(size.n, sizeof(int*));

    if (!matrix) {
        return NULL;
    }

    for (int i = 0; i < size.n; i++) {
        matrix[i] = malloc(size.m * sizeof(int));

        if (!matrix[i]) {
            return NULL;
        }
    }

    return matrix;
}

int** create_matrix(FILE *fp, int *size_n, int *size_m) {
    int** matrix;
    char ch;
    int num, n, m;
    char nm[128];

    // читаем размерность матрицы
    fscanf(fp, "%d", &num);
    n = num;
    *size_n = num;
    fscanf(fp, "%d", &num);
    m = num;
    *size_m = num;

    matrix_size_t size = {n, m};
    // выделяем память для матрицы
    matrix = allocate_matrix(size);

    // заполняем матрицу
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fscanf(fp, "%d", &num);
            matrix[i][j] = num;
        }
    }

    return matrix;
}

int find_column_min(int** matrix, int n, int column) {
    int result = -1;

    for (int i = 0; i < n; i++) {
        // printf("result %d matrix[i][column] %d \n", result, matrix[i][column]);
        if (result == -1 || matrix[i][column] < result) {
            result = matrix[i][column];
        }
    }

    return result;
}

int** reduce_columns(int** matrix, matrix_size_t size) {
    int column_min;
    int** reduced_columns_matrix = allocate_matrix(size);

    // идем по столбцам и находим минимальный элемент в каждом столбце
    for (int j = 0; j < size.m; j++) {
        column_min = find_column_min(matrix, size.n, j);
        // printf("column %d min: %d \n", j, column_min);

        for (int i = 0; i < size.n; i++) {
            if (matrix[i][j] == INF) {
                reduced_columns_matrix[i][j] = matrix[i][j];
            } else {
                reduced_columns_matrix[i][j] = matrix[i][j] - column_min;
            }
        }
    }

    return reduced_columns_matrix;
}

int find_row_min(int** matrix, int row, int m) {
    int result = -1;

    // фиксируем строку и идем по столбцам, находя в строке минимальный элемент
    for (int j = 0; j < m; j++) {
        if (result == -1 || matrix[row][j] < result) {
            result = matrix[row][j];
        }
    }

    return result;
}

int** reduce_rows(int** matrix, matrix_size_t size) {
    int row_min;
    int** reduced_rows_matrix = allocate_matrix(size);
    // идем по строкам и находим минимальный элемент в каждой строке
    for (int i = 0; i < size.n; i++) {
        row_min = find_row_min(matrix, i, size.m);
        // printf("row %d min: %d \n", i, row_min);

        for (int j = 0; j < size.m; j++) {
            if (matrix[i][j] == INF) {
                reduced_rows_matrix[i][j] = matrix[i][j];
            } else {
                reduced_rows_matrix[i][j] = matrix[i][j] - row_min;
            }
        }
    }

    return reduced_rows_matrix;
}

int find_row_min_for_estimate(int** matrix, int row, int m, int column) {
    int result = -1;

    for (int j = 0; j < m; j++) {
        // фиксируем строку и идем по столбцам, находя в строке минимальный элемент
        // не учитывая 0 для которого считается оценка
        if ((result == -1 || matrix[row][j] < result) && j != column) {
            result = matrix[row][j];
        }
    }
    return result;
}

int find_column_min_for_estimate(int** matrix, int n, int column, int row) {
    int result = -1;

    for (int i = 0; i < n; i++) {
        // фиксируем столбец и идем по строкам, находя в столбце минимальный элемент
        // не учитывая 0 для которого считается оценка
        if ((result == -1 || matrix[i][column] < result) && i != row) {
            result = matrix[i][column];
        }
    }
    return result;
}

max_zero_estimate_t find_max_zero_estimate(int** matrix, matrix_size_t size) {
    int min_row;
    int min_column;
    int estimate;

    max_zero_estimate_t zero_estimate;
    zero_estimate.estimate = -1;
    zero_estimate.pos.i = -1;
    zero_estimate.pos.j = -1;

    for (int i = 0; i < size.n; i++) {
        for (int j = 0; j < size.m; j++) {
            if (matrix[i][j] == 0) {
                min_row = find_row_min_for_estimate(matrix, i , size.m, j);
                min_column = find_column_min_for_estimate(matrix, size.n, j, i);

                // printf("min in row %d: %d\n", i, min_row);
                // printf("min in column %d: %d", j, min_column);

                // printf("\n--------\n\n");
                estimate = min_row + min_column;
                if (zero_estimate.estimate == -1 || zero_estimate.estimate < estimate) { 
                    zero_estimate.estimate = estimate;
                    zero_estimate.pos.i = i;
                    zero_estimate.pos.j = j;
                }
            }
        }
    }

    return zero_estimate;
}

matrix_t reduce_matrix(int** matrix, matrix_size_t old_size, pos_t pos) {
    matrix_t result;
    matrix_size_t new_size;
    new_size.n = old_size.n - 1;
    new_size.m = old_size.m - 1;

    int row_offset = 0;
    int column_offset = 0;

    int** new_matrix = allocate_matrix(new_size);

    matrix[pos.j][pos.i] = INF;

    // проходимся по элементам старрый матрицы
    for (int i = 0; i < old_size.n; i++) {
        if (i == pos.i) { // когда доходим до строки которую хотим удалить
            row_offset = 1; // в новой матрице пятая строка должна стать четвертой поэтому нужно смещение
        } else {
            for (int j = 0; j < old_size.m; j++) {
                if (j == pos.j) { // такая же логика когда доходим до столбца который хотим удалить
                    column_offset = 1;
                } else {
                    new_matrix[i - row_offset][j - column_offset] = matrix[i][j];
                }
            }
            column_offset = 0;
        }
    }

    result.data = new_matrix;
    result.size = new_size;
    return result;
}