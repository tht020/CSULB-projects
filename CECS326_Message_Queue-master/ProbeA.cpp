// Timothy Tran, William Jorgensen
// ProbeA.cpp
/*The following program will generate a random value.
If that value is divisible by the magic number will send a message to the Hub.
It will wait for a return message from the Hub before attempting to send another message.
If the value generated is less than 100, it instantly exits and tells the Hub.*/

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
//Post-Condition:Exits on generating a value < 100 from the random generator.
int main()
{
	//gen random number using distribution(rng)
	default_random_engine rng(random_device{}());
	uniform_int_distribution<> distribution(0,100000);
	int msgkey=1234;
	int qid = msgget(msgkey,IPC_CREAT | 0666); //queue id for the message queue.
	
	int value;
	


	struct buf
	{
    	long mtype;// Required
    	char message[50]; //Message content
	};

	buf msg;																// initialized msg 
	msg.mtype = 997;														// ProbeA's mtype, its magic seed, set as 997
	strcpy(msg.message, "ProbeA");
	int size = sizeof(msg)-sizeof(long);									//set size of msg as a variable.

	while(true)
	{
		value = distribution(rng);											// random value
		
		if(value < 100)														// If random value is less than 100, stop sending messages
			{
				break;
			}
		else if(value%msg.mtype==0){						// If the value is divisible by magic seed, send message.
			
			msgsnd(qid, (struct msgbuf *)&msg, size,0);						// Sends message with mtype 997.
			
			msgrcv(qid, (struct msgbuf *)&msg, size,41,0);			// Received acknowledgment from the Hub that message was sent.
			printf("%s\n",msg.message);
			
			msg.mtype = 997;
			strcpy(msg.message, "ProbeA");
			
		}		
	
	}
	msg.mtype=1;															// Sets mtype to 1 for Hub to set its ProbeA flag on
	strcpy(msg.message,"ProbeA Closed");
	msgsnd(qid,(struct msgbuf *) &msg, size,0);								// Sends message with mtype as 1.

	return 0;

}



