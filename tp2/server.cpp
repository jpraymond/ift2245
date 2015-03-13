#include "serverThreads.h"

int main(int argc, char *argv[])
{
	ServerThreads server;
	
	//Read the parameters from the configuration file specified
	server.readConfigurationFile("initValues.cfg");
	
	//Fill the Banker's Algorithm data structures with 
	//whatever you think convenient
	server.initializationOfDataStructures();
	
	//Start the server with the configuration loaded
	server.createAndStart();
	
	//Signal the client process to end
	server.signalFinishToClient();
	
	/// Do not erase or modify this part
	server.printAndSaveResults("temp/resultsServer");
	
	return 0;	
}

