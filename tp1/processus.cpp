// IFT2245
// Devoir 1
// 12 fevrier 2015
// Eric Larsen
// Jean-Philippe Raymond

#include "processus.h"
#include <iostream>

using namespace std;

// Corps de la classe Processus

// constructeur
Processus::Processus (int id, const string &nom) {
    this->id = id;
    this->nom = nom;
}

// ajout d'un enfant a la liste des enfants du processus
void Processus::ajouter_enfant (Processus &enfant) {
    enfants.push_back (&enfant);
}

// impression formatee de l'arbre du processus
void Processus::imprimer_aux (int niveau_indentation) {
    string indentation (niveau_indentation * TAILLE_INDENTATION, ' ');
    cout << indentation << id << " " << nom << endl;
   
    typedef list<Processus*>::const_iterator ListIterator;
    for (ListIterator it = enfants.begin(); it != enfants.end(); it++) {
        (*it)->imprimer_aux (niveau_indentation + 1);
    }
}

// impression lorsque le processus est au sommet de l'arborescence
void Processus::imprimer () {
    imprimer_aux (0);
}


/*
int main () {
    Processus p (0, "Process 0");
    
    Processus p1 (1, "Process 1");
    p.ajouter_enfant (p1);
    
    Processus p2 (2, "Process 2");
    p.ajouter_enfant (p2); 
    Processus p21 (21, "Process 21");
    p2.ajouter_enfant (p21);
    Processus p22 (22, "Process 22");
    p2.ajouter_enfant (p22);

    Processus p3 (3, "Process 3");
    p.ajouter_enfant (p3);
    
    p.imprimer();
    
    return 0;
}
*/
