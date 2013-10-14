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

// Verify if it is possible to start at this pixel for contour labeling

bool ValidContPoint(Image *bin, AdjRel *L, AdjRel *R, int p)
{
  int i,q,n,left,right;
  Pixel u,v,l,r;
  bool found=false;

  u.x = p%bin->ncols;
  u.y = p/bin->ncols;
  n   = L->n;

  for (i=0; i < n; i++) {
    v.x = u.x + L->dx[i];
    v.y = u.y + L->dy[i];
    if (ValidPixel(bin,v.x,v.y)){
      q = v.x + bin->tbrow[v.y];
      if ((bin->val[q]==1)&&(p!=q)){
	l.x = u.x + L->dx[i];
	l.y = u.y + L->dy[i];
	r.x = u.x + R->dx[i];
	r.y = u.y + R->dy[i];
	if (ValidPixel(bin,l.x,l.y))
	  left = l.x + bin->tbrow[l.y];
	else
	  left = -1;
	if (ValidPixel(bin,r.x,r.y))
	  right = r.x + bin->tbrow[r.y];
	else
	  right = -1;
	if (((left!=-1)&&(right!=-1)&&(bin->val[left]!=bin->val[right]))||
	    ((left==-1)&&(right!=-1)&&(bin->val[right]==1)) ||
	    ((right==-1)&&(left!=-1)&&(bin->val[left]==1))){
	  found = true;
	  break;
	}
      }
    }
  }

  return(found);
}

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

// Assigs a distinct label from 1 to N to each contour

Image *LambdaContour(Image *bin)
{
  Image *bndr=NULL;
  Image *color=NULL,*pred=NULL,*lambda=NULL;
  int p=0,q,r,i,j,left=0,right=0,n,*LIFO,last,l=1;
  AdjRel *A,*L,*R;
  Pixel u,v,w;

  A     = Circular(1.0);
  n     = bin->ncols*bin->nrows;
  bndr  = CreateImage(bin->ncols,bin->nrows);
  for (p=0; p < n; p++){
    if (bin->val[p]==1){
      u.x = p%bin->ncols;
      u.y = p/bin->ncols;
      for (i=1; i < A->n; i++){
	v.x = u.x + A->dx[i];
	v.y = u.y + A->dy[i];
	if (ValidPixel(bin,v.x,v.y)){
	  q = v.x + bin->tbrow[v.y];
	  if (bin->val[q]==0){
	    bndr->val[p]=1;
	    break;
	  }
	} else {
	    bndr->val[p]=1;
	    break;
	}
      }
    }
  }
  DestroyAdjRel(&A);

  A      = Circular(1.5);
  L      = LeftSide(A);
  R      = RightSide(A);
  lambda = CreateImage(bndr->ncols,bndr->nrows);
  color  = CreateImage(bndr->ncols,bndr->nrows);
  pred   = CreateImage(bndr->ncols,bndr->nrows);
  LIFO   = AllocIntArray(n);
  last   = NIL;
  for (j=0; j < n; j++){
    if ((bndr->val[j]==1)&&
	(color->val[j]!=BLACK)&&
	ValidContPoint(bin,L,R,j)){
      last++; LIFO[last]    = j;
      color->val[j] = GRAY;
      pred->val[j] = j;
      while(last != NIL){
	p = LIFO[last];	last--;
	color->val[p]=BLACK;
	u.x = p%bndr->ncols;
	u.y = p/bndr->ncols;
	for (i=1; i < A->n; i++){
	  v.x = u.x + A->dx[i];
	  v.y = u.y + A->dy[i];
	  if (ValidPixel(bndr,v.x,v.y)){
	    q = v.x + bndr->tbrow[v.y];
	    if ((q==j)&&(pred->val[p]!=j)){
	      last = NIL;
	      break;
	    }
	    w.x = u.x + L->dx[i];
	    w.y = u.y + L->dy[i];
	    if (ValidPixel(bndr,w.x,w.y))
	      left = w.x + bndr->tbrow[w.y];
	    else
	      left = -1;
	    w.x = u.x + R->dx[i];
	    w.y = u.y + R->dy[i];
	    if (ValidPixel(bndr,w.x,w.y))
	      right = w.x + bndr->tbrow[w.y];
	    else
	      right = -1;

	    if ((bndr->val[q]==1)&&
		(color->val[q] != BLACK)&&
		(((left!=-1)&&(right!=-1)&&(bin->val[left]!=bin->val[right]))||
		 ((left==-1)&&(right!=-1)&&(bin->val[right]==1)) ||
		 ((right==-1)&&(left!=-1)&&(bin->val[left]==1))) ) {
	      pred->val[q] = p;
	      if (color->val[q] == WHITE){
		last++; LIFO[last] = q;
		color->val[q]=GRAY;
	      }
	    }
	  }
	}
      }
      r = p;
      while(pred->val[p]!=p){
	lambda->val[p] = l;
	p = pred->val[p];
      }
      if (r != p){
	lambda->val[p] = l;
	l++;
      }
    }
  }

  DestroyAdjRel(&A);
  DestroyAdjRel(&L);
  DestroyAdjRel(&R);
  DestroyImage(&bndr);
  DestroyImage(&color);
  DestroyImage(&pred);
  free(LIFO);


  return(lambda);
}

// Assigs a distinct subsequent integer number to each contour pixel from 1 to N

Image *LambdaContPixel(Image *bin)
{
  Image *bndr=NULL;
  Image *color=NULL,*pred=NULL,*lambda=NULL;
  int p=0,q,r,i,j,n,left=0,right=0,*LIFO,last,l;
  AdjRel *A,*L,*R;
  Pixel u,v,w;

  A     = Circular(1.0);
  n     = bin->ncols*bin->nrows;
  bndr  = CreateImage(bin->ncols,bin->nrows);
  for (p=0; p < n; p++){
    if (bin->val[p]==1){
      u.x = p%bin->ncols;
      u.y = p/bin->ncols;
      for (i=1; i < A->n; i++){
	v.x = u.x + A->dx[i];
	v.y = u.y + A->dy[i];
	if (ValidPixel(bin,v.x,v.y)){
	  q = v.x + bin->tbrow[v.y];
	  if (bin->val[q]==0){
	    bndr->val[p]=1;
	    break;
	  }
	} else {
	    bndr->val[p]=1;
	    break;
	}
      }
    }
  }
  DestroyAdjRel(&A);

  A      = Circular(1.5);
  L      = LeftSide(A);
  R      = RightSide(A);
  lambda = CreateImage(bndr->ncols,bndr->nrows);
  color  = CreateImage(bndr->ncols,bndr->nrows);
  pred   = CreateImage(bndr->ncols,bndr->nrows);
  n      = bndr->ncols*bndr->nrows;
  LIFO   = AllocIntArray(n);
  last   = NIL;
  for (j=0; j < n; j++){
    if ((bndr->val[j]==1)
	&&(color->val[j]!=BLACK)
	&&ValidContPoint(bin,L,R,j)){
      last++;
      LIFO[last]    = j;
      color->val[j] = GRAY;
      pred->val[j] = j;
      while(last != NIL){
	p = LIFO[last]; last--;
	color->val[p]=BLACK;
	u.x = p%bndr->ncols;
	u.y = p/bndr->ncols;
	for (i=1; i < A->n; i++){
	  v.x = u.x + A->dx[i];
	  v.y = u.y + A->dy[i];
	  if (ValidPixel(bndr,v.x,v.y)){
	    q = v.x + bndr->tbrow[v.y];
	    if ((q==j)&&(pred->val[p]!=j)){
	      last = NIL;
	      break;
	    }

	    w.x = u.x + L->dx[i];
	    w.y = u.y + L->dy[i];
	    if (ValidPixel(bndr,w.x,w.y))
	      left = w.x + bndr->tbrow[w.y];
	    else
	      left = -1;
	    w.x = u.x + R->dx[i];
	    w.y = u.y + R->dy[i];
	    if (ValidPixel(bndr,w.x,w.y))
	      right = w.x + bndr->tbrow[w.y];
	    else
	      right = -1;

	    if ((bndr->val[q]==1)&&
		(color->val[q] != BLACK)&&
		(((left!=-1)&&(right!=-1)&&(bin->val[left]!=bin->val[right]))||
		 ((left==-1)&&(right!=-1)&&(bin->val[right]==1)) ||
		 ((right==-1)&&(left!=-1)&&(bin->val[left]==1)))){
	      pred->val[q] = p;
	      if (color->val[q] == WHITE){
		last++;
		LIFO[last] = q;
		color->val[q]=GRAY;
	      }
	    }
	  }
	}
      }
      r = p;
      l = 1;
      while(pred->val[p]!=p){
	lambda->val[p] = l;
	p = pred->val[p];
	l++;
      }

      if (r != p) {
	lambda->val[p] = l;
      }

    }
  }


  DestroyAdjRel(&A);
  DestroyAdjRel(&L);
  DestroyAdjRel(&R);
  DestroyImage(&bndr);
  DestroyImage(&color);
  DestroyImage(&pred);
  free(LIFO);
  return(lambda);
}


// Computes the perimeter of each contour

Image *Perimeter(Image *bin)
{
  int p,n;
  Image *cont,*perim;
  int *hist;

  cont  = LambdaContour(bin);
  n     = cont->ncols*cont->nrows;
  for (p=0; p < n; p++) {
    if(cont->val[p]==NIL)
      cont->val[p]=0;
  }

  hist  = AllocIntArray(MaximumValue(cont)+1);
  perim = CreateImage(cont->ncols,cont->nrows);
  for (p=0; p < n; p++)
    hist[cont->val[p]]++;
  for (p=0; p < n; p++)
    if (cont->val[p] > 0)
      perim->val[p] = hist[cont->val[p]];

  free(hist);
  DestroyImage(&cont);

  return(perim);
}

// Computes multiscale skeletons

Image *MSSkel(Image *I)
{
  int i,p,q,n,maxd1,maxd2,d1,d2,MaxD;
  Pixel u,v;
  Image *msskel,*cont=NULL,*perim=NULL,*label=NULL;
  AdjRel *A;
  Forest *F=NULL;

  /* Compute MS Skeletons */

  cont   = LambdaContour(I);
  F      = DistTrans(cont); // Euclidean distance transform of the contours
  label  = LambdaContPixel(I);
  n      = I->ncols*I->nrows;
  perim  = Perimeter(I);
  A      = Circular(1.0);
  msskel = CreateImage(I->ncols,I->nrows);


  MaxD = INT_MIN;
  for (p=0; p < n; p++) {
    if (F->R->val[p] != p) { // avoid computation on the contours
      u.x = p%I->ncols;
      u.y = p/I->ncols;
      maxd1 = maxd2 = INT_MIN;
      for (i=1; i < A->n; i++){
	v.x = u.x + A->dx[i];
	v.y = u.y + A->dy[i];
	if (ValidPixel(I,v.x,v.y)){
	  q = v.x + I->tbrow[v.y];
	  if (cont->val[F->R->val[p]] == cont->val[F->R->val[q]]){
	    d2   = label->val[F->R->val[q]]-label->val[F->R->val[p]];
	    if (d2 > (perim->val[F->R->val[p]]-d2)){
	      d2 = (perim->val[F->R->val[p]]-d2);
	    }
	    if (d2 > maxd2){
	      maxd2 = d2;
	    }
	  } else {
	    d1 = cont->val[F->R->val[q]] - cont->val[F->R->val[p]];
	    if (d1 > maxd1)
	      maxd1 = d1;
	  }
	}
      }
      if (maxd1 > 0) {
	msskel->val[p] = INT_MAX;
      } else {
	msskel->val[p] = MAX(maxd2,0);
	if (msskel->val[p] > MaxD)
	  MaxD = msskel->val[p];
      }
    }
  }

  for (p=0; p < n; p++) { /* Set the SKIZ */
    if (msskel->val[p] == INT_MAX)
      msskel->val[p] = MaxD + 1;
  }

  DestroyImage(&cont);
  DestroyImage(&perim);
  DestroyImage(&label);
  DestroyAdjRel(&A);
  DestroyForest(&F);

  return(msskel);
}


int main(int argc, char **argv)
{
  timer    *t1=NULL,*t2=NULL;
  Image    *img,*skel,*aux;
  Image   *msskel;
  int      maxval;
  char   outfile[100];
  char   *file_noext;

  /* The following block must the remarked when using non-linux machines */

  void *trash = malloc(1);
  struct mallinfo info;
  int MemDinInicial, MemDinFinal;
  free(trash);
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*----------------------------------------------------------------------*/

  if (argc != 2) {
    printf("Usage: %s <image.pgm>\n",argv[0]);
    printf("image.pgm: binary image for which a multiscale skeleton will be computed\n");
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

  msskel=MSSkel(img);
  
  t2 = Toc();

  fprintf(stdout,"Skeletonization in %f ms\n",CTime(t1,t2));

  sprintf(outfile,"%s_msskel.pgm",file_noext);
  WriteImage(msskel,outfile);

  maxval = MaximumValue(msskel);

  /* It is usually better to treat the external and internal skeletons
     separately, with different thresholds. The results below are
     showing both together and using a single threshold, for sake of
     simplicity. */
  
  skel = Threshold(msskel,(int)(0.05*maxval),maxval);
  sprintf(outfile,"%s_skel-a.pgm",file_noext);
  WriteImage(skel,outfile);
  DestroyImage(&skel);

  skel = Threshold(msskel,(int)(0.35*maxval),maxval);
  sprintf(outfile,"%s_skel-b.pgm",file_noext);
  WriteImage(skel,outfile);
  DestroyImage(&skel);

  skel = Threshold(msskel,(int)(0.75*maxval),maxval);
  sprintf(outfile,"%s_skel-c.pgm",file_noext);
  WriteImage(skel,outfile);
  DestroyImage(&skel);
  
  DestroyImage(&msskel);
  DestroyImage(&img);

  /* The following block must the remarked when using non-linux machines */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);


  return(0);
}
