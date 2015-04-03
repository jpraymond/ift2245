#ifndef PHYSICALMEMORY_H
#define PHYSICALMEMORY_H

#include "common.h"

class PhysicalMemory
{
public:
    char physicalMemoryData[PHYSICAL_MEMORY_SIZE];
    ifstream backingStoreFile;

    PhysicalMemory();
    ~PhysicalMemory();

    //Looks for a free frame to store the page read from the BACKING_STORE file
    int findFreeFrame();

    //Reads a certain page (256 bytes) from the BACKING_STORE, starting at page 'pageNumber'
    int demandPageFromBackingStore(unsigned int pageNumber);

    //Given the frameNumber and the offset get the char value at that physical address
    char getValueFromFrameAndOffset(unsigned int frameNumber, unsigned int offset);

    //----------DO NOT CHANGE/ERASE THIS------------------
    // this function is neccesary to output the physical
    // memory state at the end of execution. It needs to
    // be generated for evaluation purposes.
    void printPhysicalMemory();
    //----------------------------------------------------
};

#endif // PHYSICALMEMORY_H
