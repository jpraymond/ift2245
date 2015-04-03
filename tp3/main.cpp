#include "virtualmemorymanager.h"

int main()
{
    ifstream commands("../VirtualMemoryManager/addresses.txt",ios::in);

    VirtualMemoryManager vm;

    int logicAddres;
    do{
        commands >> logicAddres;
        vm.addCommandFromLogicAddress(logicAddres);
    }while(commands.good());

    vm.applyCommands();
    vm.printResults();

    commands.close();
    return 0;
}

