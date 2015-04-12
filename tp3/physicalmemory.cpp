#include "physicalmemory.h"

PhysicalMemory::PhysicalMemory(){
  //Open the BACKING_STORE file to being able to read from it afterwards
  backingStoreFile.open("../VirtualMemoryManager/BACKING_STORE.txt",ios::in );

  // fills the PhysicalMemory with spaces ' '
  // initialise la pile des cadres disponibles
  for (int i=0; i<PHYSICAL_MEMORY_SIZE; ++i)
    {
      physicalMemoryData[i] = ' ';
      freeFrames.push_front(i);
    }
}

PhysicalMemory::~PhysicalMemory(){
  //Close the BACKING_STORE file
  backingStoreFile.close();
}

// retourne un numero de cadre libre et met a jour la piled des cadres disponibles
int PhysicalMemory::findFreeFrame(){
  int frameNumber = freeFrames.front();
  freeFrames.pop_front();
  return frameNumber;
}

// TODO: configurer correctement la valeur retournee
// recherche directement la page indiquee dans la reserve et la copie dans le cadre indique
int PhysicalMemory::demandPageFromBackingStoreDirect(unsigned int fromPageNumber, unsigned int toFrameNumber)
{  
  // nombre d'entrees apparaissant dans les pages precedentes de la reserve
  unsigned int beforePage = fromPageNumber * PAGE_FRAME_SIZE;
  backingStoreFile.seekg(beforePage);
  char * buffer = new char [PAGE_FRAME_SIZE];
  backingStoreFile.read (buffer, PAGE_FRAME_SIZE);
  // nombre d'entrees apparaissant dans les cadres precedents de la memoire
  int beforeFrame = toFrameNumber * PAGE_FRAME_SIZE;
  // copie la page demandee depuis la reserve vers la memoire
  for (int i = 0; i < PAGE_FRAME_SIZE; i++)
    {      
      physicalMemoryData[beforeFrame + i] = buffer[i];
    }
  // reinitialisation de la reserve
  backingStoreFile.clear();
  backingStoreFile.seekg(0, ios::beg);
  return 0;
}

// TODO: configurer correctement la valeur retournee
// recherche lineaire de la page indiquee dans la reserve et la copie dans le cadre indique
int PhysicalMemory::demandPageFromBackingStoreLinear(unsigned int fromPageNumber, unsigned int toFrameNumber)
{  
  // nombre d'entrees apparaissant dans les pages precedentes de la reserve
  unsigned int beforePage = fromPageNumber * PAGE_FRAME_SIZE;
  char token;
  // parcours des entrees situees dans les pages precedentes
  for (int i = 0; i < beforePage; i++)
    {
      backingStoreFile >> token;
    }
  // nombre d'entrees apparaissant dans les cadres precedents de la memoire
  int beforeFrame = toFrameNumber * PAGE_FRAME_SIZE;
  // copie la page demandee depuis la reserve vers la memoire
  for (int i = 0; i < PAGE_FRAME_SIZE; i++)
    {
      backingStoreFile >> token;
      physicalMemoryData[beforeFrame + i] = token;
    }
  // reinitialisation de la reserve
  backingStoreFile.clear();
  backingStoreFile.seekg(0, ios::beg);
  return 0;
}

// retourne l'entree se trouvant au cadre et decalage indiques
char PhysicalMemory::getValueFromFrameAndOffset(unsigned int frameNumber, unsigned int offset){
  unsigned int position = frameNumber * PAGE_FRAME_SIZE + offset;
  return physicalMemoryData[position];
}

//----------DO NOT CHANGE/ERASE THIS------------------
// this function is neccesary to output the physical
// memory state at the end of execution. It needs to
// be generated for evaluation purposes.
void PhysicalMemory::printPhysicalMemory(){
  ofstream physicalMemoryFile("PhysicalMemory.txt",ios::out );
  for (int i=0; i<PHYSICAL_MEMORY_SIZE; ++i)
    physicalMemoryFile << physicalMemoryData[i];
  physicalMemoryFile.close();
}

