/*
    Copyright (C) <2010> <Alexandre Xavier FalcÃ£o and Thiago Vallin Spina>

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

#include "ift.h"

/* Paper related to this program:

@article{Falcao02,
author     = "A.X. Falc{\~{a}}o and L.F. Costa and B.S. da Cunha",
title      = "Multiscale skeletons by image foresting transform and its applications to neuromorphometry",
journal    = "Pattern Recognition",
publisher  = "Elsevier ",
pages      = "1571-1582",
volume     = "35",
number     = "7",
month      = "Apr",
year       = "2002"
}

*/

// Creates Empty Forest

typedef struct _forest {
  Image *P; // predecessor map
  Image *R; // root map
  Image *V; // distance (cost or connectivity) map
} Forest;

Forest *CreateForest(int ncols, int nrows)
{
  Forest *F=(Forest *)calloc(1,sizeof(Forest));

  F->P = CreateImage(ncols,nrows);
  F->R = CreateImage(ncols,nrows);
  F->V = CreateImage(ncols,nrows);

  return(F);
}

// Destroys Forest

void DestroyForest(Forest **F)
{
  Forest *tmp=*F;

  if (tmp != NULL) {
    DestroyImage(&(tmp->P));
    DestroyImage(&(tmp->R));
    DestroyImage(&(tmp->V));
    free(tmp);
    *F = NULL;
  }
}

// Euclidean distance transform

Forest *DistTrans(Image *I)
{
  int p,q,n=I->ncols*I->nrows,i,tmp;
  Pixel u,v,w;
  AdjRel *A=Circular(1.5),*A4=Circular(1.0);
  Forest *F=CreateForest(I->ncols,I->nrows);
  GQueue *Q=CreateGQueue(1024,n,F->V->val);

  // Trivial path initialization

  for (p=0; p < n; p++) {
    u.x = p % I->ncols;
    u.y = p / I->ncols;
    F->V->val[p]=INT_MAX; F->R->val[p]=p; F->P->val[p]=NIL;
    if (I->val[p]!=0){ // p belongs to an object's border
      F->V->val[p]=0;
      InsertGQueue(&Q,p);
    }
  }

  // Path propagation

  while(!EmptyGQueue(Q)){
    p = RemoveGQueue(Q);
    u.x = p % I->ncols;
    u.y = p / I->ncols;
    w.x = F->R->val[p] % I->ncols;
    w.y = F->R->val[p] / I->ncols;
    for (i=1; i < A->n; i++) {
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      if (ValidPixel(I,v.x,v.y)){
	q   = v.x + I->tbrow[v.y];
	if (F->V->val[q]>F->V->val[p]){
	  tmp = (v.x-w.x)*(v.x-w.x)+(v.y-w.y)*(v.y-w.y);
	  if (tmp < F->V->val[q]){
	    if (F->V->val[q]!=INT_MAX) RemoveGQueueElem(Q, q);
	    F->V->val[q]=tmp; F->R->val[q]=F->R->val[p]; F->P->val[q]=p;
	    InsertGQueue(&Q,q);
	  }
	}
      }
    }
  }

  DestroyGQueue(&Q);
  DestroyAdjRel(&A);
  DestroyAdjRel(&A4);

  return(F);
}

int main(int argc, char **argv)
{
  int p;
  char outfile[100];
  char *file_noext;
  timer    *t1=NULL,*t2=NULL;
  Image    *img,*aux, *sqrt_edt;
  Forest   *edt;

  /* The following block must the remarked when using non-linux machines */

  void *trash = malloc(1);
  struct mallinfo info;
  int MemDinInicial, MemDinFinal;
  free(trash);
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*----------------------------------------------------------------------*/

  if (argc != 2) {
    printf("Usage: %s <image.pgm>\n", argv[0]);
    printf("image.pgm: a binary image for which an Euclidian Distance Transform will be computed\n");
    exit(0);
  }

  aux = ReadImage(argv[1]);

  file_noext = strtok(argv[1],".");

  if (MaximumValue(aux)!=1){
    fprintf(stderr,"Input image must be binary with values 0/1 \n");
    fprintf(stderr,"Assuming lower threshold 100 for this conversion\n");
    img = Threshold(aux,100,INT_MAX);
    WriteImage(img,"shape.pgm");
  }else{
    img = CopyImage(aux);
  }
  DestroyImage(&aux);

  t1 = Tic();

  edt = DistTrans(img);

  t2 = Toc();

  fprintf(stdout,"Euclidian Distance Transform in %f ms\n",CTime(t1,t2));

  sqrt_edt = CreateImage(img->ncols, img->nrows);
  for(p = 0; p < img->ncols*img->nrows;p++)
    sqrt_edt->val[p] = (int)sqrtf(edt->V->val[p]);

  sprintf(outfile,"%s_edt.pgm",strtok(argv[1],"."));

  WriteImage(sqrt_edt,outfile);

  DestroyForest(&edt);
  DestroyImage(&img);
  DestroyImage(&sqrt_edt);

  /* The following block must the remarked when using non-linux machines */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);


  return(0);
}
