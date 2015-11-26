#include "LabelRosenfeld.hpp"
#include <pthread.h>

/**********************
**        e2         **
**                   **
**    e4   x    ?    **
**                   **
**         ?         **
************************/

//TODO: Deplace dans le hpp

//
 void* labeliseThread(void* td);

 typedef struct thread_data{
    Region32* region;
    LabelRosenfeld* lr;
 }thread_data;

 void* labeliseThreadLauncher(void*);

/* Constructeur par defaut */
LabelRosenfeld::LabelRosenfeld() {
}

LabelRosenfeld::~LabelRosenfeld() {
}

/* Fonctions communes */
uint32_t LabelRosenfeld::FindRoot(uint32_t* T, uint32_t i) {
    uint32_t r;
    r = i;
    while (T[r]<r) {
        r = T[r];
    }
    return r;
}


void LabelRosenfeld::SetRoot(uint32_t* T, uint32_t i, uint32_t r) {
    uint32_t j;

    while (T[i]<i) {
        j = T[i];
        i = j;
    }
    T[i] = r;
}
void LabelRosenfeld::updateTable(uint32_t* T, uint32_t e, uint32_t epsilon) {
    uint32_t m;

    if ((!e) || (e == epsilon)) return;

    while (T[e] != epsilon) {
        m = T[e];
        T[e] = epsilon;
        e = m;
    }
}

uint32_t LabelRosenfeld::ui32MinNonNul2(uint32_t a, uint32_t b) {
    uint32_t m;

    if (a) {
        if (b) {
            if (a<b) m = a;
            else m =b;
        } else {
            m = a;
        }
    } else {
        m = b;
    }
    return m;
}
uint32_t LabelRosenfeld::ui32MinNonNul3(uint32_t a, uint32_t b, uint32_t c) {
    uint32_t m32 = 0xffffffff;
    uint32_t m = m32;

    if (a) m = a;
    if (b && (b<m)) m = b;
    if (c && (c<m)) m = c;

    if (m==m32) m = 0;
    return m;
}
uint32_t LabelRosenfeld::ui32MinNonNul4(uint32_t a0, uint32_t b0, uint32_t a1, uint32_t b1) {
    uint32_t m, m0,m1;

    m0 = ui32MinNonNul2(a0, b0);
    m1 = ui32MinNonNul2(a1, b1);
    m  = ui32MinNonNul2(m0, m1);

    return m;
}

/* Autres fonctions utiles */

uint32_t LabelRosenfeld::solvePackTable(uint32_t* T, uint32_t ne) {
    uint32_t e;
    uint32_t na; // ancetre packe

    na = 0;
    for (e=1; e<=ne; e++) { //On parcourt l'ensemble des labels
        if (e != T[e]) { // Si on a une relation d'équivalence
            T[e] = T[T[e]]; //On va chercher l'ancetre et on le range
        } else {
            T[e] = ++na; //Sinon...
        }
    }
    return na; //Renvoie l'ancetre courant
}


/**
 * Après avoir simplifié la table d'équivalence on remet les étiquettes bien comme il faut
 *
 */
void LabelRosenfeld::updateLabel(uint32_t **E, int i0, int i1, int j0, int j1, uint32_t* T) {
    int i, j;

    //TODO: TWO FOR LOOPS LIKE THAT ??? MAYBE WE SHOULD PARRALELIZZZZZZZ BRO ?
    for (i=i0; i<i1; i++) {
        for (j=j0; j<j1; j++) {
            E[i][j] = T[E[i][j]]; //On va chercher dans la table d'équivalence la bonne valeur de l'étiquette
        }
    }
}

uint32_t LabelRosenfeld::line0Labeling4C(unsigned char **X, int i, uint32_t **E, uint32_t* T, int largeur, uint32_t ne) {
    int j;
    unsigned char x;
    uint32_t e4, r4;

    // Premier point
    x = X[i][0];
    if (x) {
        E[i][0] = ++ne;
    } else {
        E[i][0] = 0;
    }

    for (j=1; j<largeur; j++) {
        x = X[i][j];
        if (x)  {
            e4 = E[i  ][j-1];

            if (e4 == 0) {
                E[i][j] = ++ne;
            } else {
                r4 = FindRoot(T, e4);
                E[i][j] = r4;
            }
        }
    }
    return ne;
}



/**
 * params:
 * i: numéro de la ligne à labeliser
 */
uint32_t LabelRosenfeld::lineLabeling4C(unsigned char **X, int i, uint32_t **E, uint32_t* T, int largeur, uint32_t ne) {
    int j;
    unsigned char x;
    uint32_t epsilon;
    uint32_t e2, e4;
    uint32_t r2, r4;

    // premier point de la ligne
    x = X[i][0];

    if (x) { //Si x n'est pas du fond
        e2 = E[i-1][0]; //On note l'étiquette du point au dessus de x dans e2
         if (e2) { //Si e2 n'est pas du fond
            E[i][0] = e2; //l'etiquette de x devient l'etiquette de e2
        } else { //Si e2 c'est du fond
            E[i][0] = ++ne; //On donne une nouvelle étiquette à x
        }
    }

    //On attaque le reste de la ligne
    for (j=1; j<largeur; j++) {
        x = X[i][j]; //On note la valeur du pixel X
        if (x)  {
            e2 = E[i-1][j  ]; //On récupère l'étiquette au dessus
            e4 = E[i  ][j-1]; // On récupère l'étiquette à gauche

            if ( (e2 == 0) && (e4 == 0) ) { //Si e2 ET e4 sont du fond
                E[i][j] = ++ne; //Nouvelle étiquette pour x !!
            } else {            //SINON
                if (e2 == e4) { // e2 et e4 ont la même étiquette
                    E[i][j] = e2; //Alors x prend leur étiquette
                } else {          //Sinon on est surounded par des étiquettes différentes !
                    r2 = FindRoot(T, e2); //On trouve l'ancetre de e2
                    r4 = FindRoot(T, e4); //On trouve l'ancetre de e4
                    epsilon = ui32MinNonNul2(r2, r4); //Et on choisi le plus petit non nul

                    //On définit epsilon étant l'ancetre de e2 et e4
                    if ((e2) && (e2 != epsilon)) SetRoot(T, e2, epsilon);
                    if ((e4) && (e4 != epsilon)) SetRoot(T, e4, epsilon);

                    //On défini l'étquiette de X
                    E[i][j] = epsilon;
                }
            }
        }
    }
    return ne; //On retourne l'étiquette courante.
}


/**
 * params:
 * i: numéro de la ligne à labeliser
 */
uint32_t LabelRosenfeld::line0Labeling8C(unsigned char **X, int i, uint32_t **E, uint32_t* T, int largeur, uint32_t ne) {
    int j;
    unsigned char x;
    uint32_t e4,e;

    // Premier point
    x = X[i][0];
    if (x) { //SI X non nul i.e PAS DU FOND
            E[i][0] = ++ne; //ON L'ETIQUETTE (dans l'image etiquetté)
    } else {
        E[i][0] = 0; //SINON ON L'ETIQUETTE AVEC ZERO=FOND
    }

    //ON FAIT LE RESTE DE LA LIGNE
    for (j=1; j<largeur; j++) {
        x = X[i][j];
        if (x)  { //Si X non nul i.e Si X n'est PAS LE FOND
            e4 = E[i][j-1];
            if (e4) {
                e = FindRoot(T,e4);
            } else {
                e = ++ne;
            }
            E[i][j] = e;
        }
    }
    return ne;
}


uint32_t LabelRosenfeld::lineLabeling8C(unsigned char **X, int i, uint32_t **E, uint32_t* T, int largeur, uint32_t ne) {
    int j;
    unsigned char x;
    uint32_t epsilon;
    uint32_t e1, e2, e3, e4;
    uint32_t r1, r2, r3, r4;

    // premier point de la ligne
    x = X[i][0];
    if (x) {
        e2 = E[i-1][0];
        e3 = E[i-1][1];

        if ( (e2==0) && (e3==0)) {
            E[i][0] = ++ne;
        } else {
            if (e2==e3) {
                E[i][0] = e2;
            } else {
                r2 = FindRoot(T,e2);
                r3 = FindRoot(T,e3);

                epsilon = ui32MinNonNul2(r2, r3);

                if ((e2) && (e2 != epsilon)) SetRoot(T, e2, epsilon);
                if ((e3) && (e3 != epsilon)) SetRoot(T, e3, epsilon);

                E[i][0] = epsilon;
            }
        }
    }

    for (j=1; j<largeur-1; j++) {
        x = X[i][j];
        if (x)  {
            e1 = E[i-1][j-1];
            e2 = E[i-1][j  ];
            e3 = E[i-1][j+1];
            e4 = E[i  ][j-1];

            // nouvel element
            if ( (e1==0) && (e2==0) && (e3==0) && (e4==0)) {
                E[i][j] = ++ne;
            } else {
                if ((e1 == e2) && (e1 == e3) && (e1 == e4)) {
                    E[i][j] = e1;
                } else {

                    r1 = FindRoot(T,e1);
                    r2 = FindRoot(T,e2);
                    r3 = FindRoot(T,e3);
                    r4 = FindRoot(T,e4);

                    epsilon = ui32MinNonNul4(r1, r2, r3, r4);

                    if ((e1) && (e1 != epsilon)) SetRoot(T, e1, epsilon);
                    if ((e2) && (e2 != epsilon)) SetRoot(T, e2, epsilon);
                    if ((e3) && (e3 != epsilon)) SetRoot(T, e3, epsilon);
                    if ((e4) && (e4 != epsilon)) SetRoot(T, e4, epsilon);

                    E[i][j] = epsilon;
                }
            }
        }
    }

    j = largeur-1;
    x = X[i][j];
    if (x)  {
        e1 = E[i-1][j-1];
        e2 = E[i-1][j  ];
        e3 = 0;
        e4 = E[i  ][j-1];

        // nouvel element
        if ( (e1==0) && (e2==0) && (e3==0) && (e4==0)) {
            E[i][j] = ++ne;

        } else {
            if ((e1 == e2) && (e1 == e3) && (e1 == e4)) {
                E[i][j] = e1;
            } else {

                r1 = FindRoot(T,e1);
                r2 = FindRoot(T,e2);
                r3 = FindRoot(T,e3);
                r4 = FindRoot(T,e4);

                epsilon = ui32MinNonNul4(r1, r2, r3, r4);

                if ((e1) && (e1 != epsilon)) SetRoot(T, e1, epsilon);
                if ((e2) && (e2 != epsilon)) SetRoot(T, e2, epsilon);
                if ((e3) && (e3 != epsilon)) SetRoot(T, e3, epsilon);
                if ((e4) && (e4 != epsilon)) SetRoot(T, e4, epsilon);

                E[i][j] = epsilon;
            }
        }
    }
    return ne;
}



/* Labelise en sequentiel */
void LabelRosenfeld::labeliseSequetiel4C(Region32& region32) {

    /* Declaration des variables */
    int i;
    uint32_t ne;

    int i0 			= 	region32.i0;
    int i1 			= 	region32.i1;
    int j0 			= 	region32.j0;
    int j1 			= 	region32.j1;
    int largeur 	= 	j1-j0;

    /* Netoyage des pr�c�dents traitements */
    region32.cleanRegions32();

    /* Premier etiquetage */
    ne = 0;
    //On labelise la première ligne
    ne = line0Labeling4C(region32.X, i0, region32.E, region32.T, largeur, ne);

    //On labelise toutes les autres lignes
    for (i=i0+1; i<i1; i++) {
        ne = lineLabeling4C(region32.X, i, region32.E, region32.T, largeur, ne);
    }

    /* Résolution des équivalences */
    region32.neFinal = solvePackTable(region32.T, ne); //On recupère l'ancetre courant

    /* Mise à jour sur l'image */
    updateLabel(region32.E, i0, i1, j0, j1, region32.T);

    /* Mémorisation du nombre d'étiquettes */
    region32.ne = ne;
}
void LabelRosenfeld::labeliseSequetiel8C(Region32& region32) {

    /* Declaration des variables */
    int i;
    uint32_t ne;

    int i0 			= 	region32.i0;
    int i1 			= 	region32.i1;
    int j0 			= 	region32.j0;
    int j1 			= 	region32.j1;
    int largeur 	= 	j1-j0;

    /* Nettoyage des précédents traitements */
    region32.cleanRegions32();

    /* ETAPE 1
     * Labelisation
     */
    ne = 0;

    ne = line0Labeling8C(region32.X, i0, region32.E, region32.T, largeur, ne);
    for (i=i0+1; i<i1; i++) {
        ne = lineLabeling8C(region32.X, i, region32.E, region32.T, largeur, ne);
    }

    /* ETAPE 2
     * Résolution des équivalences
     */
    region32.neFinal = solvePackTable(region32.T, ne);

    /* ETAPE 3
     * Mise à jour sur l'image
     */
    updateLabel(region32.E, i0, i1, j0, j1, region32.T);

    /* Mémorisation du nombre d'étiquettes */
    region32.ne = ne;
}




/**
 * Labelise en parallele
 * authors: Leignier - Pasquereau - Vercueil
 * date: nov. 13 2015
 * TODO: WRITE THE CODE !!!!
 */
void LabelRosenfeld::labeliseParallele4C(Region32& region32) {

  /* Declaration des variables */
  int i;
  uint32_t ne;

  int i0 			= 	region32.i0;
  int i1 			= 	region32.i1;
  int j0 			= 	region32.j0;
  int j1 			= 	region32.j1;
  int largeur 	= 	j1-j0;
  int hauteur 	= 	i1-i0;

  /* Netoyage des précédents traitements */
  region32.cleanRegions32();

  // On va lancer un thread par région
  int NbrThreads = region32.Regions.size();

  //Initialisation du tableau de thread_data
  thread_data* threadDataArray = (thread_data*) malloc(NbrThreads*sizeof(thread_data));

  //Initialisation du tableau de threads
  pthread_t* threads = (pthread_t*) malloc(NbrThreads*sizeof(pthread_t));

  //Launching...
  int rc; //Return code
  int thr_counter;
  LabelRosenfeld* lr_array = (LabelRosenfeld*) malloc(NbrThreads*sizeof(LabelRosenfeld));

  for ( thr_counter = 0; thr_counter < NbrThreads; thr_counter++) {

      threadDataArray[thr_counter].region = &region32.Regions.at(thr_counter);
      threadDataArray[thr_counter].lr = &lr_array[thr_counter];

      //Go! Go! Go!
      rc = pthread_create(&threads[thr_counter], NULL, labeliseThreadLauncher,
        (void *) &threadDataArray[thr_counter]);
        if(rc) cout << "Problem launching threads" << endl;

  }


  //Wait for the threads to be done
  for ( thr_counter = 0; thr_counter < NbrThreads; thr_counter++) {
    pthread_join(threads[thr_counter], NULL);
  }

  //Il faut maintenant résoudre les problèmes de connexité
  //aux jointures entre les régions de threads
  //C'est un solvePackTable + updateLabel global

//Création d'une nouvelle table d'équivalence globale




uint32_t global_size = 0;
for ( thr_counter = 0; thr_counter < NbrThreads; thr_counter++) {
  global_size += region32.Regions.at(thr_counter).ne;

}

int counter;
int partialcounter = 0;
int currentThread = 0;
int offset = 0; //Decalage
for ( counter = 0; counter < global_size; counter++) {


  if(counter == offset + region32.Regions.at(currentThread).ne){
    offset += region32.Regions.at(currentThread).ne;
    currentThread++;
    partialcounter = 0;
    cout <<  "Changing...  " << endl;
    cout << endl << "*****" << currentThread << "*****" << endl;
  }

  region32.T[counter] = offset + region32.Regions.at(currentThread).T[partialcounter];
  partialcounter++;
  cout << counter << " -> " << region32.T[counter] << endl;





}


solvePackTable(region32.T, global_size);

for ( counter = 0; counter < global_size; counter++) {
  cout << counter << " -> " << region32.T[counter] << endl;

}


  cout << "******************** DEBUG *********" << endl;
//Creation d'un nouveau tableau de label global
int row, col;
 currentThread = 0;
 offset = 0;
for (row = 0; row < hauteur; row++) {

  for(col = 0; col < largeur; col++){

    if( region32.E[row][col] == offset + region32.Regions.at(currentThread).ne ){
      offset += region32.Regions.at(currentThread).ne;
      currentThread++;
    }

    region32.E[row][col] = region32.E[row][col] == 0 ? 0 : offset +region32.E[row][col];
    //  cout << region32.E[row][col] << " ";
  }

    cout << endl << "*****" << currentThread << "*****" << endl;
}



updateLabel(region32.E, i0, i1, j0, j1, region32.T);

}

/*************************
 * Labelise en parallele
 *
 */
void LabelRosenfeld::labeliseParallele8C(Region32& region32) {

}


void LabelRosenfeld::labeliseThreadFunction(void* md){

  thread_data* mydata =(thread_data*) md;
  Region32* myregion = mydata->region;

  myregion->ne = 0;
  int largeur 	= 	myregion->j1 - myregion->j0;
  //On labelise la première ligne
  myregion->ne = line0Labeling4C(myregion->X, myregion->i0, myregion->E, myregion->T, largeur, myregion->ne);

  //On labelise toutes les autres lignes
  int i;
  for (i=myregion->i0+1; i<myregion->i1; i++) {
      myregion->ne = lineLabeling4C(myregion->X, i, myregion->E, myregion->T, largeur, myregion->ne);
  }

  //Resolution de la table d'équivalence partielle
  //myregion->neFinal = solvePackTable(myregion->T, myregion->ne);

  //Mise à jour de la table d'étiquettes partielle
  //updateLabel(myregion->E, myregion->i0, myregion->i1, myregion->j0, myregion->j1, myregion->T);




}

void* labeliseThreadLauncher(void* td){


  //Recupération des datas
  thread_data* mydata = (thread_data*) td;

  ((LabelRosenfeld *)mydata->lr)->labeliseThreadFunction( td );


}
