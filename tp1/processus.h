#include <string>
#include <list>


using namespace std;


class Processus {
    private:
        static const int TAILLE_INDENTATION = 2;

        int id;
        string nom;
        list<Processus*> enfants;

        void imprimer_aux (int);

    public:
        Processus (int, const string&);
        void ajouter_enfant (Processus&);
        void imprimer();
};
