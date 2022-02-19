#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "tree.h"

int main(void) {
    FILE *fp;
    char fileName[] = "../tests/70.txt";
    char ch;
    matrix_t matrix;

    fp = fopen(fileName, "r");

    create_tree(fp);

    return 0;
}