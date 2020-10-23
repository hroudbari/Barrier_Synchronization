#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

#define MICROSECOND 1000000


typedef struct node {
	int id;
	bool wakeup;
	int numChildren;
	bool haveChild[4];
	bool childReady[4]; 	

	// Need to hold pointers to the children
	// for sending output signals.
	bool* hasArrived;
	bool* childWakeup[2]={NULL};	

}node;


// Declare a vector to hold all the nodes of the List.
node** nodeList;

// 
static int* count;

// Binary Wakeup is zero, arrival phase is 1;
bool phase;


// Global variable to see if everyone got the wakeups.
int allWokeUp;




void initializeTree(node** nodeList, int &count);
void waitOnBarrier(node** nodeList, int &threadId);
bool haveChildrenArrived(node** nodeList, int &threadId);
void sendWakeupForChildren(node** nodeList, int &threadId);



void initializeTree(node** nodeList, int &count){

	std::cout<<"The count from the subroutne is : "<<count<<std::endl;

	// Step-1 create node for each thread / process.
	for(int id=0; id<count; ++id){
		nodeList[id] = (node*) malloc(1*sizeof(node));
		nodeList[id]->id = id;
		nodeList[id]->numChildren = 0;
		nodeList[id]->wakeup = false;
		nodeList[id]->hasArrived = NULL;
		nodeList[id]->childWakeup[0] = NULL;
		nodeList[id]->childWakeup[1] = NULL;
		

		memset(nodeList[id]->haveChild,0,4*sizeof(bool));
		memset(nodeList[id]->childReady,0,4*sizeof(bool));	
	
	}

	// Create and link the ArrivalTree.
	int temp = count;
	int itr = 0;

	while(temp>1){
		for(int child=0; child<4; ++child){

			if((4*itr+child+1) < count){	
				nodeList[itr]->haveChild[child]=1;
				++nodeList[itr]->numChildren;
				
				nodeList[4*itr+child+1]->hasArrived = &(nodeList[itr]->childReady[child]);

				--temp;
			}
		}
        
        // increment the iteration.
		++itr;
	}

	// create and ink the Binary Wakeup Tree.
	
	for(int node=0; node<count; ++node){

		if( (2*node+1) < count){
			nodeList[node]->childWakeup[0] = &(nodeList[2*node+1]->wakeup);
		}
		if( (2*node+2) < count){
			nodeList[node]->childWakeup[1] = &(nodeList[2*node+2]->wakeup);
		}
	
	}	

}


bool haveChildrenArrived(node** nodeList, int &threadId){
	
	int temp=0;
	for(int child=0; child<nodeList[threadId]->numChildren; ++child){
		if(nodeList[threadId]->haveChild[child] && nodeList[threadId]->childReady[child]){
			++temp;
		}
	}
	if(temp == nodeList[threadId]->numChildren){
		return true;
	}else{
		return false;
	}

}

void sendWakeupForChildren(node** nodeList, int &threadId){
	for(int child=0; child<2; ++child){
		if(nodeList[threadId]->childWakeup[child]!= NULL){
			*(nodeList[threadId]->childWakeup[child])=true;
		}
	}
}

void waitOnBarrier(node** nodeList, int &threadId){

	while(1){

		// Check if you have received a binary wakeup call
		if(nodeList[threadId]->wakeup){

			// Clear the flags in childReady array.
			memset(nodeList[threadId]->childReady,0,4*sizeof(bool));
						
			// First notify the children
			sendWakeupForChildren(nodeList, threadId);


			// Reset your own wakeup flag as well.
			nodeList[threadId]->wakeup = false;

			// update the allWokeUp flag.
			#pragma omp critical
			{
				++allWokeUp;
			
				if(allWokeUp >= *count){
					allWokeUp = 0;
					phase = true;
				}
			}
			break;

		}else{
			
			// Check if this is a root Node.
			if(nodeList[threadId]->hasArrived == NULL){

				// Check if all the children have arrived at the barrier.
				if( haveChildrenArrived(nodeList,threadId) && phase ){

					// Set the Binary Wakeup Phase.
					phase=false;
					//printf("\n");

					// Set your own wakeup flag since you are the root.
					nodeList[threadId]->wakeup = true;

				}else{

					// Clear the flags in childReady array.
					memset(nodeList[threadId]->childReady,0,4*sizeof(bool));
				}
		
			}else if((nodeList[threadId]->numChildren) == 0){ // check if this is a Leaf node.								
					// Set your own arrival flag with parent since you hit the barrier.
					if(phase){

                        // 
						*(nodeList[threadId]->hasArrived) = true;

					}else{

						// Clear the flags in childReady array.
						memset(nodeList[threadId]->childReady,0,4*sizeof(bool));
					}
		
			}else{ // This is a middle node. 
		
				// Check if all the children have arrived at the barrier.
				if( haveChildrenArrived(nodeList,threadId) && phase ){
					
                    // Set your arrival flag with the parent
					*(nodeList[threadId]->hasArrived) = true;

				}else{

					// Clear the flags in childReady array.
					memset(nodeList[threadId]->childReady,0,4*sizeof(bool));
				}
			}
		}

	} // END of While loop.
}


