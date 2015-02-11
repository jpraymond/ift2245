// IFT2245
// Devoir 1
// 12 fevrier 2015
// Eric Larsen
// Jean-Philippe Raymond

#include <string>
#include <list>

using namespace std;


// Entete de la classe Processus
class Processus {
    private:
        static const int TAILLE_INDENTATION = 2;

	//numero du processus
        int id;
	//nom du processus
        string nom;
	// liste des enfants du processus
        list<Processus*> enfants;

        void imprimer_aux (int);

    public:
        Processus (int, const string&);
        void ajouter_enfant (Processus&);
        void imprimer();
};
