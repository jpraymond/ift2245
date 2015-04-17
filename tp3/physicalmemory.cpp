#include "physicalmemory.h"

PhysicalMemory::PhysicalMemory(){
  //Open the BACKING_STORE file to being able to read from it afterwards
  backingStoreFile.open("BACKING_STORE.txt",ios::in );

  // fills the PhysicalMemory with spaces ' '
  // initialise la pile des cadres disponibles
  for (int i=0; i<PHYSICAL_MEMORY_SIZE; ++i)
    {
      physicalMemoryData[i] = ' ';
    }

  // initialise la pile des cadres disponibles
  for (int i=0; i<NUM_FRAMES; ++i)
    {
      freeFrames.push_back(i);
    }
  
}

PhysicalMemory::~PhysicalMemory(){
  //Close the BACKING_STORE file
  backingStoreFile.close();
}

// retourne un numero de cadre libre et met a jour la piled des cadres disponibles
// TODO: Si freeFrames est vide ...?
int PhysicalMemory::findFreeFrame(){
  int frameNumber = freeFrames.front();
  freeFrames.pop_front();
  return frameNumber;
}

// recherche directement la page indiquee dans la reserve et la copie dans le cadre indique
void PhysicalMemory::demandPageFromBackingStore(unsigned int fromPageNumber, unsigned int toFrameNumber)
{  
  // nombre d'entrees apparaissant dans les pages precedentes de la reserve
  unsigned int beforePage = fromPageNumber * PAGE_FRAME_SIZE;
  backingStoreFile.seekg(beforePage);
  char * buffer = new char [PAGE_FRAME_SIZE];
  backingStoreFile.read (buffer, PAGE_FRAME_SIZE);
  // nombre d'entrees apparaissant dans les cadres precedents de la memoire
  int beforeFrame = toFrameNumber * PAGE_FRAME_SIZE;
  // copie la page demandee depuis la reserve vers la memoire
  memcpy(&physicalMemoryData[beforeFrame], buffer, PAGE_FRAME_SIZE);
  delete[] buffer;
  // reinitialisation de la reserve
  backingStoreFile.clear();
  backingStoreFile.seekg(0, ios::beg);
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

