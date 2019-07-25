# CECS326_Message_Queue
Timothy Tran(tht020@ucsd.edu),Wiiliam Jorgensen (William.Jorgensen@student.csulb.edu)
3/12/19
Course Number: CECS326

Software Description for Message Queue Program:

The delivered software will consist of 4 programs and require at minimum 4 terminal windows to operate.
 The first program is the Hub which will receive and read out messages in the message queue. 
 It will also operate until the other 3 programs have stopped operation.
 It will count how many messages it has received and kill the ProbeB program once it hits 10,000. 
 The other 3 programs share the following properties: they each have an individual random number generator and a magic "seed", they each will send messages, they each have their own conditions to being closed. 
 ProbeA has a magic seed of 997, where whenever the random number generator is divisible by 997 it will send a message.
 ProbeA is the only program who waits for a response from the Hub before sending another message. 
 ProbeA closes when the random number generator generates a value below 100. ProbeB has a magic seed of 257. 
 ProbeB will send messages until the Hub has received 10,000m messages and then the Hub will close it. 
 ProbeC has a magic seed of 251. 
 ProbeC will send messages until another terminal remotely closes the C process using the kill -10 command. 
 Upon all 3 probes being killed, the Hub will delete the message queue and close. 

Critique:

The programs are non-portable. 
Must be re-compiled between 32 and 64 bit operating systems.
 Will not cleanly execute if the target message queue is not empty at the start of execution. 
 This can be from improperly closing the Hub process or any of the Probe processes.
 Testing can vary and be inconsistent due to the random nature of the Probe messages. 
 ProbeA can have instances of not sending any messages other than the kill message due to its closing condition being easy to fulfill before its send message condition. 
 ProbeB may not send any messages either if the other probes are allowed to run to 10,000 messages before ProbeB can send its first. 
 (This condition may be met if ProbeB is the last to be executed as ProbeC can execute 10,000 messages quickly.) 
 Also it is difficult to see a variance in program execution as ProbeC's kill condition is difficult to quickly execute without too many ProbeC messages filling up the Hub's terminal window.


