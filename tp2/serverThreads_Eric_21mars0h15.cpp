#include "serverThreads.h"
#include "common.h"

///In this function initialize the Banker's Algorithm
///data structures as you see convinient

// modif:
void ServerThreads::initializationOfDataStructures()
{
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
	  //TODO configurer metaMax
	  Available[j] = 1 + rand()%metaMax;
	  for (int i = 0 ; i < numClients ; i++)
	    Max[i][j] =   1+ rand()%Available[j]; 
	}
          // The initilized values will be written to a file for the
      // client to read them afterwards
      writeMaxToFile();
    }
}


int Serverthreads:: parseCheckRequest(char & Buffer[], int& clientThreadId,  int& Request[])
{ 
  istringstream iss(Buffer);
  string token;
  if (!(token << iss) || (clientThreadId = strtol(token, NULL, 0 ) == NULL) || (clientThreadId < 0))
    {
      cout <<  "unaccceptable request:  misspecified client thread ID" << endl;
      return -1;
    }
  int ind = 1;
  int pos = 0;
  while(iss >> token)
    {       
      if ( Request[ind] = strtol(token, NULL, 0 ) == NULL)
	{
	  cout <<  "unaccceptable request: unreadable" << endl;
 	  return -2;
	}
      if (Request[ind] >= 0)
	pos++;
      ind++;
    }
  if (ind - 1 > pos)
    {
      cout <<  "unaccceptable request: mixed signs" << endl;
      return -3;
    }
  if (ind - 1 != numResources)
    {
      cout <<  "unaccceptable request: wrong number of resources" << endl;
      return -4;
    }
  return 1;
}


void ServerThreads::processRequest(int threadID, int socketFD)
{
  // timer from http://stackoverflow.com/questions/588307/c-obtaining-milliseconds-time-on-linux-clock-doesnt-seem-to-work-properl/588377#588377
  struct timspec start, end;
  long newProcMilli, seconds, useconds;    
  clock_gettime(CLOCK_MONOTONIC, &start);

  int maxChar = sizeof(signed int) * (numResources + 1) + numResources + 1;
  char Buffer[maxChar];
  bzero(Buffer, maxChar);
  int n = read(socketFD,Buffer,maxChar);
  if (n < 0) error("ERROR reading from socket");
		
  cout << "Thread " << threadID << " received the request: " << Buffer << endl;
	
  int RecRequest[numResources];
  int recClientThreadID;
  int answerToClient;
  int delay;

  int ZeroGauge[numResources];
  bzero(ZeroGauge, numResources);

  // requete bien formee
  if ( 0 < parseCheckRequest(Buffer, recClientThreadId,  RecRequest))
    {
      if (ClientsReleased[recClientThreadID] || (numRequestPerClient == NumProcPerClient[recClientThreadID]))
	{
	  error("ERROR client has sent a request after being fully released or beyond numRequestPerClient");
	}
      // cas liberation de ressources
      else  if (isSmallerOrEqual(ZeroGauge, RecRequest))
	{	
	  if(isSmallerOrEqual(RecRequest, allocation[recClientThreadId]))
	    {
	      answerToClient = 0;
	      countOnwait --;
	      countAccepted++;

	      addTo(RecRequest, Available, numResources);
	      subtractFrom(RecRequest, Allocation[recClientThreadId], numResources);

	      cout << "request to RELEASE resources from client thread  " recClientThreadID " ACCEPTED" << Buffer << endl;
	    }
	  else
	    {
	      answerToClient = -1;
	      countOnwait --;
	      countInvalid++;
	      cout << "request to RELEASE resources from client thread  " recClientThreadID " REJECTED: EXCESSIVE" << Buffer << endl;
	    }
	}
      // cas demande de ressources
      else
	{
	  // ressources demandees excessives
	  if (isExcessive(clientThreadId, RecRequest))
	    {
	      answerToClient = -1;
	      countOnwait --;
	      countInvalid++;
	      cout << "request to OBTAIN resources from client thread  " recClientThreadID " REJECTED: EXCESSIVE" << Buffer << endl;
	    }
	  // ressources demandees non excessives
	  else
	    {
	      // unsafe demand
	      if (isNotSafe(recClientThreadID, RecRequest))
		{
		  delay = lastProcMilli * 10;
		  answerToClient = delay;
		  cout << "request to OBTAIN resources from client thread  " recClientThreadID " DELAYED: UNSAFE" << Buffer << endl;
		}
	      // safe demand
	      else
		{
		  answerToClient = 0;
		  countOnwait --;
		  countAccepted++;
		  subtractFrom(RecRequest, Available, numResources);
		  addTo(RecRequest, Allocation[recClientThreadId], numResources);
		  cout << "request to OBTAIN resources from client thread  " recClientThreadID " ACCEPTED" << Buffer << endl;
		}
	    } // fin ressources demandees non excessives
	} // fin demande de ressources
    } // fin requete bien specifiee
  // requete mal specifiee
  else
    {
      answerToClient = -1;
      countOnwait --;
      countInvalid++;
      cout << "request to OBTAIN resources from client thread  " recClientThreadID " REJECTED: UNREADABLE " << Buffer << endl;
    } // fin requete mal specifiee
		  
  n = sprintf(Buffer,"%d",answerToClient); 
  n = write(socketFD,Buffer,n);
  if (n < 0) error("ERROR writing to socket");
	
  // This part lets the server know how many request it has
  // processed so far, so that it can finish when all the 
  // excpected requests have been proccesed.
  if (read(socketFD,Buffer,255) == 0)
    { //Nothing else to read
      requestProcesed++;
      if (numRequestPerClient ==  ++NumProcPerClient[recClientThreadID])
	{
	  countClientsDispatched++;

		  //TODO: liberer ressources
	  clientsReleased[recClientThreadID] = true;

	}

      else if (isEqualToZero(Allocation[recClientThreadID], numberResources))
	{
	  countClientsDispatched++;
	  clientsReleased[recClientThreadID] = true;
     	}
    }
  // chronometrage
  clock_gettime(CLOCK_MONOTONIC, &end);
  seconds  = end.tv_sec  - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  newProcMilli = ((seconds) * 1000 + useconds/1000.0) + 0.5;
  printf("Elapsed time: %ld milliseconds\n", newProcMilli);
  // lissage exponentiel
  double alpha = 0.5;
  lastProcMilli = alpha * newProcMilli + (1 - alpha) * lastProcMilli;
}


boolean ServerThreads::isExcessive(int & id, int & request [])
{
  boolean value = false;
  for (int i = 0 ; i < numResources ; i++)
    {
      if (Allocation[id][i] - request[i] > Max[id][i])
	{
	  value = true;
	  break;
	}
    }
  return value;
}


boolean ServerThreads::isNotSafe(int & id, int & request [])
{
  boolean value = true;
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
      if(isSmaller(HypoNeed[ind], Work))
	{
	  Finish[ind] = true;
	  solved++;
	  fail = 0;
	  for (int k = 0; k < numResources;  k++)
	    {
	      work[k] += HypoAllocation[ind][k];
	    }
	}
      else
	{
	  failed++;
	}
      ind = ++ind % numClients
	}
  if (solved == numClients)
    {
      value = false;
    }
  return value;
}


void ServerThreads::addTo(int & First [], int & Second [], int dim)
{
  for (i = 0; i < dim; i++)
    {
      Second[i] += First[i];    
    }
 }


void ServerThreads::subtractFrom(int & First [], int & Second [], int dim)
{
  for (i = 0; i < dim; i++)
    {
      Second[i] -= First[i];    
    }
 }



boolean ServerThreads::isEqualToZero(int & Tab [], int dim)
{
  value = true;
  for (i = 0; i < dim; i++)
    {
      if (Tab[i] != 0)
	{
	  value = false;
	  break;
	} 
    }
  return value;
}


boolean ServerThreads::isSmallerOrEqual(int & Left [], int & Right [], int dim)
{
  value = true;
  for (i = 0; i < dim; i++)
    {
      if (Left[i] > Right[i])
	{
	  value = false;
	  break;
	} 
    }
  return value;
}


/// Change the sleep() function for a correct solution for
/// the ending synchronization problem. The server has to let
/// the client know that it has finished, so that the client
/// can finish itself. HINT: Look for named pipes
void ServerThreads::signalFinishToClient()
{

	
	sleep(2);

	// TODO: 



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

  // modif
  countOnWait = totalNumRequests;

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

  Finish = new int[numClients];
  Work = new int[numResources];
  HypoAllocation = new int*[numClients];
  HypoNeed = new int*[numClients];
  NumProcPerClient = new int[numClients];
  ClientsReleased = new int[numClients];

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

// Initialization of result variables 
int ServerThreads::countAccepted = 0;
int ServerThreads::countOnWait = 0;
int ServerThreads::countInvalid = 0;
int ServerThreads::countClientsDispatched = 0;

int* ServerThreads::Available = NULL;
int** ServerThreads::Max = NULL;
int** ServerThreads::Allocation = NULL;
int** ServerThreads::Need = NULL;


int ServerThreads::numClients = 0;
int ServerThreads::numResources = 0;
int ServerThreads::numRequestsPerClient = 0;

int ServerThreads::lastProcMilli = 500;
boolean* ServerThreads::Finish = NULL;
int* ServerThreads::Work = NULL;
int** ServerThreads::HypoAllocation = NULL;
int** ServerThreads::HypoNeed = NULL;
int* ServerThreads::NumProcPerClient = NULL;
boolean* ServerThreads::ClientsReleased = NULL;
