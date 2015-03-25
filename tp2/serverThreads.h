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

static int parseCheckRequest(char Buffer[], int &clientThreadId, int *&Request);
static bool isNotSafe(int id, int Request[]);
static bool returnsBeyondAllocation(int id, int Request[]);
static bool returnsExactAllocation(int id, int Request[]);
static bool asksBeyondMax(int id, int Request[]);
static bool isSmallerOrEqual(int Left[], int Right[], int dim);
static void addToAlloc(int id, int Request[]);
static void addToAvail(int Request[]);
static void subtractFromAlloc(int id, int Request[]);
static void subtractFromAvail(int Request[]);

//static bool isExcessive(int id, int numRes, int Request[]);
static bool isEqualToZero(int Tab[], int dim);
 
 
  static int lastProcMilli; // dernier temps de traitement

  //Internal server parameters
  int numServerThreads; 			// Number of different threads that the server will use to accept connections
  int serverBacklogSize; 			// Maximum number of pending connections for each thread
  static int numResources;				// Number of diferent resources
  int portNumber;					// The port that the server will listen to
  static int numClients;					// Number of different clients (threads) that will connect to the server
  static int numRequestsPerClient;		// Number of requests to be received by each client
  bool initDataProvided;
  static int availFactor; /* positive constant used in setting the maximum availability of each resource through: maximum availability = availFactor * numResources * numClients */
	
  static int serverSocketFD; 		// Main server Socket File Descriptor
  static int maxWaitTime; 		// Maximum number of seconds that the server program will run
  static int requestProcesed;  	// Number of request already processed
  static int totalNumRequests; 	// Total number of request to be processed (numClients*numRequestPerClient)
  		
  //Banker's Algorithm data structures (see the book for further details)
  static int *Available;
  static int **Max;
  static int **Allocation;
  static int **Need;
  static bool *Finish;
  static int *Work;
  static int **HypoAllocation;
  static int **HypoNeed;
  static int *NumProcPerClient;
  static bool *ClientsAccomodated;

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
	
  // http://stackoverflow.com/questions/7977255/custom-raii-c-implementation-for-scoped-mutex-locks
  // http://stackoverflow.com/questions/17729421/does-a-mutex-unlock-when-it-comes-out-of-scope
  // usage de Lockguard garantit le deblocage du verrou a la sortie d'un bloc, meme en cas d'exception
  /*
    usage des instances de la classe LockGuard:
    pthread_mutex_t mtx; // hors du bloc vise
    LockGuard lock(mtx); // a l'interieur et au debut du bloc vise
  */

  struct LockGuard
  {
  public:
  LockGuard(pthread_mutex_t  & mutex) : ref(mutex) 
    { 
       pthread_mutex_lock(& ref); 
    }
    ~LockGuard() 
    { 
      pthread_mutex_unlock(& ref); 
    }

   // LockGuard(LockGuard const &) = delete;
   // void operator=(LockGuard &) = delete;
 
 private:
     LockGuard(const LockGuard&); // or use c++0x ` = delete`

    pthread_mutex_t & ref;
  };



  static pthread_mutex_t lock_lastProcMilli;
  static pthread_mutex_t lock_serverSocketFD; 	
  static pthread_mutex_t lock_requestProcesed;  
  static pthread_mutex_t lock_Available;
  static pthread_mutex_t lock_Max;
  static pthread_mutex_t lock_Allocation;
  static pthread_mutex_t lock_Need;
  static pthread_mutex_t lock_Finish;
  static pthread_mutex_t lock_Work;
  static pthread_mutex_t lock_HypoAllocation;
  static pthread_mutex_t lock_HypoNeed;
  static pthread_mutex_t lock_NumProcPerClient;
  static pthread_mutex_t lock_ClientsAccomodated;
  static pthread_mutex_t lock_countAccepted;	
  static pthread_mutex_t lock_countOnWait;	
  static pthread_mutex_t lock_countInvalid;	
  static pthread_mutex_t lock_countClientsDispatched;


};




















#endif // SERVERTHREADS_H
