#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

int main(void) {
    FILE *fp;
    char fileName[] = "test1.txt";
    char ch;
    matrix_t matrix;

    fp = fopen(fileName, "r");

    matrix = create_matrix(fp);
    print_matrix(matrix);


    // редукция строк
    matrix_t reduced_rows_matrix;
    reduced_rows_matrix = reduce_rows(matrix);
    printf("\nreduced_rows_matrix \n");
    print_matrix(reduced_rows_matrix);

    printf("\n");

    // редукция столбцов
    matrix_t reduced_columns_matrix;
    reduced_columns_matrix = reduce_columns(reduced_rows_matrix);
    printf("\nreduced_columns_matrix \n");
    print_matrix(reduced_columns_matrix);

    printf("\n");

    // поиск позиции нуля с наибольшоей оценкой и вычисление самой оценки
    max_zero_estimate_t estimate;
    estimate = find_max_zero_estimate(reduced_columns_matrix);
    printf("estimate: %d, i: %d, j: %d", estimate.estimate, estimate.pos.i, estimate.pos.j);

    printf("\nasdf");

    // удаление строки и столбца где содержится 0 с максимальной оценкой
    matrix_t reduced_matrix;
    reduced_matrix = reduce_matrix(reduced_columns_matrix, estimate.pos);
    print_matrix(reduced_matrix);
    printf("\nasdf");

    return 0;
}