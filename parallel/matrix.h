#include <stdio.h>

#ifndef __MATRIX__H__
#define __MATRIX__H__

#define INF 9999

typedef struct matrix_size {
    short n;
    short m;
} matrix_size_t;

typedef struct matrix_el {
    short weight;
    short start;
    short end;
} matrix_el_t;

typedef struct matrix {
    matrix_el_t** data;
    matrix_size_t size;
} matrix_t;

typedef struct reduced_cols_matrix {
    matrix_t matrix;
    short sum_column_min;
} reduced_cols_matrix_t;

typedef struct reduced_rows_matrix {
    matrix_t matrix;
    short sum_rows_min;
} reduced_rows_matrix_t;

typedef struct pos {
    short i;
    short j;
} pos_t;

typedef struct max_zero_estimate {
    pos_t pos;
    short value;
} max_zero_estimate_t;

void print_string(char *str);

void print_matrix(matrix_t matrix);

void print_matrix_ways(matrix_t matrix);

short is_one_element_matrix(matrix_t matrix);

matrix_el_t** allocate_matrix(matrix_size_t size);

matrix_t copy_matrix(matrix_t matrix_to_copy);

matrix_t create_matrix(FILE *fp);

short find_column_min(matrix_t matrix, short n, short column);

short find_row_min(matrix_t matrix, short row, short m);

reduced_cols_matrix_t reduce_columns(matrix_t matrix);

reduced_rows_matrix_t reduce_rows(matrix_t matrix);

short find_row_min_for_estimate(matrix_t matrix, short row, short m, short column);

short find_column_min_for_estimate(matrix_t matrix, short n, short column, short row);

max_zero_estimate_t find_max_zero_estimate(matrix_t matrix);

matrix_t reduce_matrix(matrix_t matrix, pos_t pos);

#endif