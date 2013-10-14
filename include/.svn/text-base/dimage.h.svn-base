#ifndef _DIMAGE_H_
#define _DIMAGE_H_

//This module is deprecated.
//This file is still available only for
//compatibility purposes.
//You should use "RealMatrix" instead,
//which is far more superior. It supports
//both linear and two-dimensional access
//(M->val[0][p] or M->val[i][j] for a entry
//(i,j) at address p=j+i*ncols) and its
//element values precision can be easily
//changed (float,double,..).

#include "common.h"
#include "image.h"

typedef struct _DImage{
  double *val;
  int ncols,nrows;
  int *tbrow;
} DImage;


DImage *CreateDImage(int ncols, int nrows);
void    DestroyDImage(DImage **dimg);

DImage  *ReadDImage(char *filename);
void    WriteDImage(DImage *dimg, char *filename);
double  MinimumDImageValue(DImage *dimg);
double  MaximumDImageValue(DImage *dimg);
void    SetDImage(DImage *dimg, double value);
bool    ValidDImagePixel(DImage *dimg, int x, int y);
DImage  *CopyDImage(DImage *dimg);
Image   *ConvertDImage2Image(DImage *dimg);
Image   *DImageThreshold(DImage *img, double lower, double higher);

#endif
