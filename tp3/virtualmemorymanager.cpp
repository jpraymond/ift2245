#include "virtualmemorymanager.h"

// Extract the page number and the offset from the logical
// address according to the TP3, then save it in the
// corresponding 'pageNumber' and 'offset' varibles
Command::Command(unsigned int logicAddress): logicalAdd(logicAddress){
    /// --------TP3__TO_DO---------
    ///
    ///
   offset = logicAddress & 0xFF; // least-significant byte.
   pageNumber = (logicAddress & 0xFFFF) >> 8; // 2nd least-significant byte.
}

// After reading all commands and storing them in the commandList
// traverse the list to apply each command.
void VirtualMemoryManager::applyCommands(){
    for (list<Command>::iterator it = commandList.begin(); it != commandList.end(); it++){
        Command c = *it;
        signed char val = '\0';

        /// --------TP3__TO_DO---------
        ///
        ///
        int physicalAddress = 0;


        cout << "Original Addr: " << setw(5) << c.logicalAdd
             << "\tPage: " << setw(3) << c.pageNumber
             << "\tOffset: " << setw(3) << c.offset
             << "\tPhysical Addr: " << setw(5) << physicalAddress
             << "\tValue: " << val << endl;

    }
}

void VirtualMemoryManager::printResults(){
    cout << endl;
    cout << "Page-Faults: " << pageFaultCount << "\tPage founds: " << pageFoundCount << endl;
    cout << "TLB-Hit: " << TLBHitCount << "\tTLB-miss: " << TLBMissCount << endl;
    cout << "Page-Fault rate: " << pageFaultCount/float(pageFaultCount+pageFoundCount) << endl;
    cout << "TLB-Hit rate: " << TLBHitCount/float(TLBHitCount+TLBMissCount) << endl;

    //----------DO NOT CHANGE/ERASE THIS------------------
    physicalMemory.printPhysicalMemory();
    tlb.printTLB();
    //-----------------------------------------------------
}

VirtualMemoryManager::VirtualMemoryManager() :
    pageFaultCount(0), pageFoundCount(0){
}

void VirtualMemoryManager::addCommandFromLogicAddress(unsigned int logicAddres){
    commandList.push_back(Command(logicAddres));
}
