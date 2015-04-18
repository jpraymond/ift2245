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
          // cas LRU
          if (TLB_MODE == 1) {
            // On retourne la page utilisee au debut de la file.
            lruFifoQueue.remove(i);
            lruFifoQueue.push_back(i);

            /*
            cout << "TLB: Page " << pageNumber
                 << " is moved to the back of the queue." << endl;
            cout << this->queueToString() << endl;
            */
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
      lruFifoQueue.push_back(nextEntryAvailable);

      /*
      cout << "TLB: Page " << pageNumber << " is enqueued." << endl;
      cout << this->queueToString() << endl;
      */
      
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
      
      int select = lruFifoQueue.front();
      lruFifoQueue.pop_front();

      int dequeuedPage = TLBTable[select].pageNumber;
      
      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      
      // mise a jour de la file FIFO/LRU
      lruFifoQueue.push_back(select);

      /*
      cout << "TLB: Page " << dequeuedPage << " is dequeued and page "
           << pageNumber << " is enqueued." << endl;
      cout << this->queueToString() << endl;
      */
    }
}


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
      lruFifoQueue.push_back(nextEntryAvailable);
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

      int select = lruFifoQueue.front();
      lruFifoQueue.pop_front();
      
      int pageToRemove = TLBTable[select].pageNumber;

      TLBTable[select].pageNumber = pageNumber;
      TLBTable[select].frameNumber = frameNumber;
      
      // mise a jour du dictionnaire de recherche
      TLBSearchMap.erase(pageToRemove);
      TLBSearchMap[pageNumber] = select;

      // mise a jour de la file PEPS
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

string TLB::queueToString() {
  string str = "(";
  list<int>::const_iterator it;
  for (it = lruFifoQueue.begin(); it != lruFifoQueue.end(); it++) {
    str = str + to_string((long long) TLBTable[*it].pageNumber) + " ";
  }
  return str.substr(0, str.length() - 1) + ")";
}
