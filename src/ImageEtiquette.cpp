#include "ImageEtiquette.hpp"

void Palette_18Colors(Palette *palette) {
    int i;
    unsigned char rr = 255, gg = 255, bb = 255;
    unsigned char  r  =127, g = 127,  b = 127;

    Palette pattern[18];

    pattern[0].red   					= rr;
    pattern[0].green 					= 0;
    pattern[0].blue  					= 0;

    pattern[1].red   					= 0;
    pattern[1].green 					= gg;
    pattern[1].blue  					= 0;

    pattern[2].red   					= 0;
    pattern[2].green 					= 0;
    pattern[2].blue  					= bb;

    pattern[3].red   					= rr;
    pattern[3].green 					= gg;
    pattern[3].blue  					= 0;

    pattern[4].red   					= rr;
    pattern[4].green 					= 0;
    pattern[4].blue  					= bb;

    pattern[5].red   					= 0;
    pattern[5].green 					= gg;
    pattern[5].blue  					= bb;

    pattern[6].red   					= r;
    pattern[6].green 					= 0;
    pattern[6].blue  					= 0;

    pattern[7].red   					= 0;
    pattern[7].green 					= g;
    pattern[7].blue  					= 0;

    pattern[8].red   					= 0;
    pattern[8].green					= 0;
    pattern[8].blue  					= b;

    pattern[9].red   					= r;
    pattern[9].green 					= g;
    pattern[9].blue  					= 0;

    pattern[10].red   					= r;
    pattern[10].green 					= 0;
    pattern[10].blue  					= b;

    pattern[11].red  					= 0;
    pattern[11].green 					= g;
    pattern[11].blue  					= b;

    pattern[12].red   					= rr;
    pattern[12].green 					= g;
    pattern[12].blue  					= 0;

    pattern[13].red   					= rr;
    pattern[13].green 					= 0;
    pattern[13].blue  					= b;

    pattern[14].red   					= 0;
    pattern[14].green 					= gg;
    pattern[14].blue  					= b;

    pattern[15].red   					= r;
    pattern[15].green 					= gg;
    pattern[15].blue  					= 0;

    pattern[16].red   					= r;
    pattern[16].green 					= 0;
    pattern[16].blue  					= bb;

    pattern[17].red   					= 0;
    pattern[17].green 					= g;
    pattern[17].blue  					= bb;

    for (i=0; i<255; i++) {
        palette[1+i].red      			= pattern[i%18].red;
        palette[1+i].green    			= pattern[i%18].green;
        palette[1+i].blue     			= pattern[i%18].blue;
    }
    palette[0].blue     				= (unsigned char) 0;
    palette[0].green    				= (unsigned char) 0;
    palette[0].red      				= (unsigned char) 0;

    palette[255].blue    				= (unsigned char) 255;
    palette[255].green    				= (unsigned char) 255;
    palette[255].red      				= (unsigned char) 255;
}
char *readitem(FILE *file,char *buffer) {
    char *aux;
    int k;

    k=0;
    aux=buffer;
    while (!feof(file)) {
        *aux=fgetc(file);
        switch (k) {
        case 0:
            if (*aux=='#') k=1;
            if (isalnum(*aux)) k=2,aux++;
            break;
        case 1:
            if (*aux==0xA) k=0;
            break;
        case 2:
            if (!isalnum(*aux)) {
                *aux=0;
                return buffer;
            }
            aux++;
            break;
        }
    }
    *aux=0;
    return buffer;
}
unsigned char** LoadPGM_ui8matrix(char *filename, int *sizeH, int *sizeW) {
    int height, width, gris;
    size_t temp;
    unsigned char **m;
    FILE *file;

    char *buffer;
    char msg[1024];
    int i;

    buffer = (char*) calloc(80, sizeof(char));

    /* ouverture du fichier */
    file = fopen(filename,"rb");
    if (file==NULL) {
        cout << "Ouverture du fichier " << filename << " impossible dans LoadPGM\n";
    }

    /* lecture de l'entete du fichier pgm */
    readitem(file, buffer);

    if (strcmp(buffer, "P5") != 0)
        cout << "Entete du fichier " << filename << " %s invalide\n";

    width  = atoi(readitem(file, buffer));
    height = atoi(readitem(file, buffer));
    gris   = atoi(readitem(file, buffer));

    *sizeH = height;
    *sizeW = width;

    m						=	(unsigned char**)malloc(height *sizeof(unsigned char*));
    for (i=0; i<height; i++) {
        m[i] 				= 	(unsigned char*)malloc(width*sizeof(unsigned char));
    }

    for (i=0; i<height; i++) {
        temp = fread(m[i], sizeof(unsigned char), width, file);
    }

    fclose(file);
    free(buffer);

    return m;
}
int SaveBMP_ui8matrix(unsigned char **m, int sizeH, int sizeW, char *filename) {

    int rc = 0;
    long height = sizeH;
    long width  = sizeW;
    long size = height * width;

    long padding_len;

    BitmapFileHeader Bitmap_File_Header;
    BitmapInfoHeader Bitmap_Info_Header;

    Palette palette_RGBQuad[256];

    char Padding[3];

    FILE *fichier;
    int  i;
    Palette_18Colors(palette_RGBQuad);

    if (sizeof(BitmapFileHeader) != 14) {
        printf("SaveMBP sizeof(BitmapFileHeader) = %ld\n", sizeof( BitmapFileHeader));
    }

    Padding[0] = 0;
    Padding[1] = 0;
    Padding[2] = 0;

    /* --- Header --- */
    Bitmap_File_Header.bfType      = (unsigned short) BM;   //
    Bitmap_File_Header.bfSize      = (unsigned long int) sizeof(BitmapFileHeader)+sizeof(BitmapInfoHeader)+256*sizeof(Palette);//termi+width;    /* taille avec header et palette */
    Bitmap_File_Header.bfReserved1 = (unsigned short) 0; /* 0                             */
    Bitmap_File_Header.bfReserved2 = (unsigned short) 0; /* 0                             */
    Bitmap_File_Header.bfOffBits   = (unsigned long int) sizeof(BitmapFileHeader) +
                                     (unsigned long int) sizeof(BitmapInfoHeader) +
                                     (unsigned long int) sizeof(Palette)*256;   /* */


    Bitmap_Info_Header.biSize          = (unsigned long int) sizeof(BitmapInfoHeader);
    Bitmap_Info_Header.biWidth         = (long int)  width;   // width sans padding
    Bitmap_Info_Header.biHeight        = (long int)  height;  // height sans padding
    Bitmap_Info_Header.biPlanes        = (unsigned short)  1;       // 1
    Bitmap_Info_Header.biBitCount      = (unsigned short)  8;       // 8 en 256 colors
    Bitmap_Info_Header.biCompression   = (unsigned long int)  BI_RGB;  // BI_RGB
    Bitmap_Info_Header.biSizeImage     = (unsigned long int)  0;       // 0 si BI_RGB
    Bitmap_Info_Header.biXPelsPerMeter = (long int)  0;       // ???
    Bitmap_Info_Header.biYPelsPerMeter = (long int)  0;       // ???
    Bitmap_Info_Header.biClrUsed       = (unsigned long int)  256;     // 256
    Bitmap_Info_Header.biClrImportant  = (unsigned long int)  0;       // 0

    fichier  = fopen(filename, "wb");

    /* enregistrement de l'image au format bmp */
    size = fwrite(&Bitmap_File_Header, sizeof(BitmapFileHeader), 1, fichier);
    size = fwrite(&Bitmap_Info_Header, sizeof(BitmapInfoHeader), 1, fichier);
    size = fwrite(palette_RGBQuad, sizeof(Palette), 256, fichier);

    /* en 2x car le compilo est trop bête ... */
    padding_len = width % 4;
    padding_len = (4 - padding_len) % 4;

    for (i=height-1; i>=0; i--) {
        fwrite(m[i], sizeof(char), width, fichier);
        fwrite(Padding, sizeof(unsigned char), padding_len, fichier);
    }

    fclose(fichier);
    return rc;
}



/* Constructeur par defaut */
ImageEtiquette::ImageEtiquette() {
    this->imageEtiquette				=	NULL;
    this->imageEtiquetteReference		= 	NULL;
    this->imageSource					=	NULL;
    this->height						=	0;
    this->width							=	0;
}

/* Destructeur par defaut */
ImageEtiquette::~ImageEtiquette() {
    int i;

    if (this->imageSource!=NULL && this->imageEtiquette!=NULL && this->imageEtiquetteReference!=NULL) {
        for (i=0; i<this->height; i++) {
            free(this->imageSource[i]);
            free(this->imageEtiquette[i]);
            free(this->imageEtiquetteReference[i]);
        }
        free(this->imageSource);
        free(this->imageEtiquette);
        free(this->imageEtiquetteReference);
    }

    this->height							=	0;
    this->width								=	0;
}

/* Nettoyage des images */
void ImageEtiquette::cleanImageEtiquette(){
    int i, j;

    for (i=0; i<this->height; i++) {
        for (j=0; j<this->width; j++) {
            this->imageEtiquette[i][j] = 0;
        }
    }
}
void ImageEtiquette::cleanImageEtiquetteReference(){
    int i, j;

    for (i=0; i<this->height; i++) {
        for (j=0; j<this->width; j++) {
            this->imageEtiquetteReference[i][j] = 0;
        }
    }
}

/* Gestion des images */
bool ImageEtiquette::loadImageSource(string chemin) {
    int i,j;

    this->imageSource = LoadPGM_ui8matrix((char *)(chemin.c_str()), &(this->height), &(this->width));

    this->imageEtiquette 					= 	(uint32_t**)malloc(this->height *sizeof(uint32_t*));
    this->imageEtiquetteReference 			= 	(uint32_t**)malloc(this->height *sizeof(uint32_t*));
    for (i=0; i<height; i++) {
        this->imageEtiquette[i] 			= 	(uint32_t*)malloc(this->width*sizeof(uint32_t));
        this->imageEtiquetteReference[i]	= 	(uint32_t*)malloc(this->width*sizeof(uint32_t));
    }

    for (i=0; i<this->height; i++) {
        for (j=0; j<this->width; j++) {
            this->imageEtiquette[i][j] 		= 	0;
            this->imageEtiquetteReference[i][j] 	= 	0;
        }
    }

    return true;
}
bool ImageEtiquette::saveImageEtiquette(string chemin) {
    int i, j;

    unsigned char** temp = new unsigned char*[this->height];
    for (i=0; i<height; i++) {
        temp[i] 			= 	new unsigned char[this->width];
    }

    for (i=0; i<this->height; i++) {
        for (j=0; j<this->width; j++) {
            temp[i][j] = (unsigned char)(this->imageEtiquette[i][j]);
        }
    }

    SaveBMP_ui8matrix(temp,this->height,this->width, (char *)(chemin.c_str()));

    for (i=0; i<this->height; i++) {
        delete [] temp[i];
    }
    delete temp;

    return true;
}
bool ImageEtiquette::saveImageEtiquetteReference(string chemin) {
    int i, j;

    unsigned char** temp = new unsigned char*[this->height];
    for (i=0; i<height; i++) {
        temp[i] 			= 	new unsigned char[this->width];
    }

    for (i=0; i<this->height; i++) {
        for (j=0; j<this->width; j++) {
            temp[i][j] = (unsigned char)(this->imageEtiquetteReference[i][j]);
        }
    }

    SaveBMP_ui8matrix(temp,this->height,this->width, (char *)(chemin.c_str()));

    for (i=0; i<this->height; i++) {
        delete [] temp[i];
    }
    delete temp;

    return true;
}
bool ImageEtiquette::saveImageDifference(string chemin){
	int i, j;

    unsigned char** temp = new unsigned char*[this->height];
    for (i=0; i<height; i++) {
        temp[i] 			= 	new unsigned char[this->width];
    }

    for (i=0; i<this->height; i++) {
        for (j=0; j<this->width; j++) {
            temp[i][j] = (unsigned char)(this->imageEtiquetteReference[i][j] - this->imageEtiquette[i][j]);
        }
    }

    SaveBMP_ui8matrix(temp,this->height,this->width, (char *)(chemin.c_str()));

    for (i=0; i<this->height; i++) {
        delete [] temp[i];
    }
    delete temp;

    return true;
}

bool ImageEtiquette::compareImagesEtiquette(string message) {
    int i, j;
    bool retour = false;

    for (i=0; i<this->height; i++) {
        for (j=0; j<this->width; j++) {
            if (this->imageEtiquette[i][j] != this->imageEtiquetteReference[i][j]) {
                retour = true;
            }
        }
    }

    if (retour == true) {
        cout << "**** ATTENTION : DIFFERENCE sur " << message << " ****" << endl;
    }
    return retour;
}
