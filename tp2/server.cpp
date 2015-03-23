#include "serverThreads.h"
#include <stdio.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
  ServerThreads server;
	
  //Read the parameters from the configuration file specified
  server.readConfigurationFile("initValues.cfg");

  cout << "apres lecture config" << endl;
	
  //Fill the Banker's Algorithm data structures with 
  //whatever you think convenient
  server.initializationOfDataStructures();
	
  cout << "apres initialisation" << endl;

  //Start the server with the configuration loaded
  server.createAndStart();
	
  cout << "apres create and start" << endl;

  //Signal the client process to end
  server.signalFinishToClient();

  cout << "apres signal finish" << endl;
	
  /// Do not erase or modify this part
  server.printAndSaveResults("temp/resultsServer");
	
  return 0;	
}

