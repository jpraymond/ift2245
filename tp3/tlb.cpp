#include "tlb.h"

TLB_entry::TLB_entry(int page, int frame) :
    pageNumber(page), frameNumber(frame){
}

// Looks for the pageNumber in the table, if it finds it it return
// the coresponding frame if not it returns -1;
int TLB::findPage(int pageNumber){
    /// --------TP3__TO_DO---------
    ///
    ///
    return 0;
}

//Adds an entry-pair (pageNumber,frameNumber) to the TLB
//Here you have to implement a replacement technique for
//when the TLB is full. DONT FORGET to implement two different
//function for this, even if you only call one of them
void TLB::addEntry(int pageNumber, int frameNumber){
    /// --------TP3__TO_DO---------
    ///
    ///
}



TLB::TLB() : nextEntryAvailable(0){
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
