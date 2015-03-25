#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <string>

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
	static int sendRequest(int clientID, int requestID, int socketFD,
                               int *&requestQuantities);
	static void waitUntilServerFinishes();
	static void printAndSaveResults(const char* fileName);
	static int readConfigurationFile(const char *fileName);
	static void readMaxFromFile();
	
private:
	// Maximum number of instances of each resource type 
	// that each client MAY need
	static int **Max;
        static int **allocatedResources;  
	
	// Results variables
	static int countAccepted;			// Result counter for total acepted requests
	static int countOnWait;				// Result counter for total request denied and put to wait
	static int countInvalid;			// Result counter for total invalid requests
	static int countClientsDispatched;	        // Result counter for total clients correctly finished
        static int countClientsProcessed;               // Qui ont envoye toutes leurs requetes.

        static pthread_mutex_t mutexCountAccepted;
        static pthread_mutex_t mutexCountOnWait;
        static pthread_mutex_t mutexCountInvalid;
        static pthread_mutex_t mutexCountClientsDispatched;
        static pthread_mutex_t mutexCountClientsProcessed;
	
	static int count;					// Common counter of created ClienThread to asign an ID
	pthread_t pt_tid;					// The thread identifier
	pthread_attr_t pt_attr;				        // Set of thread identifiers
	
	//Function that will be called by every client thread
	static void *clientThreadCode(void * param);

        static void randomAllocations(int clientID, int allocations[]);
        static void randomReleases(int clientID, int releases[]);

        static bool allocationPossible(int clientID);
        static bool releasePossible(int clientID);

        static int sum(int integers[], int length);
};

#endif // CLIENTTHREAD_H
