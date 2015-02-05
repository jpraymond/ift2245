// TODO: By reference, by value, ...?
// TODO: Tester sur VirtualBox.

#include <iostream>
#include <string>
#include <list>


using namespace std;


class Processus{
    static const int TAILLE_INDENTATION = 2;

    int id;
    string nom;
    list<Processus> enfants;

    void imprimer_aux (int niveau_indentation);

    public:
        Processus (int, string);
        void ajouter_enfant (Processus);
        void imprimer ();
};

Processus::Processus (int id, string nom) {
    this->id = id;
    this->nom = nom;
}

void Processus::ajouter_enfant (Processus enfant) {
    enfants.push_back (enfant);
}

void Processus::imprimer_aux (int niveau_indentation) {
    string indentation (niveau_indentation * TAILLE_INDENTATION, ' ');
    cout << indentation << id << " " << nom << endl;
    for (list<Processus>::iterator p = enfants.begin(); p != enfants.end(); ++p) {
        p->imprimer_aux (niveau_indentation + 1);
    }
}

void Processus::imprimer () {
    imprimer_aux (0);
}


int main () {
    Processus p (0, "Process 0");
    
    p.ajouter_enfant ( Processus (1, "Process 1"));
    
    Processus p2 (2, "Process 2");
    p2.ajouter_enfant ( Processus (21, "Process 21"));
    p2.ajouter_enfant ( Processus (22, "Process 22"));

    p.ajouter_enfant(p2);

    p.ajouter_enfant( Processus (3, "Process 3"));
    
    p.imprimer();
    
    return 0;
}
