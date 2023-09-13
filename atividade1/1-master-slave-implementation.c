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
#include <unistd.h> // For Linux
// #include <windows.h> // For Windows

/* GLOBAL VALUES */

#define MIN_NUMBERS 1000
#define MAX_NUMBERS 2000

#define MIN_VALUE 0
#define MAX_VALUE 99

#define MIN_TAG 0
#define MAX_TAG 3

#define MIN_TASKS 5
#define MAX_TASKS 5

#define MIN_SLEEP 1
#define MAX_SLEEP 1

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

// generates a random sleep time for slaves
void slaveRest(){
  sleep(getRNG(MIN_SLEEP, MAX_SLEEP));
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

// returns the lesser integer value
int minInt(int a, int b){
  if(a > b)
    return b;
  return a;
}

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

    default:
      return -1;
  }
}

// creates a random task and sends it to a slave
void createAndSendTask(int *array, int slave, int task){
  int tag = setTag();
  int size = setArraySize();
  setArrayRNG(array, size);

  printf("> MASTER: Slave nº%d will be assigned with task #%d: a tag %d task and a %d sized array\n", slave, task, tag, size);
  MPI_Send(array, size, MPI_INT, slave, tag, MPI_COMM_WORLD);
}

// receives a task from a slave, prints the result and returns the sender
void receiveAndPrintResult(MPI_Status * status){
  float slave_response;

  MPI_Recv(&slave_response, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
  printf("> MASTER: %f result received from slave %d\n", slave_response, status->MPI_SOURCE);
}

/* ---------------------------------------------------- */

int main(int argc, char** argv) {
  // program init
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
  
  const int tasks = setTasks();
  
  MPI_Status status;
  
  if (world_rank == 0) {
    // Master Process
    printf("> MASTER: %d tasks created for %d slaves to perform\n", tasks, world_size-1);

    int current_task = 0;
    for(; current_task < minInt(tasks, world_size-1); current_task++) // Initial task set
      createAndSendTask(numbers, current_task+1, current_task);

    for(; current_task < tasks; current_task++){ // Resending tasks
      receiveAndPrintResult(&status);
      createAndSendTask(numbers, status.MPI_SOURCE, current_task);
    }

    current_task = 0;
    for(; current_task < minInt(tasks, world_size-1); current_task++) // Receiving last tasks
      receiveAndPrintResult(&status);
    
    for(int slave=1; slave < world_size; slave++) // Turns slaves off
      MPI_Send(&slave, 1, MPI_INT, slave, 10, MPI_COMM_WORLD);
    
    if(current_task == minInt(tasks, world_size-1))
      printf("> MASTER: All tasks finished! Shutting down...\n");
  } else {
    // slave Process
    
    int current_size, current_tag;
    while(1){
      MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      current_tag = status.MPI_TAG;

      if(current_tag == 10){ // final tag
        printf("> SLAVE: Slave n%d finished all their tasks. Shutting down...\n", world_rank);
        break;
      }

      MPI_Get_count(&status, MPI_INT, &current_size);
      
      printf("> SLAVE: Task received from slave nº%d with tag %d and %d length\n", world_rank, current_tag, current_size);

      float returnValue = setOperation(current_tag, numbers, current_size);
      slaveRest();
      
      printf("> SLAVE: Slave nº%d obtained %f as the result of their task and sent back to master\n", world_rank, returnValue);
      MPI_Send(&returnValue, 1, MPI_FLOAT, 0, current_tag, MPI_COMM_WORLD);
    }
  }

  // program shutdown
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}