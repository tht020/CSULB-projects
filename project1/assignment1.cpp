// Timothy Tran
// assignment1.cpp, cover1.txt
// The following program will physically allocate memory to hold random chars in arrays of predetermined sizes.

#include <iostream>
#include <vector>
#include <random>
#include <functional> 
#include <algorithm>  
#include <memory>
#include <array>
#include <string>
#include <vector>
#include <bits/stdc++.h> 

#define ARRAY_SIZE 20


//The struct we will be using will be an object that holds the char pointers in an array with 2 arrays of the same length
//to determine the size of the pointers and their current allocation status.
struct trackchars {
	
    char* charptrs[ARRAY_SIZE]={0};
    int indexsize[ARRAY_SIZE]={0};
    int deleted[ARRAY_SIZE]={0};
	
	
}randchars;

//This function is to make a set of uppercase chars to randomly select from
//Pre-Condition:None
//Post-Condition:outputs char vector.
std::vector<char> upcasechars(){
	std::vector<char> v ={
				'A','B','C','D','E','F',
			'G','H','I','J','K',
			'L','M','N','O','P',
			'Q','R','S','T','U',
			  'V','W','X','Y','Z'};
	return v;
}

//Runs only once at the start. Will allocate memory needed for the char pointers and then fill the memory with random chars
//Pre-Condition:None
//Post-Condition:All char pointers are allocated and filled
void allocateAll () {
	
	//creating the random generator
	std::vector<char> ch_set = upcasechars();
	std::default_random_engine rng(std::random_device{}()); 
    std::uniform_int_distribution<> dist(0, ch_set.size()-1);
	
	//initialized f(0) state
	int n = 0;
	randchars.indexsize[n]=2700;
	
	//function is f(n+1)=2*f(n) for the size of each pointer. goes up to f(n+1)=20.
	for(n; n<19; n++){
		randchars.indexsize[n+1]=2*(randchars.indexsize[n]);
	}
	

	for(n=0; n<20; n++){
		
		//allocates the memory according to size held in indexsize[n]
		randchars.charptrs[n]=new(std::nothrow) char [(randchars.indexsize[n])];
		
		//inidividually fills each byte in our char pointer with a random char. O(i)
		for(int i =0;i<(randchars.indexsize[n]);i++){
				randchars.charptrs[n][i]=ch_set[dist(rng)];
		}

		//prints to keep track of program's status
		printf("Allocated Array number: %d \n",n);
	}
	
}

//Runs when re-allocating pointers at an index.
//Pre-Condition:the charptr entry at allindex is found to contain no allocated memory
//Post-Condition:the charptrs entry is allocated and filled with chars
void allocateOne(int allindex){

	//creating random generator
	std::vector<char> ch_set = upcasechars();
	std::default_random_engine rng(std::random_device{}()); 
    std::uniform_int_distribution<> dist(0, ch_set.size()-1);
	
	//allocates memory according to the size held in indexsize[n] and fills it byte by byte
	randchars.charptrs[allindex]=new(std::nothrow) char [(randchars.indexsize[allindex])];
	for(int i =0;i<(randchars.indexsize[allindex]);i++){
				randchars.charptrs[allindex][i]=ch_set[dist(rng)];
	}
	
}

//Runs when accessing an index.
//Pre-Condition:an index is selected to be read or deleted
//Post-Condition:submenu exits either by deleting the index for the charptrs or manually
int subcommands(int pointerdex){

	//checks if the index is deleted then calls allocateOne.
	if(randchars.deleted[pointerdex]==1){
		printf("This index has been deallocated. Reallocating...\n");
		allocateOne(pointerdex);
		randchars.deleted[pointerdex]=0;
	}
	
	//clears from main menu's input.
	std::cin.clear();
	std::cin.ignore(10000,'\n');
	char option = '0';
	
	//can continuously list. will move to main menu after a delete.
	while(option != '3'){
		
		printf("Sub-Menu: \n (1) Print the first 10 char's in the chosen array \n (2) Delete all the char's associated with this pointer \n (3) Return to main menu\n");
		std::cin.get(option);
				
		//ends the sub-menu.
		if(option == '3'){
			break;
		}
		
		//lists the first 10 entries of the pointer 
		else if(option=='1'){
			for(int n=0;n<10;n++){
			std::cout<<randchars.charptrs[pointerdex][n];
			std::cout<<'\n';
			}
		}
		
		//deletes the pointers at the desired index and then flags it as deleted 
		else if(option == '2'){
			randchars.deleted[pointerdex]=1;
			delete[] randchars.charptrs[pointerdex];
			printf("Chars deleted\n");
			
			break;
		}

		//when an invalid command is run.
		else{
			printf("Invalid subcommand.\n");
		}
		std::cin.clear();
		std::cin.ignore(10000,'\n');
	}
	//return statement for extension of sub-menu output.
	return 0;
}

//Pre-Condition:Start of Program
//Post-Condition:Exits with all memory de-allocated cleanly.
int main(){
	
	//Start with allocating the memory.
	printf("Allocating...\n");
	allocateAll();
	char option = '0';
    int index;
	
	//Can continuously run the program until exit or goes into sub-command.
	while(option != '4'){
		
		printf("Main Menu: \n (1) Access a pointer (index) \n (2) List deallocated memory (index) \n (3) Deallocate all memory \n (4) Exit program \n");
		
        std::cin.get(option);
		
		//Exit the program
		if(option=='4'){
		  printf("Exiting...\n");
		  break;
		}
		//Access the index.
		else if(option=='1'){
			printf("Type in the index(0-19):\n");
			
			std::cin>>index;
			while(std::cin.fail()){
				std::cin.clear();
				std::cin.ignore(10000,'\n');
				printf("Not a valid index. Please enter a value from 0 to 19.\n");
				std::cin>>index;
			}
            subcommands(index);
		}
		//list indexes of de-allocated pointers.
		else if(option == '2'){
			printf("The indexes of deallocated memory are:\n");
               for(int n=0;n<20;n++){
					if(randchars.deleted[n]==1)
						printf("%d \n",n);
				}
			
		}
		//delete all the pointers not already deleted and flag those as deleted.
		else if(option == '3'){
                        
			
                   for(int n=0;n<20;n++){
                       if(randchars.deleted[n]!=1)
                           delete[] randchars.charptrs[n];
                           randchars.deleted[n]=1;
			}
			
			printf("All indexes have been deleted\n");
		}
		//invalid command, loop back
		else{
			printf("Invalid command.\n");
		}
		
		//clear for next loop.
		std::cin.clear();
		std::cin.ignore(10000,'\n');
	}
	
	//clear our memory before exiting.
	for(int n=0;n<20;n++){
            if(randchars.deleted[n]!=1)
                delete[] randchars.charptrs[n];
            randchars.deleted[n]=1;
	}
	//return needed for main.
    return 0;
}

	
