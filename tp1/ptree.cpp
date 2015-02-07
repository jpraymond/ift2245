// TODO: Tester.
// TODO: Tester sur VirtualBox.
// TODO: Vérifier la gestion de la mémoire.

/* File system */
#include <dirent.h>

/* Streams */
#include <iostream>  /* IO stream */
#include <fstream>   /* File stream */
#include <sstream>   /* String stream */

/* Data structures */
#include <string>
#include <map>

#include <stdlib.h>

#include "processus.h"


using namespace std;


const string DOSSIER_PROCESSUS = "/proc/";
const string FICHIER_STAT = "stat";
const int ID_PROCESSUS_RACINE = 0;
const string NOM_PROCESSUS_RACINE = "[SCHED]";


string i_to_str (int n) {
    stringstream ss;
    ss << n;
    return ss.str();
}

string n_ieme_token (istringstream &iss, int n) {
    string token;
    for (int i = 0; i < n; i++) {
        iss >> token;
    }
    return token;
}

void ajouter_processus (int id, map<int, Processus*> &les_processus, Processus *enfant) {
    map<int, Processus*>::iterator it = les_processus.find (id);
    if (it == les_processus.end()) { // Le processus n'a pas déjà été traité.
        string chemin = DOSSIER_PROCESSUS + i_to_str (id) + "/" + FICHIER_STAT;
        ifstream fichier (chemin.c_str());
        if (fichier.good()) {
            string ligne;
            getline (fichier, ligne);
            fichier.close();

            istringstream iss (ligne);
            
            string nom = n_ieme_token (iss, 2);
            nom = nom.substr (1, nom.length() - 2);
            
            int id_parent = atoi (n_ieme_token (iss, 2).c_str());

            Processus *p = new Processus (id, nom);
            if (enfant != NULL) {
                p->ajouter_enfant (*enfant);
            }
            les_processus[id] = p;

            if (id_parent == ID_PROCESSUS_RACINE) {
                les_processus[ID_PROCESSUS_RACINE]->ajouter_enfant (*p);
            }
            else {
                ajouter_processus (id_parent, les_processus, p);
            }
        }
        else { // Ne devrait pas survenir.
            fichier.close();
        }
    }
    else { // Le processus a déjà été traité.
        if (enfant != NULL) {
            (*it->second).ajouter_enfant (*enfant);
        }
    }
}

void ajouter_processus (int id, map<int, Processus*> &les_processus) {
    ajouter_processus (id, les_processus, NULL);
}


int main(int argc, char *argv[]) {
    map<int, Processus*> les_processus;
    
    Processus p (ID_PROCESSUS_RACINE, NOM_PROCESSUS_RACINE);
    les_processus[ID_PROCESSUS_RACINE] = &p;
    
    DIR *dossier_processus = opendir(DOSSIER_PROCESSUS.c_str());
    
    struct dirent * d;
    while ((d = readdir(dossier_processus)) != NULL) {
        int id = atoi(d->d_name); // Retourne 0 si l'argument n'est pas un entier.
        if (id != 0) {
            ajouter_processus (id, les_processus);
        }
    }

    closedir(dossier_processus);

    // TODO: Supporter une liste de PIDs. Ignorer les PIDs invalides.
    int id = ID_PROCESSUS_RACINE;
    if (argc != 1) {
        id = atoi (argv[1]);
    }    
    les_processus[id]->imprimer();
}
