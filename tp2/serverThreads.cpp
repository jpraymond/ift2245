#include "serverThreads.h"
#include "common.h"


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
  //printf("intit 0");
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
  //printf("intit 1");
  for (int i = 0 ; i < numResources ; i++)
    {
      Work[i] = 0;
    }
  // printf("intit 2");
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
    }
  if (ind != numResources)
    {
      cout <<  "unaccceptable request: wrong number of resources" << ind << endl;
      return -1;
    }
  if (pos == 0)
    return 1;
  else 
    return 2;
}


void ServerThreads::processRequest(int threadID, int socketFD)
{
  // timer from http://stackoverflow.com/questions/538609/high-resolution-timer-with-c-and-linux?lq=1
  // http://stackoverflow.com/questions/588307/c-obtaining-milliseconds-time-on-linux-clock-doesnt-seem-to-work-properl?rq=1

  //TODO: regler question du chrono

  /*
    struct timespec start, end;
    long newProcMilli, seconds, useconds;    
    clock_gettime(CLOCK_MONOTONIC, &start);
  */
  int maxChar = sizeof(signed int) * (numResources + 1) + numResources + 1;
  char Buffer[maxChar];
  bzero(Buffer, maxChar);
  int n = read(socketFD,Buffer,maxChar);
  if (n < 0) error("ERROR reading from socket");
		
  cout << "Thread " << threadID << "received the request:" << Buffer << endl;
	
  int RecRequest[numResources];
  int recClientThreadID;
  int answerToClient;
  int delay;

  //int ZeroGauge[numResources];
  //bzero(ZeroGauge, numResources);

  int response = parseCheckRequest(Buffer, recClientThreadID,  RecRequest);
   
  // requete bien formee
  if (0 < response)
    {
      if (ClientsReleased[recClientThreadID] || numRequestsPerClient == NumProcPerClient[recClientThreadID])
	{
	  //error("ERROR: client has sent a request after being fully released or beyond numRequestsPerClient");
	  cout << "client" <<  recClientThreadID << "has sent the request" << Buffer 
	       << "after asking resources to be fully released or beyond numRequestsPerClient: DISREGARDED" << endl;
	  answerToClient == NULL;
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
    }    // fin requete mal specifiee
		  
  n = sprintf(Buffer,"%d",answerToClient); 
  n = write(socketFD,Buffer,n);
  if (n < 0) error("ERROR writing to socket");
	
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

/// Change the sleep() function for a correct solution for
/// the ending synchronization problem. The server has to let
/// the client know that it has finished, so that the client
/// can finish itself. HINT: Look for named pipes
void ServerThreads::signalFinishToClient(){
	/// TP2_TO_DO
	
	sleep(2);
	
	/// TP2_END_TO_DO
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

mutex_t mtx;

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









