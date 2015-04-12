#include "tlb.h"

TLB_entry::TLB_entry(int page, int frame) :
  pageNumber(page), frameNumber(frame){
}

// recherche lineaire de la page 'pageNumber' dans la table TLB, si succes retourne le numero du cadre memoire correspondant,
//  sinon retourne -1
int TLB::findPage(int pageNumber){
  int found = -1;
  for (int i = 0; i < TLB_NUM_ENTRIES; i ++)
    {
      if (TLBTable[i].pageNumber == pageNumber)
	{
	  found = TLBTable[i].frameNumber;
	  // mise a jour de l'estampille de la page trouvee pour permettre l'usage de addEntryLRU()
	  sequNumbers[i] = currentSequNumber;
	  currentSequNumber++;
	  return found;
	}
    }
  return found;
}

// Ajoute une paire (pageNumber,frameNumber) au tableau TLB
// en appliquant la methode PEPS (FIFO) lorsque le tableau TLB est plein
// Adapte a la recherche lineaire
void TLB::addEntryFIFO(int pageNumber, int frameNumber){   
  // le tableau TLB n'est pas encore plein
  if (nextEntryAvailable >= 0 && nextEntryAvailable < TLB_NUM_ENTRIES)
    {
      // insertion dans le tableau TLB
      TLBTable[nextEntryAvailable].pageNumber = pageNumber;
      TLBTable[nextEntryAvailable].frameNumber = frameNumber;
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
      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      // mise a jour de la file PEPS
      fifoQueue.push_back(select);
    }
}

// Ajoute une paire (pageNumber,frameNumber) au tableau TLB avec remplacement
// suivant la methode MRU (LRU) lorsque le tableau TLB est plein
// Adapte a la recherche lineaire
void TLB::addEntryLRU(int pageNumber, int frameNumber){
  // le tableau TLP n'est pas encore plein
  if (nextEntryAvailable >= 0 && nextEntryAvailable < TLB_NUM_ENTRIES)
    {
      // insertion dans le tableau TLP
      TLBTable[nextEntryAvailable].pageNumber = pageNumber;
      TLBTable[nextEntryAvailable].frameNumber = frameNumber;
      // mise a jour de l'estampille de la page nouvellement inseree
      sequNumbers[nextEntryAvailable] = currentSequNumber;
      currentSequNumber++;
      // mise a jour du numero du prochain emplacement libre
      nextEntryAvailable++;
      if (nextEntryAvailable == TLB_NUM_ENTRIES)
	{
	  nextEntryAvailable = -1;
	}   
    }
  // le tableau TLP est plein
  else
    {
      int min = currentSequNumber;
      int select = -1;
      // recherche lineaire de la page la moins recemment consultee
      for (int i = 0; i < TLB_NUM_ENTRIES; i ++)
	{
	  if (sequNumbers[i] <= min)
	    {
	      min = sequNumbers[i];
	      select = i;
	    }
	}
      // substitution dans le tableau TLP
      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      // mise a jour de l'estampille de la page nouvellement inseree
      sequNumbers[select] = currentSequNumber;
      currentSequNumber++;
    }
}

// recherche avec dictionnaire auxiliaire de la page 'pageNumber' dans la table TLB, si succes retourne le numero du cadre memoire correspondant,
//  sinon retourne -1
int TLB::findPageSearchMap(int pageNumber){
  int found = -1;
  unordered_map<int, int>::iterator it = TLBSearchMap.find(pageNumber);
  if (it == TLBSearchMap.end())
    found = -1;
  else
    {
      found = TLBTable[it->second].frameNumber;
      // mise a jour de l'estampille de la page trouvee pour permettre l'usage de addEntryLRU()
      sequNumbers[it->second] = currentSequNumber;
      currentSequNumber++;
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
      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      // mise a jour du dictionnaire de recherche
      TLBSearchMap[pageNumber] = select;
      // mise a jour de la file PEPS
      fifoQueue.push_back(select);    
    }
}

// Ajoute une paire (pageNumber,frameNumber) au tableau TLB avec remplacement
// suivant la methode MRU (LRU) lorsque le tableau TLB est plein
// Adapte a la recherche avec dictionnaire auxiliaire
void TLB::addEntryLRUSearchMap(int pageNumber, int frameNumber){
  // le tableau TLP n'est pas encore plein
  if (nextEntryAvailable >= 0 && nextEntryAvailable < TLB_NUM_ENTRIES)
    {
      // insertion dans le tableau TLP
      TLBTable[nextEntryAvailable].pageNumber = pageNumber;
      TLBTable[nextEntryAvailable].frameNumber = frameNumber;
      // mise a jour du dictionnaire de recherche
      TLBSearchMap[pageNumber] = nextEntryAvailable;
      // mise a jour de l'estampille de la page nouvellement inseree
      sequNumbers[nextEntryAvailable] = currentSequNumber;
      currentSequNumber++;
      // mise a jour du numero du prochain emplacement libre
      nextEntryAvailable++;
      if (nextEntryAvailable == TLB_NUM_ENTRIES)
	{
	  nextEntryAvailable = -1;
	}   
    }
  // le tableau TLP est plein
  else
    {
      int min = currentSequNumber;
      int select = -1;
      // recherche lineaire de la page la moins recemment consultee
      for (int i = 0; i < TLB_NUM_ENTRIES; i ++)
	{
	  if (sequNumbers[i] <= min)
	    {
	      min = sequNumbers[i];
	      select = i;
	    }
	}
      // substitution dans le tableau TLP
      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      // mise a jour du dictionnaire de recherche
      TLBSearchMap[pageNumber] = select;
      // mise a jour de l'estampille de la page nouvellement inseree
      sequNumbers[select] = currentSequNumber;
      currentSequNumber++;     
    }
}














TLB::TLB() : nextEntryAvailable(0), currentSequNumber(0){
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
