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

    matrix = create_matrix(fp);
    return 0;
}