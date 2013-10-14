#ifndef _FEATURE_H_
#define _FEATURE_H_

#include "common.h"
#include "cimage.h"
#include "color.h"

typedef struct _felem {
  float *feat;
} FElem;


typedef struct _features {
  FElem *elem;
  int  nfeats;
  int  nelems;
  int  nrows,ncols;
  int  Imax;
} Features;

/// Allocates memory for features
Features* CreateFeatures(int ncols, int nrows, int nfeats);

/// Deallocates memory for features
void DestroyFeatures(Features **f);

Features* CopyFeatures(Features* feat);

// using convolution with gaussian filters
Features *GaussImageFeats(Image *img, int nscales);
Features *GaussCImageFeats(CImage *cimg, int nscales);

Features* LabFeats(Features* rgb);

#endif // _FEATURE_H_
