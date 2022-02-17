#include "mpi.h"
#include <iostream>
using namespace std;
const int Tag = 0;
const int root = 0;
double sum_array(double *array, int n) {
  double sum = 0;
  for (int i = 0; i < n; ++i) {
    sum += array[i];
  }
  return sum;
}
int main() {
  int rank, commSize;
  
  MPI_Init(NULL, NULL); //В программах на C каждому процессу при инициализации передаются аргументы функции main, полученные из командной строки. 
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // 
  MPI_Comm_size(MPI_COMM_WORLD, &commSize); // Функция возвращает количество процессов в области связи коммуникатора comm.

  
  double *arr, sum = 0, buffer;
  int n;
  MPI_Status status;
  
  if (root == rank) {
    cout << "n : ";
    cin >> n;
    
    arr = new double[n];
    for (int i = 0; i < n; ++i) 
      cin >> arr[i];
    
    int partSize = n/commSize;
    
    int shift = n%commSize;
    for (int i = root+1; i < commSize; ++i) {
      MPI_Send(arr + shift + partSize*i, partSize, MPI_DOUBLE, i, Tag, MPI_COMM_WORLD);
    }
    
    sum = sum_array(arr, shift + partSize);
    
    for (int i = root+1; i < commSize; ++i) {
      MPI_Recv(&buffer, 1, MPI_DOUBLE, i, Tag, MPI_COMM_WORLD, &status);
      sum += buffer;
    }
  }
  else {
    MPI_Probe(root, Tag, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_DOUBLE, &n);
    
    arr = new double[n];
    MPI_Recv(arr, n, MPI_DOUBLE, root, Tag, MPI_COMM_WORLD, &status);
    
    sum = sum_array(arr, n);
    
    MPI_Send(&sum, 1, MPI_DOUBLE, root, Tag, MPI_COMM_WORLD);
  }
  
  delete[] arr;
  
  cout << rank << " : " << sum << endl;
  MPI_Finalize(); // Функция закрывает все MPI-процессы и ликвидирует все области связи.
}