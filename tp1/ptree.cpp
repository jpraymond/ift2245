// IFT2245
// Devoir 1
// 12 fevrier 2015
// Eric Larsen
// Jean-Philippe Raymond

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

// entete de la classe processus
#include "processus.h"

// usage de l'espace de nom std
using namespace std;


// Entree du programme


// definition des constantes
const string DOSSIER_PROCESSUS = "/proc/";
const string FICHIER_STAT = "stat";
const int ID_PROCESSUS_RACINE = 0;
const string NOM_PROCESSUS_RACINE = "[SCHED]";


// methodes auxiliaires

// retourne un entier sous forme de chaine
string i_to_str (int n) {
    stringstream ss;
    ss << n;
    return ss.str();
}


// extrait la nieme unite lexicale d'un flux de chaines
string n_ieme_token (istringstream &iss, int n) {
    string token;
    for (int i = 0; i < n; i++) {
        iss >> token;
    }
    return token;
}


/* methode principale
Met a jour le dictionnaire liant les numeros de processus et les processus enfants: cas general
id: numero du processus
les_processus: reference vers le dictionnaire  
enfant: pointeur vers un enfant supplementaire
 */
void ajouter_processus (int id, map<int, Processus*> &les_processus, Processus *enfant) {

    // recherche le numero de processus parmi les clefs existantes
    map<int, Processus*>::iterator it = les_processus.find (id);

    // le processus n'a pas deja ete traite
    if (it == les_processus.end()) {

        //definit le chemin d'acces au fichier symbolique
        string chemin = DOSSIER_PROCESSUS + i_to_str (id) + "/" + FICHIER_STAT;

	// definit le flux d'acces au fichier 
        ifstream fichier (chemin.c_str());

	// le flux est disponible
        if (fichier.good()) {
	    // obtient la ligne 
            string ligne;
            getline (fichier, ligne);
            fichier.close();
	    
	    // definit un flux de chaines a partir de la ligne obtenue
            istringstream iss (ligne);
            
	    // extrait le nom du processus
            string nom = n_ieme_token (iss, 2);
            nom = nom.substr (1, nom.length() - 2);

	    // extrait et convertit en entier le numero du processus parent            
            int id_parent = atoi (n_ieme_token (iss, 2).c_str());

	    // definit un nouveau processus pour le numero et nom obtenus
            Processus *p = new Processus (id, nom);
	    
	    // ajoute l'enfant au processus
            if (enfant != NULL) {
                p->ajouter_enfant (*enfant);
            }

	    // ajoute le processus au dictionnaire
            les_processus[id] = p;

	    // recursion
	    // cas racine
            if (id_parent == ID_PROCESSUS_RACINE) {
                les_processus[ID_PROCESSUS_RACINE]->ajouter_enfant (*p);
            }

	    // cas general
            else {
                ajouter_processus (id_parent, les_processus, p);
            }
        }
	// Ne devrait pas survenir.
        else { 
            fichier.close();
        }
    }
    // Le processus a deja ete traite.
    else { 
        if (enfant != NULL) {
            it->second->ajouter_enfant (*enfant);
        }
    }
}

// Met a jour le dictionnaire liant les numeros de processus et les processus enfants: visite initiale
void ajouter_processus (int id, map<int, Processus*> &les_processus) {
    ajouter_processus (id, les_processus, NULL);
}


int main(int argc, char *argv[]) {

    // declare le dictionnaire liant les numeros des processus et les listes de leurs enfants
    map<int, Processus*> les_processus;
    
    // construit le processus racine
    Processus *p = new Processus (ID_PROCESSUS_RACINE, NOM_PROCESSUS_RACINE);

    // le depose dans le dictionnaire
    les_processus[ID_PROCESSUS_RACINE] = p;
    
    // met en place l'appareillage de visite 
    DIR *dossier_processus = opendir (DOSSIER_PROCESSUS.c_str());
    
    // declare la structure typique
    struct dirent * d;

    // itere sur les structures presentes
    while ((d = readdir (dossier_processus)) != NULL) {

        // tente de convertir le champ indiquant le numero du processus en entier
        int id = atoi(d->d_name); // Retourne 0 si l'argument n'est pas un entier.
        if (id != 0) {
            ajouter_processus (id, les_processus);
        }
    }

    // fermeture de l'appareillage de visite
    closedir (dossier_processus);    
   
    // aucun argument n'est fourni en parametre: l'arbre est genere a partir de la racine 
    if (argc == 1) {
      	les_processus[ID_PROCESSUS_RACINE]->imprimer();
    }

    // autrement, les arbres des processus existants fournis en parametre sont generes dans l'ordre
    else{
        for (int i=1; i < argc; i++) {

            // conversion de la chaine fournie en parametre identifiant le processus
            int id = atoi (argv[i]);
            if (les_processus[id] != NULL) {
                 les_processus[id]->imprimer();
            }
        }
    }

    /* Toute la mémoire est normalement libérée à la fin de l'exécution du
     * programme, même celle allouée avec 'new'. On la libère tout de même
     * explicitement juste au cas. */
   map<int, Processus*>::iterator it;
   for (it = les_processus.begin(); it != les_processus.end(); it++) {
       delete it->second;
   }
}
