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

#include "image.h"

Image *CreateImage(int ncols, int nrows)
{
    Image *img=NULL;
    int i;

    img = (Image *) calloc(1,sizeof(Image));
    if (img == NULL)
    {
        Error(MSG1,"CreateImage");
    }

    img->val   = AllocIntArray(nrows*ncols);
    img->tbrow = AllocIntArray(nrows);

    img->tbrow[0]=0;
    for (i=1; i < nrows; i++)
        img->tbrow[i]=img->tbrow[i-1]+ncols;
    img->ncols = ncols;
    img->nrows = nrows;

    return(img);
}

void DestroyImage(Image **img)
{
    Image *aux;

    aux = *img;
    if (aux != NULL)
    {
        if (aux->val != NULL)   free(aux->val);
        if (aux->tbrow != NULL) free(aux->tbrow);
        free(aux);
        *img = NULL;
    }
}

Image *ReadImage(char *filename)
{
    FILE *fp=NULL;
    unsigned char *value=NULL;
    char type[10];
    int  i,ncols,nrows,n;
    Image *img=NULL;
    char z[256];

    fp = fopen(filename,"rb");
    if (fp == NULL)
    {
        fprintf(stderr,"Cannot open %s\n",filename);
        exit(-1);
    }
    if (fscanf(fp,"%s\n",type)!=0);
    if ((strcmp(type,"P5")==0))
    {
        NCFgets(z,255,fp);
        sscanf(z,"%d %d\n",&ncols,&nrows);
        n = ncols*nrows;
        NCFgets(z,255,fp);
        sscanf(z,"%d\n",&i);
	fgetc(fp);
        value = (unsigned char *)calloc(n,sizeof(unsigned char));
        if (value != NULL)
        {
	  if(fread(value,sizeof(unsigned char),n,fp)!=0);
        }
        else
        {
            fprintf(stderr,"Insufficient memory in ReadImage\n");
            exit(-1);
        }
        fclose(fp);
        img = CreateImage(ncols,nrows);
        for (i=0; i < n; i++)
            img->val[i]=(int)value[i];
        free(value);
    }
    else
    {
        if ((strcmp(type,"P2")==0))
        {
            NCFgets(z,255,fp);
            sscanf(z,"%d %d\n",&ncols,&nrows);
            n = ncols*nrows;
            NCFgets(z,255,fp);
            sscanf(z,"%d\n",&i);
            img = CreateImage(ncols,nrows);
            for (i=0; i < n; i++)
	      if(fscanf(fp,"%d",&img->val[i])!=0);
            fclose(fp);
        }
        else
        {
            fprintf(stderr,"Input image must be P2 or P5\n");
            exit(-1);
        }
    }

    return(img);
}

void WriteImage(Image *img,char *filename)
{
    FILE *fp;
    int i, n, Imax;

    fp = fopen(filename,"wb");
    if (fp == NULL)
    {
        fprintf(stderr,"Cannot open %s\n",filename);
        exit(-1);
    }
    n    = img->ncols*img->nrows;
    if ((Imax=MaximumValue(img))==INT_MAX)
    {
        Warning("Image with infinity values","WriteImage");
        Imax = INT_MIN;
        for (i=0; i < n; i++)
            if ((img->val[i] > Imax)&&(img->val[i]!=INT_MAX))
                Imax = img->val[i];
        fprintf(fp,"P2\n");
        fprintf(fp,"%d %d\n",img->ncols,img->nrows);
        fprintf(fp,"%d\n",Imax+1);
    }
    else
    {
        fprintf(fp,"P2\n");
        fprintf(fp,"%d %d\n",img->ncols,img->nrows);
        if (Imax==0) Imax++;
        fprintf(fp,"%d\n",Imax);
    }

    for (i=0; i < n; i++)
    {
        if (img->val[i]==INT_MAX)
            fprintf(fp,"%d ",Imax+1);
        else
            fprintf(fp,"%d ",img->val[i]);
        if (((i+1)%17) == 0)
            fprintf(fp,"\n");
    }

    fclose(fp);

}


Image  *CopyImage(Image *img)
{
    Image *imgc;

    imgc = CreateImage(img->ncols,img->nrows);
    memcpy(imgc->val,img->val,img->ncols*img->nrows*sizeof(int));

    return(imgc);
}

int MinimumValue(Image *img)
{
    int i,min,n;

    n = img->ncols*img->nrows;
    min = img->val[0];
    for (i=1; i < n; i++)
        if (img->val[i] < min)
            min = img->val[i];

    return(min);
}

int MaximumValue(Image *img)
{
    unsigned int i, n, r;
    int max;

    max = img->val[0];
    n = img->ncols*img->nrows - 1;
    r = n%4;
    n -= r;
    for (i=1; i < n; i+=4)
    {
        if (img->val[i] > max)
            max = img->val[i];
        if (img->val[i+1] > max)
            max = img->val[i+1];
        if (img->val[i+2] > max)
            max = img->val[i+2];
        if (img->val[i+3] > max)
            max = img->val[i+3];
    }
    while (r != 0)
    {
        if (img->val[i+r-1] > max)
            max = img->val[i+r-1];
        --r;
    }

    return(max);
}

void SetImage(Image *img, int value)
{
    int i,n;
    n = img->ncols*img->nrows;
    for (i=0; i < n; i++)
    {
        img->val[i]=value;
    }
}

bool ValidPixel(Image *img, int x, int y)
{
    if ((x >= 0)&&(x < img->ncols)&&
            (y >= 0)&&(y < img->nrows))
        return(true);
    else
        return(false);
}

Image *AddFrame(Image *img, int sz, int value)
{
    Image *fimg;
    int y,*dst,*src,nbytes,offset;

    fimg = CreateImage(img->ncols+(2*sz),img->nrows+(2*sz));
    SetImage(fimg,value);
    nbytes = sizeof(int)*img->ncols;
    offset = sz+fimg->tbrow[sz];
    for (y=0,src=img->val,dst=fimg->val+offset; y < img->nrows;y++,src+=img->ncols,dst+=fimg->ncols)
    {
        memcpy(dst,src,nbytes);
    }
    return(fimg);
}

Image *RemFrame(Image *fimg, int sz)
{
    Image *img;
    int y,*dst,*src,nbytes,offset;

    img = CreateImage(fimg->ncols-(2*sz),fimg->nrows-(2*sz));
    nbytes = sizeof(int)*img->ncols;
    offset = sz+fimg->tbrow[sz];
    for (y=0,src=fimg->val+offset,dst=img->val; y < img->nrows;y++,src+=fimg->ncols,dst+=img->ncols)
    {
        memcpy(dst,src,nbytes);
    }
    return(img);
}

Image *Threshold(Image *img, int lower, int higher)
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

