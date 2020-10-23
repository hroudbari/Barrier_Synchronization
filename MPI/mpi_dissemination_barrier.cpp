#include "mpi_dissemination_barrier.h"

using namespace std;

//main function
int main (int argc, char *argv[]) 
{	
    // Initalize the MPI Communication World.
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numNodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &nodeID);


    // Open a csv file to record the measurements for barrier.
	char *filename = (char*) calloc(100,sizeof(char));
    sprintf(filename, "%s__mpinodes_%d__ompthreads_%d.csv", "mpi_dissemination_barrier", numNodes, 1);
	FILE *fp;
	fp = fopen(filename, "a+");


	//calculating number of rounds based on total num of nodes.
	rounds = log2(numNodes);
	if(exp2(rounds) < numNodes){
        ++rounds;
    }

    // Declare structures to measure time.
    struct timeval start, stop;

    // Variable to store the cumulative delta.
    unsigned long long cumulativeDelta = 0;  // Remember this is per each node. 

    int numIterations = 100000;


    for(int iter=0; iter<100000; ++iter){

        // start the timer.
        gettimeofday(&start, NULL);  

        // straggler thread has to enter the MPI barrier.
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


	MPI_Finalize();
	return 0;
}
