#!bin/bash

# IFT2245
# devoir 1
# 12 fevrier 2015

# par
# Eric Larsen
# Jean-Philippe Raymond


# 1 Saisie des parametres

utilisateur=$1
passe=$2


# 2 Verification de l'existence du nom d'utilisateur

grep "^$utilisateur" /etc/passwd >/dev/null
retour=$? 
#echo $retour


# 3 Controle preliminaire des parametres

if (($retour == 0)) || (($# != 2))
then
	echo "Il faut soumettre dans l'ordre un nom d'utilisateur inexistant et un mot de passe: veuillez recommencer."
	exit 2
fi

echo "fin 3"


# 4 Controle et creation du nom d'utilisateur
 
useradd -m $utilisateur
retour=$?

if (($retour != 0))
then
	echo "Nom d'utilisateur invalide: veuillez recommencer."
	exit 2
fi

echo "fin 4"	


# 5 Controle et creation du mot de passe

echo "$utilisateur:$passe" | chpasswd
retour=$?

if (($retour != 0))
then
	echo "Mot de passe invalide: veuillezrecommencer."
	exit 2
fi

echo "fin 5"


# 6 Creation des repertoires

cd /home/$utilisateur
mkdir tp1

cd tp1
mkdir src
mkdir bin
mkdir result
#ls -l | cat

echo "fin 6"


# 7 changement des proprietes et des droits

chown -R $utilisateur. /home/$utilisateur/tp1
#ls -l | cat

chmod -R 755 /home/$utilisateur/tp1
# avec rapport
#chmod -R -v a+wrx /home/$utilisateur/tp1
#chmod -R -v og-w /home/$utilisateur/tp1
#ls -l | cat

echo "fin 7"


# 8 Modification du chemin d'execution

export PATH=$PATH:/home/$utilisateur/tp1/bin
#echo $PATH
#sudo -u $utilisateur echo $PATH 

echo "fin 8"


# 9 Depot des fichiers

install -o $utilisateur -g $utilisateur /home/os/setup.sh /home/$utilisateur/tp1/src
install -o $utilisateur -g $utilisateur /home/os/processus.h /home/$utilisateur/tp1/src
install -o $utilisateur -g $utilisateur /home/os/processus.cpp /home/$utilisateur/tp1/src
install -o $utilisateur -g $utilisateur /home/os/ptree.cpp /home/$utilisateur/tp1/src

#ls -l /home/$utilisateur/tp1/src | cat

echo "fin 9"


# 10 Production et depot de l'executable

g++ -o /home/$utilisateur/tp1/bin/ptree /home/$utilisateur/tp1/src/processus.cpp /home/$utilisateur/tp1/src/ptree.cpp
 
chmod -R o=r-x /home/$utilisateur/tp1/bin/ptree

echo "fin 10"


# 11 Production et depot des sorties

ptree > /home/$utilisateur/tp1/result/zero
ptree 1 2 > /home/$utilisateur/tp1/result/one.two

echo "fin 11"


# 12 Ajout du fichier d'identification

echo -e "Eric Larsen\nJean-Philippe Raymond" > /home/$utilisateur/auteur

echo "fin 12"






exit 0




