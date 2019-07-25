//Timothy Tran 
//Student ID: 012398631
//forkprogram.cpp, coverfork.txt,testcase.txt,Testdoc.txt
/*The following program reads the contents of a file (Testdoc.txt).
  The program then saves a copy into shared memory where the user
  will perform string replacement on it until the user types !wq.
*/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <boost/algorithm/string.hpp>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>


using namespace std;


/*
Pre-Condition: Have a .txt file named Testdoc in the same directory.
			   Have at least 8192 bytes of memory available.
Parameters:None.
Post-Condition: Child exits, shared memory is cleared, main program exits
				with changes to streamed string unsaved.
*/
int main(){
	
	//initialize strings for string replacement.
	string tstring;
	string rstring;
	string repfile;
	string::size_type position;
	
	//initialize parameters for child/parent processes.
	pid_t pid;
	int status;	
	int count;
	
	//Read from the Testdoc.txt file and save it as a string.
	ifstream textfile("Testdoc.txt");
	string prepfile((istreambuf_iterator<char>(textfile)),(istreambuf_iterator<char>()));

	//Create a block of shared memory that will hold the contents read from Testdoc.txt.
	key_t key = ftok("keygen",33);
    int sh_mid= shmget(key,8192,0666|IPC_CREAT);
	
	char* savedata =(char*) shmat(sh_mid,(void*)0,0);
	
	strcpy(savedata,const_cast<char*>(prepfile.c_str()));
		
		
	//Continuously read user input and do string replacement until !wq is typed.
	while(true){
		//Refresh data collection on every loop.
		repfile=savedata;
		position=0;
		count = 0;
		
		//Read user input for string replacement.
		printf("Please input a target string.\n");

		cin >> tstring;
		cin.clear();
		cin.ignore(10000,'\n');

		if(tstring=="!wq")
			break;

		printf("Please input a replacement string.\n");

		cin >> rstring;
		cin.clear();
		cin.ignore(10000,'\n');

		if(rstring=="!wq")
			break;
		
		//Split into child and parent processes
		
		//Fork fails.
		if((pid=fork())<0){
			printf("Fork failed!\n");
			exit(1);
		}
		//Child process.
        else if(pid==0){
			//Child counts how many instance of string exists in the document. 
			while((position=(repfile).find(tstring,position))!=string::npos){			

				count+=1;
				position+=tstring.length();
			
			}
			// Use boost algorithm to replace all instances of the string in the document.
            boost::replace_all(repfile,tstring,rstring);
			
			//FORCED ERROR:WILL REQUIRE USER TO REMOTELY KILL CHILD.
			while(count==0){
				//infinite loop where string replacement is attempted and every attempt prints '.'
				boost::replace_all(repfile,tstring,rstring);
				printf("%c",'.');
			}
			
			//Child reads out the count. Copies over the changed document to shared memory.
			printf("Child says that there's %d instances of replacement\n",count);
			
			strcpy(savedata,const_cast<char*>(repfile.c_str()));			
            shmdt(savedata);
			
			exit(1);
		}
		
		//Parent Process
		else{
			//Parent waits until child exits
			wait(&status);
		}

	}
	//Detach contents into shared memory.
	shmdt(savedata);
	//Clear shared memory.
	shmctl(sh_mid,IPC_RMID,NULL);
	//Exit program.
	exit(0);
}
