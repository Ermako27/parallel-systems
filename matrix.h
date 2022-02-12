#include <stdio.h>

#ifndef __MATRIX__H__
#define __MATRIX__H__

typedef struct matrix_size {
    int n;
    int m;
} matrix_size_t;

typedef struct matrix_el {
    int weight;
    int start;
    int end;
} matrix_el_t;

typedef struct matrix {
    matrix_el_t** data;
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

void print_matrix(matrix_t matrix);

matrix_el_t** allocate_matrix(matrix_size_t size);

matrix_t create_matrix(FILE *fp);

int find_column_min(matrix_t matrix, int n, int column);

int find_row_min(matrix_t matrix, int row, int m);

matrix_t reduce_columns(matrix_t matrix);

matrix_t reduce_rows(matrix_t matrix);

int find_row_min_for_estimate(matrix_t matrix, int row, int m, int column);

int find_column_min_for_estimate(matrix_t matrix, int n, int column, int row);

max_zero_estimate_t find_max_zero_estimate(matrix_t matrix);

matrix_t reduce_matrix(matrix_t matrix, pos_t pos);

#endif