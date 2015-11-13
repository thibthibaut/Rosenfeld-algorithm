#include "Region32.hpp"

/* Constructeur par defaut */
Region32::Region32() {
    this->X								=	NULL;
    this->E								=	NULL;

    this->i0							=	0;
    this->i1							=	0;
    this->j0							=	0;
    this->j1							=	0;

    this->ne							=	0;
	this->neFinal						=	0;

    this->p								=	0;
    this->np							=	0;

    this->initialiseTables(NEMAX);
}

Region32::Region32(unsigned char **X, uint32_t **E, int i0, int i1, int j0, int j1, int nbThreads){

    this->X								=	X;
    this->E								=	E;

    this->i0							=	i0;
    this->i1							=	i1;
    this->j0							=	j0;
    this->j1							=	j1;

    this->initialiseTables(NEMAX);

    this->ne							=	0;
	this->neFinal						=	0;

    this->p								=	0;
    this->np							=	nbThreads;

    /* Pour la gestion des bandes parallèles */
    if (nbThreads >= 1) {
        int h = i1-i0; 													// Hauteur de l'image
        int p, i0p = 0, i1p = 0, hp; 										// Identificateurs des debut/fin de chaque partie et hauteur de chaque partie
        Region32* temp;

        /* Calcul de la hauteur de chaque bande */
        hp = h / nbThreads;
        if (h % np != 0) hp++;

        for (p=0; p<nbThreads; p++) {
            if (p== 0) {
                i0p = i0;
            } else {
                i0p = i0p + hp;
            }
            if (p==nbThreads-1) {
                i1p = i1;
            } else {
                i1p = i0p + hp;
            }

            temp 						=	new Region32(X,E,i0p,i1p,j0,j1,0);	// Creation de la region en lui indiquant qu'elle est seul (nbThreads = 0)
            this->Regions.push_back(*temp);										// Ajout de cette région à la liste des régions du maître
            this->Regions[p].p			=	p;									// On indique à la sous-région son numéro
        }
    }
}

/* Destructeur par defaut */
Region32::~Region32() {
    this->i0							=	0;
    this->i1							=	0;
    this->j0							=	0;
    this->j1							=	0;

    this->ne							=	0;
	this->neFinal						=	0;

    this->Regions.clear();

    this->p								=	0;
    this->np							=	0;
}

void Region32::cleanRegions32(){
		int p;

		this->initialiseTables(this->ne+1);

		this->ne							=	0;
		this->neFinal						=	0;

		for (p=0; p<this->np; p++) {
			this->Regions[p].cleanRegions32();
		}
}

void Region32::initialiseTables(uint32_t nb) {
    uint32_t i;
    for (i=0; i<nb; i++){
        this->T[i] = i;
    }
}
