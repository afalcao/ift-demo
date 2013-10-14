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

@article{Falcao04a,
author = "A.X. Falc{\~{a}}o and F.P.G. Bergo",
title  = "Interactive Volume Segmentation with Differential Image Foresting Transforms",
journal = "IEEE Trans. on Medical Imaging", 
volume = 23, 
number = 9,
pages = "1100--1108", 
month = "Sep", 
year = 2004
}

*/

/* Data strutcure to store the forest maps */

typedef struct _ImageForest {
  Image *cost;
  Image *label;
  Image *pred;
  Image *root;
} ImageForest;

/* Allocate memory for forest maps */

ImageForest *CreateImageForest(Image *img)
{
  int p, n = img->ncols * img->nrows; 
  ImageForest *fst = (ImageForest *)calloc(1,sizeof(ImageForest));
  
  fst->cost  = CreateImage(img->ncols,img->nrows);
  fst->pred  = CreateImage(img->ncols,img->nrows);
  fst->label = CreateImage(img->ncols,img->nrows);
  fst->root  = CreateImage(img->ncols,img->nrows);

  for (p=0; p < n; p++) {
    fst->cost->val[p]  = INT_MAX;
    fst->root->val[p]  = p;
    fst->pred->val[p]  = NIL;
    fst->label->val[p] = 0;
  }
  return(fst);
}

/* Free memory of the forest maps */

void DestroyImageForest(ImageForest **fst)
{
  ImageForest *aux=*fst;

  if (aux != NULL){
    DestroyImage(&aux->cost);
    DestroyImage(&aux->root);
    DestroyImage(&aux->pred);
    DestroyImage(&aux->label);
    free(aux);
    *fst = NULL;
  }
}

/* Forest removal from pixels selected in the removal marker set Rm */

Set *ForestRemoval(ImageForest *fst, Set *Rm, AdjRel *A)
{
  int  i, p, q, n = fst->cost->ncols*fst->cost->nrows;
  Image *cost=fst->cost,*pred=fst->pred,*root=fst->root;
  Pixel u,v;
  Set  *Frontier=NULL, *aux=NULL;
  BMap *RemRoot = BMapNew(n), *inFrontier = BMapNew(n);  
  int  *RemNode=AllocIntArray(n),first=0,last=0;

  /* Mark all removal roots, whose trees will be removed, reinitialize
  cost and predecessor values of those roots, and insert them in
  RemNode, in order to reinitialize the cost and predecessor values of
  the remaining nodes of the removed trees. */

  aux = Rm;
  while (aux != NULL) { 
    p = root->val[aux->elem]; // get a removal root p 
    if (_fast_BMapGet(RemRoot,p)==0){ // p is not in RemRoot
      cost->val[p] = INT_MAX;  pred->val[p] = NIL;
      RemNode[last]=p; last++;
      _fast_BMapSet1(RemRoot,p);
    }
    aux = aux->next;
  }

  /* Forest removal: reinitialize nodes of the removed trees and
     create frontier set for the next DIFT. */

  while (first != last) {
    p = RemNode[first]; first++;
    u.x = p % cost->ncols; 
    u.y = p / cost->ncols; 
    for (i=1; i < A->n; i++) {
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      if (ValidPixel(cost,v.x,v.y)){
	q   = v.x + cost->tbrow[v.y];
	if (pred->val[q]==p){
	  cost->val[q]=INT_MAX; pred->val[q] = NIL;
	  RemNode[last]=q; last++;
	}else{ 
	  if (_fast_BMapGet(RemRoot,root->val[q])==0){ 	                                   if (_fast_BMapGet(inFrontier,q)==0){
	      InsertSet(&Frontier,q); _fast_BMapSet1(inFrontier,q);
	    }
	  }
	}
      }
    }
  }

  BMapDestroy(inFrontier);
  BMapDestroy(RemRoot);
  free(RemNode);

  return(Frontier);
}

/* Differential watershed by marker imposition */ 

void DiffWatershed(Image *grad, ImageForest *fst, Set *Obj, Set *Bkg, Set *Rm)
{
  AdjRel *A=NULL;
  GQueue *Q=NULL;
  Pixel   u,v;
  int     i,p,q,n,tmp,Cmax=MaximumValue(grad);
  Set    *aux, *Frontier=NULL;
  Image  *cost=fst->cost,*pred=fst->pred,*label=fst->label,*root=fst->root;

  n     = grad->ncols*grad->nrows;
  Q     = CreateGQueue(Cmax+1,n,cost->val);
  A     = Circular(1.5);

  if (Rm != NULL) { // Treat removed trees
    Frontier = ForestRemoval(fst,Rm,A);
    while (Frontier != NULL) {
      p = RemoveSet(&Frontier);
      InsertGQueue(&Q,p);
    }
  }
 
  /* Trivial path initialization for new seeds */

  aux = Obj;
  while(aux != NULL){
    p=aux->elem;
    if (Q->L.elem[p].color == GRAY) { /* p is also a frontier pixel,
		but the priority is it as a seed. */
      RemoveGQueueElem(Q,p);
    }
    label->val[p]=1; cost->val[p]=0; root->val[p]=p; pred->val[p]=NIL; 
    InsertGQueue(&Q,p);
    aux = aux->next;
  }

  aux = Bkg;
  while(aux != NULL){
    p=aux->elem;
    if (Q->L.elem[p].color == GRAY) { /* p is also a frontier pixel,
		but the priority is it as a seed. */
      RemoveGQueueElem(Q,p);
    }
    label->val[p]=0; cost->val[p]=0; root->val[p]=p; pred->val[p]=NIL;
    InsertGQueue(&Q,p);
    aux = aux->next;
  }

  /* Path propagation */

  while (!EmptyGQueue(Q)){
    p   = RemoveGQueue(Q);
    u.x = p%grad->ncols;
    u.y = p/grad->ncols;
    for (i=1; i < A->n; i++) {
      v.x = u.x + A->dx[i];
      v.y = u.y + A->dy[i];
      if (ValidPixel(grad,v.x,v.y)){
	q   = v.x + grad->tbrow[v.y];
	if (Q->L.elem[q].color != BLACK) {
	  tmp = MAX(cost->val[p] , grad->val[q]);
	  if ((tmp < cost->val[q])||(pred->val[q]==p)){
	    if (Q->L.elem[q].color == GRAY) { 
	      RemoveGQueueElem(Q,q);
	    }
	    cost->val[q]  = tmp;
	    label->val[q] = label->val[p];
	    root->val[q]  = root->val[p];
	    pred->val[q]  = p; 
	    InsertGQueue(&Q,q);
	  }
	}
      }
    }
  }

  DestroyGQueue(&Q);
  DestroyAdjRel(&A);
}

int main(int argc, char **argv)
{
  timer    *t1=NULL,*t2=NULL;
  Image    *img=NULL,*grad=NULL;
  ImageForest *fst=NULL;
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
    fprintf(stderr,"Usage: diffwatershed <image.pgm> <gradient.pgm> <seeds.txt>\n");
    fprintf(stderr,"image.pgm: image to overlay the watershed lines on it\n");
    fprintf(stderr,"gradient.pgm: gradient image to compute the watershed segmentation\n");
    fprintf(stderr,"seeds.txt: seed pixels\n");
    exit(-1);
  }

  img   = ReadImage(argv[1]);
  grad  = ReadImage(argv[2]);
  ReadSeeds(argv[3],&Obj,&Bkg);
  fst   = CreateImageForest(img);

  file_noext = strtok(argv[1],".");

  // Add object and background seeds

  t1 = Tic();
  DiffWatershed(grad,fst,Obj,Bkg,NULL); 
  t2 = Toc();
  fprintf(stdout,"Adding object and background seeds in %f ms\n",CTime(t1,t2));
  cimg = DrawLabeledRegions(img,fst->label);
  sprintf(outfile,"%s_result-a.ppm",file_noext);
  WriteCImage(cimg,outfile);
  DestroyCImage(&cimg);
  
  // Remove background trees 

  t1 = Tic();
  DiffWatershed(grad,fst,NULL,NULL,Bkg);
  t2 = Toc();
  fprintf(stdout,"Removing background trees in %f ms\n",CTime(t1,t2));
  cimg = DrawLabeledRegions(img,fst->label);
  sprintf(outfile,"%s_result-b.ppm",file_noext);
  WriteCImage(cimg,outfile);
  DestroyCImage(&cimg);

  // Adding background trees back to the forest  

  t1 = Tic();
  DiffWatershed(grad,fst,NULL,Bkg,NULL);
  t2 = Toc();
  fprintf(stdout,"Adding the removed background trees in %f ms\n",CTime(t1,t2));
  cimg = DrawLabeledRegions(img,fst->label);
  sprintf(outfile,"%s_result-c.ppm",file_noext);
  WriteCImage(cimg,outfile);
  DestroyCImage(&cimg);


  DestroyImageForest(&fst);
  DestroyImage(&img);
  DestroyImage(&grad);
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
