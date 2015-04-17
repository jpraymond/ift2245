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
  // permet l'implementation du remplacement suivant PEPS (FIFO)
  list<int> fifoQueue;

  // dictionnaire auxiliaire permettant la recherche rapide dans le TLB
  // cle: numero de page memoire; valeur: indice du TLB
  unordered_map<int, int> TLBSearchMap;

  // permet l'implementation du remplacement suivant MRU (LRU)
  // TODO: Il me semble que l'utilisation d'un numero de sequence va poser
  //       probleme lorsqu'on va atteindre la valeur maximale d'un int.
  int currentSequNumber;
  // TODO: Pourquoi ne pas simplement ajouter un attribute a TLB_entry?
  int sequNumbers[TLB_NUM_ENTRIES];

 public:
  TLB_entry TLBTable[TLB_NUM_ENTRIES];

  TLB();
  ~TLB();
  // Looks for the pageNumber in the table, if it finds it it return
  // the coresponding frame if not it returns -1;
  int findPage(int pageNumber);
  int findPageSearchMap(int pageNumber);
  
  // Ajoute une paire (pageNumber,frameNumber) au tableau TLB avec remplacement
  // suivant la methode PEPS (FIFO) lorsque le tableau TLB est plein
  void addEntryFIFO(int pageNumber, int frameNumber);
  void addEntryFIFOSearchMap(int pageNumber, int frameNumber);

  // Ajoute une paire (pageNumber,frameNumber) au tableau TLB avec remplacement
  // suivant la methode MRU (LRU) lorsque le tableau TLB est plein
  void addEntryLRU(int pageNumber, int frameNumber);
  void addEntryLRUSearchMap(int pageNumber, int frameNumber);

  int nextEntryAvailable;

  //----------DO NOT CHANGE/ERASE THIS------------------
  // this function is neccesary to output the physical
  // memory state at the end of execution. It needs to
  // be generated for evaluation purposes.
  void printTLB();
  //----------------------------------------------------
};

#endif // TLB_H
