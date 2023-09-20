/* ----------------------------------------------------
* Autor: Vinícius Eduardo de Souza Honório
* Matrícula: 2021.1.08.024
* ---------------------------------------------------- */

/* COMPILE AND RUNNING INSTRUCTIONS [REQUIRES MPI] */

// COMPILE
// mpicc [file name].c -o [output file name]

// RUN
// mpirun -np [number of processors] --hostfile [hostfile name] [output file name]

/* LIBRARIES IMPORT */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <unistd.h> // For Linux
// #include <windows.h> // For Windows

/* GLOBAL VALUES */

#define MATRIX_A "matrix_a"
#define MATRIX_B "matrix_b"
#define MATRIX_C "matrix_c"

#define MIN_VALUE 10
#define MAX_VALUE 50

/* GENERAL FUNCTIONS */

// generates a random int value from the numeric set [min, max]
int getRNG(int min, int max){
  return (rand() / (float)RAND_MAX) * (max-min) + min;
}

// generates a random value
int setValue(){
  return getRNG(MIN_VALUE, MAX_VALUE);
}

/* MATRIX FUNCTIONS */

// generates random values for a matrix
void generateMatrix(int * matrix, int width, int height){
  for(int i=0; i < width * height; i++)
    matrix[i] = setValue();
}

// allocates memory for a matrix
int *allocateMatrix(int width, int height) {
  int *new_matrix = (int *)malloc(sizeof(int) * width * height);
  assert(new_matrix != NULL);

  return new_matrix;
}

// allocates memory for a row or column
int *allocateLine(int size) {
  int *new_line = (int *)malloc(sizeof(int) * size);
  assert(new_line != NULL);

  return new_line;
}

// prints a matrix
void printMatrix(char * matrix_name, int * matrix, int width, int height){
  printf("[%s(%d x %d)]\n\n", matrix_name, width, height);
  for(int i=0; i < height; i++){
    for(int j=0; j < width; k++)
      printf("%d ", matrix[i * width + j]);
    putchar('\n');
  }
}

// transposes a matrix
void transposeMatrix(char * matrix, int width, int height){
  char *temp_matrix = (char *)malloc(sizeof(int) * width * height);
  for(int i=0; i < height; i++)
    for(int j=0; j < width; j++)
      temp_matrix[i + j * height] = matrix[i * width + j];

  for(int i=0; i < width * height;i++)
    matrix[i] = temp_matrix[i];
  
  free(temp_matrix);
}

// calculates an element for the product matrix
int setProduct(char * row, char * column, int size){
  int result = 0;
  for(int i=0; i < size; i++){
    row[i] = row[i] * column[i];
    result+=row[i];
  }
  return result;
}

// pushes a calculated part of the matrix into it
void pushToMatrix(int * matrix, int * progress, int position, int length){
  for(int i=0; i < length; i++)
    matrix[position+1] = progress[i];
}

/* ---------------------------------------------------- */

int main(int argc, char** argv) {
  // init params check
  if(argc != 3){
    fprintf(stderr, "Usage: multi-max matrix-a-height matrix-b-width\n");
    exit(1);
  }

  // RNG seed start
  srand(time(NULL));

  // mpi init
  MPI_Init(NULL, NULL);

  // world_rank: current process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);  

  // world_size: number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // setting up matrixes dimentions
  int matrix_a_height = atoi(argv[1]);
  int matrix_b_width = atoi(argv[2]);
  int matrix_a_width = world_size-1;
  int matrix_b_height = world_size-1; 

  // setting up matrixes and their values
  int * matrix_a = NULL;
  if(world_rank == 0){
    matrix_a = allocateMatrix(matrix_a_width, matrix_a_height);
    generateMatrix(matrix_a, matrix_a_width, matrix_a_height);
   printMatrix(MATRIX_A, matrix_a, matrix_a_width, matrix_a_height);
  }

  int * matrix_b = NULL;
  if(world_rank == 0){
    matrix_b = allocateMatrix(matrix_b_width, matrix_b_height);
    generateMatrix(matrix_b, matrix_b_width, matrix_b_height);
    printMatrix(MATRIX_B, matrix_b, matrix_b_width, matrix_b_height);
    transposeMatrix(matrix_b, matrix_b_width, matrix_b_height);
  }

  int * matrix_c = NULL;
  if(world_rank == 0)
    matrix_c = allocateMatrix(matrix_a_height, matrix_b_width);

  // setting up auxilar values
  int * row = NULL;
  row = allocateLine(matrix_a_width);

  int * column = NULL;
  column = allocateLine(matrix_b_height);

  int * partial_result = NULL;
  partial_result = allocateLine(world_size-1);

  while(for progress=0; progress < matrix_a_height * matrix_b_width; progress+=(world_size-1)){
    // scatters matrix across slaves
    MPI_Scatter(matrix_a, matrix_a_width, MPI_INT, row, matrix_a_height, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(matrix_b, matrix_b_height, MPI_INT, column, matrix_b_height, MPI_INT, 0, MPI_COMM_WORLD);

    // gets the result for the product matrix
    int operation_result = setProduct(row, column, world_size-1);

    // gathers and prints the result matrix
    MPI_Gather(&operation_result, 1, MPI_INT, partial_result, world_size-1, MPI_INT, 0, MPI_COMM_WORLD)
    pushToMatrix(matrix_c, partial_result, progress, world_size-1);
  }
  printMatrix(MATRIX_C, matrix_c, matrix_a_height, matrix_b_width);

  // program shutdown
  if(world_rank == 0){
    free(matrix_a);
    free(matrix_b);
    free(matrix_c);
  }
  free(row);
  free(column);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
