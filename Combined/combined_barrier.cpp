#include "combined_barrier.h"


//main function
int main (int argc, char *argv[]){

    // Initialize the number of OMP threads on this node.
    max_threads = omp_get_max_threads();

	if(argc!=2){
		printf("\n\tPlease enter the number of OMP threads to be used as command line argument.");
		printf("\n\t\t%s <#threads> \n",argv[0]);
		printf("\n\tThe max number of threads allowed on this node are %d\n\n",max_threads);
		exit(-1);
	}

    // Get the number of omp threads and initialize shared count variable.
    threads = atoi(argv[1]);
    count = (int*) calloc(1,sizeof(int));
    *count=threads;

	
    // Initalize the MPI Communication World.
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numNodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &nodeID);



    // open a csv file to record the barrier measurements.
	char *filename = (char*) calloc(100,sizeof(char));
    sprintf(filename, "%s__mpinodes_%d__ompthreads_%d.csv", "combined_barrier", numNodes, threads);
	FILE *fp;
	fp = fopen(filename, "a+");





	//calculating number of rounds based on total num of nodes.
	rounds = log2(numNodes);
	if(exp2(rounds) < numNodes){
        ++rounds;
    }


  // Initialize the omp parallel region.
  #pragma omp parallel num_threads(threads)
	{
        bool localSense = globalSense; 
        int threadID = omp_get_thread_num();

        // Declare structures to measure the time.
        struct timeval start, stop;

        // Variable to store the cumulative delta.
        unsigned long long cumulativeDelta = 0;  // Remember this is per each node. 

        int numIterations = 10000;

        for(int iter=0; iter<numIterations; ++iter){

            // start the timer.
            gettimeofday(&start, NULL); 

		    // Update the local sense.
			localSense = globalSense;

		    #pragma omp critical
		    {
                //printf("Printing from Node: %d, Thread: %d, Iteration: %d\n",nodeID, threadID,itr);

			    // Straggler thread should decrement the count here.
			    (*count)--;
		    }

		    // Reset the count to max number of threads if count is zero.
		    if(*count==0) {

                // straggler thread has to enter the MPI barrier.
                waitOnBarrier();

                // Now reset the count and flip the sense to release the local
                // threads for next iteration.
			    *count=threads;                 
			    globalSense = !(globalSense);

		    }else{
                // Spin on the globalSense flag to be flipped
                // after the barrier has been reached.
			    while(localSense==globalSense);		

		    }	

            // stop the timer.
            gettimeofday(&stop, NULL); 

		    unsigned long long delTime = (stop.tv_sec - start.tv_sec) * MICROSECOND + (stop.tv_usec - start.tv_usec);

            // Sum all the deltas seen by this node.
            cumulativeDelta += delTime;
			//fprintf(fp, "%d, %d, %lld\n", iter, nodeID, delTime);	

	    }  // END of for loop on threads      

          // Find the average over the total iterations.
          double avgDelTime = (double)cumulativeDelta / (double)numIterations;

          // Find the average over all the iterations.
          fprintf(fp, "Node: %d Thread: %d, avg barrier-time over %d iterations is : %f\n", nodeID, threadID, numIterations, avgDelTime);  
    
           #pragma omp critical
            {
                 printf("Node: %d Thread: %d, avg barrier-time over %d iterations is : %f\n", nodeID, threadID, numIterations, avgDelTime);
            }

    } // End of parallel region.    	

	MPI_Finalize();
	return 0;
}
