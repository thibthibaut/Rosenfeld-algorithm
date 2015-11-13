#ifndef _IMAGE_ETIQUETTE_
#define _IMAGE_ETIQUETTE_

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BM 0x4d42
#define BI_RGB 0L
#pragma pack(push, palette, 1)

using namespace std;


typedef struct {
    uint16_t    bfType;            //  WORD
    uint32_t    bfSize;            // DWORD
    uint16_t    bfReserved1;       //  WORD
    uint16_t    bfReserved2;       //  WORD
    uint32_t    bfOffBits;         // DWORD
} BitmapFileHeader;

typedef struct {
    uint32_t    biSize;            // DWORD
    int32_t     biWidth;           // LONG
    int32_t     biHeight;          // LONG
    uint16_t    biPlanes;          // WORD
    uint16_t    biBitCount;        // WORD
    uint32_t    biCompression;     // DWORD
    uint32_t    biSizeImage;       // DWORD
    int32_t     biXPelsPerMeter;   // LONG
    int32_t     biYPelsPerMeter;   // LONG
    uint32_t    biClrUsed;         // DWORD
    uint32_t    biClrImportant;    // DWORD
} BitmapInfoHeader;

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char reserved;
} Palette;

void Palette_18Colors(Palette *palette);
char *readitem(FILE *file,char *buffer);
unsigned char** LoadPGM_ui8matrix(char *filename, int *sizeH, int *sizeW);
int SaveBMP_ui8matrix(unsigned char **m, int sizeH, int sizeW, char *filename);


class ImageEtiquette {
private :

public :

    uint32_t**	imageEtiquette;
    uint32_t**	imageEtiquetteReference;
    unsigned char**		imageSource;
    int			height;
    int		    width;

    /* Constructeur par defaut */
    ImageEtiquette();

    /* Destructeur par defaut */
    ~ImageEtiquette();

    /* Nettoyage des images */
    void cleanImageEtiquette();
    void cleanImageEtiquetteReference();

    /* Gestion des images */
    bool loadImageSource(string chemin);
    bool saveImageEtiquette(string chemin);
    bool saveImageEtiquetteReference(string chemin);
	bool saveImageDifference(string chemin);

    bool compareImagesEtiquette(string message);
};

#endif /* _IMAGE_ETIQUETTE_ */
