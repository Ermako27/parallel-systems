#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_string(char *str) {
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        printf("%c", str[i]);
    }
}

void print_matrix(int** matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int** allocate_matrix(int n, int m) {
    int **matrix = calloc(n, sizeof(int*));

    if (!matrix) {
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(m * sizeof(int));

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

    // выделяем память для матрицы
    matrix = allocate_matrix(n, m);

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

void reduce_columns(int** matrix, int n, int m) {
    int column_min;
    // идем по столбцам и находим минимальный элемент в каждом столбце
    for (int j = 0; j < m; j++) {
        column_min = find_column_min(matrix, n, j);
        printf("column %d min: %d \n", j, column_min);
    }
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

void reduce_rows(int** matrix, int n, int m) {
    int row_min;
    // идем по строкам и находим минимальный элемент в каждой строке
    for (int i = 0; i < n; i++) {
        row_min = find_row_min(matrix, i, m);
        printf("row %d min: %d \n", i, row_min);
    }
}