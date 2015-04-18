#include "virtualmemorymanager.h"

// Extract the page number and the offset from the logical
// address according to the TP3, then save it in the
// corresponding 'pageNumber' and 'offset' varibles
Command::Command(unsigned int logicAddress): logicalAdd(logicAddress){
  pageNumber = leastSignificantByte(logicAddress, 2);
  offset = leastSignificantByte(logicAddress);
}

// After reading all commands and storing them in the commandList
// traverse the list to apply each command.
void VirtualMemoryManager::applyCommands(){
  // initialisation des variables comptables
  pageFaultCount = 0;
  pageFoundCount = 0;
  TLBHitCount = 0;
  TLBMissCount = 0;
  // initialisation du chronometre
  long duration = 0;

  // parcours de la liste des adresses demandees
  for (list<Command>::iterator it = commandList.begin(); it != commandList.end(); it++){
    // conversion de l'adresse décimale en page et décalage
    Command c = *it;

    // demarrage du chronometre
    timespec start;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    
    // recherche de la page virtuelle dans la cache de la table de pagination
    int frameNumber = tlb.findPage(c.pageNumber);

    if (frameNumber == -1) { // La page n'etait pas dans le TLB.
      Page *page = &pageTable[c.pageNumber];
      if (!page->verificationBit) { // La page n'etait pas en memoire.
	page->frameNumber = physicalMemory.findFreeFrame();
	physicalMemory.demandPageFromBackingStore(c.pageNumber,
						  page->frameNumber);
	page->verificationBit = true;
	pageFaultCount++;
      }
      else { // La page etait en memoire.
	pageFoundCount++;
      }
      frameNumber = page->frameNumber;
      tlb.addEntry(c.pageNumber, frameNumber);
      TLBMissCount++;
    }
    else { // La page etait dans le TLB.
      pageFoundCount++;
      TLBHitCount++;
    }
    
    // arret du chronometre
    timespec end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    // mise a jour de la duree totale de calcul
    duration += diff(start, end).tv_nsec;
        
    // construction de l'adresse physique
    int physicalAddress = frameNumber*PAGE_FRAME_SIZE + c.offset;
    // lecture dans la memoire physique
    signed char val = physicalMemory.getValueFromFrameAndOffset(frameNumber, c.offset);
    // affichage du resultat de la lecture
    cout << "Original Addr: " << setw(5) << c.logicalAdd
	 << "\tPage: " << setw(3) << c.pageNumber
	 << "\tOffset: " << setw(3) << c.offset
	 << "\tPhysical Addr: " << setw(5) << physicalAddress
	 << "\tValue: " << val << endl;
  }

  // cout << duration / (1000.0 * 1000.0 * 1000.0) << endl;
}

// affiche les resultats comptables 
void VirtualMemoryManager::printResults(){
  cout << endl;
  cout << "Page-Faults: " << pageFaultCount << "\tPage founds: " << pageFoundCount << endl;
  cout << "TLB-Hit: " << TLBHitCount << "\tTLB-miss: " << TLBMissCount << endl;
  cout << "Page-Fault rate: " << pageFaultCount/float(pageFaultCount+pageFoundCount) << endl;
  cout << "TLB-Hit rate: " << TLBHitCount/float(TLBHitCount+TLBMissCount) << endl;

  //----------DO NOT CHANGE/ERASE THIS------------------
  physicalMemory.printPhysicalMemory();
  tlb.printTLB();
  //-----------------------------------------------------
}

VirtualMemoryManager::VirtualMemoryManager() :
  pageFaultCount(0), pageFoundCount(0){
}

// 
void VirtualMemoryManager::addCommandFromLogicAddress(unsigned int logicAddres){
  commandList.push_back(Command(logicAddres));
}
