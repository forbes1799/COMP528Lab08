#include<omp.h>
#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(void){

    int commSz, myRank;
    int root = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &commSz);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    //64 Matrices with size N*N
    const int BATCH = 64;
    const int N = 1024;

    //MPI can only send data if it is contiguous (elements are right next to each other)
    //Declaring the matrices as 1D arrays, guarantees contiguouity. 
    //They can be reshaped to 3D arrays later.
    float *mat1 = (float*)malloc(BATCH * N * N * sizeof(float));
    float *mat2 = (float*)malloc(BATCH * N * N * sizeof(float));
    float *res = (float*)malloc(BATCH * N * N * sizeof(float));

    int batchPerProcess;

    //Rank 0 sets up the problem by finding the batch size per process
    if(myRank == root){
        
        batchPerProcess = BATCH / commSz;

        //If the batchPerProcess is not divisible by the commSz, then increase batch size by 1 to ensure that all matrices are completed.
        if(batchPerProcess % commSz != 0){
            batchPerProcess++;
        }

        srand(time(NULL));

        //Initialise mat1 and mat2
        for(int b = 0; b < BATCH; b++){
            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){
                    mat1[b * i + j] = rand() % 50;
                    mat2[b * i + j] = rand() % 50;
                }
            }
        }
    }


    MPI_Bcast(&batchPerProcess, 1, MPI_INT, root, MPI_COMM_WORLD);

    //Create local matrices
    float *localMat1 = (float*)malloc(batchPerProcess * N * N * sizeof(float));
    float *localMat2 = (float*)malloc(batchPerProcess * N * N * sizeof(float));
    float *localRes = (float*)malloc(batchPerProcess * N * N * sizeof(float));

    int sndCount = batchPerProcess * N * N;

    MPI_Scatter(mat1, sndCount, MPI_FLOAT, localMat1, sndCount, MPI_FLOAT, root, MPI_COMM_WORLD);
    MPI_Scatter(mat2, sndCount, MPI_FLOAT, localMat2, sndCount, MPI_FLOAT, root, MPI_COMM_WORLD);

    //Reshapes the 1D matrices into 3D matrices using "pointer magic" so that they can be accessed like lMat1[i][j][k] etc.
    //The pointers point to the original location. Therefore, lMat1[i][j] affects the memory location of localMat1[i * N + j]
    float (*lMat1)[N][N] = (float(*)[N][N]) localMat1;
    float (*lMat2)[N][N] = (float(*)[N][N]) localMat2;
    float (*lRes)[N][N] = (float(*)[N][N]) localRes;

    double sTime, eTime;
    int currB = 0;

    //Timing begins
    sTime = MPI_Wtime();

    //TASK: Lab08 ADD OPENMP HERE!
    //WHAT's the best way to parallelise it????
    while(currB < batchPerProcess){


        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                lRes[currB][i][j] = 0; 
                for(int k = 0; k < N; k++){
                    lRes[currB][i][j] += lMat1[currB][i][k] *  lMat2[currB][k][j];
                }
            }
        }
    
        currB++;
    }


    eTime = MPI_Wtime();
    //Finish timing

    MPI_Gather(localRes, sndCount, MPI_FLOAT, res, sndCount, MPI_FLOAT, root, MPI_COMM_WORLD);

    if(myRank == root){
        printf("Time taken = %f seconds", eTime - sTime);
    }

    MPI_Finalize();



}
