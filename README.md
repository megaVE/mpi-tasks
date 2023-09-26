# MPI Tasks (Tarefas MPI)
Resolução minha dos trabalhos proposto durante a disciplina de Programação Paralela e Distribuída no 6º Período do curso | My solution for the proposed tasks from the Parallel and Distributed Programming discipline, during the 6th period of the course

## Environment Setup (Configuração de Ambiente)

### Installing MPI on Linux (Instalando o MPI no Linux)
- apt-get install libopenmpi-dev openmpi-bin

### Creating a Hostfile (Criando o hostfile)
- localhost slots=10
sidenote: the text must be inside a file, typically named "hostfile"
(observação: o texto deve estar dentro de um arquivo, típicamente nomeado como "hostfile")

### Compiling (Compilando)
- mpicc [input file name].c -o [output file name]

### Running (Rodando)
- mpirun -np [number of processors] --hostfile [hostfile name] [output file name]

## 1 - Leader/Follower Implementation (Implementação Líder-Seguidores)
Complete development of a programa in which the Leader generates a random number of sum, mean, median and max value tasks to the elements of an array and distributes them to each Follower to perform.
(Desenvolvimento completo de um programa no qual o Líder gera um número aleatório tarefas de calcular soma, média, mediana e maior valor dos elementos de um vetor e as distribui para cada um dos Seguidores realizar.)

## 2 - Collective Messages (Mensagens Coletivas)
Complete development of a program that receives two input paramethers refering to the dimentions of two matrixes filled with random values and calculates their product, being each processor responsable for a column of the matrix B.
(Desenvolvimento completo de um programa que recebe dois parâmetros de entrada referentes às dimensões de duas matrizes preenchidas com valores aleatórios e calcula seu produto, com cada processador assumindo uma coluna da matriz B.)
