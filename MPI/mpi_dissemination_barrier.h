//necessary libraries for IO streams
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <cmath> 
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include <mpi.h>

#define MICROSECOND 1000000

// MPI variables & data structures.
int nodeID;
int numNodes;

MPI_Status status;

// Algorithm variables & data structures.
int rounds = -1;
int outMsg[1] = { -1 };
int inMsg[1]  = { -1 };
int tag = -1;
int dest = -1;
int src = -1;

void waitOnBarrier();
void waitOnBarrier()
{
	for(int round = 0; round < rounds; ++round)
	{	
		//calculate destination node based on sender rank
		dest = (nodeID + (int)exp2(round)) % numNodes;

		//send MSG to the desination node
		outMsg[0] = nodeID;  tag = nodeID; 
		MPI_Send(&outMsg, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);


		//calculate source node based on current (destination) node's rank
		src = (nodeID - (int)exp2(round));
		if(src < 0){
             src += numNodes ;
        }

		//wait to receive MSG from source node
		inMsg[0] = -1; tag = src;
		MPI_Recv(&inMsg, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);
		//printf("Round: %d Node:  %d  received MSG from: %d \n", z, rank, my_src);
			
		//usleep(10000);
	}	

}

