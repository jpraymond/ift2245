#include "virtualmemorymanager.h"

int main()
{
  // ouverture du fichier d'adresses de lecture
  ifstream commands("addresses.txt",ios::in);

  // instanciation du gestionnaire de lecture et notamment:
  // memoire physique, table de pagination et cache de table de pagination
  VirtualMemoryManager vm;

  // constitution de la liste d'adresses de lecture
  int logicAddres;
  do{
    commands >> logicAddres;
    vm.addCommandFromLogicAddress(logicAddres);
  }while(commands.good());

  // traitement de la liste d'adresses
  vm.applyCommands();

  // impression des resultats comptables
  vm.printResults();

  // fermeture du fichier d'adresses
  commands.close();
  return 0;
}

