/*
    Copyright (C) <2010> <Alexandre Xavier FalcÃo and Thiago Vallin Spina>

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

    Modified by Cris Luengo <cris@cb.uu.se> to test RGQueue.
    Added some code from Image.c, modified for float images.

*/

#include "ift.h"
#include "rgqueue.h"

typedef struct _fimage {
  float *val;
  int ncols,nrows;
  int *tbrow;
} FImage;

FImage *CreateFImage(int ncols, int nrows)
{
    FImage *img=NULL;
    int i;

    img = (FImage *) calloc(1,sizeof(FImage));
    if (img == NULL)
    {
        Error(MSG1,"CreateFImage");
    }

    img->val   = AllocFloatArray(nrows*ncols);
    img->tbrow = AllocIntArray(nrows);

    img->tbrow[0]=0;
    for (i=1; i < nrows; i++)
        img->tbrow[i]=img->tbrow[i-1]+ncols;
    img->ncols = ncols;
    img->nrows = nrows;

    return(img);
}

void DestroyFImage(FImage **img)
{
    FImage *aux;

    aux = *img;
    if (aux != NULL)
    {
        if (aux->val != NULL)   free(aux->val);
        if (aux->tbrow != NULL) free(aux->tbrow);
        free(aux);
        *img = NULL;
    }
}

bool ValidFPixel(FImage *img, int x, int y)
{
    if ((x >= 0)&&(x < img->ncols)&&
            (y >= 0)&&(y < img->nrows))
        return(true);
    else
        return(false);
}

float MaximumFValue(FImage *img)
{
    int i,n;
    float max;

    n = img->ncols*img->nrows;
    max = img->val[0];
    for (i=1; i < n; i++)
        if (img->val[i] > max)
            max = img->val[i];

    return(max);
}

void CopyImageToFloatWithNoise(Image *in, FImage *out)
{
    int i,n;
    n = in->ncols*in->nrows;
    for (i=1; i < n; i++)
        out->val[i] = in->val[i] + ((float)rand()/RAND_MAX);
}

// Watershed from binary marker

Image *WatershedFloat(FImage *img, Set *Obj, Set *Bkg)
{
  AdjRel  *A=NULL;
  RGQueue *Q=NULL;
  FImage  *cost=NULL;
  Image   *label=NULL;
  Pixel    u,v;
  int      i,p,q,n;
  float    Cmax=MaximumFValue(img);
  Set     *S;
  float    tmp;

  cost  = CreateFImage(img->ncols,img->nrows);
  label = CreateImage(img->ncols,img->nrows);
  n     = img->ncols*img->nrows;
  Q     = CreateRGQueue((int)(Cmax+1),n,cost->val);
  A     = Circular(1.5);

  /* Trivial path initialization */

  for (p=0; p < n; p++){
    cost->val[p] = (float)INT_MAX;
  }
  S = Obj;
  while(S != NULL){
    p=S->elem;
    label->val[p]=1;
    cost->val[p]=0;
    InsertRGQueue(Q,p);
    S = S->next;
  }

  S = Bkg;
  while(S != NULL){
    p=S->elem;
    label->val[p]=0;
    cost->val[p]=0;
    InsertRGQueue(Q,p);
    S = S->next;
  }



  /* Path propagation */

  while (!EmptyRGQueue(Q)){
    p   = RemoveRGQueue(Q);
    printf("p %d cost %f\n",p,cost->val[p]);
    u.x = p%img->ncols;
    u.y = p/img->ncols;
    for (i=1; i < A->n; i++) {
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      if (ValidFPixel(img,v.x,v.y)){
        q   = v.x + img->tbrow[v.y];
        if (cost->val[p] < cost->val[q]){
          tmp = MAX(cost->val[p] , img->val[q]);
          if (tmp < cost->val[q]){
            label->val[q]=label->val[p];
            UpdateRGQueueElem(Q,q,tmp);
          }
        }
      }
    }
  }

  DestroyRGQueue(&Q);
  DestroyFImage(&cost);
  DestroyAdjRel(&A);

  return(label);
}

void TestRGQueue()
{
  RGQueue *Q=NULL;
  float   *cost=NULL;
  int      n=10;
  float    Cmax=100;

  cost  = AllocFloatArray(n);
  Q     = CreateRGQueue((int)(Cmax+1),n,cost);

  cost[0]=0.6;
  InsertRGQueue(Q,0);
  cost[1]=0.5;
  InsertRGQueue(Q,1);
  cost[2]=0.4;
  InsertRGQueue(Q,2);
  cost[3]=0.3;
  InsertRGQueue(Q,3);
  cost[4]=0.2;
  InsertRGQueue(Q,4);
  cost[5]=0.2;
  InsertRGQueue(Q,5);
  cost[6]=0.3;
  InsertRGQueue(Q,6);
  cost[7]=0.0;
  InsertRGQueue(Q,7);
  PrintHeap(Q,7);
  printf("first %d\n",Q->C.first[0]);
  printf("last  %d\n",Q->C.last[0]);
  int p, cont=0;
  while (!EmptyRGQueue(Q)){
    p   = RemoveRGQueue(Q);
    printf("p %d cost %f\n",p,cost[p]);
    PrintHeap(Q,Q->C.first[0]);
    printf("first %d\n",Q->C.first[0]);
    printf("last  %d\n",Q->C.last[0]);
    cont++;
    if (cont==2) break; 
  }
  cost[8]=0.0;
  InsertRGQueue(Q,8);
  PrintHeap(Q,Q->C.first[0]);
  printf("first %d\n",Q->C.first[0]);
  printf("last  %d\n",Q->C.last[0]);
    exit(0);
  
  DestroyRGQueue(&Q);
  free(cost);
}

int main(int argc, char **argv)
{
  timer    *t1=NULL,*t2=NULL;
  Image    *img=NULL,*grad=NULL;
  FImage   *fgrad=NULL;
  Image    *label=NULL;
  CImage   *cimg=NULL;
  Set      *Obj=NULL,*Bkg=NULL;
  char     outfile[100];
  char     *file_noext;
  /*--------------------------------------------------------*/

  void *trash = malloc(1);
  struct mallinfo info;
  int MemDinInicial, MemDinFinal;
  free(trash);
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=4){
    fprintf(stderr,"Usage: watershed <image.pgm> <gradient.pgm> <seeds.txt>\n");
    fprintf(stderr,"image.pgm: image to overlay the watershed lines on it\n");
    fprintf(stderr,"gradient.pgm: gradient image to compute the watershed segmentation\n");
    fprintf(stderr,"seeds.txt: seed pixels\n");
    exit(-1);
  }

  TestRGQueue();
  exit(0);

  img   = ReadImage(argv[1]);
  grad  = ReadImage(argv[2]);
  ReadSeeds(argv[3],&Obj,&Bkg);

  file_noext = strtok(argv[1],".");

  fgrad  = CreateFImage(grad->ncols,grad->nrows);
  CopyImageToFloatWithNoise(grad, fgrad);

  t1 = Tic();

  label = WatershedFloat(fgrad,Obj,Bkg);

  t2 = Toc();

  fprintf(stdout,"Processing time in %f ms\n",CTime(t1,t2));

  cimg = DrawLabeledRegions(img,label);
  sprintf(outfile,"%s_result.ppm",file_noext);
  WriteCImage(cimg,outfile);
  DestroyImage(&grad);
  DestroyImage(&img);
  DestroyFImage(&fgrad);
  DestroyImage(&label);
  DestroyCImage(&cimg);
  DestroySet(&Obj);
  DestroySet(&Bkg);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
           MemDinInicial,MemDinFinal);

  return(0);
}
