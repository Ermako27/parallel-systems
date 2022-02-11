#include <stdio.h>

#ifndef __MATRIX__H__
#define __MATRIX__H__

void print_string(char *str);

void print_matrix(int** matrix, int n, int m);

int** allocate_matrix(int n, int m);

int** create_matrix(FILE* fp);

#endif