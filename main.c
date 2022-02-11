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


    reduce_columns(matrix, n, m);
    printf("\n\n");
    reduce_rows(matrix, n, m);

    return 0;
}