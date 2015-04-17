#include "tlb.h"

TLB_entry::TLB_entry(int page, int frame) :
  pageNumber(page), frameNumber(frame){
}

// recherche lineaire de la page 'pageNumber' dans la table TLB.
// si succes retourne le numero du cadre memoire correspondant, sinon retourne -1
int TLB::findPage(int pageNumber){
  int found = -1;
  for (int i = 0; i < TLB_NUM_ENTRIES; i ++)
    {
      if (TLBTable[i].pageNumber == pageNumber)
	{
	  found = TLBTable[i].frameNumber;
          if (TLB_MODE == 1) { // LRU
            // On retourne la page utilisee au debut de la file.
            fifoQueue.remove(i);
            fifoQueue.push_back(i);
          }
	  return found;
	}
    }
  return found;
}

// Ajoute une paire (pageNumber,frameNumber) au tableau TLB.
// Adapte a la recherche lineaire
void TLB::addEntry(int pageNumber, int frameNumber){   
  // le tableau TLB n'est pas encore plein
  if (nextEntryAvailable >= 0 && nextEntryAvailable < TLB_NUM_ENTRIES)
    {
      // insertion dans le tableau TLB
      TLBTable[nextEntryAvailable].pageNumber = pageNumber;
      TLBTable[nextEntryAvailable].frameNumber = frameNumber;
      
      // mise a jour de la file FIFO/LRU
      fifoQueue.push_back(nextEntryAvailable);
      
      // mise a jour du numero du prochain emplacement libre
      nextEntryAvailable++;
      if (nextEntryAvailable == TLB_NUM_ENTRIES)
	{
	  nextEntryAvailable = -1;
	}
    }
  // le tableau TLB est plein
  else
    {
      // substitution dans le tableau TLB (suivant la methode FIFO ou LRU)
      
      int select = fifoQueue.front();
      fifoQueue.pop_front();
      
      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      
      // mise a jour de la file FIFO/LRU
      fifoQueue.push_back(select);
    }
}


/*
Version alternative des fonctions precedentes qui utilise un dictionnaire pour
chercher l'emplacement d'une page dans le TLB.

N'a pas ete retenue car semble moins rapide pour un TLB de 16 entrees.
*/

// recherche avec dictionnaire auxiliaire de la page 'pageNumber' dans la table TLB.
// si succes retourne le numero du cadre memoire correspondant, sinon retourne -1.
int TLB::findPageSearchMap(int pageNumber){
  int found = -1;
  unordered_map<int, int>::iterator it = TLBSearchMap.find(pageNumber);
  if (it != TLBSearchMap.end())
    {
      found = TLBTable[it->second].frameNumber;
    }
  return found;
}

// Ajoute une paire (pageNumber,frameNumber) au tableau TLB
// en appliquant la methode PEPS (FIFO) lorsque le tableau TLB est plein
// Adapte a la recherche avec dictionnaire auxiliaire
void TLB::addEntryFIFOSearchMap(int pageNumber, int frameNumber){   
  // le tableau TLB n'est pas encore plein
  if (nextEntryAvailable >= 0 && nextEntryAvailable < TLB_NUM_ENTRIES)
    {
      // insertion dans le tableau TLB
      TLBTable[nextEntryAvailable].pageNumber = pageNumber;
      TLBTable[nextEntryAvailable].frameNumber = frameNumber;
      // mise a jour du dictionnaire de recherche
      TLBSearchMap[pageNumber] = nextEntryAvailable;
      // mise a jour de la file PEPS
      fifoQueue.push_back(nextEntryAvailable);
      // mise a jour du numero du prochain emplacement libre
      nextEntryAvailable++;
      if (nextEntryAvailable == TLB_NUM_ENTRIES)
	{
	  nextEntryAvailable = -1;
	}
    }
  // le tableau TLB est plein
  else
    {
      // substitution dans le tableau TLB d'apres PEPS

      int select = fifoQueue.front();
      fifoQueue.pop_front();
      
      int pageToRemove = TLBTable[select].pageNumber;

      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      
      // mise a jour du dictionnaire de recherche
      TLBSearchMap.erase(pageToRemove);
      TLBSearchMap[pageNumber] = select;

      // mise a jour de la file PEPS
      fifoQueue.push_back(select);    
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
