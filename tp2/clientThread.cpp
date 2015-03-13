#include "clientThread.h"
#include "common.h"

/// If you need to initialize something else
/// use this function that is called only one time
/// before all the threads start
void ClientThread::additionalInitialization(){


}

/// Implement in this function the code necesesary to generate and
/// sent each request based on the parameters given.
void ClientThread::sendRequest( int clientID, int requestID, int socketFD){
	/// TP2_TO_DO

	cout << "Client " << clientID << " is sending its " << requestID << " request" << endl;
	
	/// TP2_END_TO_DO
}

/// This function is called by each client thread and starts
/// a loop to do each of the numRequest requests
/// complete the TO_DO part to make the socket conection
/// with the server and send the request
/// USE the ''localhost'' address and the portNumber port
void *ClientThread::clientThreadCode(void * param){
	ClientThread* clientThreadPtr = (ClientThread*)param;

	int sockfd;
    
    
    for (int rID = 0 ; rID < numRequests ; rID++ ) {
		//Start creating conextions to send the request
		
		/// TP2_TO_DO
		
		sendRequest(clientThreadPtr->ID,rID,sockfd);
		
		/// TP2_END_TO_DO 
	}
	pthread_exit(NULL);
}

/// Change the sleep() function for a correct solution for
/// the ending synchronization problem. The client has to
/// wait for the server to proccess all request before ending
/// its excecution. HINT: Look for named pipes
void ClientThread::waitUntilServerFinishes(){
	/// TP2_TO_DO
	
	sleep(2); 
	
	
	/// TP2_END_TO_DO
}

/// You can modify this function to print other values at
/// the end of the client application excecution. But do not
/// modify the result file output, as it is neccesary for evaluation
void ClientThread::printAndSaveResults(const char* fileName){
	cout << endl << "------Client Results-----" << endl;
	cout << "Requests accepted:\t\t" << countAccepted << endl;
	cout << "Requests sent to wait:\t\t" << countOnWait << endl;
	cout << "Invalid requests:\t\t" << countInvalid << endl;
	cout << "Clients dispatched:\t\t" << countClientsDispatched << endl;
	
	///DO NOT MODIFY THIS PART
	// Save the counted values for evaluation
	if (fileExists(fileName)) remove(fileName);
	ofstream fs(fileName);
	fs << countAccepted << " " << countOnWait << " " << countInvalid << " " << countClientsDispatched << endl;
	fs.close();
}

/// You can modify this function if you want to add other fields
/// to the configuration fiel, but normally you will not need to do it
int ClientThread::readConfigurationFile(const char *fileName){
	if (!fileExists(fileName)) {
		cout << "No configuration file " << fileName << " found" << endl;
		exit(1);
	}
	
	libconfig::Config cfg;
	cfg.readFile(fileName);
	cfg.lookupValue("portNumber",portNumber);
	cfg.lookupValue("numClients",numClients);
	cfg.lookupValue("numResources",numResources);
	cfg.lookupValue("numRequestsPerClient",numRequests);
	
	Max = new int*[numClients];
	for (int i = 0 ; i < numClients ; i++)
		Max[i] = new int[numResources];
		
	readMaxFromFile();
	additionalInitialization();
	
	return numClients;
}

/// The rest of the code is neccesary for the correct functionality
/// You can add extra stuff, but try to keep the provided code as it is

void ClientThread::readMaxFromFile(){
	fstream fs;
	if (fileExists("temp/Max")) {
		fs.open("temp/Max", fstream::in);
		
		for (int i = 0 ; i < numClients ; i++)
			for (int j = 0 ; j < numResources ; j++){
				fs >> Max[i][j];
			}
				
		fs.close();
	}
}

ClientThread::ClientThread(){
	ID = count++;
}

ClientThread::~ClientThread(){
	
	if (Max != NULL) {
		for (int i = 0 ; i < numResources  ; i++)
			delete []Max[i];
		delete []Max;
		Max = NULL;
	}
}

void ClientThread::createAndStartThread(){	
	pthread_attr_init(&pt_attr);
	pthread_create(&pt_tid,&pt_attr,&clientThreadCode,this);
	pthread_detach(pt_tid);
}

//Initialization of static variables
int ClientThread::count = 0;
int ClientThread::portNumber = 0;
int ClientThread::numClients = 0;
int ClientThread::numResources = 0;
int ClientThread::numRequests = 0;

// Initialization of result variables 
int ClientThread::countAccepted = 0;
int ClientThread::countOnWait = 0;
int ClientThread::countInvalid = 0;
int ClientThread::countClientsDispatched = 0;

int **ClientThread::Max = NULL;
