// Timothy Tran, William Jorgensen
// Hub.cpp
/* The following program relies on 3 other programs for operation. It receives messages from ProbeA,ProbeB,ProbeC.
It will send a message telling ProbeA it has received a message. It will remotely close ProbeB once it reaches 10,000 messages.*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <random>
#include "force_patch.h"
//shortens call of std library functions
using namespace std;

//Pre-Condition:Start of Program
//Post-Condition:Exits by deallocating the message queue on completion.
int main(){
	int msgkey=1234; //key for message queue

	int qid = msgget(msgkey,IPC_CREAT | 0666); 	//creates the queue's id.
	int Aoff,Boff,Coff=0;						//flags for ProbeA, ProbeB, ProbeC being on or off
	int received=0;								//counts the amount of messges Hub has received.
	int pid=1;									//hold pid of ProbeB for exiting purposes.
	
	struct buf
	{
    	long mtype;// Required
    	char message[50]; //Message content
	};

	buf msg;									// initialize the msg
	int size = sizeof(msg)-sizeof(long);		// sets size of message to a variable.


	//Different messages
	while(true)
	{															// While loop continues running until all flags ProbeA, ProbeB, ProbeC are on
		
		if(msgrcv(qid, (struct msgbuf *)&msg, size,0,0)!=-1)	// First if statement: print message if there are any messages in queue
		{	
			if(msg.mtype==41){ 		//Hub took the message from ProbeA, need to resend it. Does not increment any values.
				msg.mtype = 41;
				strcpy(msg.message, "Message Received.");
				msgsnd(qid, (struct msgbuf *)&msg, size,0);
			}
			
			else{
				printf("%s\n",msg.message);
						
				if(received>10000&& pid!=1 ){						// Once Hub receives over 10,000 messages, force_patch ProbeB to terminate. Set ProbeB flag on.
					force_patch(pid);
					Boff=1;
				}
				
				if(msg.mtype==997){									// mtype 997 are messages from ProbeA
					msg.mtype = 41;
					strcpy(msg.message, "Message Received.");
					msgsnd(qid, (struct msgbuf *)&msg, size,0);		// Hub sends return message signaling it has received the message from ProbeA.
				}

				else if(msg.mtype==1){									// ProbeA sends mtype as 1 when it has closed
					Aoff=1;											// Set ProbeA flag on
				}
				else if(msg.mtype==3){									// ProbeC sends mtype as 3 when kill_patch is called to it.
					Coff=1;											// Set ProbeC flag on
				}
				else if(msg.mtype==100){							// ProbeB sends mtype as 100 when it sends its pid as the message.
					pid=stoi(msg.message);							// Convert message from string to int and set it as pid.
				}
					
				
				if(Aoff==1&&Boff==1&&Coff==1)							// Break out of the while loop, stop receiving messages when all Probe flags are on.
				{
					break;
				}

				received++;												// Count the number of messages received.
			}
		}
	}
	msgctl (qid, IPC_RMID, NULL);								// Clears the message queue.

	return 0;
}



