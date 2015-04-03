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
public:
    TLB_entry TLBTable[TLB_NUM_ENTRIES];

    TLB();
    ~TLB();
    // Looks for the pageNumber in the table, if it finds it it return
    // the coresponding frame if not it returns -1;
    int findPage(int pageNumber);

    //Adds an entry-pair (pageNumber,frameNumber) to the TLB
    //Here you have to implement a replacement technique for
    //when the TLB is full. DONT FORGET to implement two different
    //function for this, even if you only call one of them
    void addEntry(int pageNumber, int frameNumber);


    int nextEntryAvailable;

    //----------DO NOT CHANGE/ERASE THIS------------------
    // this function is neccesary to output the physical
    // memory state at the end of execution. It needs to
    // be generated for evaluation purposes.
    void printTLB();
    //----------------------------------------------------
};

#endif // TLB_H
