/**
 * Código base (incompleto) para implementação de relógios vetoriais.
 * Meta: implementar a interação entre três processos ilustrada na figura
 * da URL: 
 * 
 * https://people.cs.rutgers.edu/~pxk/417/notes/images/clocks-vector.png
 * 
 * Compilação: mpicc -o rvet rvet.c
 * Execução:   mpiexec -n 3 ./rvet
 */
 
#include <stdio.h>
#include <string.h>  
#include <mpi.h>     


typedef struct Clock { 
   int p[3];
} Clock;

void Event(int pid, Clock *clock){
   clock->p[pid]++;   
}


void Send(int pid, int dest, Clock *clock){
   //printf("%d enviando para %d\n", pid, dest);
   Event(pid, clock);
   MPI_Send(clock, 3, MPI_INT, dest, 0, MPI_COMM_WORLD); // Envia a mensagem pro processo dest
   
}

void Receive(int pid, int enviou, Clock *clock){
   //printf("%d recebendo de %d\n", pid, enviou);
   
   Clock recebido;
   
   MPI_Recv(&recebido, 3, MPI_INT, enviou, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recebe a mensagem do processo enviou
   //printf("%d %d %d\n", recebido.p[0], recebido.p[1], recebido.p[2]);
   Event(pid, clock);
   int i;
   for(i=0; i < 3; i++){ 
      //Atualiza o relogio
      if(recebido.p[i] > clock->p[i]){
         clock->p[i] = recebido.p[i];
      }
   }
}

// Representa o processo de rank 0
void process0(){
   Clock clock = {{0,0,0}};
   Event(0, &clock); 
   Send(0, 1, &clock);
   Receive(0, 1, &clock);
   Send(0, 2, &clock);
   Receive(0, 2, &clock);
   Send(0, 1, &clock);
   Event(0, &clock);
   printf("Process: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);
}

// Representa o processo de rank 1
void process1(){
   Clock clock = {{0,0,0}};
   Send(1, 0, &clock);
   Receive(1, 0, &clock);
   Receive(1, 0, &clock);
   printf("Process: %d, Clock: (%d, %d, %d)\n", 1, clock.p[0], clock.p[1], clock.p[2]);
}

// Representa o processo de rank 2
void process2(){
   Clock clock = {{0,0,0}};
   Event(2, &clock);
   Send(2, 0, &clock);
   Receive(2, 0, &clock);
   printf("Process: %d, Clock: (%d, %d, %d)\n", 2, clock.p[0], clock.p[1], clock.p[2]);
}

int main(void) {
   int my_rank;               

   MPI_Init(NULL, NULL); 
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   
   
   if (my_rank == 0) { 
      process0();
   } else if (my_rank == 1) {  
      process1();
   } else if (my_rank == 2) {  
      process2();
   }

   /* Finaliza MPI */
   MPI_Finalize(); 

   return 0;
}  /* main */
