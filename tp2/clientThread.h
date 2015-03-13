#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

//POSIX library for threads
#include <pthread.h>
#include <unistd.h>

class ClientThread {
	
public:
	int ID;
	static int portNumber;
	static int numClients;
	static int numResources;
	static int numRequests;
	
	ClientThread();
	~ClientThread();
	void createAndStartThread();
	static void additionalInitialization();
	static void sendRequest( int clientID, int requestID, int socketFD);
	static void waitUntilServerFinishes();
	static void printAndSaveResults(const char* fileName);
	static int readConfigurationFile(const char *fileName);
	static void readMaxFromFile();
	
private:
	// Maximum number of instances of each resource type 
	// that each client MAY need
	static int **Max; 			
	
	// Results variables
	static int countAccepted;			// Result counter for total acepted requests
	static int countOnWait;				// Result counter for total request denied and put to wait
	static int countInvalid;			// Result counter for total invalid requests
	static int countClientsDispatched;	// Result counter for total clients correctly finished
	
	static int count;					// Common counter of created ClienThread to asign an ID
	pthread_t pt_tid;					// The thread identifier
	pthread_attr_t pt_attr;				// Set of thread identifiers
	
	//Function that will be called by every client thread
	static void *clientThreadCode(void * param);
};

#endif // CLIENTTHREAD_H
