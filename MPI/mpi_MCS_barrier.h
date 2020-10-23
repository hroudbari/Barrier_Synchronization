#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sys/time.h>
#include <time.h>

#include <mpi.h>

#define MICROSECOND 1000000

int nodeID;
int numNodes;


int numArrivalNodes;

int arrivalNodes[4] = {-1, -1, -1, -1};
int arrivalParent = -1;

int wakeupNodes[2] = {-1, -1};
int wakeupParent = -1;

int inMsg[1]  = { -1 };
int outMsg[1] = { -1 };

int src = -1;
int dest = -1;

int tag = -1;
MPI_Status status;

void waitOnBarrier();



void waitOnBarrier(){

    if( (arrivalParent != -1) && (numArrivalNodes == 0) ){  // Arrival-Tree Leaf-node.
       // Send the arrival information to the parent.
       outMsg[0]=nodeID; tag = nodeID; dest = arrivalParent;
       MPI_Send(&outMsg, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);

       // Wait for the wakeup again from wakeup parent.
       inMsg[0] = -1; tag = wakeupParent; src = wakeupParent; 
       MPI_Recv(&inMsg, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);     

       // Send wakeups to children as per WakeupTree.         
       for(int child=0; child<2; ++child){
          if(wakeupNodes[child] != -1){
              outMsg[0]=nodeID; tag = nodeID; dest = wakeupNodes[child];
              MPI_Send(&outMsg, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
          }       
       }

       //break;

    }else if( (arrivalParent != -1) && (numArrivalNodes > 0) ){ // arrival-Tree Middle-Node
        // Poll for the arrival-children messages.
        for(int child=0; child<numArrivalNodes; ++child){
            inMsg[0] = -1; tag = arrivalNodes[child]; src = arrivalNodes[child];         
            MPI_Recv(&inMsg, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);
        }

       // Now, send the arrival information to the parent.
       outMsg[0]=nodeID; tag = nodeID; dest = arrivalParent;
       MPI_Send(&outMsg, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
        
       // Wait for the wakeup again from wakeup parent.
       inMsg[0] = -1; tag = wakeupParent; src = wakeupParent; 
       MPI_Recv(&inMsg, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);  

       // Once wakeup received, send the wakeup to children. 
       for(int child=0; child<2; ++child){
            if(wakeupNodes[child] != -1){
                outMsg[0]=nodeID; tag = nodeID; dest = wakeupNodes[child];
                MPI_Send(&outMsg, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            }       
        }

       //break;

    }else if( (arrivalParent == -1) ){ // Root Node
        // Poll for the arrival children messages.    
        for(int child=0; child<numArrivalNodes; ++child){
            inMsg[0] = -1; tag = arrivalNodes[child]; src = arrivalNodes[child];         
            MPI_Recv(&inMsg, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);
        }

        /*

        printf("\n\t--------------------------------");
        printf("\n\t\tBarrier Reached");
        printf("\n\t--------------------------------\n\n");
        
        usleep(10000);

        */

        // Reached the barrier. Send the wakeups to children.
        for(int child=0; child<2; ++child){
            if(wakeupNodes[child] != -1){
                outMsg[0]=nodeID; tag = nodeID; dest = wakeupNodes[child];
                MPI_Send(&outMsg, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            }       
        }

        //break;
    }

}   












