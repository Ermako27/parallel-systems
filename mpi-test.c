#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int main(void) {
    MPI_Init(NULL,NULL);
	int pid, num, count_f_elements;
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Status status;

    if (pid == 0) {
        int* arr = malloc(3*sizeof(int));

        arr[0] = 9;
        arr[1] = 8;
        arr[2] = 7;

        for (int i = 0; i < 3; i++) {
            printf("arr[%d] %d\n", i, arr[i]);
        }

        MPI_Send(arr, 3, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count_f_elements);

        int* arr1 = malloc(count_f_elements*sizeof(int));

        printf("[in %d process] count_of_elements=%d", pid, count_f_elements);
    }

    MPI_Finalize();
    return 0;
}