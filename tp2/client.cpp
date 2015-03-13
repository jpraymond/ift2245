#include "clientThread.h"

int main(int argc, char *argv[])
{
	//Read the parameters from the configuration file specified
	int N = ClientThread::readConfigurationFile("initValues.cfg");
	
	//Create the N different client threads and run the clientThreadCode function
	ClientThread *clientThreads = new ClientThread[N];
	for (int i = 0 ; i < N ; i++)
		clientThreads[i].createAndStartThread();
	
	//Wait until the server process ends
	ClientThread::waitUntilServerFinishes();
	
	/// Do not erase or modify this part
	ClientThread::printAndSaveResults("temp/resultsClient");
	
	//Deallocate the memory used for the ClientThread objects
	delete []clientThreads;
	
	return 0;	
}
