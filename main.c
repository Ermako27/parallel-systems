#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

int main(void) {
    FILE *fp;
    char fileName[] = "test1.txt";
    char ch;
    int n;
    int m;
    int** matrix;

    fp = fopen(fileName, "r");

    matrix = create_matrix(fp, &n, &m);
    print_matrix(matrix, n, m);


    int** reduced_rows_matrix;
    reduced_rows_matrix = reduce_rows(matrix, n, m);
    printf("\nreduced_rows_matrix \n");
    print_matrix(reduced_rows_matrix, n, m);

    printf("\n");

    int** reduced_columns_matrix;
    reduced_columns_matrix = reduce_columns(reduced_rows_matrix, n, m);
    printf("\nreduced_columns_matrix \n");
    print_matrix(reduced_columns_matrix, n, m);

    return 0;
}