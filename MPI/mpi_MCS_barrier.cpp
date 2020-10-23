#include "mpi_MCS_barrier.h"


int main(int argc, char** argv){

	// Initalize the MPI Communication World.
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numNodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &nodeID);



    // Open a CSV file to output the time.
	char *filename = (char*) calloc(100,sizeof(char));
    sprintf(filename, "%s__mpinodes_%d__ompthreads_%d.csv", "mpi_MCS_barrier", numNodes, 1);
	FILE *fp;
	fp = fopen(filename, "a+");



    // Initialize the arrival children.
    numArrivalNodes = 0;


    // Initialize the arrival parent for this node;
    if(nodeID == 0){
        arrivalParent = -1;
    }else{
        if(nodeID < 4){
            arrivalParent = 0;
        }
        else if(nodeID%4==0){
            arrivalParent = (nodeID/4) - 1;
        }else{
            arrivalParent = (nodeID/4);
        }
    }

    // initialize the wakeup parent for this node.
    if(nodeID == 0){
        wakeupParent = -1;
    }else{
        if(nodeID < 2){
            wakeupParent = 0;
        }
        else if(nodeID%2==0){
            wakeupParent = (nodeID/2) - 1;
        }else{
            wakeupParent = (nodeID/2);
        }
    }


    for(int child=0;child<4; ++child){
        if((nodeID*4 + child + 1) < numNodes){
            arrivalNodes[child] = nodeID*4 + child + 1 ;
            ++numArrivalNodes;
        }
    }

    // Initialize the binary Wakeup Children.
    for(int child=0; child<2; ++child){
        if((nodeID*2 + 1 + child) < numNodes){
            wakeupNodes[child] = nodeID*2 + child + 1;
        }
    }
    

    /*

    // Printing the nodes seen by each nodeID:
    printf("\tNode: %d\n",nodeID);
    printf("\tArrivalParent: %d\n",arrivalParent);
    printf("\tWakeupParent: %d\n",wakeupParent);
    printf("\tWakeup: %d\n",wakeup);
    printf("\tphase: %d\n",phase);
    printf("\tnumArrivalNodes: %d\n",numArrivalNodes);
    printf("\tarrivalNodes: |%d|%d|%d|%d|\n",arrivalNodes[0],arrivalNodes[1],arrivalNodes[2],arrivalNodes[3]);
    printf("\twakeupNodes: |%d|%d|\n\n",wakeupNodes[0],wakeupNodes[1]);

   */
    
    // Declare structures to measure the time.
    struct timeval start, stop;

    // Variable to store the cumulative delta.
    unsigned long long cumulativeDelta = 0;  // Remember this is per each node. 

    int numIterations = 100000;

    for(int iter=0; iter<numIterations; ++iter){
        //printf("\t Printing from Node: %d, Iteration: %d\n",nodeID,itr); 

        // start the timer.
        gettimeofday(&start, NULL); 

        waitOnBarrier();

        // stop the timer.
        gettimeofday(&stop, NULL); 

	    unsigned long long delTime = (stop.tv_sec - start.tv_sec) * MICROSECOND + (stop.tv_usec - start.tv_usec);

        // Sum all the deltas seen by this node.
        cumulativeDelta += delTime;

		    //fprintf(fp, "%d, %d, %lld\n", iter, nodeID, delTime);
    }

    // Find the average over the total iterations.
    double avgDelTime = (double)cumulativeDelta / (double)numIterations;

    // Find the average over all the iterations.
    fprintf(fp, "Node: %d, avg barrier-time over %d iterations is : %f\n", nodeID, numIterations, avgDelTime);
    printf("Node: %d, avg barrier-time over %d iterations is : %f\n", nodeID, numIterations, avgDelTime);
    //MPI_Barrier(MPI_COMM_WORLD);
    // Finalize all the processes and close the MPI World
    MPI_Finalize();
}
