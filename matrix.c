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

int** create_matrix(FILE *fp) {
    int** matrix;
    char ch;
    int num, n, m;
    char nm[128];

    // читаем размерность матрицы
    fgets(nm, 128, fp);
    n = atoi(&nm[0]);
    m = atoi(&nm[2]);

    // выделяем память для матрицы
    matrix = allocate_matrix(n, m);

    // заполняем матрицу
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fscanf(fp, "%d", &num);
            matrix[i][j] = num;
        }
    }

    print_matrix(matrix, n, m);
    return matrix;
}
