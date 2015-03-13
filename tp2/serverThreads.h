#ifndef SERVERTHREADS_H
#define SERVERTHREADS_H

//POSIX library for threads
#include <pthread.h>
#include <unistd.h>

class ServerThreads {
	
public:
	ServerThreads();
	~ServerThreads();
	
	void readConfigurationFile(const char *fileName);
	void createAndStart();
	void initializationOfDataStructures();
	static void processRequest(int threadID, int socketFD);
	void printAndSaveResults(const char* fileName);
	void signalFinishToClient();
	
private:
	//Internal server parameters
	int numServerThreads; 			// Number of different threads that the server will use to accept connections
	int serverBacklogSize; 			// Maximum number of pending connections for each thread
	int numResources;				// Number of diferent resources
	int portNumber;					// The port that the server will listen to
	int numClients;					// Number of different clients (threads) that will connect to the server
	int numRequestsPerClient;		// Number of requests to be received by each client
	bool initDataProvided;
	
	static int serverSocketFD; 		// Main server Socket File Descriptor
	static int maxWaitTime; 		// Maximum number of seconds that the server program will run
	static int requestProcesed;  	// Number of request already processed
	static int totalNumRequests; 	// Total number of request to be processed (numClients*numRequestPerClient)
	
	//Banker's Algorithm data structures (see the book for further details)
	static int *Available;
	static int **Max;
	static int **Allocation;
	static int **Need;
	
	// Results variables
	static int countAccepted;		// Result counter for total acepted requests
	static int countOnWait;			// Result counter for total request denied and put to wait
	static int countInvalid;		// Result counter for total invalid requests
	static int countClientsDispatched;// Result counter for total clients correctly finished
	
	//Server threads
	int	*realID;			 // Real ID of each thread (from 0 to numServerThreads-1)
	pthread_t *pt_tid;		 // The thread identifier
	pthread_attr_t *pt_attr; // Set of thread identifiers	
	
	//Function that will be called by every server thread
	static void *threadCode(void * param);
	
	//Function to write the Max values to a file
	void writeMaxToFile();
	
};

#endif // SERVERTHREADS_H
