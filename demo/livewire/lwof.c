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

/* Papers related to this program:

@article{Falcao00a, 
 author    = "A.X. Falc{\~{a}}o and J.K. Udupa and F.K. Miyazawa", 
 title     = "An ultra-fast user-steered image segmentation paradigm:
{L}ive-wire-on-the-fly",
 journal   = "IEEE Trans. on Medical Imaging",
 volume = "19",
 number = "1",
 pages = "55--62",
 month = "Jan",
 year = 2000
}

@article{Falcao98,
author = "A.X. Falc{\~{a}}o and J.K. Udupa and S. Samarasekera and S. Sharma and B.E. Hirsch and R.A. Lotufo",
title = "User-Steered Image Segmentation Paradigms: Live Wire and Live Lane",
journal = "Graphical Models and Image Processing",
publisher = "Academic Press",
volume = "60", 
number = "4",
pages =  "233--260",
month = "Jul",
year = 1998
}

*/

int Gmax;

void ComputePath(Image *I, Image *V, Image *P, Image *O, GQueue **Q, AdjRel *A, AdjRel *L, AdjRel *R, int last)
{
  int p,q,i,left,right,tmp;
  Pixel u,v;


  if ((*Q)->L.elem[last].color != BLACK){
    p=NIL;
    while ((!EmptyGQueue(*Q))&&(p!=last)){
      p   = RemoveGQueue(*Q);
      u.x = p%I->ncols; u.y = p/I->ncols;
      for (i=1; i < A->n; i++) {
	v.x = u.x + A->dx[i]; v.y = u.y + A->dy[i];
	if (ValidPixel(I,v.x,v.y)){
	  q   = v.x + I->tbrow[v.y];
	  if (V->val[p] < V->val[q]){
	    // treat orientation
	    v.x = u.x + L->dx[i]; v.y = u.y + L->dy[i];
	    if (ValidPixel(I,v.x,v.y))
	      left = v.x + I->tbrow[v.y];
	    else
	      left = q;
	    v.x = u.x + R->dx[i]; v.y = u.y + R->dy[i];
	    if (ValidPixel(I,v.x,v.y))
	      right = v.x + I->tbrow[v.y];
	    else
	      right = q;

	    if (O->val[left] <= O->val[right])
	      tmp = V->val[p]+(int)pow(I->val[q],1.5);
	    else
	      tmp = V->val[p]+Gmax;

	    if ((V->val[left] == INT_MIN)&&(V->val[right]==INT_MIN))
	      tmp = INT_MAX; // avoid self-crossing

	    if (tmp < V->val[q]){
	      if (V->val[q]!=INT_MAX)
		RemoveGQueueElem(*Q,q);
	      V->val[q] =tmp;
	      P->val[q] =p;
	      InsertGQueue(Q,q);
	    }
	  }
	}
      }
    }
  }
}

void DisplayPath(Image *img, Image *P, int p, int i, const char* origfile)
{
  CImage *cimg=NULL;
  int q,n=img->ncols*img->nrows;
  char filename[200];

  if (P->val[p]!=NIL){
    sprintf(filename,"%s_lwof%03d.ppm",origfile,i);
    cimg = CreateCImage(img->ncols,img->nrows);

    for (q=0; q < n; q++) {
      cimg->C[0]->val[q]=img->val[q];
      cimg->C[1]->val[q]=img->val[q];
      cimg->C[2]->val[q]=img->val[q];
    }

    q = p;
    while(q!=NIL){
      cimg->C[0]->val[q]=255;
      cimg->C[1]->val[q]=255;
      q = P->val[q];
    }
    WriteCImage(cimg,filename);
    DestroyCImage(&cimg);
  }
}

/* Code for LiveWireOnTheFly

V is the cost or connectivity map
P is the predecessor map
O is the object membership map
G is the gradient image
I is the image to overlay the optimum contour

Actions:

1 - Select point
2 - Compute and display optimum segment
3 - Correct optimum contour up to selected point
4 - Close contour from a selected point and quit
5 - quit

*/

int main(int argc, char **argv)
{
  timer    *t1=NULL,*t2=NULL;
  AdjRel *A=NULL,*L=NULL,*R=NULL;
  GQueue *Q=NULL;
  Image  *V=NULL,*P=NULL,*I=NULL,*G=NULL,*O=NULL;
  int     p,q,n,action,dist,mindist,cstpt,curr,i;
  Pixel   u,v;
  FILE   *fp=NULL;
  char    quit=0;
  char   *file_noext;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);
  struct mallinfo info;
  int MemDinInicial, MemDinFinal;
  free(trash);
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=5){
    fprintf(stderr,"Usage: lwof <image.pgm> <gradient.pgm> <objectmap.pgm> <actions.txt>\n");
    fprintf(stderr,"image.pgm: image to overlay the optimum contour\n");
    fprintf(stderr,"gradient.pgm: gradient image to compute optimum paths\n");
    fprintf(stderr,"objectmap.pgm: object map to exploit contour orientation\n");
    fprintf(stderr,"actions.txt: actions to simulate LWOF\n");
    exit(-1);
  }

  t1  = Tic();


  I     = ReadImage(argv[1]);

  file_noext = strtok(argv[1],".");

  n     = I->ncols*I->nrows;
  G     = ReadImage(argv[2]);
  Gmax  = MaximumValue(G);
  for (p=0; p < n; p++)
    G->val[p] = Gmax - G->val[p];
  Gmax  = (int)(pow(Gmax,1.5)+0.5);
  O     = ReadImage(argv[3]);
  V     = CreateImage(I->ncols,I->nrows);
  P     = CreateImage(I->ncols,I->nrows);
  Q     = CreateGQueue(Gmax+1,n,V->val);
  A     = Circular(1.5);
  L     = LeftSide(A);
  R     = RightSide(A);
  fp    = fopen(argv[4],"r");


  // Set predecessor map to trivial forest
  for (p=0; p < n; p++) {
    P->val[p]=NIL;
    V->val[p]=INT_MAX;
  }

  curr = NIL; // current end point of the last selected segment
  i    = 1; // first output image


  // main loop of the user interface

  while (!quit) {

    if (fscanf(fp,"%d %d %d",&action,&u.x,&u.y)!=0);
    p = u.x + I->tbrow[u.y];

    if (V->val[p]!=INT_MIN){

      switch(action) {

      case 1: // select point p


	if (curr!=NIL){
	  DisplayPath(I,P,p,i,file_noext); i++;
	  ResetGQueue(Q);
	  // mark selected segment as permanent
	  q = p;
	  while(q != P->val[curr]){ V->val[q]=INT_MIN; q = P->val[q];}
	}

	curr = p;
	for(q=0; q < n; q++) // (re)initialize path values
	  if (V->val[q]!=INT_MIN) V->val[q]=INT_MAX;
	V->val[curr]=0;
	InsertGQueue(&Q,curr);
	break;

      case 2: // compute and display path
	if (curr!=NIL){
	  ComputePath(G,V,P,O,&Q,A,L,R,p);
	  DisplayPath(I,P,p,i,file_noext); i++;
	}
	break;

      case 3: // erase path and display the remaining edges
	// Find the closest point on contour
	mindist=INT_MAX; cstpt=-1; q = curr;
	while(q!=NIL){
	  v.x  = q % I->ncols; v.y  = q / I->ncols;
	  dist = abs(v.x-u.x)+abs(v.y-u.y);
	  if (dist<mindist){ mindist = dist; cstpt=q; }
	  q = P->val[q];
	}
	// Erase path
        q = curr;
	while (q!=cstpt){
	  V->val[q]=INT_MAX; q = P->val[q];
	}
	// select point cstpt
	curr = cstpt;
	ResetGQueue(Q);
	for(q=0; q < n; q++) // reinitialize path values
	  if (V->val[q]!=INT_MIN) V->val[q]=INT_MAX;
	V->val[curr]=0;
	InsertGQueue(&Q,curr);
	DisplayPath(I,P,curr,i,file_noext);i++;
	break;

      case 4: // select point, close contour, display and quit
	if (curr!=NIL){	// select point
	  ComputePath(G,V,P,O,&Q,A,L,R,p);
	  ResetGQueue(Q);
	  q = p;// mark selected segment as permanent
	  while(q != P->val[curr]){ V->val[q]=INT_MIN; q = P->val[q];}

	  curr = p;
	  for(q=0; q < n; q++)  // reinitialize path values
	    if (V->val[q]!=INT_MIN) V->val[q]=INT_MAX;
	  V->val[curr]=0;
	  InsertGQueue(&Q,curr);
	  q = curr; p = P->val[q];
	  while(P->val[p]!=NIL){ // find first contour point
	    q = p;
	    p = P->val[p];
	  }
	  P->val[q]=NIL; // new first point
	  V->val[p]=INT_MAX;
	  ComputePath(G,V,P,O,&Q,A,L,R,p);
	  DisplayPath(I,P,p,i,file_noext); i++;
	}
	quit = 1;

	break;

      case 5:
	quit=1;
	break;

      default:
	fprintf(stderr,"Error: invalid action %d\n",action);
	fprintf(stderr,"Valid actions are:\n");
	fprintf(stderr,"1: select point x y\n");
	fprintf(stderr,"2: display path up to point x y\n");
	fprintf(stderr,"3: erase path up to point x y\n");
	fprintf(stderr,"4: close contour and quit\n");
	fprintf(stderr,"5: quit\n");
	quit = 1;
      }
    }
  }


 t2 = Toc();

 fprintf(stdout,"lwof in %f ms\n",CTime(t1,t2));
 fclose(fp);
 DestroyGQueue(&Q);
 DestroyImage(&V);
 DestroyImage(&P);
 DestroyAdjRel(&A);
 DestroyAdjRel(&L);
 DestroyAdjRel(&R);
 DestroyImage(&I);
 DestroyImage(&G);
 DestroyImage(&V);
 DestroyImage(&O);


  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);

  return(0);
}
