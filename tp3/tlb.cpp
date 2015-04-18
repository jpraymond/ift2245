#include "tlb.h"

TLB_entry::TLB_entry(int page, int frame) :
  pageNumber(page), frameNumber(frame){
}

// recherche lineaire de la page 'pageNumber' dans la cache TLB
// si succes retourne le numero du cadre memoire correspondant, sinon retourne -1
// permet le traitement des regles de remplacement FIFO et LRU
int TLB::findPage(int pageNumber){
  int found = -1;
  for (int i = 0; i < TLB_NUM_ENTRIES; i ++)
    {
      if (TLBTable[i].pageNumber == pageNumber){
	found = TLBTable[i].frameNumber;
	// cas LRU
	if (TLB_MODE == 1) {
	  // On retourne la page utilisee au debut de la file.
	  lruFifoQueue.remove(pageNumber);
	  lruFifoQueue.push_back(pageNumber);
	}
	return found;
      }
    }
  return found;
}

// Ajoute une paire (pageNumber,frameNumber) dans la cache TLB.
// Adapte a la recherche lineaire.
void TLB::addEntry(int pageNumber, int frameNumber){   
  // la cache TLB n'est pas encore pleine
  if (nextEntryAvailable >= 0 && nextEntryAvailable < TLB_NUM_ENTRIES)
    {
      // insertion dans la cache TLB
      TLBTable[nextEntryAvailable].pageNumber = pageNumber;
      TLBTable[nextEntryAvailable].frameNumber = frameNumber;
      
      // mise a jour de la file FIFO/LRU
      lruFifoQueue.push_back(nextEntryAvailable);
      
      // mise a jour du numero du prochain emplacement libre
      nextEntryAvailable++;
      if (nextEntryAvailable == TLB_NUM_ENTRIES)
	{
	  nextEntryAvailable = -1;
	}
    }
  // la cache TLB est pleine
  else
    {
      // substitution dans la cache TLB      
      int select = lruFifoQueue.front();
      lruFifoQueue.pop_front();      
      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      
      // mise a jour de la file FIFO/LRU
      lruFifoQueue.push_back(select);
    }
}

/*
  findPageSearchMap et addEntrySearchMap sont des versions alternatives des fonctions precedentes
  qui utilisent plutot un dictionnaire de hachage pour  rechercher l'emplacement d'une page dans la cache
  TLB.  Ces versions n'ont pas ete retenues car moins  rapides pour une cache TLB comportant 
  seulement 16 entrees.
*/

// recherche avec dictionnaire de hachage auxiliaire de la page 'pageNumber' dans la cache TLB.
// si succes retourne le numero du cadre memoire correspondant, sinon retourne -1.
// permet le traitement des regles de remplacement FIFO et LRU
int TLB::findPageSearchMap(int pageNumber){
  int found = -1;
  unordered_map<int, int>::iterator it = TLBSearchMap.find(pageNumber);
  if (it != TLBSearchMap.end())
    {
      found = TLBTable[it->second].frameNumber;
      // cas LRU
      if (TLB_MODE == 1) {
	// On retourne la page utilisee au debut de la file.
	lruFifoQueue.remove(pageNumber);
	lruFifoQueue.push_back(pageNumber);
      }
    }
  return found;
}

// Ajoute une paire (pageNumber,frameNumber) dans la cache TLB
// Adapte a la recherche avec dictionnaire de hachage auxiliaire
void TLB::addEntrySearchMap(int pageNumber, int frameNumber){   
  // la cache TLB n'est pas encore pleine
  if (nextEntryAvailable >= 0 && nextEntryAvailable < TLB_NUM_ENTRIES)
    {
      // insertion dans le tableau TLB
      TLBTable[nextEntryAvailable].pageNumber = pageNumber;
      TLBTable[nextEntryAvailable].frameNumber = frameNumber;
      // mise a jour du dictionnaire de hachage
      TLBSearchMap[pageNumber] = nextEntryAvailable;
      // mise a jour de la file FIFO/LRU
      lruFifoQueue.push_back(nextEntryAvailable);
      // mise a jour du numero du prochain emplacement libre
      nextEntryAvailable++;
      if (nextEntryAvailable == TLB_NUM_ENTRIES)
	{
	  nextEntryAvailable = -1;
	}
    }
  // la cache TLB est pleine
  else
    {
      // substitution dans la cache TLB
      int select = lruFifoQueue.front();
      lruFifoQueue.pop_front();      
      int pageToRemove = TLBTable[select].pageNumber;
      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      
      // mise a jour du dictionnaire de hachage
      TLBSearchMap.erase(pageToRemove);
      TLBSearchMap[pageNumber] = select;

      // mise a jour de la file FIFO/LRU
      lruFifoQueue.push_back(select);    
    }
}

TLB::TLB() : nextEntryAvailable(0) {
}

TLB::~TLB(){
}

//----------DO NOT CHANGE/ERASE THIS------------------
// this function is neccesary to output the TLB state
// at the end of execution. It needs to
// be generated for evaluation purposes.
void TLB::printTLB()
{
  ofstream tlbFile("TLB.txt",ios::out );\
  for (int i=0; i<TLB_NUM_ENTRIES; ++i)
    tlbFile << TLBTable[i].pageNumber << " " << TLBTable[i].frameNumber << endl;
  tlbFile.close();
}
