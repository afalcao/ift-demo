#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "common.h"

typedef struct _pixel {
  int x,y;
} Pixel;

typedef struct _image {
  int *val;
  int ncols,nrows;
  int *tbrow;
} Image;

Image  *CreateImage(int ncols,int nrows);
void    DestroyImage(Image **img);
Image  *ReadImage(char *filename);
void    WriteImage(Image *img, char *filename);
Image  *CopyImage(Image *img);

int     MinimumValue(Image *img);
int     MaximumValue(Image *img);
void    SetImage(Image *img, int value);
bool    ValidPixel(Image *img, int x, int y);
Image  *AddFrame(Image *img, int sz, int value);
Image  *RemFrame(Image *fimg, int sz);

Image *Threshold(Image *img, int lower, int higher);

#endif





