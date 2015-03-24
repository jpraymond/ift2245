#ifndef COMMON_H
#define COMMON_H

//Common headers and useful functions

//Standar C/C++ libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
//Libconfig libraries to read configuration from file
#include <libconfig.h++>
//Named pipes libraries
#include <sys/stat.h>
#include <fcntl.h>
//Socket implementation libraries
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
//Read and write of files
#include <fstream>
#include <ostream>

#include <sstream>

using namespace std;

//Outputs a readable error message and exits the program
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//Return true if the specified file exist
bool fileExists(const char *fileName)
{
	struct stat fileExistTest; 
	return (stat(fileName,&fileExistTest) == 0);
}

// retourne un entier sous forme de chaine
string i_to_str (int n) {
    stringstream ss;
    ss << n;
    return ss.str();
}

string ints_to_str(int integers[], int length) {
    string str ("");
    for (int i = 0; i < length; i++) {
        str += (i_to_str(integers[i]) + " ");
    }
    return str.substr(0, str.length() - 1);
}

#endif // COMMON_H
