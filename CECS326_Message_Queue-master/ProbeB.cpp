// Timothy Tran, William Jorgensen
// ProbeB.cpp
/*The following program will generate a random value.
If that value is divisible by the magic number will send a message to the Hub.
The Hub will remotely close this program when the Hub has received more than 10000 messages.*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <random>
//shortens call of std library functions
using namespace std;

//Pre-Condition:Start of Program
//Post-Condition:Exits remotely by the Hub.
int main()
{
	//gen random number using distribution(rng)
	default_random_engine rng(random_device{}());
	uniform_int_distribution<> distribution(0,100000);
	int msgkey=1234;
	int qid = msgget(msgkey,IPC_CREAT | 0666); 					//Queue id.

	int value;

	struct buf
	{
    	long mtype;// Required
    	char message[50]; //Message content
	};

	buf msg;													//initializes the msg.
	
	// First message sent from ProbeB is the pid as msg.message with msg.mtype as 100.
	msg.mtype = 100;
	int size = sizeof(msg)-sizeof(long);						// initializes the size as a variable.
	strcpy(msg.message,to_string(getpid()).c_str());

	msgsnd(qid, (struct msgbuf *)&msg, size,0);

	while(true)													// ProbeB will continue to send messages until the Hub
	{															// receives over 10,000 messages and then forces ProbeB to exit via force_patch.

		value = distribution(rng);								// random value to compare with magic seed.
		msg.mtype = 257;										// magic seed set as 257
                
		if(value%msg.mtype==0)									// If random value is divisible by magic seed, message will be sent to Hub.
		{
			strcpy(msg.message, "ProbeB");
			msgsnd(qid, (struct msgbuf *)&msg, size,0);
			
		}
    }

	return 0;

}



