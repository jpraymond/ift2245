#include "serverThreads.h"
#include "common.h"



// methodes auxiliaires

bool ServerThreads::isExcessive(int id, int numRes, int request[])
{
  bool value = false;
  for (int i = 0 ; i < numRes ; i++)
  {
      if (Allocation[id][i] - request[i] > Max[id][i])
  	{
	  value = true;
	  break;
	}
    }
  return value;
}

void ServerThreads::addTo(int First[], int Second[], int dim)
{
  for (int i = 0; i < dim; i++)
    {
      Second[i] += First[i];    
    }
 }

void ServerThreads::subtractFrom(int First[], int Second[], int dim)
{
  for (int i = 0; i < dim; i++)
    {
      Second[i] -= First[i];    
    }
 }

bool ServerThreads::isSmallerOrEqual(int Left[], int Right[], int dim)
{
  bool value = true;
  for (int i = 0; i < dim; i++)
    {
      if (Left[i] > Right[i])
	{
	  value = false;
	  break;
	} 
    }
  return value;
}

bool ServerThreads::isEqualToZero(int Tab[], int dim)
{
  bool value = true;
  for (int i = 0; i < dim; i++)
    {
      if (Tab[i] != 0)
	{
	  value = false;
	  break;
	} 
    }
  return value;
}


void ServerThreads::initializationOfDataStructures()
{
//TODO configurer correctement metaMax
	int metaMax = 100;

  for (int i = 0 ; i < numClients ; i++)
    {
      Finish[i] = NULL;
      NumProcPerClient[i] = 0;
      ClientsReleased[i] = false;
      for (int j = 0 ; j < numResources ; j++)
	{
	  Allocation[i][j] = 0;
	  Need[i][j] = 0;
	  HypoAllocation[i][j] = 0;
	  HypoNeed[i][j] = 0;
	}
    }  

  for (int i = 0 ; i < numResources ; i++)
    {
      Work[i] = 0;
    }

  if (!initDataProvided)
    {
      for (int j = 0 ; j < numResources ; j++)
	{
	  
	  Available[j] = 1 + rand()%metaMax;
	  for (int i = 0 ; i < numClients ; i++)
	    Max[i][j] =   1+ rand()%Available[j]; 
	}
          // The initilized values will be written to a file for the
      // client to read them afterwards
      writeMaxToFile();
    }
}

<<<<<<< HEAD
int ServerThreads::parseCheckRequest(char Buffer[], int &clientThreadID, int *&Request) {
  istringstream iss(Buffer);  
  int token;

  // On recupere clientThreadID.
  int localClientThreadID;
  if (!(iss >> token)) {
    return 0;
  }
  if (token < 0 || token >= numClients) {
    return 0;
  }
  localClientThreadID = token;

  /** =====================================================================
  On recupere le contenu de Request.
  ---------------------------------------------------------------------- */
  int* localRequest = new int[numResources];
  int isPositive = 0; // Indetermine (-1 ==> faux, 1 => vrai).
  bool isOK = true;
  
  int i = 0;
  while (iss >> token) {    
    // On s'assure que les quantites sont soit toutes positives soit toutes
    // negatives.
    if (isPositive != 0) {
      if (isPositive > 0) {
        if (token < 0) {
          isOK = false;
        }
      }
      else { // isPositive < 0
        if (token > 0) {
          isOK = false;
        }
      }
=======
int ServerThreads::parseCheckRequest(char Buffer[], int clientThreadID, int Request[])
{
	istringstream iss(Buffer);
	char * token;
	char * error;

 if (!(iss >> token))
   	 {
      cout <<  "unaccceptable request: EMPTY" << endl;
      return 0;
	}

  clientThreadID = strtol(token, &error, 10);
  if (*error || clientThreadID < 0)
	{
	cout <<  "unaccceptable request: unreadable or negative client thread ID" << error << endl;
	return 0;
	}
  
 
  int ind = 0;
  int pos = 0;
	
  while(iss >> token)
    {   
	 Request[ind] = strtol(token, &error, 10);
  	if (*error)
	{
	cout <<  "unaccceptable request: unreadable resource amount demanded" << error << endl;
	return -1;
	}
  
      if (Request[ind] >= 0)
	pos++;
      ind++;
    }
  if (ind > pos && pos != 0)
    {
      cout <<  "unaccceptable request: mixed signs" << endl;
      return -1;
>>>>>>> 51b703e8740f4b230391c516e256f0a020f01028
    }
    else { // isPositive n'est toujours pas determine.
      if (token > 0) {
        isPositive = 1;
      }
      else if (token < 0) {
        isPositive = -1;
      }
    }
<<<<<<< HEAD
    localRequest[i] = token;
    i++;
  }
  
  if (i != numResources) {
    isOK = false;
  }

  if (!isOK) {
    delete localRequest;
    return -1;
  }
  /** ================================================================== */

  // Requete conforme. On prepare clientThreadID et Request.
  clientThreadID = localClientThreadID;
  Request = localRequest;

  return (isPositive > 0) + 1;
=======
	if (pos == 0)
 	 return 1;
	else 
	return 2;
>>>>>>> 51b703e8740f4b230391c516e256f0a020f01028
}


void ServerThreads::processRequest(int threadID, int socketFD)
{
  // timer from http://stackoverflow.com/questions/538609/high-resolution-timer-with-c-and-linux?lq=1
  // http://stackoverflow.com/questions/588307/c-obtaining-milliseconds-time-on-linux-clock-doesnt-seem-to-work-properl?rq=1

//TODO: regler question du chrono

<<<<<<< HEAD
  /*
    struct timespec start, end;
    long newProcMilli, seconds, useconds;    
    clock_gettime(CLOCK_MONOTONIC, &start);
  */
  
  // TODO: Pas certain de comprendre la relation entre 'sizeof(signed int)' et le
  //       nombre de caracteres de la requete.
  // int maxChar = sizeof(signed int) * (numResources + 1) + numResources + 1;
  
  // Utilisons plutot 6 digits par clientID/quantite (-99999 a 999999) + les espaces.
  int maxChar = 6 * (numResources + 1) + numResources;
=======
/*
  struct timespec start, end;
  long newProcMilli, seconds, useconds;    
  clock_gettime(CLOCK_MONOTONIC, &start);
*/
  int maxChar = sizeof(signed int) * (numResources + 1) + numResources + 1;
>>>>>>> 51b703e8740f4b230391c516e256f0a020f01028
  char Buffer[maxChar];
  bzero(Buffer, maxChar);
  int n = read(socketFD,Buffer,maxChar-1);
  if (n < 0) error("ERROR reading from socket");
		
  cout << "Thread " << threadID << "received the request:" << Buffer << endl;
	
  int *RecRequest;
  int recClientThreadID;
  int answerToClient;
  int delay;

<<<<<<< HEAD
  //int ZeroGauge[numResources];
  //bzero(ZeroGauge, numResources);

  int response = parseCheckRequest(Buffer, recClientThreadID,  RecRequest);

=======
int response = parseCheckRequest(Buffer, recClientThreadID,  RecRequest);
{
 
 
LockGuard lock_lPC(lock_lastProcMilli);
 //LockGuard lock3(lock_serverSocketFD); 	
  LockGuard lock_rP(lock_requestProcesed);  
  LockGuard lock_Av(lock_Available);
  LockGuard lock_M(lock_Max);
  LockGuard lock_Al(lock_Allocation);
  LockGuard lock_N(lock_Need);
  LockGuard lock_F(lock_Finish);
  LockGuard lock_W(lock_Work);
  LockGuard lock_HA(lock_HypoAllocation);
  LockGuard lock_HN(lock_HypoNeed);
  LockGuard lock_NPPC(lock_NumProcPerClient);
  LockGuard lock_CR(lock_ClientsReleased);
  LockGuard lock_cA(lock_countAccepted);	
  LockGuard lock_cOW(lock_countOnWait);	
  LockGuard lockcI(lock_countInvalid);	
  LockGuard lock_cCD(lock_countClientsDispatched);
 
   
 
>>>>>>> 51b703e8740f4b230391c516e256f0a020f01028
  // requete bien formee
  if (0 < response)
    {



      if (ClientsReleased[recClientThreadID] || numRequestsPerClient == NumProcPerClient[recClientThreadID])
	{
	  //error("ERROR: client has sent a request after being fully released or beyond numRequestsPerClient");
<<<<<<< HEAD
	  cout << "client" <<  recClientThreadID << "has sent the request" << Buffer 
	       << "after asking resources to be fully released or beyond numRequestsPerClient: DISREGARDED" << endl;
	  answerToClient = -1; // TODO: Anciennement 'answerToClient == NULL;'.
=======
	cout << "client" <<  recClientThreadID << "has sent the request" << Buffer 
	<< "after asking resources to be fully released or beyond numRequestsPerClient: DISREGARDED" << endl;

	answerToClient == NULL;

>>>>>>> 51b703e8740f4b230391c516e256f0a020f01028
	}
      // cas liberation de ressources
      else if (response == 2)
	{	

	// TODO: check second parameter in isSmaller
	  if(isSmallerOrEqual(RecRequest, Allocation[recClientThreadID], numRequestsPerClient))
	    {
	      answerToClient = 0;
	      countOnWait --;
	      countAccepted++;

	      addTo(RecRequest, Available, numResources);
	      subtractFrom(RecRequest, Allocation[recClientThreadID], numResources);

	      cout << "request to RELEASE resources from client thread" << recClientThreadID << "ACCEPTED" << Buffer << endl;
	    }
	  else
	    {
	      answerToClient = -1;
	      countOnWait --;
	      countInvalid++;
	      cout << "request to RELEASE resources from client thread" << recClientThreadID << "REJECTED: EXCESSIVE" << Buffer << endl;
	    }
	}
      // cas demande de ressources
      else if (response == 1)
	{
	  // ressources demandees excessives
	  if (isExcessive(recClientThreadID, numResources, RecRequest))
	    {
	      answerToClient = -1;
	      countOnWait --;
	      countInvalid++;
	      cout << "request to OBTAIN resources from client thread" << recClientThreadID << "REJECTED: EXCESSIVE" << Buffer << endl;
	    }
	  // ressources demandees non excessives
	  else
	    {
	      // unsafe demand
	      if (isNotSafe(recClientThreadID, RecRequest))
		{
		  delay = lastProcMilli * 10;
		  answerToClient = delay;
		  cout << "request to OBTAIN resources from client thread" << recClientThreadID << "DELAYED: UNSAFE" << Buffer << endl;
		}
	      // safe demand
	      else
		{
		  answerToClient = 0;
		  countOnWait --;
		  countAccepted++;
		  subtractFrom(RecRequest, Available, numResources);
		  addTo(RecRequest, Allocation[recClientThreadID], numResources);
		  cout << "request to OBTAIN resources from client thread" << recClientThreadID << "ACCEPTED" << Buffer << endl;
		}
	    } // fin ressources demandees non excessives
	} // fin demande de ressources
    } // fin requete bien specifiee
  // requete mal specifiee dont l'auteur est lisible
  else if (response == -1)
    {
      answerToClient = -1;
      countOnWait --;
      countInvalid++;
      cout << "request to OBTAIN resources from client thread" << recClientThreadID << "REJECTED: UNREADABLE" << Buffer << endl;
    }

// requete mal specifiee dont auteur est illisible
  else // (response == 0)
    {
      answerToClient = -1;
      countOnWait --;
      countInvalid++;
      cout << "request to OBTAIN resources from unkwnown client thread DISREGARDED" << endl;
    }

 // fin requete mal specifiee
		  
  n = sprintf(Buffer,"%d",answerToClient); 
  n = write(socketFD,Buffer,n);
  if (n < 0) error("ERROR writing to socket");
	
  // This part lets the server know how many request it has
  // processed so far, so that it can finish when all the 
  // excpected requests have been proccesed.
  if (response != 0 && read(socketFD,Buffer,255) == 0)
    { //Nothing else to read
      requestProcesed++;
      if (numRequestsPerClient ==  ++NumProcPerClient[recClientThreadID])
	{
	  countClientsDispatched++;

	for (int i = 0; i < numResources; i++)
	{
	Allocation[recClientThreadID][i] = 0;
	}
		
	  ClientsReleased[recClientThreadID] = true;

	}

      else if (isEqualToZero(Allocation[recClientThreadID], numResources))
	{
	  countClientsDispatched++;
	  ClientsReleased[recClientThreadID] = true;
     	}
    }
}
// TODO: regler le cas du chronometrage

  // chronometrage
/*
  clock_gettime(CLOCK_MONOTONIC, &end);
  seconds  = end.tv_sec  - start.tv_sec;
  useconds = end.tv_nsec - start.tv_nsec;
  newProcMilli = ((seconds) * 1000 + useconds/1000.0) + 0.5;
  printf("Elapsed time: %ld milliseconds\n", newProcMilli);
  // lissage exponentiel
  double alpha = 0.5;
  lastProcMilli = alpha * newProcMilli + (1 - alpha) * lastProcMilli;
*/
 lastProcMilli = 500;

}


bool ServerThreads::isNotSafe(int id, int request [])
{
  bool value = true;
  for (int i = 0 ; i < numClients ; i++)
    {
      Finish[i] = false;
      for (int j = 0 ; j < numResources ; j++)
	{
	  HypoAllocation[i][j] = Allocation[i][j];
	  HypoNeed[i][j] = Need[i][j];
	}
    }
  for (int i = 0 ; i < numResources ; i++)
    {
      Work[i] = Available[i];
      HypoAllocation[id][i] = Allocation[id][i] + request[i];
      HypoNeed[id][i] = Need[id][i] - request[i];
    }
  int ind = 0, solved = 0, failed = 0;
  while(solved < numClients && failed < numClients)
    {
      if (Finish[ind])
	{
	  ind = ++ind % numClients;
	  failed++;
	  continue;
	}

	int RowHypoNeed[numResources];
	for (int m = 0 ; m < numResources; m++)
	{
	RowHypoNeed[m] = HypoNeed[ind][m];
	}

      if(isSmallerOrEqual(RowHypoNeed, Work, numResources))
	{
	  Finish[ind] = true;
	  solved++;
	  failed = 0;
	  for (int k = 0; k < numResources;  k++)
	    {
	      Work[k] += HypoAllocation[ind][k];
	    }
	}
      else
	{
	  failed++;
	}
      ind = ++ind % numClients;
	}
  if (solved == numClients)
    {
      value = false;
    }
  return value;
}

// see http://tuxthink.blogspot.ca/2012/02/inter-process-communication-using-named.html	
// http://stackoverflow.com/questions/10847237/how-to-convert-from-int-to-char
void ServerThreads::signalFinishToClient(){
	
string name_str = i_to_str(portNumber);

const char* name_char = name_str.c_str();

	sleep(2);
	int createdPipe;
	createdPipe = mkfifo(name_char, 0666); 
	if (createdPipe < 0)
		error("ERROR creating a fifo server pipe");

	cout << "fifo pipe" << portNumber << "created successfully" << endl;

	bool finish = true;
	int serverPipe;
	serverPipe = open(name_char, O_RDWR);
	if (serverPipe < 1)
		error("ERROR opening fifo server pipe");

 	write(serverPipe, &finish, sizeof(bool));

 	cout << "server FINISHED message sent to client through pipe" << portNumber << endl;

	close(serverPipe);
}




/// Do not modify this function
/// Rather use it as an example of socket functonality
/// to do the conections on the clients
void* ServerThreads::threadCode(void * param){
	int ID = *((int*)param);
	
	struct sockaddr_in thread_addr;
	socklen_t threadSL = sizeof(thread_addr); //Thread socket lenght
	int threadSocketFD = -1;
	int start = time(NULL);
	
	// Loop until accept() returns the first valid conection
	while (threadSocketFD < 0 ){
		threadSocketFD = accept(serverSocketFD, 
								(struct sockaddr *) &thread_addr, 
								&threadSL);
								
		if ((time(NULL) - start) >= maxWaitTime || 
			requestProcesed == totalNumRequests)
			break;
	}
	
	// Now loop until the server has completely dispatched all clients
	while ( requestProcesed < totalNumRequests ){
		
		if ((time(NULL) - start) >= maxWaitTime){
			cerr << "Time out on thread " << ID << endl;
			pthread_exit(NULL);
		}
		
		if (threadSocketFD > 0){
			// Process request from connection
			processRequest(ID,threadSocketFD);
			close(threadSocketFD);
		}
				
		//Take next conextion available
		threadSocketFD = accept(serverSocketFD, 
								(struct sockaddr *) &thread_addr, 
								&threadSL); 
	}
}

/// Do not modify this function
/// Rather use it as an example to uderstand socket functionality
void ServerThreads::createAndStart(){

<<<<<<< HEAD
// mutex_t mtx; TODO: !?!
=======
>>>>>>> 51b703e8740f4b230391c516e256f0a020f01028

	//Create the main server socket and define a port to listen to
	serverSocketFD = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (serverSocketFD < 0) 
		error("ERROR opening socket");
		
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portNumber);
	if (bind(serverSocketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
		
	listen(serverSocketFD,serverBacklogSize);
		
	//Create the thread variables
	realID = new int[numServerThreads];
	pt_tid = new pthread_t[numServerThreads];
	pt_attr = new pthread_attr_t[numServerThreads];
	
	cout << "Now waiting for clients..." << endl;
	
	//Finally create each child thread and run the function threadCode
	for (int i = 0 ; i < numServerThreads ; i++) {
		realID[i] = i; //Assign the real ID (from 0 to numServerThreads-1)
		pthread_attr_init(&pt_attr[i]);
		pthread_create(&pt_tid[i],&pt_attr[i],&threadCode,&realID[i]);
	}
	
	for (int i = 0 ; i < numServerThreads ; i++)
		pthread_join(pt_tid[i],NULL);
}

/// You can modify this function to print other values at
/// the end of the client application excecution. But do not
/// modify the results file output, as it is neccesary for evaluation
void ServerThreads::printAndSaveResults(const char* fileName){
	cout << endl << "------Server Results-------" << endl;
	cout << "Requests accepted:\t\t" << countAccepted << endl;
	cout << "Requests sent to wait:\t\t" << countOnWait << endl;
	cout << "Invalid requests:\t\t" << countInvalid << endl;
	cout << "Clients dispatched:\t\t" << countClientsDispatched << endl;
	cout << "Total requests proccesed:\t" << requestProcesed << endl;
	
	///DO NOT MODIFY THIS PART
	// Save the counted values for evaluation
	if (fileExists(fileName)) remove(fileName);
	ofstream fs(fileName);
	fs << countAccepted << " " << countOnWait << " " << countInvalid << " " << countClientsDispatched << endl;
	fs.close();
}

/// You can modify this function if you want to add other fields
/// to the configuration field, but normally you will not need to
void ServerThreads::readConfigurationFile(const char *fileName){	
	if (!fileExists(fileName)) {
		cout << "No configuration file " << fileName << " found" << endl;
		exit(1);
	}
	
	libconfig::Config cfg;
	cfg.readFile(fileName);
	
	cfg.lookupValue("portNumber",portNumber);
	cfg.lookupValue("maxWaitTime",maxWaitTime);
	cfg.lookupValue("serverThreads",numServerThreads);
	cfg.lookupValue("serverBacklogSize",serverBacklogSize);
	cfg.lookupValue("numClients",numClients);
	cfg.lookupValue("numResources",numResources);
	cfg.lookupValue("numRequestsPerClient",numRequestsPerClient);
	totalNumRequests = numClients*numRequestsPerClient;
	
	

 //Dynamic allocation of memory (will be freed at destructor)
  Available = new int[numResources];
  Max = new int*[numClients];
  Allocation = new int*[numClients];
  Need = new int*[numClients];

  Finish = new bool[numClients];
  Work = new int[numResources];
  HypoAllocation = new int*[numClients];
  HypoNeed = new int*[numClients];
  NumProcPerClient = new int[numClients];
  ClientsReleased = new bool[numClients];

  for (int i = 0 ; i < numClients ; i++){
    Max[i] = new int[numResources];
    Allocation[i] = new int[numResources];
    Need[i] = new int[numResources];
    HypoAllocation[i] = new int[numResources];
    HypoNeed[i] = new int[numResources];
  }

	
	cfg.lookupValue("initialValuesProvided",initDataProvided);
	
	if (initDataProvided) {
		//Initialize the Available and Max structures
		libconfig::Setting& available = cfg.lookup("availableResources");
		libconfig::Setting& maximum = cfg.lookup("maximumPerClient");
		
		for (int i = 0 ; i < numResources ; i++)
			Available[i] = available[i];
		
		for (int i = 0 ; i < numClients ; i++){
			for (int j = 0 ; j < numResources ; j++){
				Max[i][j] = maximum[i][j];
				//Verification
				if (Max[i][j] > Available[j]){
					cerr << "Invalid maximumPerClient values" << endl;
					exit(1);
				}
			}
		}
	}
	
	cout << "Server started with configuration: " << endl << endl;
	cout << "Maximum wait time: " << maxWaitTime << endl;
	cout << "Number of server threads: " << numServerThreads << endl;
	cout << "Server backlog size: " << serverBacklogSize << endl;
	cout << "Number of clients: " << numClients << endl;
	cout << "Number of resources: " << numResources << endl ;
	if (initDataProvided) {
		cout << "Available resources at start:" << endl;
		for (int i = 0 ; i < numResources ; i++)
			cout << Available[i] << " ";
		cout << endl << endl;
	}
	else
		cout << endl << "No initial values provided, needs aditional initialization" << endl;
}

/// The rest of the code is neccesary for the correct functionality
/// You can add extra stuff, but try to keep the provided code as it is

void ServerThreads::writeMaxToFile(){
	
	if (!fileExists("temp")) mkdir("temp",0755);
	if (fileExists("temp/Max")) remove("temp/Max");
	ofstream fs("temp/Max");
	
	for (int i = 0 ; i < numClients ; i++){
		for (int j = 0 ; j < numResources ; j++)
			fs << Max[i][j]  << " ";
		fs << endl;
	}
	fs.close();
}

ServerThreads::ServerThreads(){
	// General initialization
	realID = NULL;
	pt_tid = NULL;
	pt_attr = NULL;
}

ServerThreads::~ServerThreads(){
	if (realID != NULL)
		delete []realID;
	if (pt_tid != NULL)
		delete []pt_tid;
	if (pt_attr != NULL)
		delete []pt_attr;
		
	if (Available != NULL)
		delete []Available;
	if (Max != NULL){
	  for (int i = 0 ; i < numClients  ; i++){ // modif
			delete []Max[i];
			delete []Allocation[i];
			delete []Need[i];
		}
		delete []Max;
		delete []Allocation;
		delete []Need;
	}		
	if (Work != NULL)
		delete []Work;
	if (Finish != NULL)
		delete []Finish;
	if (HypoNeed != NULL){
		for (int i = 0 ; i < numResources  ; i++){
		              delete []HypoAllocation[i];
			delete []HypoNeed[i];
		}
		delete []HypoAllocation;
		delete []HypoNeed;
	}

	if (serverSocketFD == -1)
		close(serverSocketFD);
<<<<<<< HEAD
=======



pthread_mutex_destroy(&lock_lastProcMilli);
 pthread_mutex_destroy(&lock_serverSocketFD); 	
  pthread_mutex_destroy(&lock_requestProcesed);  
  pthread_mutex_destroy(&lock_Available);
  pthread_mutex_destroy(&lock_Max);
  pthread_mutex_destroy(&lock_Allocation);
  pthread_mutex_destroy(&lock_Need);
  pthread_mutex_destroy(&lock_Finish);
  pthread_mutex_destroy(&lock_Work);
  pthread_mutex_destroy(&lock_HypoAllocation);
  pthread_mutex_destroy(&lock_HypoNeed);
  pthread_mutex_destroy(&lock_NumProcPerClient);
  pthread_mutex_destroy(&lock_ClientsReleased);
  pthread_mutex_destroy(&lock_countAccepted);	
  pthread_mutex_destroy(&lock_countOnWait);	
  pthread_mutex_destroy(&lock_countInvalid);	
  pthread_mutex_destroy(&lock_countClientsDispatched);




>>>>>>> 51b703e8740f4b230391c516e256f0a020f01028
}


//Initialization of static variables
int ServerThreads::requestProcesed = 0;
int ServerThreads::totalNumRequests = 0;
int ServerThreads::maxWaitTime = 0;
int ServerThreads::serverSocketFD = -1; 
int ServerThreads::numResources = 0;
int ServerThreads::numClients = 0;
int ServerThreads::numRequestsPerClient = 0;

// Initialization of result variables 
int ServerThreads::countAccepted = 0;
int ServerThreads::countOnWait = 0;
int ServerThreads::countInvalid = 0;
int ServerThreads::countClientsDispatched = 0;

int* ServerThreads::Available = NULL;
int** ServerThreads::Max = NULL;
int** ServerThreads::Allocation = NULL;
int** ServerThreads::Need = NULL;

int ServerThreads::lastProcMilli = 500;
bool* ServerThreads::Finish = NULL;
int* ServerThreads::Work = NULL;
int** ServerThreads::HypoAllocation = NULL;
int** ServerThreads::HypoNeed = NULL;
int* ServerThreads::NumProcPerClient = NULL;
bool* ServerThreads::ClientsReleased = NULL;
<<<<<<< HEAD
=======

 pthread_mutex_t ServerThreads::lock_lastProcMilli= PTHREAD_MUTEX_INITIALIZER;
 pthread_mutex_t ServerThreads::lock_serverSocketFD= PTHREAD_MUTEX_INITIALIZER; 	
  pthread_mutex_t ServerThreads::lock_requestProcesed= PTHREAD_MUTEX_INITIALIZER;  
  pthread_mutex_t ServerThreads::lock_Available= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_Max= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_Allocation= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_Need= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_Finish= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_Work= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_HypoAllocation= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_HypoNeed= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_NumProcPerClient= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_ClientsReleased= PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t ServerThreads::lock_countAccepted= PTHREAD_MUTEX_INITIALIZER;	
  pthread_mutex_t ServerThreads::lock_countOnWait= PTHREAD_MUTEX_INITIALIZER;	
  pthread_mutex_t ServerThreads::lock_countInvalid= PTHREAD_MUTEX_INITIALIZER;	
  pthread_mutex_t ServerThreads::lock_countClientsDispatched= PTHREAD_MUTEX_INITIALIZER;




>>>>>>> 51b703e8740f4b230391c516e256f0a020f01028
