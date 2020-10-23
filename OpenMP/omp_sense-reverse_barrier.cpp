#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <sys/time.h>
#include <time.h>

#include <omp.h>

#define MICROSECOND 1000000


int main (int argc, char** argv){

	int max_threads = omp_get_max_threads();

	if(argc!=2){
		printf("\n\tPlease enter the number of threads to be used as command line argument.");
		printf("\n\t\t%s <#threads> \n",argv[0]);
		printf("\n\tThe max number of threads allowed on this node are %d\n\n",max_threads);
		exit(-1);
	}
	
	int threads = atoi(argv[1]);
	int *count = (int*) calloc(1,sizeof(int));
	
	*count=threads;

	bool globalSense = false;

	char *filename = (char*) calloc(100,sizeof(char));
    sprintf(filename, "%s__ompthreads_%d.csv", "omp_sense-reverse_barrier", threads);
	FILE *fp;
	fp = fopen(filename, "a+");



    #pragma omp parallel num_threads(threads) default(shared)
	{ 
		bool currentSense = globalSense; 
        int threadID = omp_get_thread_num();

        // Declare structures to measure the time.
        struct timeval start, stop;

        // Variable to store the cumulative delta.
        unsigned long long cumulativeDelta = 0;  // Remember this is per each node. 

        int numIterations = 100000;


		for(int iter=0; iter<numIterations; ++iter){

            // start the timer.
            gettimeofday(&start, NULL);            

			// Update the local sense.
			currentSense = globalSense;

			#pragma omp critical
			{
                //printf("Printing thread: %d, Iteration: %d\n",threadID,iter);

				// Decrement the count here.
				(*count)--;
			}

			// Reset the count to max number of threads if count is zero.
			if(*count==0) {
				*count=threads;
				globalSense = !(globalSense);

			}else{
				//while(*count>0);
				while(currentSense==globalSense);		
			}		

            // stop the timer.
            gettimeofday(&stop, NULL); 

		    unsigned long long delTime = (stop.tv_sec - start.tv_sec) * MICROSECOND + (stop.tv_usec - start.tv_usec);
            
            // Sum all the deltas seen by this node.
            cumulativeDelta += delTime;
			//fprintf(fp, "%d, %d, %lld\n", iter, threadID, delTime);

		} // End of for-loop

          // Find the average over the total iterations.
          double avgDelTime = (double)cumulativeDelta / (double)numIterations;

          // Find the average over all the iterations.
          fprintf(fp, "Thread: %d, avg barrier-time over %d iterations is : %f\n", threadID, numIterations, avgDelTime);  
    
           #pragma omp critical
            {
                 printf("Thread: %d, avg barrier-time over %d iterations is : %f\n", threadID, numIterations, avgDelTime);
            }
	} // End of Parallel region

return 0;
}
