#ifndef _MORPHOLOGY_H_
#define _MORPHOLOGY_H_

#include "common.h"
#include "adjacency.h"
#include "image.h"
#include "set.h"
#include "gqueue.h"

/* Explanation for some variables

V is the cost or connectivity map
P is the predecessor map
I is the mask image 
J is the marker image

*/

Image *Dilate(Image *img, AdjRel *A);
Image *Erode(Image *img, AdjRel *A);
Image *Open(Image *img, AdjRel *A);
Image *Close(Image *img, AdjRel *A);

Set *ImageBorder(Image *img);

// Superior reconstruction img <= marker without marker imposition
Image *SupRec(Image *img, Image *marker);

// Inferior Reconstruction img >= marker without marker imposition
Image *InfRec(Image *img, Image *marker);

// Superior reconstruction img <= marker with marker imposition
Image *SupRecMI(Image *img, Set **S);

// Inferior Reconstruction img >= marker with marker imposition
Image *InfRecMI(Image *img, Set **S);

Image *CloseBasins(Image *img);
Image *OpenDomes(Image *img);

Image *FastDilate(Image *I,Set **S, float radius);
Image *FastErode(Image *I,Set **S, float radius);
Image *FastClose(Image *I, float radius);
Image *FastOpen(Image *I, float radius);
Image *FastCloseRec(Image *I, float radius);

Image *CloseRec(Image *I, AdjRel *A);
Image *OpenRec(Image *I, AdjRel *A);

Image *FastOpenRec(Image *I, float radius);

Image *CloseHoles(Image *img);

#endif
