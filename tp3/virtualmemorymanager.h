#ifndef VIRTUALMEMORYMANAGER_H
#define VIRTUALMEMORYMANAGER_H

#include "physicalmemory.h"
#include "tlb.h"
#include "common.h"

class Command{
 public:
  unsigned int logicalAdd;
  unsigned int pageNumber;
  unsigned int offset;

  // Extract the page number and the offset from the logical
  // address according to the TP3, then save it in the
  // corresponding 'pageNumber' and 'offset' varibles
  Command(unsigned int logicAddress);
};

class Page{
 public:
  bool verificationBit;
  int frameNumber;

 Page() :verificationBit(false),frameNumber(-1){}
};

class VirtualMemoryManager
{
 public:
  // variables comptables
  int pageFaultCount;
  int pageFoundCount;
  int TLBHitCount;
  int TLBMissCount;

  // memoire physique 
  PhysicalMemory  physicalMemory;
  // cache de table de pagination
  TLB             tlb;
  // table de pagination
  Page            pageTable[NUM_PAGES];
  // liste d'adresses de lecture
  list<Command>   commandList;

  VirtualMemoryManager();
  // After reading all commands and storing them in the commandList
  // traverse the list to apply each command.
  void addCommandFromLogicAddress(unsigned int logicAddres);
  void applyCommands();
  void printResults();

};

#endif // VIRTUALMEMORYMANAGER_H
