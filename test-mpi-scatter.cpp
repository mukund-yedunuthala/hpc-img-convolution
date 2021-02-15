#include <iostream>
#include <mpi.h>
using namespace std;
int main(int argc, char **argv)
{
 int rank, size, ROOT = 0;
 MPI_Init(&argc,&argv);
 MPI_Comm_size(MPI_COMM_WORLD,&size);
 MPI_Comm_rank(MPI_COMM_WORLD,&rank);
 cout << "Rank = " << rank;
 cout << ", Size = " << size << endl;
 MPI_Barrier(MPI_COMM_WORLD);
 int sendsize = size*20;
 int* sendbuf = new int[sendsize];
 int* finalbuf = new int[sendsize];
 for(int i = 0; i < sendsize; i++){
  sendbuf[i] = i+1;
 }
 int recvbuf[20];
 MPI_Scatter(sendbuf, 20, MPI_INT, recvbuf, 20, MPI_INT, ROOT, MPI_COMM_WORLD);
 MPI_Barrier(MPI_COMM_WORLD);
 for(int j = 0; j < 20; j++){
   recvbuf[j] = 2*recvbuf[j];
 }
 MPI_Barrier(MPI_COMM_WORLD);
 MPI_Gather(recvbuf, 20, MPI_INT, finalbuf, 20, MPI_INT, ROOT, MPI_COMM_WORLD);
 if(rank==ROOT){
  for(int k = 0; k < sendsize; k++){
   cout << finalbuf[k] << " ";
  }
  cout << endl;
 }
 MPI_Finalize();
 return 0;
}
