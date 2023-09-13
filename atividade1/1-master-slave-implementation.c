/* ----------------------------------------------------
* Autor: Vinícius Eduardo de Souza Honório
* Matrícula: 2021.1.08.024
* ---------------------------------------------------- */
 
/*
* Exercício
* Criar um modelo master/slave onde o processo 0 é o master que "dispara" tarefas diversificadas para os slaves. Os slaves saberão qual
* tarefa realizar pelo valor do TAG recebido. Assim que um slave terminar a tarefa, o master deverá atribuir uma nova tarefa
* TAG=0 -> Soma dos valores
* TAG=1 -> Média dos valores
* TAG=2 -> Maior valor
* TAG=3 -> Mediana dos valores
* TAG=10 -> Finalizar
*/

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

/* GLOBAL VALUES */

#define MIN_NUMBERS 1000
#define MAX_NUMBERS 2000

#define MIN_VALUE 0
#define MAX_VALUE 99

#define MIN_TAG 0
#define MAX_TAG 3

#define MIN_TASKS 10
#define MAX_TASKS 20

/* TASKS FUNCTIONS */

// tag 0: returns the sum of the values
float sumValues(int *data, int size){
  float sum = 0;
  for(int i = 0; i < size; i++)
    sum+=data[i];
  return sum;
}

// tag 1: returns the mean of the values
float meanValues(int *data, int size){
  return (float)sumValues(data, size)/size;
}

// tag 2: returns the greatest value
float maxValues(int *data, int size){
  float max = 0;
  for(int i = 0; i < size; i++)
    if(data[i] > max)
      max = data[i];
  return max;
}

// tag 3: returns the median of the values
float medianValues(int *data, int size){
  for(int i = 0; i < size-1; i++){ // bubble sort
    for(int j = 0; j < size-i-1; ++j){
      if(data[i] > data[j]){
        int aux = data[i];
        data[i] = data[j];
        data[j] = aux;
      }
    }
  }
  return (float)data[size/2];
}

// set the operation according to the received tag
float setOperation(int tag, int *data, int size){
  switch(tag){
    case 0:
      return sumValues(data, size);
    
    case 1:
      return meanValues(data, size);

    case 2:
      return maxValues(data, size);

    case 3:
      return medianValues(data, size);

    case 10:
      return -1;

    default:
      return -10;
  }
}

/* GENERAL FUNCTIONS */

// generates a random int value from the numeric set [min, max]
int getRNG(int min, int max){
  return (rand() / (float)RAND_MAX) * (max-min) + min;
}

// generates a random number of tasks
int setTasks(){
  return getRNG(MIN_TASKS, MAX_TASKS);
}

// generates a random array size
int setArraySize(){
  return getRNG(MIN_NUMBERS, MAX_NUMBERS);
}

// generates a random tag
int setTag(){
  return getRNG(MIN_TAG, MAX_TAG);
}

// generates a random value
int setValue(){
  return getRNG(MIN_VALUE, MAX_VALUE);
}

// sets an array with random numbers
void setArrayRNG(int *array, int size){
  for(int i=0; i < size; i++)
    array[i] = setValue();
}

// sets the next the slave
int setSlave(int slave, int amount){
  if(slave < amount)
    return ++slave;
  return 1;
}

/* ---------------------------------------------------- */

int main(int argc, char** argv) {
  // program init
  MPI_Status status;
  MPI_Init(NULL, NULL);

  // RNG seed start
  srand(time(NULL));

  /* VAR LIST */
  
  // world_size: number of process
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // world_rank: current process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // numbers: task array of values
  int numbers[MAX_NUMBERS];
  
  const int tasks = 2; //setTasks()
  
  int tag, size;
  
  if (world_rank == 0) {
    // Master Process
    printf("%d tasks created for %d slaves to perform\n", tasks, world_size-1);

    int slave = 0;    
    for(int task_number = 0; task_number < tasks; task_number++){ // For each slave
      slave = setSlave(slave, world_size-1);
      tag = setTag();
      size = setArraySize();
      setArrayRNG(numbers, size);
      printf("Slave nº%d will be assigned with task #%d: a tag %d task and a %d sized array\n", slave, task_number, tag, size);

      MPI_Send(numbers, size, MPI_INT, slave, tag, MPI_COMM_WORLD);
    }

    // for(int p = 1; p < world_size; p++) // Ends task sending
    //   MPI_Send(0, 1, MPI_INT, p, 10, MPI_COMM_WORLD);
    // for(int p = 1; p < world_size; p++){
    //   MPI_Recv(&somaproc, 1, MPI_INT, p, 5, MPI_COMM_WORLD, &status);
    //   soma += somaproc;
    // }
    // printf("A soma total foi %d e a soma local foi %d\n", soma, somalocal);
    printf("All tasks finished! Shutting down...\n");
  } else {
    // slave Process
    int currentSize, currentTag;
    MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    currentTag = status.MPI_TAG;
    MPI_Get_count(&status, MPI_INT, &currentSize);
    
    printf("Task received from slave nº%d with tag %d and %d length\n", world_rank, currentTag, currentSize);

    float returnValue = setOperation(currentTag, numbers, currentSize);
    MPI_Send(&returnValue, 1, MPI_FLOAT, 0, currentTag, MPI_COMM_WORLD);
    printf("Slave nº%d obtained %f as the result of their task and sent back to master\n", world_rank, returnValue);
  }

  // program shutdown
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}


