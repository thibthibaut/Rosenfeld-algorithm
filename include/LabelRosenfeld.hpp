#ifndef _LABEL_ROSENFELD_H_
#define _LABEL_ROSENFELD_H_

#include "Region32.hpp"

#include <iostream>
#include <stdint.h>
#include <vector>
using namespace std;



class LabelRosenfeld {
protected :

    /* Fonctions communes */
    uint32_t FindRoot(uint32_t* T, uint32_t i);
    void SetRoot(uint32_t* T, uint32_t i, uint32_t r);
    void updateTable(uint32_t* T, uint32_t e, uint32_t epsillon);

    uint32_t solvePackTable(uint32_t* T, uint32_t ne);
    void updateLabel(uint32_t **E, int i0, int i1, int j0, int j1, uint32_t* T);

    /* Autres fonctions utiles */
    uint32_t ui32MinNonNul2(uint32_t a, uint32_t b);
    uint32_t ui32MinNonNul3(uint32_t a, uint32_t b, uint32_t c);
    uint32_t ui32MinNonNul4(uint32_t a0, uint32_t b0, uint32_t a1, uint32_t b1);

    /* Fonctions d'etiquetage */
    uint32_t line0Labeling4C(unsigned char **X, int i, uint32_t **E, uint32_t* T, int largeur, uint32_t ne);
    uint32_t lineLabeling4C(unsigned char **X, int i, uint32_t **E, uint32_t* T, int largeur, uint32_t ne);
    uint32_t line0Labeling8C(unsigned char **X, int i, uint32_t **E, uint32_t* T, int largeur, uint32_t ne);
    uint32_t lineLabeling8C(unsigned char **X, int i, uint32_t **E, uint32_t* T, int largeur, uint32_t ne);

public :

    /* Fonctions */
    /* Constructeur par defaut */
    LabelRosenfeld();

    /* Destructeur par defaut */
    ~LabelRosenfeld();

    /* Labelise en sequentiel*/
    void labeliseSequetiel4C(Region32& region32);
    void labeliseSequetiel8C(Region32& region32);

    /* Labelise en parall�le*/
    void labeliseParallele4C(Region32& region32);
    void labeliseParallele8C(Region32& region32);

    /* Fonctions de thread */
   void labeliseThreadFunction(Region32, void*);

};

#endif // _LABEL_ROSENFELD_H_
