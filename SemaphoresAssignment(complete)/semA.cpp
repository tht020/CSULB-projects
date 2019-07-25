//Timothy Tran,
//Student IDs: 012398631,
//semA.cpp, semaphore.cpp,semaphore.h, coversem.txt
/* The following program creates 5 processes each which concurrently look through
blocks of shared memory and does a random swap between two chunks. When a swap
is performed, semaphore functions are used to safekeep that chunk's content until the
swap is completed. Each process performs a number of swaps equal to a number given
through user input. The last process to finish their swaps will clean up all semaphore
objects and shared memory objects and then the program exits.
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <vector>
#include <bits/stdc++.h>
#include <random>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semaphore.h"

//globally declare our constants and semaphores. 
int sems[12]={1,1,1,1,1,1,1,1,1,1,1,1};
int mux=1;
const int BUFFSIZE=512;
const int CHUNKAMT=3;
const int CHECK=5000;
const int CHILDAMT=4;

using namespace std;

int swapchunks(SEMAPHORE &,char *block1,char *block2,int seg1, int seg2,int id1,int id2);
void genchars (const vector<char> &sizecase,int size, char* address);


/*Pre-Condition:None
Parameters:None
Post-Condition:Outputs a vector of upper case chars.
*/
vector<char> uppercase(){
	vector<char> v={
	'A','B','C','D','E',
	'F','G','H','I','J',
	'K','L','M','N','O',
	'P','Q','R','S','T',
	'U','V','W','X','Y','Z'
	};
	return v;
}
/*Pre-Condition:None
Parameters:None
Post-Condition:Outputs a vector of lower case chars.
*/
vector<char> lowercase(){
	vector<char> v={
	'a','b','c','d','e',
	'f','g','h','i','j',
	'k','l','m','n','o',
	'p','q','r','s','t',
	'u','v','w','x','y','z'
	};
	return v;
}

/*Pre-Condition:File named semaphore.cpp and semaphore.h are in the same file location.
Parameters:None
Post-Condition:Swap is made during runtime, post runtime no memory blocks or semaphore objects exist.
*/
int main(){
	//variable initializations.
	int swaps;	
	int chID;
	int numofc=4;
	int scount=0;
	int speed_check;
	int status=0;

	int shmid1;
	char *shmBUF1;

	int shmid2;
	char *shmBUF2;

	int shmid3;
	char *shmBUF3;

	int shmid4;
	char *shmBUF4;
	
	int shmflag;
	int *shmFBUF;
	
	//initialize random number engines and distributions.
	default_random_engine rng(random_device{}());
	uniform_int_distribution<> checkdist(0,INT_MAX);
	uniform_int_distribution<> gdist(0,3);
	uniform_int_distribution<> chunkdist(0,2);
	
	//create semaphores for chunk protection and finish flags.
	SEMAPHORE sem(12);
	SEMAPHORE mutex(1);
	
	//increment all semaphores so they are of value 1.
	int k = 0;
	while(k < 11){
		sem.V(sems[k]);
		k++;
	}
	mutex.V(mux);
	
	//Share memory creation for chunks and flag.
	shmid1 = shmget(IPC_PRIVATE, CHUNKAMT*BUFFSIZE*sizeof(char), PERMS);
	shmBUF1 = (char *)shmat(shmid1, 0, SHM_RND);
	shmid2 = shmget(IPC_PRIVATE, CHUNKAMT*BUFFSIZE*sizeof(char), PERMS);
	shmBUF2 = (char *)shmat(shmid2, 0, SHM_RND);
	shmid3 = shmget(IPC_PRIVATE, CHUNKAMT*BUFFSIZE*sizeof(char), PERMS);
	shmBUF3 = (char *)shmat(shmid3, 0, SHM_RND);
	shmid4 = shmget(IPC_PRIVATE, CHUNKAMT*BUFFSIZE*sizeof(char), PERMS);
	shmBUF4 = (char *)shmat(shmid4, 0, SHM_RND);
	shmflag = shmget(IPC_PRIVATE,sizeof(int),PERMS);
	shmFBUF = (int *)shmat(shmflag,0,SHM_RND);
	
	//be able to select the shared memory through indexing.
	vector<char*> bufset={ shmBUF1,shmBUF2,shmBUF3,shmBUF4 };
	
	//fill the memory with required values.
	genchars(lowercase(),CHUNKAMT*BUFFSIZE,shmBUF1);
	genchars(uppercase(),CHUNKAMT*BUFFSIZE,shmBUF2);
	genchars(uppercase(),CHUNKAMT*BUFFSIZE,shmBUF3);
	genchars(uppercase(),CHUNKAMT*BUFFSIZE,shmBUF4);
	
	*shmFBUF = 0;

	//Request from user number of swaps.
	printf("Please input a value for how many swaps for each process\n");
	cin >> swaps;
	
	//If user does not put in a proper value, repeat request.
	while(cin.fail()){
		printf("Please input a value for how many swaps for each process\n");
		cin >> swaps;
	}
	
	//create requested number of children off same parent.
	while(numofc>0){
		//child
		if(chID=fork()){
			break;
		}
		//parent
		else{
			numofc= numofc-1;
		}
	}

	//do the required number of swaps per process.
	while(scount<swaps){
		//Only perform a swap if the generated number meets requirements.
		speed_check=checkdist(rng);
		if(speed_check<CHECK)
		{	
			/*randomly generate 4 values*/
			int group1 = gdist(rng);
			int group2 = gdist(rng);
			
			char* buf1 = bufset[group1];
			char* buf2 = bufset[group2];
			
			int chunk1 =chunkdist(rng);
			int chunk2 =chunkdist(rng);
			
			/*generate id's for chunk protection*/
			int cid1 = group1*CHUNKAMT + chunk1;
			int cid2 = group2*CHUNKAMT + chunk2;
			
			//perform the swap.
			swapchunks(sem,buf1,buf2,chunk1,chunk2,cid1,cid2);
			scount++;


		}		
	}
	
	//increment the flag upon completing swaps.
	if(*shmFBUF != CHILDAMT){
		mutex.P(mux);
		*shmFBUF += 1;
		mutex.V(mux);
	}
	
	else{
		//last process finishes and cleans up.
		wait(&status);
		shmctl(shmid1, IPC_RMID, NULL);
		shmctl(shmid2, IPC_RMID, NULL);
		shmctl(shmid3, IPC_RMID, NULL);
		shmctl(shmid4, IPC_RMID, NULL);
		shmctl(shmflag, IPC_RMID, NULL);
		sem.remove();
		mutex.remove();
		printf("Clean up\n");
	}
	exit(0);
	
}

/*Pre-Condition:A valid and instantiated memory address
Parameters:sizecase is any vector of chars,size is length of generation,address is memory address generate in
Post-Condition: address holds the randomly generated chars
*/
//generate characters into an allocated shared memory.
void genchars (const vector<char> &sizecase, int size, char* address)
{
	default_random_engine rng(random_device{}());
	uniform_int_distribution<> dist(0,25);

	for(int n=0;n<size;n++){
		address[n]=sizecase[dist(rng)];
	}
}

/*Pre-Condition: Semaphores are declared, memory addresses are instantiated
Parameters:sem is the name of semaphore to be used, block 1 and 2 are the memory block locations,
	seg1 and seg 2 are the numbers of the chunks in that block. id1 and id2 are the chunk identifiers for
	semaphore purposes.
Post-Condition:The chunks found in each block at seg1 and seg2 are swapped.
*/
//atomically swap chunks one character at a time.
int swapchunks(SEMAPHORE &sem,char* block1,char* block2,int seg1, int seg2,int id1,int id2){
	//Don't need to swap if we're working on the same chunk.
	if(id1!=id2){
		//Lock the chunks and perform the swap.
		if(!sem.P(sems[id1]) && !sem.P(sems[id2]))
		{
			for(int i = 0; i < BUFFSIZE; i++){
			char temp =*(block1+BUFFSIZE*seg1+i);
			*(block1+BUFFSIZE*seg1+ i)=*(block2+BUFFSIZE*seg2+ i);
			*(block2+BUFFSIZE*seg2+ i)=temp;
			}
				
			//Upon swap completion, unlock the chunks.
			if(!sem.V(sems[id1]) && !sem.V(sems[id2])){
				printf("Swap Completed\n");
			}
		}					
	}	
	else{
		printf("ids matched\n");
	}
	return 0;
	
}


