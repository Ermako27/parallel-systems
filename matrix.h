#include <stdio.h>

#ifndef __MATRIX__H__
#define __MATRIX__H__

typedef struct matrix_size {
    int n;
    int m;
} matrix_size_t;

typedef struct matrix {
    int** data;
    matrix_size_t size;
} matrix_t;

typedef struct pos {
    int i;
    int j;
} pos_t;

typedef struct max_zero_estimate {
    pos_t pos;
    int estimate;
} max_zero_estimate_t;

void print_string(char *str);

void print_matrix(int** matrix, matrix_size_t size);

int** allocate_matrix(matrix_size_t size);

int** create_matrix(FILE* fp, int *size_n, int *size_m);

int find_column_min(int** matrix, int n, int column);

int find_row_min(int** matrix, int row, int m);

int** reduce_columns(int** matrix, matrix_size_t size);

int** reduce_rows(int** matrix, matrix_size_t size);

max_zero_estimate_t find_max_zero_estimate(int** matrix, matrix_size_t size);

matrix_t reduce_matrix(int** matrix, matrix_size_t old_size, pos_t pos);

#endif