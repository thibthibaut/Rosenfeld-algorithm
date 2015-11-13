#ifndef _REGION32_H_
#define _REGION32_H_

#include <stdint.h>
#include <vector>
#include <iostream>
using namespace std;

#define NEMAX 200000    // Nombre max d'étiquettes gérées

class  Region32 {
private :

public :

    // Table d'equivalence
    uint32_t T[NEMAX];

    unsigned char** X;							// image source
    uint32_t** E;								// image d'etiquette 32 bit

    int i0, i1, j0, j1;							// intervalle de valeur RoI

    uint32_t   ne;         						// nb d'etiquettes courante
	uint32_t   neFinal;							// nb d'etiquettes finale

    /* Gestion des bandes paralleles */
    vector<Region32> Regions; 					        // Vecteur de regions la liste des sous-regions
    int p, np;         							// numero du processeur et nb total de processeurs

    /* Fonctions */
    /* Constructeur par defaut */
    Region32();

	/* Constructeur particulier */
	Region32(unsigned char **X, uint32_t **E, int i0, int i1, int j0, int j1, int nbThreads);

    /* Destructeur par defaut */
    ~Region32();

    /* Nettoyage des regions : supprime les equivalences, les efface la table E, efface la valeur de ne */
    void cleanRegions32();

    void initialiseTables(uint32_t nb);
};

#endif // _REGION32_H_
