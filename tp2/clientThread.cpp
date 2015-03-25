#include "clientThread.h"
#include "common.h"

/// If you need to initialize something else
/// use this function that is called only one time
/// before all the threads start
void ClientThread::additionalInitialization(){
    allocatedResources = new int*[numClients];
    for (int i = 0; i < numClients; i++) {
        allocatedResources[i] = new int[numResources]();
    }
}

/// Implement in this function the code necesesary to generate and
/// sent each request based on the parameters given.
int ClientThread::sendRequest(int clientID, int requestID, int socketFD,
                              int *&requestQuantities) {
    /*
    Il est attendu que l'argument requestQuantities contienne les quantites de
    ressources a mettre dans la requete qui est envoyee au serveur. Si
    requestQuantities est NULL, la requete est generee selon clientID et requestID.

    Retourne un delai d'attente (ms) si la requete ne peut etre traitee immediatement
    par le serveur (requete 'onWait'). Retourne 0 sinon.
    */

    /// TP2_TO_DO

    if (requestQuantities == NULL || requestID == numRequests -1) {
        if (requestQuantities == NULL) {
            requestQuantities = new int[numResources]();
        }
    
        // On determine le contenu de requestQuantities.
        if (requestID == 0) { // La premiere requete.
            randomAllocations(clientID, requestQuantities);
        }
        else if (requestID == numRequests - 1) { // La derniere requete.
            copy(allocatedResources[clientID],
                 allocatedResources[clientID] + numResources,
                 requestQuantities);
        }
        // Les autres requetes.
        else if (!allocationPossible(clientID)) {
            // Le client utilise presentement toutes les ressources a sa
            // disposition.
            randomReleases(clientID, requestQuantities);
        }
        else if (!releasePossible(clientID)) {
            // Le client utilise presentement une seule ressource.
            randomAllocations(clientID, requestQuantities);
        }
        // A ce stade-ci, le client a l'option de faire une demande ou une
        // liberation de ressources. On 'flip' un trente sous.
        else if (rand() % 2) {
            randomAllocations(clientID, requestQuantities);
        }
        else {
            randomReleases(clientID, requestQuantities);
        }
    }

    // On batit la requete.
    string request = i_to_str(clientID) +
                     " " +
                     ints_to_str(requestQuantities, numResources);
    // cout << request << endl;

    write(socketFD, request.c_str(), request.length());
    
    cout << "Client " << clientID << " is sending its " << requestID << " request" << endl;

    char buffer[20];
    bzero(buffer, 20);
    read(socketFD, buffer, 19);

    // TODO: S'assurer qu'on recoit bel et bien un nombre.
    int response = atoi(buffer);
    if (response == 0) { // Requete acceptee.
        // On met a jour allocatedResources.
        for (int i = 0; i < numResources; i++) {
            allocatedResources[clientID][i] -= requestQuantities[i];
        }

        pthread_mutex_lock(&mutexCountAccepted);
        countAccepted++;
        pthread_mutex_unlock(&mutexCountAccepted);
    }
    else if (response > 0) { // Requete 'onWait'.        
        pthread_mutex_lock(&mutexCountOnWait);
        countOnWait++;
        pthread_mutex_unlock(&mutexCountOnWait);
    }
    else { // Requete invalide.
        pthread_mutex_lock(&mutexCountInvalid);
        countInvalid++;
        pthread_mutex_unlock(&mutexCountInvalid);
    }

    return response > 0 ? response : 0;

    /// TP2_END_TO_DO
}

// TODO: Les deux fonctions suivantes ont beaucoup en commun ...

void ClientThread::randomAllocations(int clientID, int allocations[]) {
    bool atLeastOne = false;
    do {
        for (int i = 0; i < numResources; i++) {
            int available = Max[clientID][i] - allocatedResources[clientID][i];
            allocations[i] = 0; // On n'est jamais trop prudent :)
            if (available != 0) {
                int allocation = rand() % (available + 1);
                allocations[i] = -allocation;                
                
                if (allocation > 0) {
                    atLeastOne = true;
                }
            }
        }            
    } while (!atLeastOne );    
}

void ClientThread::randomReleases(int clientID, int releases[]) {
    bool atLeastOne = false;
    bool notEverything = false;
    do {
        for (int i = 0; i < numResources; i++) {
            int releasable = allocatedResources[clientID][i];
            releases[i] = 0;
            if (releasable != 0) {
                int release = rand() % (releasable + 1);
                releases[i] = release;
                
                if (release > 0) {
                    atLeastOne = true;
                }

                if (release < releasable) {
                    notEverything = true;
                }
            }
        }
    } while (!atLeastOne || !notEverything);
}

bool ClientThread::allocationPossible(int clientID) {
    // Une demande de ressources est possible si le client n'utilise pas
    // presentement toutes les ressources a sa disposition.
    return sum(Max[clientID], numResources) > 
           sum(allocatedResources[clientID], numResources);
}

bool ClientThread::releasePossible(int clientID) {
    // Une liberation partielle des ressources est possible si le client
    // utilise presentement au moins deux ressources.
    return sum(allocatedResources[clientID], numResources) > 2;
}

// TODO: Ecrire un ensemble de fonctions pour faire des operations generiques
//       sur des vecteurs et les utiliser aussi dans le code du serveur.
//
//       Re-ecrire le code des fonctions precedentes en utilisant de telles
//       fonctions.
//
//       Jeter un coup d'oeil a la classe vector.
int ClientThread::sum(int integers[], int length) {
    int sum = 0;
    for (int i = 0; i < length; i++) {
        sum += integers[i];
    }
    return sum;
}

/// This function is called by each client thread and starts
/// a loop to do each of the numRequest requests
/// complete the TO_DO part to make the socket conection
/// with the server and send the request
/// USE the ''localhost'' address and the portNumber port
void *ClientThread::clientThreadCode(void * param){
    ClientThread* clientThreadPtr = (ClientThread*)param;

    int sockfd;
    struct hostent *server;
    struct sockaddr_in serv_addr;

    int *requestQuantities = NULL;

    for (int rID = 0 ; rID < numRequests ; rID++ ) {
        //Start creating conextions to send the request

	/// TP2_TO_DO

        /** =========================================================================
        Creation d'une nouvelle prise ('socket') et etablissement d'une
        connexion avec le serveur.

        Suivant http://www.linuxhowtos.org/C_C++/socket.htm.
        -------------------------------------------------------------------------- */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            error("ERROR opening socket");
        }

        server = gethostbyname("localhost");
        if (server == NULL) {
            error("ERROR, host not reachable");
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *) server->h_addr,
              (char *) &serv_addr.sin_addr.s_addr,
              server->h_length);
        serv_addr.sin_port = htons(portNumber);

        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            error("ERROR connecting"); 
        }
        /** ====================================================================== */
        
        int delai = sendRequest(clientThreadPtr->ID, rID, sockfd, requestQuantities);

        close(sockfd);

        if (delai > 0) {
            // TODO: ...
        }
        else {
            delete requestQuantities;
            requestQuantities = NULL;
        }
	
	/// TP2_END_TO_DO 
    }

    delete requestQuantities;

    // TODO: Not exactly ok.
    pthread_mutex_lock(&mutexCountClientsDispatched);
    countClientsDispatched++;
    pthread_mutex_unlock(&mutexCountClientsDispatched);

    pthread_exit(NULL);
}

/// Change the sleep() function for a correct solution for
/// the ending synchronization problem. The client has to
/// wait for the server to proccess all request before ending
/// its excecution. HINT: Look for named pipes
void ClientThread::waitUntilServerFinishes(){
    /// TP2_TO_DO

    // TODO: Fix.
    /*
    string name_str = i_to_str(portNumber);
    const char* name_char = name_str.c_str();
    int serverPipe;
    serverPipe = open(name_char, O_RDWR);
    if (serverPipe < 1)
        error("ERROR opening fifo server pipe");	
    bool finished = false;
    */

    while (countClientsDispatched != numClients);

    /*
    while (!finished)
        read(serverPipe, &finished, sizeof(bool));
    cout << "server has sent FINISHED message to client through pipe" << portNumber << endl;

    close(serverPipe);
    */

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
	if (fileExists("temp/Max")) { // TODO: Si le fichier n'existe pas?
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
    // TODO: N'est-ce-pas problematique de supprimer des variables de classe
    //       dans un destructeur?

    pthread_mutex_destroy(&mutexCountAccepted);
    pthread_mutex_destroy(&mutexCountOnWait);
    pthread_mutex_destroy(&mutexCountInvalid);
    pthread_mutex_destroy(&mutexCountClientsDispatched);
    
    if (Max != NULL) {
        for (int i = 0 ; i < numClients; i++)
            delete []Max[i];
        delete []Max;
        Max = NULL;
    }
    
    if (allocatedResources != NULL) {
        for (int i = 0; i < numClients; i++)
            delete []allocatedResources[i];
        delete []allocatedResources;
        allocatedResources = NULL;
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

pthread_mutex_t ClientThread::mutexCountAccepted = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ClientThread::mutexCountOnWait = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ClientThread::mutexCountInvalid = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ClientThread::mutexCountClientsDispatched = PTHREAD_MUTEX_INITIALIZER;

int **ClientThread::Max = NULL;
int **ClientThread::allocatedResources = NULL;
