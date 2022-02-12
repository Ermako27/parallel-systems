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

    matrix_size_t size;
    matrix = create_matrix(fp, &n, &m);
    size.n = n;
    size.m = m;
    print_matrix(matrix, size);


    // редукция строк
    int** reduced_rows_matrix;
    reduced_rows_matrix = reduce_rows(matrix, size);
    printf("\nreduced_rows_matrix \n");
    print_matrix(reduced_rows_matrix, size);

    printf("\n");

    // редукция столбцов
    int** reduced_columns_matrix;
    reduced_columns_matrix = reduce_columns(reduced_rows_matrix, size);
    printf("\nreduced_columns_matrix \n");
    print_matrix(reduced_columns_matrix, size);

    printf("\n");

    // поиск позиции нуля с наибольшоей оценкой и вычисление самой оценки
    max_zero_estimate_t estimate;
    estimate = find_max_zero_estimate(reduced_columns_matrix, size);
    printf("estimate: %d, i: %d, j: %d", estimate.estimate, estimate.pos.i, estimate.pos.j);

    printf("\n");

    // удаление строки и столбца где содержится 0 с максимальной оценкой
    matrix_t reduced_matrix;
    reduced_matrix = reduce_matrix(reduced_columns_matrix, size, estimate.pos);
    print_matrix(reduced_matrix.data, reduced_matrix.size);

    return 0;
}