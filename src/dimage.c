/*
    Copyright (C) <2010> <Alexandre Xavier Falcão and Thiago Vallin Spina>

    This file is part of IFT-demo.

    IFT-demo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    IFT-demo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with IFT-demo.  If not, see <http://www.gnu.org/licenses/>.

    please see full copyright in COPYING file.
    -------------------------------------------------------------------------

    written by A.X. Falcão <afalcao@ic.unicamp.br> and by T.V. Spina
    <tvspina@liv.ic.unicamp.br>, 2010

*/

#include "dimage.h"
#include "common.h"

DImage *CreateDImage(int ncols, int nrows)
{
  DImage *dimg=NULL;
  int i;

  dimg = (DImage *) calloc(1,sizeof(DImage));
  if (dimg == NULL){
    Error(MSG1,"CreateDImage");
  }

  dimg->val   = AllocDoubleArray(nrows*ncols);
  dimg->tbrow = AllocIntArray(nrows);

  dimg->tbrow[0]=0;
  for (i=1; i < nrows; i++)
    dimg->tbrow[i]=dimg->tbrow[i-1]+ncols;
  dimg->ncols = ncols;
  dimg->nrows = nrows;

 return(dimg);
}

void DestroyDImage(DImage **dimg)
{
  DImage *aux;

  aux = *dimg;
  if(aux != NULL){
    if (aux->val != NULL)   free(aux->val);
    if (aux->tbrow != NULL) free(aux->tbrow);
    free(aux);
    *dimg = NULL;
  }
}

DImage  *ReadDImage(char *filename){
  DImage *dimg;
  char msg[512];
  int  ncols,nrows,n;
  FILE *fp;

  fp = fopen(filename,"rb");
  if (fp == NULL){
    sprintf(msg,"Cannot open %s",filename);
    Error(msg,"ReadDImage");
  }

  if (fscanf(fp,"%d %d\n",&ncols,&nrows)!=0);
  dimg = CreateDImage(ncols, nrows);
  n = ncols*nrows;
  if(fread(dimg->val, sizeof(double), n, fp)!=0);

  return dimg;
}

void    WriteDImage(DImage *dimg, char *filename){
  char msg[512];
  int n;
  FILE *fp;

  fp = fopen(filename,"wb");
  if (fp == NULL){
    sprintf(msg,"Cannot open %s",filename);
    Error(msg,"WriteDImage");
  }

  fprintf(fp,"%d %d\n",dimg->ncols,dimg->nrows);
  n = dimg->ncols*dimg->nrows;
  fwrite(dimg->val, sizeof(double), n ,fp);
}

bool ValidDImagePixel(DImage *dimg, int x, int y)
{
  if ((x >= 0)&&(x < dimg->ncols)&&
      (y >= 0)&&(y < dimg->nrows))
    return(true);
  else
    return(false);
}

double  MaximumDImageValue(DImage *dimg)
{
  double max;
  int i,n;

  n = dimg->ncols*dimg->nrows;
  max = dimg->val[0];
  for (i=1; i < n; i++)
    if (dimg->val[i] > max)
      max = dimg->val[i];

  return(max);
}

double  MinimumDImageValue(DImage *dimg)
{
  double min;
  int i,n;

  n = dimg->ncols*dimg->nrows;
  min = dimg->val[0];
  for (i=1; i < n; i++)
    if (dimg->val[i] < min)
      min = dimg->val[i];

  return(min);
}

void SetDImage(DImage *dimg, double value)
{
  int i,n;
  n = dimg->ncols*dimg->nrows;
  for (i=0; i < n; i++){
    dimg->val[i]=value;
  }
}

DImage *CopyDImage(DImage *dimg)
{
  DImage *dimgc;

  dimgc = CreateDImage(dimg->ncols,dimg->nrows);
  memcpy(dimgc->val,dimg->val,dimg->ncols*dimg->nrows*sizeof(double));

  return(dimgc);
}


Image *ConvertDImage2Image(DImage *dimg){
  Image *img;
  double max,min;
  int p,n;

  n = dimg->ncols*dimg->nrows;
  img = CreateImage(dimg->ncols, dimg->nrows);
  max = MaximumDImageValue(dimg);
  min = MinimumDImageValue(dimg);
  if (max==min) {
    for(p=0; p<n; p++)
      img->val[p] = max;
    return img;
  }

  for(p=0; p<n; p++){
    img->val[p] = ROUND(4095.0*(dimg->val[p]-min)/(max-min));
  }

  return img;
}

Image *DImageThreshold(DImage *img, double lower, double higher)
{
  Image *bin=NULL;
  int p,n;

  bin = CreateImage(img->ncols,img->nrows);
  n = img->ncols*img->nrows;
  for (p=0; p < n; p++)
    if ((img->val[p] >= lower)&&(img->val[p] <= higher))
      bin->val[p]=1;
  return(bin);
}
