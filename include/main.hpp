#ifndef _MAIN_
#define _MAIN_

#include "Region32.hpp"
#include "LabelRosenfeld.hpp"
#include "ImageEtiquette.hpp"

#ifdef _OPENMP
#include <omp.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <iomanip>

#define ITER 200        // Nombre d'itérations pour en faire une moyenne
#define RUN 2           // Nombre de tests successifs à réaliser
#define FREQ 1600e6     // Fréquence du processeur
#define NB_THREADS 1    // Nombre de threads à utiliser par défaut

#define CONNEXITE 4     // Connexité par défaut à utiliser

#define BENCH(X, cpp) tmin=1e20; for(r=0; r<run; r++) { t0=time();  for(k=0; k<iter; k++) { X; } t1=time(); dt=t1-t0; dt /= (double)iter; if(dt<tmin) tmin = dt;} cpp = tmin*freq/(sizeH*sizeW);

/*
* Get a second time
*/
double time() {
    double time;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    time = (double)tv.tv_sec + (double)tv.tv_usec * 1.0e-6;

    return time;
}

//int main (int argc, const char * argv[]);
void main_bench_Image(int nbThreads,int run,int iter,int connexite,double freq,char* filename);

#endif /* _MAIN_ */
