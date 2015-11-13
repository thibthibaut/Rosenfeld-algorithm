#include "main.hpp"

void main_bench_Image(int nbThreads,int run,int iter,int connexite,double freq,char* filename) {

    /* Declaration des Variables */
    int r,k; 					// pour le BENCH
    double t0, t1, dt, tmin;	// pour le BENCH

    double thSeq_Rosenfeld, thPar_Rosenfeld;

    int runTemp;
    int largeurAff = 10;

    /* Chargement de l'image */
    Region32* 	region32 	= 	NULL;
    Region32* 	region32Reference 	= 	NULL;

    ImageEtiquette imageEtiquette;
    imageEtiquette.loadImageSource(filename);
    int sizeH = imageEtiquette.height;
    int sizeW = imageEtiquette.width;

    /* Classe repr�sentant l'algorithme de traitement d'image */
    LabelRosenfeld* rosenfeld = new LabelRosenfeld();

    cout << "*************************************************************************" << endl;
    cout << "--> Image : " << filename << ", taille : " << sizeH << "x" << sizeW << endl;
    cout << "--> Connexite : " << connexite << endl;
    cout << "--> nbThreads : " << nbThreads << endl;

    cout << "\n" << setw(largeurAff) << "NumIter";
    cout << "\t" << setw(largeurAff) << "Seq_Rosenfeld_Sim" << "\t" << setw(largeurAff) << "Parallel_Rosenfeld" << "\t\n";

    for (runTemp = 0; runTemp<run; runTemp++) {
        /* Version de r�f�rence */
        region32Reference = new Region32(imageEtiquette.imageSource, imageEtiquette.imageEtiquetteReference, 0, imageEtiquette.height, 0, imageEtiquette.width, 0);
        if (connexite == 4) rosenfeld->labeliseSequetiel4C(*region32Reference);
        else rosenfeld->labeliseSequetiel8C(*region32Reference);
        imageEtiquette.saveImageEtiquetteReference("Image_Reference.bmp");

        /* Partie sequentielle */
        region32 = new Region32(imageEtiquette.imageSource, imageEtiquette.imageEtiquette, 0, imageEtiquette.height, 0, imageEtiquette.width, 0);
        if (connexite == 4) {
            BENCH(rosenfeld->labeliseSequetiel4C(*region32), thSeq_Rosenfeld);
            if (imageEtiquette.compareImagesEtiquette("rosenfeld - Sequentiel")){
                imageEtiquette.saveImageEtiquette("Image_Error_Sequentiel.bmp");
                imageEtiquette.saveImageDifference("Image_Error_Sequentiel_Difference.bmp");
            }
        } else {
            BENCH(rosenfeld->labeliseSequetiel8C(*region32), thSeq_Rosenfeld);
            if (imageEtiquette.compareImagesEtiquette("rosenfeld - Sequentiel")){
                imageEtiquette.saveImageEtiquette("Image_Error_Sequentiel.bmp");
                imageEtiquette.saveImageDifference("Image_Error_Sequentiel_Difference.bmp");
            }
        }
        delete region32;
        imageEtiquette.saveImageEtiquette("Image_Etiquette_Sequentiel.bmp");
        imageEtiquette.cleanImageEtiquette();

        /* Partie parallele */
        region32 = new Region32(imageEtiquette.imageSource, imageEtiquette.imageEtiquette, 0, imageEtiquette.height, 0, imageEtiquette.width, nbThreads);

        if (connexite == 4) {
            BENCH(rosenfeld->labeliseParallele4C(*region32), thPar_Rosenfeld);
            if (imageEtiquette.compareImagesEtiquette("rosenfeld - Parallele")){
                imageEtiquette.saveImageEtiquette("Image_Error_Parallele.bmp");
                imageEtiquette.saveImageDifference("Image_Error_Parallele_Difference.bmp");
            }
        } else {
            BENCH(rosenfeld->labeliseParallele8C(*region32), thPar_Rosenfeld);
            if (imageEtiquette.compareImagesEtiquette("rosenfeld - Parallele")){
                imageEtiquette.saveImageEtiquette("Image_Error_Parallele.bmp");
                imageEtiquette.saveImageDifference("Image_Error_Parallele_Difference.bmp");
            }
        }
        delete region32;
        imageEtiquette.saveImageEtiquette("Image_Etiquette_Parallele.bmp");
        imageEtiquette.cleanImageEtiquette();

        delete region32Reference;

        /* Resultats */
        cout.precision(2);
        cout << setw(largeurAff) << fixed << runTemp;
        cout << "\t" << setw(largeurAff) << thSeq_Rosenfeld << "\t\t" << setw(largeurAff) << thPar_Rosenfeld  << "\t\n";
    }
    cout << "\n";
}


int main(int argc, const char * argv[]) {

    int nbThreads = NB_THREADS;
    int nbThreads_Fin = NB_THREADS;
    int run = RUN;
    int iter = ITER;
    int connexite = CONNEXITE;
    double freq = FREQ;
    char filename[50];
    strcpy(filename,"Images/boulons_512.pgm");
    bool lancer	=	true;

    int i;

    for (i=0; i<argc-1; i++) {
        switch (argv[1+i][0]) {
        case 'T': // nbThreads
            nbThreads = atoi(argv[1+i+1]);
            i++;
            break;
        case 'R': // nbRun
            run = atoi(argv[1+i+1]);
            i++;
            break;
        case 'I': // nbIter
            iter = atoi(argv[1+i+1]);
            i++;
            break;
        case 'C': // size_min
            connexite = atoi(argv[1+i+1]);
            if (connexite!=4 && connexite!=8) {
                connexite = CONNEXITE;
                cout << "Erreur : la connexite ne peut etre que 4 ou 8\nLa valeur par defaut (" << connexite << ") a ete utilise\n";
            }
            i++;
            break;
        case 'F': // Fi : FileName, Fr : Frequence, F : nbThread_Fin
            if (argv[1+i][1]=='r') freq = (double)atoi(argv[1+i+1])*1000000;
            else if (argv[1+i][1]=='i') strcpy(filename,(char*)argv[1+i+1]);
            else nbThreads_Fin = atoi(argv[1+i+1]);
            i++;
            break;
        case 'H' : // Help
            cout << "****************************************************" << endl;
            cout << "Les parametres par defaut sont : \n";
            cout << "\t" << nbThreads << " Threads a " << nbThreads_Fin << " Threads\n";
            cout << "\t" << run << " run(s)\n";
            cout << "\t" << iter << " iterations\n";
            cout << "\tConnexite : " << connexite << "\n";
            cout << "\tFrequence : " << freq/1000000000 << " Ghz\n\n";

            cout << "****************************************************" << endl;
            cout << "Pour les modifier, tapez \"X nb\" avec \"X\" le parametre a modifier et \"nb\" sa nouvelle valeur.\n";
            cout << "Par exemple \"T 4\" pour porter le nombre de threads a 4.\n";
            cout << "Les parametres possibles sont : \n\tT : nbThreads debut\n\tF : nbThreads fin\n\tR : nbRun\n\tI : nbIter\n\tC : connexite\n\tFr : frequence (en Mhz)\n\tFi : fichier a traiter\n";
            lancer = false;
            break;
        default :
            cout << "\nParametre \""<< argv[1+i] << "\" non reconnu\n";
            break;
        }
    }

    if (argc==1) {
        cout << "\t***************************************************" << endl;
        cout << "\t*     !! Des parametres sont necessaires : !!     *" << endl;
        cout << "\t***************************************************\n" << endl;

        cout << "****************************************************" << endl;
        cout << "Les parametres par defaut sont : \n";
        cout << "\t" << nbThreads << " Threads a " << nbThreads_Fin << " Threads\n";
        cout << "\t" << run << " run(s)\n";
        cout << "\t" << iter << " iterations\n";
        cout << "\tConnexite : " << connexite << "\n";
        cout << "\tFrequence : " << freq/1000000000 << " Ghz\n\n";

        cout << "****************************************************" << endl;
        cout << "Pour les modifier, tapez \"X nb\" avec \"X\" le parametre a modifier et \"nb\" sa nouvelle valeur.\n";
        cout << "Par exemple \"T 4\" pour porter le nombre de threads a 4.\n";
        cout << "Les parametres possibles sont : \n\tT : nbThreads debut\n\tF : nbThreads fin\n\tR : nbRun\n\tI : nbIter\n\tC : connexite\n\tFr : frequence (en Mhz)\n\tFi : fichier a traiter\n";
        lancer = false;
    }

    if (lancer) {
        if (nbThreads_Fin<nbThreads) nbThreads_Fin = nbThreads;

        cout << "\n***********************************\n";
        cout << "Lancement du test avec : \n";
        cout << nbThreads << " Threads a " << nbThreads_Fin << " Threads\n";
        cout << run << " run(s)\n";
        cout << iter << " iterations\n";
        cout << "Connexite : " << connexite << "\n";
        cout << "Frequence : " << freq/1000000000 << " Ghz\n";
        cout << "***********************************\n\n";

        for (i=nbThreads; i<=nbThreads_Fin; i++) {
            main_bench_Image(i,run,iter,connexite,freq,filename);
        }
    }
    return 0;
}
