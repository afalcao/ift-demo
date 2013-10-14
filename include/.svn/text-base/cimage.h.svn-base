#ifndef _CIMAGE_H_
#define _CIMAGE_H_

#include "image.h"
#include "adjacency.h"

typedef struct cimage {
  Image *C[3];
} CImage;

CImage *CreateCImage(int ncols, int nrows);
void    DestroyCImage(CImage **cimg);
CImage *ReadCImage(char *filename);
void    WriteCImage(CImage *cimg, char *filename);
CImage *DrawLabeledRegions(Image *img, Image *label);

#endif
