#include <stdio.h>

#ifndef __MATRIX__H__
#define __MATRIX__H__

void print_string(char *str);

void print_matrix(int** matrix, int n, int m);

int** allocate_matrix(int n, int m);

int** create_matrix(FILE* fp, int *size_n, int *size_m);

int find_column_min(int** matrix, int n, int column);

int find_row_min(int** matrix, int row, int m);

void reduce_columns(int** matrix, int n, int m);

void reduce_rows(int** matrix, int n, int m);

#endif