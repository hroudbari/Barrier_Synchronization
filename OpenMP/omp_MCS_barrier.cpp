#include "omp_MCS_barrier.h"

int main (int argc, char** argv){

		int max_threads = omp_get_max_threads();

		if(argc!=2){
			printf("\n\tPlease enter the number of threads to be used as command line argument.");
			printf("\n\t\t%s <#threads> \n",argv[0]);
			printf("\n\tThe max number of threads allowed on this system are %d\n\n",max_threads);
			exit(-1);
		}
		
		int threads = atoi(argv[1]);
		count = (int*) malloc(1*sizeof(int));
		*count=threads;
	
	    char *filename = (char*) calloc(100,sizeof(char));
        sprintf(filename, "%s__ompthreads_%d.csv", "omp_MCS_barrier", threads);
	    FILE *fp;
	    fp = fopen(filename, "a+");




		// Resize the nodeList to hold nodes equal to count*.
		nodeList = (node**) malloc((*count)*sizeof(node*));
		
		

		// Before entering the parallel region, initialize the
		// Arrival and wakeup trees for the nodes.	
		initializeTree(nodeList, *count);

		// Initialize.
		allWokeUp = 0;
		phase = true;

        /*

		// -------------------------------------------------------------------
		// Debug printing.
		std::cout<<"Number of elements in the nodeList: "<<(*count)<<std::endl;

		for(int i=0; i<*count; ++i){

			printf("Address: %x\n",nodeList[i]);
			printf("Id: %d\n",nodeList[i]->id);
			printf("numChildren: %d\n",nodeList[i]->numChildren);
			printf("wakeup: %d\n",nodeList[i]->wakeup);
			printf("Address_wakeup: %x\n",&(nodeList[i]->wakeup));
			printf("ChildWakeup[0]: %x, ",nodeList[i]->childWakeup[0]);
			printf("[1]: %x \n",nodeList[i]->childWakeup[1]);
			//printf("childWakeup: %d%d\n\n",*(nodeList[i]->childWakeup[0]),*(nodeList[i]->childWakeup[1]));
			
			printf("hasArrived: %x\n",nodeList[i]->hasArrived);
			printf("HaveChild : %d%d%d%d\n",nodeList[i]->haveChild[0],nodeList[i]->haveChild[1],nodeList[i]->haveChild[2],nodeList[i]->haveChild[3]);
			printf("childReady [0]: %x, ",&(nodeList[i]->childReady[0]));
			printf("[1]: %x, ",&(nodeList[i]->childReady[1]));
			printf("[2]: %x, ",&(nodeList[i]->childReady[2]));
			printf("[3]: %x \n",&(nodeList[i]->childReady[3]));
			printf("childReady: %d%d%d%d\n",nodeList[i]->childReady[0],nodeList[i]->childReady[1],nodeList[i]->childReady[2],nodeList[i]->childReady[3]);
			
			printf("\n\n");
		}
		//------------------------------------------------------

		*/


#pragma omp parallel num_threads(threads)
	{ 
		int threadID = omp_get_thread_num();

        // Declare structures to measure the time.
        struct timeval start, stop;       

       // Variable to store the cumulative delta.
        unsigned long long cumulativeDelta = 0;  // Remember this is per each node. 

        int numIterations = 100000;
		
		for(int iter=0; iter<numIterations; ++iter){

            /*
			#pragma omp critical
				{		
					std::cout<<"\tPrinting thread: "<<threadId<<" Iteration: ["<<itr<<"]"<<std::endl;
				}
            */

            // start the timer.
            gettimeofday(&start, NULL);   		

			// Wait on the barrier.
			waitOnBarrier(nodeList,threadID);

            // stop the timer.
            gettimeofday(&stop, NULL); 

		    unsigned long long delTime = (stop.tv_sec - start.tv_sec) * MICROSECOND + (stop.tv_usec - start.tv_usec);

            // Sum all the deltas seen by this node.
            cumulativeDelta += delTime;

			//fprintf(fp, "%d, %d, %lld\n", iter, threadID, delTime);
		}
          // Find the average over the total iterations.
          double avgDelTime = (double)cumulativeDelta / (double)numIterations;

          // Find the average over all the iterations.
          fprintf(fp, "Thread: %d, avg barrier-time over %d iterations is : %f\n", threadID, numIterations, avgDelTime);  
    
           #pragma omp critical
            {
                 printf("Thread: %d, avg barrier-time over %d iterations is : %f\n", threadID, numIterations, avgDelTime);
            }
        
	}


return 0;
}
