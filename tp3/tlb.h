#ifndef TLB_H
#define TLB_H

#include "common.h"

class TLB_entry{
 public:
  int pageNumber;
  int frameNumber;

  TLB_entry(int page = -1, int frame = -1);
};

class TLB
{
 private: 
  // file permettant l'implementation du remplacement suivant LRU ou FIFO
  list<int> lruFifoQueue;

  // dictionnaire auxiliaire permettant la recherche rapide dans le TLB
  // cle: numero de page memoire; valeur: indice du TLB
  unordered_map<int, int> TLBSearchMap;
  
  int nextEntryAvailable;


  // Ajoute une paire (pageNumber,frameNumber) au tableau TLB avec remplacement
  // suivant la methode MRU (LRU) lorsque le tableau TLB est plein
  void addEntryLRU(int pageNumber, int frameNumber);
  void addEntryLRUSearchMap(int pageNumber, int frameNumber);

  string queueToString();

 public:
  TLB_entry TLBTable[TLB_NUM_ENTRIES];

  TLB();
  ~TLB();

  // Looks for the pageNumber in the table, if it finds it it return
  // the coresponding frame if not it returns -1;
  int findPage(int pageNumber);
  int findPageSearchMap(int pageNumber);

  // Ajoute une paire (pageNumber,frameNumber) au tableau TLB avec remplacement
  // suivant la methode PEPS (FIFO) ou LRU lorsque le tableau TLB est plein
  void addEntry(int pageNumber, int frameNumber);
  void addEntryFIFOSearchMap(int pageNumber, int frameNumber);
  
  //----------DO NOT CHANGE/ERASE THIS------------------
  // this function is neccesary to output the physical
  // memory state at the end of execution. It needs to
  // be generated for evaluation purposes.
  void printTLB();
  //----------------------------------------------------
};

#endif // TLB_H
